#include <format>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include <SparkWeaverCore.h>

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << "SparkWeaverCore\n\nNODE TYPES\n\n";
    for (const auto configs = SparkWeaverCore::getNodeConfigs(); const auto& config : configs) {
        std::cout << std::format(
            "{:<16} {:<24} C {}{:0>2}{}   T {}{:0>2}{}\n",
            config.name.data(),
            config.title.data(),
            config.max_color_inputs > 0 ? "->" : "  ",
            config.max_color_inputs,
            config.enable_color_outputs ? "->" : "  ",
            config.max_trigger_inputs > 0 ? "->" : "  ",
            config.max_trigger_inputs,
            config.enable_trigger_outputs ? "->" : "  ");
        for (int i = 0; i < config.params_count; i++) {
            const auto param = config.params[i];
            std::cout << std::format(
                " {} {:<16} {:<12} [{}]\n",
                i == config.params_count - 1 ? "└─" : "├─",
                std::string(param.name.data()) + ":",
                std::format("{}-{}", param.min, param.max),
                param.default_value);
        }
    }

    std::cout << "\n\nSMOKE TEST\n\nexpected FF 80 40 00\nresult   ";
    try {
        SparkWeaverCore::Builder builder;
        builder.build("DsDmxRgb 1\nSrColor 255 128 64\nC 1 0\n");
        const auto data = builder.tick();
        std::cout << std::format("{:02X} {:02X} {:02X} {:02X}\n", data[1], data[2], data[3], data[4]);
    } catch (const std::exception& e) {
        std::cout << std::string("ERROR (") + e.what() << ")\n";
        std::cerr << e.what() << '\n';
    }

    return 0;
}
