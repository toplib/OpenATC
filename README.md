# OpenATC
ATC simulator written in C++

## Description
This project is about simulating realistic ATC (Air Traffic Control) procedures, where players can take on the role of an Air Traffic Controller.
Players speak into their microphone and receive realistic voice responses from pilots.

## Building
```bash
git clone https://github.com/toplib/OpenATC.git
cd OpenATC/
git submodule update --init --recursive
mkdir build && cd build
cmake ..
cmake --build .
```

## License
This project is licensed under Apache 2.0.
