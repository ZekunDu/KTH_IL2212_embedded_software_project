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
#define WIDTH  16
#define HEIGHT 16

/* width and height of processed images */
#define WIDTH_NEW  14
#define HEIGHT_NEW 14

#define DEBUG 0

/* Global variables */
int flag_offset_1 = CORRECT_SOBEL_FLAG_ADDR; //!> offset of flag before processing
int flag_offset_2 = SOBEL_ASCII_FLAG_ADDR;   //!> offset of flag after processing
int mem_offset_1 = RESIZE_MAT_ADDR;         //!> offset of corrected image
int mem_offset_2 = SOBEL_MAT_ADDR;           //!> offset of image after sobel operator

extern void delay (int millisec);

//! A function writing image in shared memory.
/*!
  \param base pointer of an array containing pixels of image.
  \param block block index of memory.
*/
void mem_write_img(unsigned char *base, int block){
	unsigned char *shared = (unsigned char *) SHARED_ONCHIP_BASE; // image pointer to shared memory
	int i;

	shared += mem_offset_2 + (block << 11); // block selection

	for (i = 0; i < WIDTH_NEW * HEIGHT_NEW; i++){
		*shared++ = *base++; 	
	}
}

//! A function reading image from shared memory.
/*!
  \param base pointer of an array ready to hold pixels of image.
  \param block block index of memory.
*/
void mem_read_img(unsigned char *base, int block){
	unsigned char *shared = (unsigned char *) SHARED_ONCHIP_BASE; // image pointer to shared memory
	int i;

	shared += mem_offset_1 + (block << 11); // block selection

	for (i = 0; i < WIDTH * HEIGHT; i++){
		*base++ = *shared++; 	
	}
}

//! A function calculating gradient image by sobel operator.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \param g pointer of the array containing gradient values of original image.
*/
void sobel(int x, int y, unsigned char * origin_mat, unsigned char * g){
	int i, j = 0;
	unsigned char gx, gy; // horizontal and vertical gradient
	unsigned char temp[8]; // save corresponding pixels
	
	for (j = 0; j < y - 2; j++){
		for (i = 0; i < x - 2; i++){
			/* locality optimization */
			temp[0] = origin_mat[i + j * x];
			temp[3] = origin_mat[i + 1 + j * x];
			temp[5] = origin_mat[i + 2 + j * x];
			temp[1] = origin_mat[i + (j + 1) * x];
			temp[6] = origin_mat[i + 2 + (j + 1) * x];
			temp[2] = origin_mat[i + (j + 2) * x];
			temp[4] = origin_mat[i + 1 + (j + 2) * x];
			temp[7] = origin_mat[i + 2 + (j + 2) * x];
			
			/* calculate horizontal gradient by convolution */
			gx = - (temp[0] >> 2) - (temp[1] >> 1) - (temp[2] >> 2) + (temp[5] >> 2) + (temp[6] >> 1) + (temp[7] >> 2);
			/* calculate vertical gradient by convolution */
			gy = - (temp[0] >> 2) - (temp[3] >> 1) - (temp[5] >> 2) + (temp[2] >> 2) + (temp[4] >> 1) + (temp[7] >> 2);
			/* merge horizontal and vertical gradient */
			gx = abs(gx);
			gy = abs(gy);
			if (gx - gy < gy || gy - gx < gx) g[i + j * (x - 2)] = (gx >> 1) + (gy >> 1);
			else g[i + j * (x - 2)] = gx + gy;
		}
	}
}

int main(){
	/* flag pointer to shared memory */
	unsigned char *flag_1 = (unsigned char *) SHARED_ONCHIP_BASE;
	unsigned char *flag_2 = flag_1;
	unsigned char *flag_initial = flag_1 + INITIAL_FLAG_ADDR;
	
	int block = 0; // block index
	unsigned char origin_mat[WIDTH * HEIGHT];            // static array containing pixels of origin image
	unsigned char processed_mat[WIDTH_NEW * HEIGHT_NEW]; // static array containing pixels of processed image

	while (1){
		delay(1);
		if (*flag_initial == 0x0f) break; // wait until all flags are initialized to 0x0f
	}
	
	while (1){
		flag_1 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_1 + (block << 11); // block selection

		#if DEBUG == 1
		printf("\ncorrect-sobel flag: %d, %d, %d, %d-------Block%d\n", *((unsigned char *)SHARED_ONCHIP_BASE +2), *((unsigned char *)SHARED_ONCHIP_BASE +2 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +2 + 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +2 + 6144), block);		
		printf("sobel-ascii flag: %d, %d, %d, %d\n", *((unsigned char *)SHARED_ONCHIP_BASE +3), *((unsigned char *)SHARED_ONCHIP_BASE +3 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +3+ 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +3 + 6144));
		printf("flag_1 addr: %d,  %d\n",flag_1, (unsigned char *)SHARED_ONCHIP_BASE +2);
		#endif

		while (1){
			delay(1);
			if (*flag_1 == 0x00){ // 00000000 image is not processed but available
				*flag_1 = 0x01; // 00000001 image is being processed and not available
				break;
			}
		}
		mem_read_img(origin_mat, block); // read unprocessed image from memory
		*flag_1 = 0x02; // 00000010 image is processed and available
		
		#if DEBUG == 1
		printf("correct-sobel flag: %d, %d, %d, %d\n", *((unsigned char *)SHARED_ONCHIP_BASE +2), *((unsigned char *)SHARED_ONCHIP_BASE +2 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +2 + 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +2 + 6144));		
		#endif

		/* image process */
		sobel(WIDTH, HEIGHT, origin_mat, processed_mat); // sobel function
		
		flag_2 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_2 + (block << 11); // block selection

		while (1){
			delay(1);
			if (*flag_2 == 0x02){ // 00000010 image is processed and available
				*flag_2 = 0x03; // 00000011 image is being freshed and not available
				break;
			}
		}
		mem_write_img(processed_mat, block); // write processed image in memory
		*flag_2 = 0x00; // 00000000 image is not processed but available

		#if DEBUG == 1
		printf("sobel-ascii flag: %d, %d, %d, %d\n", *((unsigned char *)SHARED_ONCHIP_BASE +3), *((unsigned char *)SHARED_ONCHIP_BASE +3 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +3+ 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +3 + 6144));
		#endif
		
		block = (block + 1) % 4; // switch to next block
	}
  
	return 0;
}
