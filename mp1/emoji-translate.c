#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emoji.h"
#include "emoji-translate.h"

int i = 0;
void emoji_init(emoji_t *emoji) {
emoji->size = 0;
emoji->capacity = 10000;
emoji->rules = (EmojiRules*) calloc(emoji->capacity * sizeof(EmojiRules), sizeof(EmojiRules));
}
void emoji_add_translation(emoji_t *emoji, const unsigned char *source, const unsigned char *translation) {
    for (int i = 0; i < emoji->size; i++) {
        if (strcmp((char *)emoji->rules[i].source, (char *)source) == 0) {
            free(emoji->rules[i].translation);
            emoji->rules[i].translation = (unsigned char *)strdup((char *)translation);
            return;
        }
    }
    if (emoji->size >= emoji->capacity) {
        emoji->capacity *= 2;
        emoji->rules = (EmojiRules *)realloc(emoji->rules, emoji->capacity * sizeof(EmojiRules));
    }
    emoji->rules[emoji->size].source = (unsigned char *)strdup((char *)source);
    emoji->rules[emoji->size].translation = (unsigned char *)strdup((char *)translation);
    emoji->size++;
} 

// Translates the emojis contained in the file `fileName`.
const unsigned char *emoji_translate_file_alloc(emoji_t *emoji, const char *fileName) {
  FILE *file = fopen(fileName, "r");
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char *content = malloc(fsize + 1);
    fread(content, 1, fsize, file);
    fclose(file);
    content[fsize] = 0;
    unsigned char *result = malloc(fsize * 4 + 1); 
    int resultIndex = 0;
    for (int i = 0; i < fsize;) {
        int longestMatch = 0;
        int longestMatchIndex = -1;
        for (int j = 0; j < emoji->size; ++j) {
            int len = strlen((char *)emoji->rules[j].source);
            if (strncmp((char *)(content + i), (char *)emoji->rules[j].source, len) == 0) {
                if (len > longestMatch) {
                    longestMatch = len;
                    longestMatchIndex = j;
                }
            }
        }
        if (longestMatchIndex != -1) {
            strcpy((char *)(result + resultIndex), (char *)emoji->rules[longestMatchIndex].translation);
            resultIndex += strlen((char *)emoji->rules[longestMatchIndex].translation);
            i += longestMatch;
        } else {
            result[resultIndex++] = content[i++];
        }
    }
    result[resultIndex] = 0;
    free(content);
    return result; 
}
void emoji_destroy(emoji_t *emoji) {
  for (int i = 0; i < emoji->size; i++) {
    free(emoji->rules[i].source);
    free(emoji->rules[i].translation);
  }
  free(emoji->rules);
  emoji->capacity = 0;
  emoji->size= 0;
}
