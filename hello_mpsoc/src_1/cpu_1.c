#include <stdio.h>
#include "system.h"

/* address offsets of flags in shared memory */
#define GRAYSCALE_RESIZE_FLAG_ADDR 0
#define RESIZE_CORRECT_FLAG_ADDR   1
#define CORRECT_SOBEL_FLAG_ADDR    2
#define SOBEL_ASCII_FLAG_ADDR      3
#define ASCII_GRAYSCALE_FLAG_ADDR  4
#define INITIAL_FLAG_ADDR		   5

/* address offsets of images in shared memory */
#define GRAYSCALE_MAT_ADDR    8
#define RESIZE_MAT_ADDR       8 // 8 
#define SOBEL_MAT_ADDR    264 // 8 + 256

/* width and height of origin images */
#define WIDTH  32
#define HEIGHT 32

/* width and height of processed images */
#define WIDTH_NEW  16
#define HEIGHT_NEW 16

#define DEBUG 0

/* Global variables */
int flag_offset_1 = GRAYSCALE_RESIZE_FLAG_ADDR; //!> offset of flag before processing
int flag_offset_2 = RESIZE_CORRECT_FLAG_ADDR;   //!> offset of flag after processing
int mem_offset_1 = GRAYSCALE_MAT_ADDR;          //!> offset of grayscale image
int mem_offset_2 = RESIZE_MAT_ADDR;             //!> offset of resized image

extern void delay (int millisec);

int main(){
	/* flag pointer to shared memory */
	int block = 0, i, j, origin_height, origin_width; // block index
	unsigned char *flag_1 = (unsigned char *) SHARED_ONCHIP_BASE;
	unsigned char *flag_2 = flag_1;
	unsigned char *flag_initial = flag_1 + INITIAL_FLAG_ADDR;
	unsigned char *origin_mat_row[HEIGHT];
	unsigned char *origin_mat, *processed_mat;
	
	//unsigned char origin_mat[WIDTH * HEIGHT];            // static array containing pixels of origin image
	//unsigned char processed_mat[WIDTH_NEW * HEIGHT_NEW]; // static array containing pixels of processed image
	
	while (1){
		delay(1);
		if (*flag_initial == 0x0f) break; // wait until all flags are initialized to 0x0f
	}
	
	while (1){
		origin_mat = (unsigned char *) SHARED_ONCHIP_BASE + mem_offset_1 + (block << 11);
		processed_mat = (unsigned char *) SHARED_ONCHIP_BASE + mem_offset_2 + (block << 11);
		for (i = 0; i < HEIGHT; i++){
			origin_mat_row[i] = origin_mat + i * WIDTH;
		}

		#if DEBUG == 1
		printf("\ngray-resize flag: %d, %d, %d, %d -------Block%d\n", *((unsigned char *)SHARED_ONCHIP_BASE), *((unsigned char *)SHARED_ONCHIP_BASE + 2048), *((unsigned char *)SHARED_ONCHIP_BASE + 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE + 6144), block);
		printf("resize-correct flag: %d, %d, %d, %d \n", *((unsigned char *)SHARED_ONCHIP_BASE +1), *((unsigned char *)SHARED_ONCHIP_BASE +1 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +1+ 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +1 + 6144));
		printf("flag_1 addr: %d,  %d\n",flag_1, (unsigned char *)SHARED_ONCHIP_BASE);
		#endif

		flag_1 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_1 + (block << 11); // block selection
		while (1){
			delay(1);
			if (*flag_1 == 0x00){ // 00000000 image is not processed but available
				*flag_1 = 0x01; // 00000001 image is being processed and not available
				break;
			}
		}

		#if DEBUG == 1
		printf("gray-resize flag: %d, %d, %d, %d\n", *(unsigned char *)SHARED_ONCHIP_BASE, *((unsigned char *)SHARED_ONCHIP_BASE + 2048), *((unsigned char *)SHARED_ONCHIP_BASE + 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE + 6144));
		#endif

		/* image process */
		flag_2 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_2 + (block << 11); // block selection
		while (1){
			delay(1);
			if (*flag_2 == 0x02){ // 00000010 image is processed and available
				*flag_2 = 0x03; // 00000011 image is being freshed and not available
				break;
			}
		}
		
		for (j = 0; j < HEIGHT_NEW; j++){
			origin_height = j << 1;
			for (i = 0; i < WIDTH_NEW; i++){
				/* get new grayscale from average of corresponding four pixels */
				origin_width = i << 1;
				*processed_mat++ = (origin_mat_row[origin_height][origin_width] >> 2) + (origin_mat_row[origin_height][origin_width + 1] >> 2)
					+ (origin_mat_row[origin_height + 1][origin_width] >> 2) + (origin_mat_row[origin_height + 1][origin_width + 1] >> 2);
			}
		}

		//mem_write_img(processed_mat, block); // write processed image in memory
		*flag_1 = 0x02; // 00000010 image is processed and available
		*flag_2 = 0x00; // 00000000 image is not processed but available

		#if DEBUG == 1
		printf("resize-correct flag: %d, %d, %d, %d\n", *(unsigned char *)SHARED_ONCHIP_BASE +1, *((unsigned char *)SHARED_ONCHIP_BASE +1 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +1+ 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +1 + 6144));
		#endif

		
		block = (block + 1) % 4; // switch to next block
	}
  
	return 0;
}
