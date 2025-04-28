#include <array>
#include <cstring>

#include "Node.h"
#include "SparkWeaverCore.h"
#include "nodes/DsDmxRgb.h"
#include "nodes/FxBreathe.h"
#include "nodes/FxStrobe.h"
#include "nodes/MxAdd.h"
#include "nodes/MxSubtract.h"
#include "nodes/SrColor.h"
#include "nodes/TrCycle.h"
#include "nodes/TrDelay.h"
#include "nodes/TrRandom.h"

namespace SparkWeaverCore {
    namespace {
        template <std::size_t N>
        [[nodiscard]] std::array<uint16_t, N>
        parseParams(const std::string& name, const std::vector<std::string>& params)
        {
            if (params.size() != N) {
                throw InvalidTreeException(name + " invalid parameters");
            }
            std::array<uint16_t, N> result = {};
            try {
                for (std::size_t i = 0; i < N; ++i) {
                    const auto value = std::stoul(params[i]);
                    if (value > std::numeric_limits<uint16_t>::max()) {
                        throw InvalidTreeException(name + " parameter outside range");
                    }
                    result[i] = static_cast<uint16_t>(value);
                }
            } catch ([[maybe_unused]] const std::exception& e) {
                throw InvalidTreeException(name + " invalid parameters");
            }
            return result;
        }
    }

    void Builder::resetNodeTree() noexcept
    {
        for (const auto all_node : all_nodes) {
            delete all_node;
        }
        all_nodes.clear();
        root_nodes.clear();
    }

    void Builder::closeParam() noexcept
    {
        if (!buffer_current_param.empty()) {
            buffer_all_params.push_back(buffer_current_param);
            buffer_current_param.clear();
        }
    }

    void Builder::clearCommandBuffer() noexcept
    {
        buffer_command.clear();
        buffer_command_parsed = false;
        buffer_current_param.clear();
        buffer_all_params.clear();
    }

    void Builder::finishCommand()
    {
        closeParam();
        if (!buffer_command.empty()) {
            parseTreeCommand(buffer_command, buffer_all_params);
        }
        clearCommandBuffer();
    }

    void Builder::parseTreeCommand(const std::string& command, const std::vector<std::string>& params)
    {
        // CONNECTION
        if (command == "C" || command == "T") {
            const auto [from, to] = parseParams<2>("Connection", params);
            if (from >= all_nodes.size() || to >= all_nodes.size()) {
                throw InvalidTreeException(
                    "Connection " + std::to_string(from) + "-" + std::to_string(to) + " out of range");
            }
            if (command == "C") {
                Node::connectColor(all_nodes.at(from), all_nodes.at(to));
            } else if (command == "T") {
                Node::connectTrigger(all_nodes.at(from), all_nodes.at(to));
            }
        }

        // DESTINATION
        else if (command == "DsDmxRgb") {
            const auto [address] = parseParams<1>(command, params);
            auto* p_node         = new DsDmxRgb(address);
            all_nodes.push_back(p_node);
            root_nodes.push_back(p_node);
        }

        // EFFECT
        else if (command == "FxBreathe") {
            const auto [cycle_length, phase_offset, darken_amount] = parseParams<3>(command, params);
            auto* p_node = new FxBreathe(cycle_length, phase_offset, darken_amount);
            all_nodes.push_back(p_node);
        } else if (command == "FxStrobe") {
            const auto [flash_length] = parseParams<1>(command, params);
            auto* p_node              = new FxStrobe(flash_length);
            all_nodes.push_back(p_node);
        }

        // MIX
        else if (command == "MxAdd") {
            auto* p_node = new MxAdd();
            all_nodes.push_back(p_node);
        } else if (command == "MxSubtract") {
            auto* p_node = new MxSubtract();
            all_nodes.push_back(p_node);
        }

        // SOURCE
        else if (command == "SrColor") {
            const auto [red, green, blue] = parseParams<3>(command, params);
            auto* p_node                  = new SrColor(Color(red, green, blue));
            all_nodes.push_back(p_node);
        }

        // TRIGGER
        else if (command == "TrCycle") {
            const auto [cycle_length, phase_offset] = parseParams<2>(command, params);
            auto* p_node                            = new TrCycle(cycle_length, phase_offset);
            all_nodes.push_back(p_node);
        } else if (command == "TrDelay") {
            const auto [delay_time] = parseParams<1>(command, params);
            auto* p_node            = new TrDelay(delay_time);
            all_nodes.push_back(p_node);
        } else if (command == "TrRandom") {
            const auto [min_time, max_time] = parseParams<2>(command, params);
            auto* p_node                    = new TrRandom(min_time, max_time);
            all_nodes.push_back(p_node);
        }

        // ERROR
        else {
            throw InvalidTreeException("Unknown command: " + command);
        }
    }

    void Builder::build(const std::string& tree)
    {
        time = 0;
        resetNodeTree();
        clearCommandBuffer();

        for (const char c : tree) {
            if (c == '\n') {
                finishCommand();
            } else if (c == ' ') {
                if (buffer_command.empty()) {
                    throw InvalidTreeException();
                }
                if (buffer_command_parsed) {
                    closeParam();
                } else {
                    buffer_command_parsed = true;
                }
            } else {
                if (buffer_command_parsed) {
                    buffer_current_param.push_back(c);
                } else {
                    buffer_command.push_back(c);
                }
            }
        }
        finishCommand();
    }

    /**
     * @brief Increment global clock and execute all nodes
     * @return Pointer to 513 bytes long DMX data array output, byte number corresponds to DMX address, 0 is unused
     */
    [[nodiscard]] uint8_t* Builder::tick() noexcept
    {
        memset(dmx_data, 0, sizeof(dmx_data));
        time++;
        for (const auto root_node : root_nodes) {
            root_node->render(time, dmx_data);
        }
        return dmx_data;
    }
}
