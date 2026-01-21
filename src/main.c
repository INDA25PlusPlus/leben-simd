#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "benchmark/benchmark.h"
#include "benchmark/clock.h"
#include "benchmark/stats.h"
#include "img/stb_image_write.h"
#include "mandelbrot/mandelbrot.h"


unsigned to_int(char *str);

float to_float(char *str);

void write_img(void *file_handle, void *data, int size);

int benchmark_program(int argc, char *argv[]);

int disp_program(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--bench") == 0) {
        return benchmark_program(argc, argv);
    } else {
        return disp_program(argc, argv);
    }
}

void print_usage_info() {
    printf(
        "Usage:\n"
        "exe [<x_res> <y_res> <x_pos> <y_pos> <view_height> <max_iterations>] [<out_path> [-f]]\n"
        "exe --bench <batch_count> <batch_size> <x_res> <y_res> <x_pos> <y_pos> <view_height> <max_iterations>\n"
    );
}

int benchmark_program(int argc, char *argv[]) {
    if (argc != 10) {
        print_usage_info();
        exit(EXIT_FAILURE);
    }

    unsigned batch_count = to_int(argv[2]);
    unsigned batch_size = to_int(argv[3]);

    unsigned x_res = to_int(argv[4]);
    if (x_res % 8 != 0) {
        printf("x_res must be multiple of 8!\n");
        exit(EXIT_FAILURE);
    }
    unsigned y_res = to_int(argv[5]);

    float x_pos = to_float(argv[6]);
    float y_pos = to_float(argv[7]);

    float view_height = to_float(argv[8]);
    unsigned max_iterations = to_int(argv[9]);

    benchmark_params_t params = {
        batch_count, batch_size,
        x_res, y_res,
        x_pos, y_pos, view_height,
        max_iterations
    };

    printf(
        "Benchmarking %u batches of %u runs...\n",
        batch_count, batch_size);

    benchmark_results_t results = benchmark(params, true);

    float sisd_average = average_ns(results.sisd_times, batch_count, batch_size);
    float simd_average = average_ns(results.simd_times, batch_count, batch_size);
    float sisd_stddev = stddev_ns(results.sisd_times, batch_count, sisd_average, batch_size);
    float simd_stddev = stddev_ns(results.simd_times, batch_count, simd_average, batch_size);

    printf(
        "Results:\n"
        "SISD average: %uns\n"
        "SISD stddev:  %uns\n"
        "SIMD average: %uns\n"
        "SIMD stddev:  %uns\n",
        (unsigned) sisd_average, (unsigned) sisd_stddev,
        (unsigned) simd_average, (unsigned) simd_stddev);

    destroy_benchmark_results(&results);

    return EXIT_SUCCESS;
}

int disp_program(int argc, char *argv[]) {
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
        print_usage_info();
        exit(EXIT_FAILURE);
    }

    mandelbrot_calc_t calc = make_mandelbrot_calc(x_res, y_res);

    init_mandelbrot_calc(&calc, x_pos, y_pos, view_height, max_iterations, pixel_aspect_ratio);

    start_timer();
    run_mandelbrot_calc_simd(&calc, max_iterations);
    timespec_t duration = stop_timer();

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
        if (out_img == NULL) {
            printf("Failed to write to file '%s'\n", out_path);
            exit(EXIT_FAILURE);
        }

        char *buffer = malloc(sizeof(char) * calc.x_res * calc.y_res);
        for (unsigned y = 0; y < y_res; y++) {
            for (unsigned x = 0; x < x_res; x++) {
                unsigned index = y * x_res + x;
                buffer[index] = (char) (255 * get_mandelbrot_intensity(&calc, x, y, max_iterations));
            }
        }

        stbi_write_png_to_func(&write_img, out_img, x_res, y_res, 1, buffer, 0);

        fclose(out_img);
        free(buffer);
    }

    printf("Duration: %lus, %luns\n", duration.tv_sec, duration.tv_nsec);

    destroy_mandelbrot_calc(&calc);

    return EXIT_SUCCESS;
}

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
