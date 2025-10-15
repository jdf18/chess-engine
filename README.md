# Chess Engine

A basic Chess Engine written in C++ using the Minimax algorithm currently based on the sum of pieces value on the board at any point in time. 

## Requirements

- A C compiler toolchain
- CMake
- (Optional) a chess app supporting UCI

## Building

```bash
mkdir -p build
cd build
cmake ..
make
```

## Usage 

Full UCI Implementation is currently a work in progress (see [feature-uci](https://github.com/jdf18/chess-engine/tree/feature-uci) branch). \
The app used for testing is [Scid vs Pc](https://scidvspc.sourceforge.net/). To add the engine to the app, go to `Tools/Analysis Engines`, then `New`, the `command` field should point to the built executable, and the directory should point to the build directory. The UCI protocol should be selected. 

