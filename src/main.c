#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "img/stb_image_write.h"
#include "mandelbrot/mandelbrot.h"


unsigned to_int(char *str) {
    char *endptr;
    long num = strtol(str, &endptr, 10);
    if (*endptr != '\0') {
        printf("Invalid integer number '%s'\n", str);
        exit(EXIT_FAILURE);
    }
    return (unsigned) num;
}

float to_float(char *str) {
    char *endptr;
    float num = strtof(str, &endptr);
    if (*endptr != '\0') {
        printf("Invalid floating number '%s'\n", str);
        exit(EXIT_FAILURE);
    }
    return num;
}

void write_img(void *file_handle, void *data, int size) {
    fwrite(data, 1, size, file_handle);
}

int main(int argc, char *argv[]) {
    unsigned x_res, y_res;
    float x_pos, y_pos, view_height;
    unsigned max_iterations;
    float pixel_aspect_ratio = 2;
    char *out_path = NULL;
    bool out_overwrite = false;

    if (argc == 1) {
        x_res = 128;
        y_res = 40;
        x_pos = -.5f;
        y_pos = 0.f;
        view_height = 3.f;
        max_iterations = 20;
    } else if (argc == 7 || argc == 8 || argc == 9) {
        x_res = to_int(argv[1]);
        if (x_res % 8 != 0) {
            printf("x_res must be multiple of 8!\n");
            exit(EXIT_FAILURE);
        }
        y_res = to_int(argv[2]);
        x_pos = to_float(argv[3]);
        y_pos = to_float(argv[4]);
        view_height = to_float(argv[5]);
        max_iterations = to_int(argv[6]);
        if (argc > 7) {
            out_path = argv[7];
            pixel_aspect_ratio = 1;
            if (argc == 9) {
                if (strcmp(argv[8], "-f") != 0) {
                    printf("Invalid flag '%s'\n", argv[8]);
                    exit(EXIT_FAILURE);
                }
                out_overwrite = true;
            }
        }
    } else {
        printf(
            "Usage:\n"
            "exe [<x_res> <y_res> <x_pos> <y_pos> <view_height> <max_iterations>] [<out_path> [-f]]\n"
        );
        exit(EXIT_FAILURE);
    }

    mandelbrot_calc_t calc = make_mandelbrot_calc(x_res, y_res);

    init_mandelbrot_calc(&calc, x_pos, y_pos, view_height, max_iterations, pixel_aspect_ratio);
    run_mandelbrot_calc(&calc, max_iterations);

    if (out_path == NULL) {
        char ascii_map[12] = " .:-=+*#%@@";
        for (unsigned y = 0; y < y_res; y++) {
            printf("|");
            for (unsigned x = 0; x < x_res; x++) {
                float intensity = get_mandelbrot_intensity(&calc, x, y, max_iterations);

                unsigned ascii_index = (unsigned) floor(10 * intensity);
                char ch = ascii_map[ascii_index];
                printf("%c", ch);
            }
            printf("|\n");
        }
    } else {
        FILE *out_img = fopen(
            out_path,
            out_overwrite ? "wb+" : "wb+x");
        char *buffer = malloc(sizeof(char) * calc.x_res * calc.y_res);
        for (unsigned y = 0; y < y_res; y++) {
            for (unsigned x = 0; x < x_res; x++) {
                unsigned index = y * x_res + x;
                buffer[index] = (char) (255 * get_mandelbrot_intensity(&calc, x, y, max_iterations));
            }
        }

        stbi_write_png_to_func(&write_img, (void *) out_img, x_res, y_res, 1, buffer, 0);

        fclose(out_img);
        free(buffer);
    }

    destroy_mandelbrot_calc(&calc);

    return 0;
}
