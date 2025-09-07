
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <vector>
#include <stb/stb_image.h>



#ifndef UTILS_H
#define UTILS_H

char *readFile(const char *filename);
u_char *readPng(const char *filename, int *out_width, int *out_height, int *out_colorCHannels);



#endif // UTILS_H