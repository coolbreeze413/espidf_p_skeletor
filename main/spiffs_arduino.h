/*
 * spiffs_helper.h
 *
 *  Created on: Mar 14, 2018
 *      Author: krish
 */

#include "skeletor_config.h"

#ifdef SKELETOR_FEATURE_SPIFFSARDUINO

#ifndef MAIN_SPIFFS_HELPER_H_
#define MAIN_SPIFFS_HELPER_H_

#include "FS.h"
#include <SPIFFS.h>


// TODO: define fs helper functions such as:
// ls - list content of dir
// cat - list content of file
// cd - change to dir -> support .. and \ as well
// mkdir - create dir
// rmdir - remove dir and files inside it
// del - delete a file
// pwd - display current dir
// info - display filesystem detailed info

// we can call these functions from a CLI shell, our ntshell for example, and expose as commands.
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void renameFile(fs::FS &fs, const char * path1, const char * path2);
void deleteFile(fs::FS &fs, const char * path);
void testFileIO(fs::FS &fs, const char * path);


#endif /* MAIN_SPIFFS_HELPER_H_ */

#endif // #ifdef SKELETOR_FEATURE_SPIFFSARDUINO