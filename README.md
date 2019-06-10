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
2. [project structure](#project-structure)
3. [project setup](#project-setup)
4. [build and flash with Makefile build system (esp-idf v3.x default)](#build-flash-makefile)
5. [build and flash with CMake build system (esp-idf v4.x default)](#build-flash-cmake)
6. [spiffs filesystem and usage in project](#spiffs-usage)
7. [use with Eclipse CDT](#eclipse-setup)
8. [use with VSCODE](#vscode-setup)
9. [maintenance and keeping up with esp-idf/arduino-esp32 updates](#maintenance)


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


<a name="project-structure"/>

## project structure

TBD. Hello There !


<a name="project-setup"/>

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

4. Move to the project directory and build as usual for any esp-idf project.

Look at the appropriate steps for [Makefile Build System](#build-flash-makefile) or [CMake Build System](#build-flash-cmake).


**NOTE:** if you are starting a fresh esp-df project which uses the arduino-esp32 as a component, ensure that the base or initial sdkconfig is taken from the arduino-esp32 repo's sdkconfig, otherwise the arduino core will not behave as you would expect. The sdkconfig in the arduino-esp32 usually lives here:
	>     arduino-esp32/tools/sdk/sdkconfig
Copy this file to your project's root directory. You can make further changes as needed for your project on top of this.



<a name="build-flash-makefile"/>

## build and flash with Makefile build system (esp-idf v3.x default)

- **Remember to set the IDF_PATH environment variable to point to the correct esp-idf repo which has been aligned to the arduino-esp32 above  before project build !**
- Setup the MSYS2 based Makefile toolchain as per instructions.
- Choose the right sdkconfig - delete existing "sdkconfig" file and copy "sdkconfig-cmake" as the new "sdkconfig".
- If migrating from one build system to another, ensure to delete the "build" directory.
- build project using :
	>     make menuconfig
	>     make -j4


<a name="build-flash-cmake"/>

## build and flash with CMake build system (esp-idf v4.x default)

- **Remember to set the IDF_PATH environment variable to point to the correct esp-idf repo which has been aligned to the arduino-esp32 above  before project build !**
- Setup the CMake toolchain as per instructions.
- Choose the right sdkconfig - delete existing "sdkconfig" file and copy "sdkconfig-cmake" as the new "sdkconfig".
- If migrating from one build system to another, ensure to delete the "build" directory.
- build project using :
	>     py -2 %IDF_PATH%/tools/idf.py menuconfig
	>     py -2 %IDF_PATH%/tools/idf.py build



<a name="spiffs-usage"/>

## spiffs filesystem and usage in project

- To manipulate spiffs images on the desktop, we need the mkspiffs tool. 

	Get the latest mkspiffs tool for the ESP32 from:
	>     https://github.com/igrr/mkspiffs/releases
	>     mkspiffs-x.x.x-arduino-esp32-win32.zip -or- mkspiffs-x.x.x-esp-idf-win32.zip (Windows)
	>     Linux TBD
	
	Or, we can build it from source as well. If so, ensure that:
	>     meta_obj_len is set to 0
	to match the SPIFFS implementation of the esp-idf, else it won't be readable on the ESP32.
	
- There is a directory "fs" in the project root. This is the "downstream" SPIFFS root, i.e. we will be flashing the ESP32 SPIFFS image *from* this. Create a SPIFFS image using this directory:
	>     mkspiffs.exe -c [spiffs_root_dir] [spiffs_img_path] -b [block_size] -p [page_size] -s [image_size]
	>     mkspiffs.exe -c fs skeletor_spiffs.img -b 4096 -p 256 -s 0x1C0000
	*NOTE*: The block size and page size are ok, but the size **MUST** be set the same as the size of the SPIFFS partition size as set in the project partition table csv!
	
- Flash the created image into the ESP32 using the esptool.py:
	>     py -2 esptool.py --chip esp32 --port [port] --baud [baud] write_flash -z [spiffs_partition_addr] [spiffs_img_path]
	>     py -2 esptool.py --chip esp32 --port COM4 --baud 921600 write_flash -z 0x240000 skeletor_spiffs.img
	*NOTE*: The partition address **MUST** be set the same as the address of the SPIFFS partition set in the project partition table csv!
	
	esptool.py can usually be found in:
	>     esp-idf/components/esptool_py/esptool/esptool.py
	
- Create a separate directory in the project root named "fs_u" . This is the "upstream" SPIFFS root, i.e. we can use this to read the existing SPIFFS image from ESP32 and expand it inside this. This is kept separate from "fs" directory so that we don't inadvertently overwrite our "downstream" directory.

- Read the SPIFFS image from ESP32 into our desktop's filesystem:
	>     py -2 esptool.py --chip esp32 --port [port] --baud [baud] read_flash -z [spiffs_partition_addr] [spiffs_partition_size] [spiffs_u_img_path]
	>     py -2 esptool.py --chip esp32 --port COM4 --baud 921600 read_flash -z 0x240000 0x1C0000 skeletor_spiffs_u.img
	*NOTE*: The partition address and size should match that in the partition table csv. Also, name this image with a \_u so that we dont overwrite our "downstream" image by mistake.
	
- Decode the SPIFFS image.

	We can list the contents of the spiffs image using:
	>     mkspiffs.exe -l [spiffs_image_path]
	>     mkspiffs.exe -l skeletor_spiffs_u.img
	
	We can also extract the contents into any directory and manipulate as the host sytem's filesystem:
	>     mkspiffs.exe -u [spiffs_root_dir] [spiffs_img_path] -b [block_size] -p [page_size]
	>     mkspiffs.exe -u fs_u skeletor_spiffs_u.img -b 4096 -p 256


<a name="eclipse-setup"/>

## use with Eclipse CDT

TBD


<a name="vscode-setup"/>

## use with VSCODE

- TBD the environment variables to be set
- depending on Makefile Build System or CMake Build System, choose one of the vscode settings directory :
	- vscode-makefile
	- vscode-cmake
- copy the selected directory and rename it to ".vscode". Windows Explorer/Shell does not allow this, use a bash-like console (Cygwin/git-bash/MinGW32 bash/Cmder ...)
- launch vscode and use "open folder" and select the project folder
- allow some time for Intellisense and Parsing to complete, takes a few minutes.


We can easily browse the entire project, components and esp-idf easily and quickly


Also look at : Recommended vscode extensions for programmers to get the most out of vscode.


<a name="maintenance"/>

## maintenance and keeping up with esp-idf/arduino-esp32 updates

TBD



