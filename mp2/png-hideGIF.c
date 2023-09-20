#include "png-hideGIF.h" 

int png_hideGIF(const char *png_filename_source, const char *gif_filename, const char *png_filename_out) {
    PNG *png = PNG_open(png_filename_source, "r");
    if (!png) { return ERROR_INVALID_FILE; }
    PNG *out = PNG_open(png_filename_out, "w");
    if (!out) { return ERROR_INVALID_FILE; }
    FILE *gif_file = fopen(gif_filename, "rb");
    if (!gif_file) { return ERROR_INVALID_FILE; }
    printf("PNG Header written.\n");
    int custom_chunk_written = 0;
    while (1) {
        PNG_Chunk chunk;
        if (PNG_read(png, &chunk) == 0) {
            PNG_close(png);
            PNG_close(out);
            fclose(gif_file);
            return ERROR_INVALID_CHUNK_DATA;
        }
        PNG_write(out, &chunk);
        printf("PNG chunk %s written\n", chunk.type);
        if (!custom_chunk_written && strcmp(chunk.type, "IHDR") == 0) {
            fseek(gif_file, 0, SEEK_END);
            size_t gif_size = ftell(gif_file);
            rewind(gif_file);
            unsigned char *gif_data = malloc(gif_size);
            fread(gif_data, 1, gif_size, gif_file);
            // Create a 'uiuc' chunk
            PNG_Chunk uiuc_chunk;
            strcpy(uiuc_chunk.type, "uiuc");
            uiuc_chunk.len = gif_size;
            uiuc_chunk.data = gif_data;
            // Write the 'uiuc' chunk
            PNG_write(out, &uiuc_chunk);
            printf("Custom PNG chunk 'uiuc' written with GIF data\n");
            free(gif_data);
            custom_chunk_written = 1;
        }
        // Check for the "IEND" chunk to exit
        if (strcmp(chunk.type, "IEND") == 0) {
            PNG_free_chunk(&chunk);
            break;
        }
        PNG_free_chunk(&chunk);
    }
    fclose(gif_file);
    PNG_close(out);
    PNG_close(png);
    return 0;
}
