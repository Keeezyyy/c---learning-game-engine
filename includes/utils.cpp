
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <vector>

char *readFile(const char *filename)
{

    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        std::cout << "error";
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    // Speicher für Text + Nullterminator
    char *buffer = (char *)malloc(length + 1);
    if (!buffer)
    {
        fprintf(stderr, "Speicher konnte nicht reserviert werden\n");
        fclose(file);
        return NULL;
    }

    // Inhalt lesen
    size_t readSize = fread(buffer, 1, length, file);
    buffer[readSize] = '\0'; // Nullterminator anhängen

    fclose(file);
    return buffer;
}
