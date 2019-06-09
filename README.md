# s.k.E.l.E.t.0.r
```

███████╗██╗  ██╗███████╗██╗     ███████╗████████╗ ██████╗ ██████╗ 
██╔════╝██║ ██╔╝██╔════╝██║     ██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗
███████╗█████╔╝ █████╗  ██║     █████╗     ██║   ██║   ██║██████╔╝
╚════██║██╔═██╗ ██╔══╝  ██║     ██╔══╝     ██║   ██║   ██║██╔══██╗
███████║██║  ██╗███████╗███████╗███████╗   ██║   ╚██████╔╝██║  ██║
╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝
                                                                  
```

ASCII Art thanks to:
http://patorjk.com/software/taag/#p=display&h=0&v=0&f=ANSI%20Shadow&t=skeletor

1. esp-idf and  arduino-esp32
2. project structure and project setup
3. spiffs filesystem and usage in project
4. build and flash with Makefile build system (esp-idf v3.x default)
5. build and flash with CMake build system (esp-idf v4.x default)
6. use with Eclipse CDT
7. use with VSCODE
8. maintenance and keeping up with esp-idf/arduino-esp32 updates

## esp-idf and arduino-esp32

esp-idf is the official development SDK for the esp32 and is complete in and of itself. This has the entire core (FreeRTOS based) plus functionalities neatly divided into "components" to keep it modular.
https://github.com/espressif/esp-idf

arduino-esp32 is the arduino hal layer plus the arduino "core" wrapping around the esp-idf. This is always updated to support the current **stable** version of the esp-idf, hence will always be behind the bleeding edge of the esp-idf repo.
https://github.com/espressif/arduino-esp32

For development with esp32 there are a variety of options:
- use the Arduino core as is (with the Arduino IDE or VSCODE + Arduino Extension or Eclipse CDT + Arduino plugin ...)
This is the easiest to start with approach, but we are restricted to the configuration of the underlying esp-idf core that the arduino core decides is best as of that point of time.

- use the esp-idf as is (command-line or with Eclipse CDT or with VSCODE ...)
This provides with bleeding edge features and bug fixes straight from the core development framework and is best suited for creating new stuff.
For development for products, we would generally stick to a particular version of the esp-idf rather than the bleeding edge (the "master" branch)

- use the esp-idf and its build system as above, plus add the arduino core as a "component" of the project, so we can directly take advantage of the myriad Arduino libraries available. This can be useful, for example, for rapid development of features, and if required, they can be converted to "pure" esp-idf components to remove any dependency on the Arduino core, as well as remove possibility of bugs in the Arduino core affecting us.
However, the Arduino core is always written to run with the latest stable esp-idf version, which for product development is anyway the target.
Bleeding edge features from the latest esp-idf can in most cases be ported piecemeal into the stable esp-idf with simple changes.
This is the approach we take in this project, for all the reasons stated above, and accepting the major tradeoff of not being able to use the bleeding edge of the esp-idf as soon as new stuff is released.

## project structure and project setup

Project structure is the same as any esp-idf project. The addition is the use of the Arduino core as an esp-idf component of the project.
Both Makefile build system and CMake build system is configured and either can be used.

## spiffs filesystem and usage in project

TBD

## build and flash with Makefile build system (esp-idf v3.x default)

TBD


## build and flash with CMake build system (esp-idf v4.x default)

TBD


## use with Eclipse CDT

TBD


## use with VSCODE

TBD


## maintenance and keeping up with esp-idf/arduino-esp32 updates

TBD


