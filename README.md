# SparkWeaver Core

Node based lightning system for controlling DMX fixtures.

## Node name prefixes

All nodes extend a common Node class and configure the allowed combination of inputs and triggers. For ease of use however this naming convention should be followed.

- `ds` **Destination** (output color to fixture)
- `fx` **Effect** (modifies color somehow)
- `mx` **Mix** (combine multiple colors)
- `sr` **Source** (input color)
- `tr` **Trigger** (send or modify trigger signals)

## Principles

- Node tree must be a directed acyclic graph.
- Nodes run in ticks evaluated from the destination node.
- Nodes must evaluate all inputs every tick. **_Skipping ticks breaks delays!_**
- Tick length is not defined but assumed to be around 24ms, the time it takes to send one full 512 byte DMX packet. That's about 42 FPS. You can have faster updates by sending less than 512 bytes.

## Tree format

Each line is a single command. Commands can have 0-N parameters separated by a space.

```
COMMAND_A Param1 Param2 Param3
COMMAND_B Param1
COMMAND_C
```

Commands are either node names (SrColor, MxAdd, etc.) or a connection from node to another. Nodes are zero-indexed starting from top.

### Connection commands

#### **`C FROM_INDEX TO_INDEX`** color connection

Colors are eight bit RGB values.

#### **`T FROM_INDEX TO_INDEX`** trigger connection

Triggers are boolean signals, can be used to trigger effects.

### Example

Creates a red (255 0 0) color input node (index 0), connects it's color output to DMX fixture (index 1) at address 50.

```
SrColor 255 0 0
DsDmxRgb 50
C 0 1
```

More complex real world example for two randomly flashing strobes.

```c++
SparkWeaverCore::Builder builder;

try {
    builder.build(""
        "DsDmxRgb 1\n"
        "DsDmxRgb 5\n"
        "TrRandom 5 42\n"
        "TrRandom 5 42\n"
        "SrColor 255 255 255\n"
        "FxStrobe 1\n"
        "FxStrobe 1\n"
        "C 4 5\n"
        "C 4 6\n"
        "C 5 0\n"
        "C 6 1\n"
        "T 2 5\n"
        "T 3 6\n");
} catch (const std::exception& e) {
    Serial.println(e.what());
}

dmx_write(DMX_UART_PORT, builder.tick(), DMX_PACKET_SIZE);
```
