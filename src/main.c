#include <stdio.h>
#include <math.h>

#include "mandelbrot.h"


int main(void) {
    unsigned x_res = 128;
    unsigned y_res = 40;

    mandelbrot_calc_t calc = make_mandelbrot_calc(x_res, y_res);

    init_mandelbrot_calc(&calc, -.5f, 0.f, 3.f);
    run_mandelbrot_calc(&calc);

    char ascii_map[12] = " .:-=+*#%@@";
    for (unsigned y = 0; y < y_res; y++) {
        printf("|");
        for (unsigned x = 0; x < x_res; x++) {
            float intensity = get_mandelbrot_intensity(&calc, x, y);

            unsigned ascii_index = (unsigned) floor(10 * intensity);
            char ch = ascii_map[ascii_index];
            printf("%c", ch);
        }
        printf("|\n");
    }

    destroy_mandelbrot_calc(&calc);

    return 0;
}
