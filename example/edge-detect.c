/*
	//gcc edge-detect.c bitmap.c -O2 -ftree-vectorize -fopt-info -mavx2 -fopt-info-vec-all
	//UTILISER UNIQUEMENT DES BMP 24bits
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../lib/bitmap.h"
#include <stdint.h>

#define DIM 3
#define LENGHT DIM
#define OFFSET DIM /2

const float KERNELEDGE[DIM][DIM] = {{-1, -1,-1},
							   {-1,8,-1},
							   {-1,-1,-1}};

const float KERNELBOXBLUR[DIM][DIM] = {{0.11,0.11,0.11},
                                       {0.11,0.11,0.11},
                                       {0.11,0.11,0.11}};
const float KERNELSHARPEN[DIM][DIM] = {{0, -1,0},
                                       {-1,5,-1},
                                       {0,-1,0}};

float KERNEL[DIM][DIM] ;

enum EffectType {
   EDGE,
   BOXBLUR,
   SHARPEN
};

typedef struct Color_t {
	float Red;
	float Green;
	float Blue;
} Color_e;

void inline apply_convolution(Color_e* c, int a, int b, int x, int y, Image* img) __attribute__((always_inline));
void apply_convolution(Color_e* restrict c, int a, int b, int x, int y, Image* restrict img) {
    int xn = x + a - OFFSET;
    int yn = y + b - OFFSET;

    Pixel* p = &img->pixel_data[yn][xn];

    c->Red += ((float) p->r) * KERNEL[a][b];
    c->Green += ((float) p->g) * KERNEL[a][b];
    c->Blue += ((float) p->b) * KERNEL[a][b];
}

void apply_effect(Image* original, Image* new_i, enum EffectType type);
void apply_effect(Image* original, Image* new_i, enum EffectType type) {
    switch(type){
        case SHARPEN:
            memcpy(KERNEL, KERNELSHARPEN, sizeof(KERNEL));
            break;
        case BOXBLUR:
            memcpy(KERNEL, KERNELBOXBLUR, sizeof(KERNEL));
            break;
        case EDGE:
            memcpy(KERNEL, KERNELEDGE, sizeof(KERNEL));
            break;
        default:
            break;
    }
    int w = original->bmp_header.width;
    int h = original->bmp_header.height;

    *new_i = new_image(w, h, original->bmp_header.bit_per_pixel, original->bmp_header.color_planes);

    for (int y = OFFSET; y < h - OFFSET; y++) {
        for (int x = OFFSET; x < w - OFFSET; x++) {
            Color_e c = { .Red = 0, .Green = 0, .Blue = 0};

            apply_convolution(&c, 0, 0, x, y, original);
            apply_convolution(&c, 0, 1, x, y, original);
            apply_convolution(&c, 0, 2, x, y, original);

            apply_convolution(&c, 1, 0, x, y, original);
            apply_convolution(&c, 1, 1, x, y, original);
            apply_convolution(&c, 1, 2, x, y, original);

            apply_convolution(&c, 2, 0, x, y, original);
            apply_convolution(&c, 2, 1, x, y, original);
            apply_convolution(&c, 2, 2, x, y, original);

            Pixel* dest = &new_i->pixel_data[y][x];
            dest->r = (uint8_t)  (c.Red <= 0 ? 0 : c.Red >= 255 ? 255 : c.Red);
            dest->g = (uint8_t) (c.Green <= 0 ? 0 : c.Green >= 255 ? 255 : c.Green);
            dest->b = (uint8_t) (c.Blue <= 0 ? 0 : c.Blue >= 255 ? 255 : c.Blue);
        }
    }

}


