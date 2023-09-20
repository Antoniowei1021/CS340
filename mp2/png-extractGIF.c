#include "png-extractGIF.h"

int png_extractGIF(const char *png_filename, const char *gif_filename) {
     // Open the PNG file for reading:
     int count = 0;
    PNG *png = PNG_open(png_filename, "r");
    if (!png) { 
        return ERROR_INVALID_FILE; 
    }
  
    FILE *gif = fopen(gif_filename, "wb");
    if (!gif) { 
        PNG_close(png);
        return ERROR_INVALID_FILE; 
    }
  
    // Read chunks until reaching "IEND" or an invalid chunk:
    while (1) {
        PNG_Chunk chunk;
        if (PNG_read(png, &chunk) == 0) {
            fclose(gif);
            PNG_close(png);
            return ERROR_INVALID_CHUNK_DATA;
        }

        // If this is the "uiuc" chunk, write its data to the GIF file:
        if (strcmp(chunk.type, "uiuc") == 0) {
            if (fwrite(chunk.data, 1, chunk.len, gif) != chunk.len) {
                PNG_free_chunk(&chunk);
                fclose(gif);
                PNG_close(png);
                return ERROR_INVALID_CHUNK_DATA;
            }
            count++;
        }

        // Check for the "IEND" chunk to exit:
        if (strcmp(chunk.type, "IEND") == 0) {
            PNG_free_chunk(&chunk);
            break;
        }
        // Free the memory associated with the chunk:
        PNG_free_chunk(&chunk);
    }
        if (count == 0) {
                fclose(gif);
                PNG_close(png);
                return 255;
        }
    fclose(gif);
    PNG_close(png);
    return 0;
}
