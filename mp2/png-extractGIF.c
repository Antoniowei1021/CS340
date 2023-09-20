#include "png-extractGIF.h"

int png_extractGIF(const char *png_filename, const char *gif_filename) {
  int uiuc_count = 0;
  PNG *png = PNG_open(png_filename, "r");
  if (!png) { return ERROR_INVALID_FILE; } 
  FILE *gif_file = fopen(gif_filename, "w");
  if (!gif_filename) {
     return ERROR_INVALID_CHUNK_DATA;
  }
  while (1) {
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0) {
      PNG_close(png);
      return ERROR_INVALID_CHUNK_DATA;
    }
   if(strcmp(chunk.type, "uiuc") == 0) {
      fwrite(chunk.data, 1, chunk.len, gif_file);
      uiuc_count++;
    }
    // Check for the "IEND" chunk to exit:
    if ( strcmp(chunk.type, "IEND") == 0 ) {
      PNG_free_chunk(&chunk);
      break;  
    }
    // Free the memory associated with the chunk we just read:
    PNG_free_chunk(&chunk);
  }
   if (uiuc_count == 0){
     PNG_close(png);
    fclose(gif_file);
      return 255;
    }
  PNG_close(png);
  fclose(gif_file);
  return 0; // Change the to a zero to indicate success, when your implementaiton is complete.
}
