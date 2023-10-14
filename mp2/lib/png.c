#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "crc32.h"
#include "png.h"

const int ERROR_INVALID_PARAMS = 1;
const int ERROR_INVALID_FILE = 2;
const int ERROR_INVALID_CHUNK_DATA = 3;
const int ERROR_NO_UIUC_CHUNK = 4;


/**
 * Opens a PNG file for reading (mode == "r" or mode == "r+") or writing (mode == "w").
 * 
 * (Note: The function follows the same function prototype as `fopen`.)
 * 
 * When the file is opened for reading this function must verify the PNG signature.  When opened for
 * writing, the file should write the PNG signature.
 * 
 * This function must return NULL on any errors; otherwise, return a new PNG struct for use
 * with further fuctions in this library.
 */
PNG * PNG_open(const char *filename, const char *mode) {
  char signature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
  PNG *png = malloc(sizeof(PNG));
  FILE *fp = fopen(filename, mode);
  if (fp == NULL) {
    free(png);
    return NULL;
  }      
  png->file = fp;
  char fileContent[9];
  fread(fileContent, 1, 8, fp);
  fileContent[8] = '\0';
  if (strcmp(mode, "r") == 0 || strcmp(mode, "r+") == 0) {
    if (strncmp(fileContent, signature, 8) != 0) {
      fclose(fp);
      free(png);
      return NULL;
    }
    png->status = strdup(mode);
  } else if (strcmp(mode, "w") == 0) {
    fwrite(signature, 1, sizeof(signature), fp);
    png->status = strdup(mode);
  }
  return png;

}
/**
 * Reads the next PNG chunk from `png`.
 * 
 * If a chunk exists, a the data in the chunk is populated in `chunk` and the
 * number of bytes read (the length of the chunk in the file) is returned.
 * Otherwise, a zero value is returned.
 * 
 * Any memory allocated within `chunk` must be freed in `PNG_free_chunk`.
 * Users of the library must call `PNG_free_chunk` on all returned chunks.
 */
size_t PNG_read(PNG *png, PNG_Chunk *chunk) {
  if (png->file == NULL || chunk == NULL) {
    return 0;
  }
  //len
  uint32_t length;
  fread(&length, 1, 4, png->file);
  chunk->len = ntohl(length);
  //type
 fread(chunk->type, 1, 4, png->file);
  chunk->type[4] = '\0';
  //data
   chunk->data = malloc(chunk->len);
    fread(chunk->data, 1, chunk->len, png->file);
    free(chunk->data);  ew2q1`
  //crc
  uint32_t length1;
  fread(&length1, 1, 4, png->file);
  chunk->crc = ntohl(length1);
  return chunk->len + 12;
}


/**
 * Writes a PNG chunk to `png`.
 * 
 * Returns the number of bytes written. 
 */
size_t PNG_write(PNG *png, PNG_Chunk *chunk) {
   if (png->file == NULL || chunk == NULL) {
    return 0;
  }
size_t bytesWritten = 0;
    //len
    uint32_t lengthNetworkOrder = htonl(chunk->len);
    fwrite(&lengthNetworkOrder, sizeof(uint32_t), 1, png->file);
    bytesWritten += sizeof(uint32_t);
    //type
    fwrite(chunk->type, sizeof(char), 4, png->file);
    bytesWritten += 4;
    //data
    fwrite(chunk->data, sizeof(unsigned char), chunk->len, png->file);
    bytesWritten += chunk->len;
    //CRC
    u_int32_t chunk_crc = 0;
    unsigned char *temp_buffer = calloc(4 + chunk->len, sizeof(unsigned char));
    memcpy(temp_buffer, chunk->type, 4);
    memcpy(temp_buffer + 4, chunk->data, chunk->len);
    crc32(temp_buffer,4 + chunk->len ,&chunk_crc);
    uint32_t crcNetworkOrder = htonl(chunk_crc);
    fwrite(&crcNetworkOrder, sizeof(uint32_t), 1, png->file);
    bytesWritten += sizeof(uint32_t);
    free(temp_buffer);
    return bytesWritten;
}
  



/**
 * Frees all memory allocated by this library related to `chunk`.
 */
void PNG_free_chunk(PNG_Chunk *chunk) {
  if (chunk) {
    chunk->crc = 0;
  chunk->len = 0;
  for (int i = 0; i < 4; i++) {
    chunk->type[i] = '\0';
  }
  }
}


/**
 * Closes the PNG file and frees all memory related to `png`.
 */
void PNG_close(PNG *png) {
 if (png) {
        if (png->file) {
            fclose(png->file);
        }
        if (png->status) {
            free(png->status);
        }
        free(png);
    }
}