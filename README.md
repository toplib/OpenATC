# OpenATC
ATC simulator written in C++

## Description
This project is about simulating realistic ATC (Air Traffic Control) procedures, where players can take on the role of an Air Traffic Controller.
Players speak into their microphone and receive realistic voice responses from pilots.

## Core idea
Core idea of this project is pretty simple. I want to create program where everyone can train their communication skills. This IS NOT chatbot. AI there is only used for **Natural Language**, everything else is simulated in **simulation** module of this app. This app allows you to use it as **Pilot** or as **ATC**.

## FAQ
1. Q: How can I offer any idea to this project?
   A: You can just write your ideas into issues tab on our official github page. **Before you want to offer something please double check existing ideas!**
2. Q: How can I contribute my changes to the project?
   A: You can open PR but before create your own branch and ensure that CI and tests (in future) is passing successfully. (If change is major we strictly recommend to tell us in the issues)

## AI policy in codebase
However if this app using AI that doesn't mean you can "vibecode" everything you want. You CAN use AI but if you cant explain what you code actually does and how it works we will deny your PR anyway.

## Recommendations
1. Before you open PR please run sonar-qube analyzer to be sure that everything great. No matter if you generate this code or wrote it entirely by yourself. 

## Building
### Linux/MacOS/BSD-based OS
```bash
git clone https://github.com/toplib/OpenATC.git
cd OpenATC/
git submodule update --init --recursive
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
```


## License
This project is licensed under Apache 2.0.
