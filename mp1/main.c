#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"
#include "emoji-translate.h"

int main() {
emoji_t emoji;
  emoji_init(&emoji);

  emoji_add_translation(&emoji, (const unsigned char *) "🧡", (const unsigned char *) "heart");
  emoji_add_translation(&emoji, (const unsigned char *) "💙", (const unsigned char *) "&heart");
  unsigned char *translation = (unsigned char *) emoji_translate_file_alloc(&emoji, "tests/txt/simple.txt");
  printf("%s", emoji.rules[0].source);
  printf("%s", emoji.rules[0].translation);
  emoji_destroy(&emoji);
}


