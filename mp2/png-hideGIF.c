#include "png-hideGIF.h" 

int png_hideGIF(const char *png_filename_source, const char *gif_filename, const char *png_filename_out) {
      // Open the input PNG file
    PNG *png = PNG_open(png_filename_source, "r");
    if (!png) { return ERROR_INVALID_FILE; }

    // Open the output PNG file
    PNG *out = PNG_open(png_filename_out, "w");
    if (!out) { return ERROR_INVALID_FILE; }

    // Open the GIF file
    FILE *gif_file = fopen(gif_filename, "rb");
    if (!gif_file) { return ERROR_INVALID_FILE; }

    printf("PNG Header written.\n");

    // Variable to check if custom chunk is written
    int custom_chunk_written = 0;

    // Read chunks until reaching "IEND" or an invalid chunk
    while (1) {
        // Read a chunk from the input PNG
        PNG_Chunk chunk;
        if (PNG_read(png, &chunk) == 0) {
            PNG_close(png);
            PNG_close(out);
            fclose(gif_file);
            return ERROR_INVALID_CHUNK_DATA;
        }

        // Write the chunk to the output PNG
        PNG_write(out, &chunk);
        printf("PNG chunk %s written\n", chunk.type);

        // After IHDR and before IEND, insert the custom 'uiuc' chunk
        if (!custom_chunk_written && strcmp(chunk.type, "IHDR") == 0) {
            // Read the entire GIF into memory
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

        // Free the chunk data
        PNG_free_chunk(&chunk);
    }

    // Close all files and cleanup
    fclose(gif_file);
    PNG_close(out);
    PNG_close(png);

    return 0;
}
