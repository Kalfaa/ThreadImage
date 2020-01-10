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


void apply_effect(Image* original, Image* new_i, enum EffectType type);
void apply_effect(Image* original, Image* new_i,enum EffectType type) {
    float KERNEL[DIM][DIM] ;
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

			for(int a = 0; a < LENGHT; a++){
				for(int b = 0; b < LENGHT; b++){
					int xn = x + a - OFFSET;
					int yn = y + b - OFFSET;

					Pixel* p = &original->pixel_data[yn][xn];

					c.Red += ((float) p->r) * KERNEL[a][b];
					c.Green += ((float) p->g) * KERNEL[a][b];
					c.Blue += ((float) p->b) * KERNEL[a][b];
				}
			}

			Pixel* dest = &new_i->pixel_data[y][x];
			dest->r = (uint8_t)  (c.Red <= 0 ? 0 : c.Red >= 255 ? 255 : c.Red);
			dest->g = (uint8_t) (c.Green <= 0 ? 0 : c.Green >= 255 ? 255 : c.Green);
			dest->b = (uint8_t) (c.Blue <= 0 ? 0 : c.Blue >= 255 ? 255 : c.Blue);
		}
	}
}
/*
int main(int argc, char** argv) {

	Image img = open_bitmap("bmp_tank.bmp");
	Image new_i;
	apply_effect(&img, &new_i);
	save_bitmap(new_i, "test_out.bmp");
	return 0;
}*/