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

1. [esp-idf and arduino-esp32](#esp-idf-and-arduino-esp32)
2. project structure
3. project setup
4. spiffs filesystem and usage in project
5. build and flash with Makefile build system (esp-idf v3.x default)
6. build and flash with CMake build system (esp-idf v4.x default)
7. use with Eclipse CDT
8. use with VSCODE
9. maintenance and keeping up with esp-idf/arduino-esp32 updates


<a name="esp-idf-and-arduino-esp32"/>

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


## project structure
TBD


## project setup

Project structure is the same as any esp-idf project. The addition is the use of the Arduino core as an esp-idf component of the project.
Both Makefile build system and CMake build system is configured and either can be used.

 1. Clone the repo, cd to the repo dir, sync the submodules (as they have further submodules)

	>     git clone --recursive https://github.com/coolbreeze413/espidf_p_skeletor.git
	>     cd espidf_p_skeletor
	>     git submodule update --init --recursive
 
 2. Clone the esp-idf repo
	>     git clone --recursive https://github.com/espressif/esp-idf.git

	It is recommended to keep 2 separate esp-idf repos:
	- one for projects which use arduino-esp32 as a esp-idf component
		(**this is the one which we will be using for building this project**)
	- one for projects which do not

3. Move to the right revision of esp-idf to which the arduino-esp32 is based on. Later revisions of esp-idf may cause compilation issues in the arduino-esp32 component build. Sometimes, even this revision, though being the one on which the arduino-esp32 is based on, may cause (minor) issues, as the arduino-esp32 repo may have outstanding bugs or issues. In general this step should give a fully working setup, or very close to one.
YMMV.
	- open a console at the arduino-esp32 component of the project
		>     espidf_p_skeletor/components/arduino-esp32
	
	- search for the last update involving esp-idf
		>     git log --grep "IDF"
		
	- you should see something like:
		>     commit 0acf19af8f5e9e459f9cc39142138d2f5c351146 Author: Me No Dev
		>      <me-no-dev@users.noreply.github.com> Date:   Sun May 12 18:52:23 2019
		>      +0300
		>      
		>          Update IDF to v3.2 977854975 (#2771)
		>      
		>          * Update IDF to v3.2 977854975
		>      
		>          * Update app_httpd.cpp
		
	- take the hash of the esp-idf repo from the log
		> from the above example, the hash is 977854975
		
	- cd to the esp-idf repo and check the details of that commit, it will roughly match
		>     git show <hash>

	- you should see something like:
		>     git show 977854975                                                    
		>      commit 9778549759eb2032c72224417bedfb9a2fece5f9                       
		>      Merge: e5e226a 76c20ab                                                
		>      Author: Jiang Jiang Jian <jack@espressif.com>                         
		>      Date:   Thu Apr 11 18:44:19 2019 +0800                                
		>      
		>                                                                                                            
		>          Merge branch 'bugfix/tw28146_make_wifi_iram_optimization_configurable_v3.2' into
		>      'release/v3.2'   
		>                                                                                                            
		>          esp32: make WiFi IRAM optimization configurable (backport v3.2)                                   
		>                                                                                                            
		>          See merge request idf/esp-idf!4040

	- roll (back) the esp-idf repo to that revision
		>     git checkout <hash>

	- you should see something like (with the example above)
		>     git checkout 9778549759eb2032c72224417bedfb9a2fece5f9
		>     Checking out files: 100% (2467/2467), done.
		>     M       components/bt/lib
		>     M       components/coap/libcoap
		>     M       components/esptool_py/esptool
		>     M       components/json/cJSON
		>     M       components/mqtt/esp-mqtt
		>     Note: checking out '9778549759eb2032c72224417bedfb9a2fece5f9'.
		>     ... blah blah blah ...
		>     HEAD is now at 9778549... Merge branch 'bugfix/tw28146_make_wifi_iram_optimization_configurable_v3.2' into
		>      'release/v3.2'

	- update all the submodules of esp-idf so they align with the currently checked out revision
		>     git submodule update --init --recursive
	
	- remember that whenever we update the arduino-esp32 repo, we have to do the same steps as above.

	**NOTE:** if you are starting a fresh esp-df project which uses the arduino-esp32 as a component, ensure that the base or initial sdkconfig is taken from the arduino-esp32 repo's sdkconfig, otherwise the arduino core will not behave as you would expect. The sdkconfig in the arduino-esp32 usually lives here:
	>     arduino-esp32/tools/sdk/sdkconfig
	copy this file to your project's root directory. You can make further changes as needed for your project on top of this.
		

4. move to the project directory and build as usual for any esp-idf project.
Look at the appropriate steps for [Makefile Build System](#build-flash-makefile) or [CMake Build System](build-flash-cmake).


<a name="build-flash-makefile"/>

## build and flash with Makefile build system (esp-idf v3.x default)

### **Remember to set the IDF_PATH environment variable to point to the correct esp-idf repo which has been aligned to the arduino-esp32 above  before project build !**

Setup the msys2 based toolchain as per instructions.

>     make menuconfig
>     make -j4


<a name="build-flash-cmake"/>

## build and flash with CMake build system (esp-idf v4.x default)

### **Remember to set the IDF_PATH environment variable to point to the correct esp-idf repo which has been aligned to the arduino-esp32 above  before project build !**

Setup the CMake toolchain as per instructions.

>     py -2 %IDF_PATH%/tools/idf.py menuconfig
>     py -2 %IDF_PATH%/tools/idf.py build


## spiffs filesystem and usage in project

TBD


## use with Eclipse CDT

TBD


## use with VSCODE

TBD


## maintenance and keeping up with esp-idf/arduino-esp32 updates

TBD



