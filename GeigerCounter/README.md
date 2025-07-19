# Geiger Counter Testing Setup

1. Plug in Arduino Nano
    - take note of port number
2. Upload sketch `ssrt_geiger_prototype.ino`
    - make sure to use `ATmega328P (Old Bootloader)` for sketch to upload properly
3. Plug female XLR receiver assembly into `D3` and `GND` pins
    - orange and brown respectively
4. Plug male XLR reveiver assembly into USB/UART terminal
    - red wire is `Rx`, black wire is `GND`
5. Plug XLR cable into both receivers
6. Compile and run `read-uart-xxx.cpp` where `xxx` is either windows or linux depending on your platform
7. See hex digits printing and be happy :)
