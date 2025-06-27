# SparkWeaver Core

Node-based lighting system for controlling DMX fixtures.

![Lights running on SparkWeaver](p2.jpg)

### Motivation

Created to run simple lighting setups using only a ESP32 hooked directly to DMX. The core library contains no platform-specific code, see below for implementations.

---

## Web interface

For editing and simulating node trees, see [SparkWeaver Web](https://github.com/rzuppur/sparkweaver.web) UI. It provides an easy editor to create node trees and visualize them using WebAssembly to run this core library.

![Screenshot of node based DMX lightning editor user interface](screen_web.jpg)

---

## Running on a microcontroller

To send DMX signals from an ESP32, see [SparkWeaver ESP](https://github.com/rzuppur/sparkweaver.esp) for a complete project compatible with SparkWeaver Web bluetooth interface. You need to connect a DMX board or build your own.

![ESP32 + Keyestudio DMX Shield](p1.jpg)

---

## Development

### Node name prefixes

All nodes extend a common Node class and configure the allowed combination of inputs, triggers and parameters. This naming convention should be followed.

- `ds` **Destination** (output color to fixture)
- `fx` **Effect** (modifies color somehow)
- `mx` **Mix** (combine multiple colors)
- `sr` **Source** (input color)
- `tr` **Trigger** (send or modify trigger signals)

### Principles

- Node tree must be a directed acyclic graph.
- Nodes run in ticks evaluated from the destination node.
- Nodes must evaluate all inputs at every tick (otherwise delays would break, for example). Node output may be requested multiple times in a single tick.
- Tick length is not defined but assumed to be around 24 ms, the time it takes to send one full 512-byte DMX packet. That's about 42 FPS. You can have faster updates by sending less than 512 bytes.

### Tree format

Each line is a single command. Commands can have 0-`NODE_PARAMS_MAX` parameters separated by a space.

```
COMMAND_A Param1 Param2 Param3
COMMAND_B Param1
COMMAND_C
```

Commands are either node names (SrColor, MxAdd, etc.) or a list of connections to a node. Nodes are zero-indexed starting from the top. Connections should be after all node definitions.

Parameters must be unsigned 16-bit integers.

### Connection commands

- **`CI SOURCE_INDEX [FROM_INDEX ...]`** color inputs
- **`CO SOURCE_INDEX [TO_INDEX ...]`** color outputs

Colors are eight-bit RGB values.

- **`TI SOURCE_INDEX [FROM_INDEX ...]`** trigger inputs
- **`TO SOURCE_INDEX [TO_INDEX ...]`** trigger outputs

Triggers are boolean signals, can be used to trigger effects.

### Example

Creates a red (255 0 0) color input node (index 0), connects its color output to DMX fixture (index 1) at address 50.

```
SrColor 255 0 0
DsDmxRgb 50
CO 0 1
CI 1 0
```

---

## License

MIT License

SparkWeaver Copyright (c) 2025 Reino Zuppur

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
