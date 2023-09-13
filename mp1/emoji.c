#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Return your favorite emoji.  Do not allocate new memory.
// (This should **really** be your favorite emoji, we plan to use this later in the semester. :))
char *emoji_favorite() {
  return "\xF0\x9F\x98\x85";
}

// Count the number of emoji in the UTF-8 string `utf8str`, returning the count.  You should
// consider everything in the ranges starting from (and including) U+1F000 up to (and including) U+1FAFF.
int emoji_count(const unsigned char *utf8str) {
  int count = 0;
  int i = 0;
      while ( utf8str[i] != '\0') {
    if (utf8str[i] == 0xF0  && utf8str[i + 1] == 0x9F && utf8str[i + 2] >= 0x80 && utf8str[i + 2] <= 0xAF && utf8str[i + 3] >= 0X80 && utf8str[i + 3] <= 0xFF) {
            count++;
            i += 4;
    } else {
      i++;
    }
}
 return count;
}
// Return a random emoji stored in new heap memory you have allocated.  Make sure what
// you return is a valid C-string that contains only one random emoji.
char *emoji_random_alloc() {
  char * result = calloc(5, sizeof(char));
    result[0] = 0xF0;
    result[1] = 0x9F;
    result[2] = 0x90 + (rand() % (0x9F - 0x90 + 1));
    result[3] = 0x80 + (rand() % (0xBF - 0x80 + 1));  
    result[4] = '\0';
  return result;
}
// Modify the UTF-8 string `utf8str` to invert the FIRST character (which may be up to 4 bytes)
// in the string if it the first character is an emoji.  At a minimum:
// - Invert "😊" U+1F60A ("\xF0\x9F\x98\x8A") into ANY non-smiling face.
// - Choose at least five more emoji to invert.
void emoji_invertChar(unsigned char *str) {
    if (strncmp(str, "\xF0\x9F\x98\x8A", 4) == 0) { 
        strncpy(str, "\xF0\x9F\x98\x9E", 4); 
    } else if (strncmp(str, "\xF0\x9F\x98\x81", 4) == 0) { 
        strncpy(str, "\xF0\x9F\x98\x94", 4); 
    } else if (strncmp(str, "\xF0\x9F\x98\x82", 4) == 0) { 
        strncpy(str, "\xF0\x9F\x98\xA9", 4); 
    } else if (strncmp(str, "\xF0\x9F\x91\x8D", 4) == 0) { 
        strncpy(str, "\xF0\x9F\x91\x8E", 4); 
    } else if (strncmp(str, "\xF0\x9F\x92\xA9", 4) == 0) {
        strncpy(str, "\xF0\x9F\x8E\x80", 4); 
    } else if (strncmp(str, "\xF0\x9F\x98\x8D", 4) == 0) { 
        strncpy(str, "\xF0\x9F\x98\xA0", 4); 
    }
}


// Modify the UTF-8 string `utf8str` to invert ALL of the character by calling your
// `emoji_invertChar` function on each character.
void emoji_invertAll(unsigned char *utf8str) {
 int len = strlen(utf8str);
    for (int i = 0; i < len - 3; i++) {
        emoji_invertChar(utf8str + i);
    }
}


// Reads the full contents of the file `fileName, inverts all emojis, and
// returns a newly allocated string with the inverted file's content.
unsigned char *emoji_invertFile_alloc(const char *fileName) {
  FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        return NULL;
    }
    // Get the file size
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    // Allocate memory for file content and null-terminator
    unsigned char *fileContent = (unsigned char *)calloc(fileSize + 1, sizeof(unsigned char));
    if (fileContent == NULL) {
        fclose(fp);
        return NULL;
    }
    // Read file into memory
    fread(fileContent, 1, fileSize, fp);
    fileContent[fileSize] = '\0';
    // Close the file
    fclose(fp);
    emoji_invertAll(fileContent);
    return fileContent;
}
