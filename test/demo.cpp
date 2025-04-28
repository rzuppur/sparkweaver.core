#include <format>
#include <iostream>
#include <string>

#include <SparkWeaverCore.h>

int main()
{
    std::cout << "SparkWeaverCore\n\nNODE TYPES\n\n";
    for (const auto configs = SparkWeaverCore::getNodeConfigs(); const auto& config : configs) {
        std::cout << std::format(
            "{:<16} {:<32} Color in {:>2}, out {};   Trigger in {:>2}, out {}\n",
            std::string_view(config.name.data()),
            std::string_view(config.title.data()),
            config.max_color_inputs,
            config.enable_color_outputs ? " enabled" : "disabled",
            config.max_trigger_inputs,
            config.enable_trigger_outputs ? " enabled" : "disabled");
    }

    SparkWeaverCore::Builder builder;

    std::cout << "\n\nSMOKE TEST\n\nexpected FF 80 40 00\n";
    builder.build("DsDmxRgb 1\nSrColor 255 128 64\nC 1 0\n");
    const auto data = builder.tick();
    std::cout << std::format("result   {:02X} {:02X} {:02X} {:02X}\n", data[1], data[2], data[3], data[4]);

    return 0;
}
