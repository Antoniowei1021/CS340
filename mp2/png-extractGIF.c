#include "png-extractGIF.h"
int png_extractGIF(const char *png_filename, const char *gif_filename) {
  int uiuc_count = 0;
  int valid_png = 0;
  PNG *png = PNG_open(png_filename, "r");
  if (!png) { return ERROR_INVALID_FILE; } 

  while (1) {
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      return ERROR_INVALID_CHUNK_DATA;
    }
    if (strcmp(chunk.type, "uiuc") == 0) {
      FILE *gif_file = fopen(gif_filename, "w");
      fwrite(chunk.data, 1, chunk.len, gif_file);
      uiuc_count++;
      fclose(gif_file);
    }
    if ( strcmp(chunk.type, "IEND") == 0 ) {
      valid_png = 1; 
      PNG_free_chunk(&chunk);
      break;  
    }
    PNG_free_chunk(&chunk);
  }

  PNG_close(png);

  if (uiuc_count == 0) {
    if (valid_png) {
      return 254; 
    }
    return 255;
  }

  return 0; 
}
