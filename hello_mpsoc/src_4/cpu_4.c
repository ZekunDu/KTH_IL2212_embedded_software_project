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
#define WIDTH  14
#define HEIGHT 14

#define DEBUG 0

/* Global variables */
int flag_offset_1 = SOBEL_ASCII_FLAG_ADDR; 		//!> offset of flag before processing
int flag_offset_2 = ASCII_GRAYSCALE_FLAG_ADDR; 	//!> offset of flag after processing
int mem_offset = SOBEL_MAT_ADDR;           		//!> offset of image after sobel operator
unsigned char ascii_gray[] = {' ', '.', ':', '-', '=', '+', '/', 't', 'z', 'U', 'w', '*', '0', '#', '%', '@'};

extern void delay(int millisec);

//! A function reading image from shared memory.
/*!
  \param base pointer of an array ready to hold pixels of image.
  \param block block index of memory.
*/
void mem_read_img(unsigned char *base, int block){
	unsigned char *shared = (unsigned char *) SHARED_ONCHIP_BASE; // image pointer to shared memory
	int i;

	shared += mem_offset + (block << 11);

	for (i = 0; i < WIDTH * HEIGHT; i++){
		*base++ = *shared++; 	
	}
}

void mem_write_img(unsigned char *base, int block){
	unsigned char *shared = (unsigned char *) SHARED_ONCHIP_BASE; // image pointer to shared memory
	int i;

	shared += mem_offset + (block << 11);

	for (i = 0; i < WIDTH * HEIGHT; i++){
		*shared++ = *base++; 	
	}
}

//! A function converting gradient values of an image into ascii characters.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing gradient values of image.
*/
void * ascii(int x, int y, unsigned char * origin_mat){
	int i, n;
	
	for (i = 0; i < x * y; i++){
		/* convert gradient into corresponding ascii character */
		n = (*origin_mat) >> 4; 
		*origin_mat++ = ascii_gray[n];
	}
}

//! A function printing ascii characters of an image.
/*!
  \param x width of input image.
  \param y height of input image.
  \param mat pointer of an array containing ascii characters of image.
*/
void print_img(int x, int y, unsigned char * mat){
	int i, j;
	
	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			printf("%c", *mat++);
		}
		printf("\n");
	}
}

//! A function initializing flags to 0x0f.
void initial_flag(){
	int i, *p;
	unsigned char *flag = (unsigned char *) SHARED_ONCHIP_BASE; // flag pointer to shared memory
	
	for (i = 0; i < 4; i++) {
		flag = (unsigned char *) SHARED_ONCHIP_BASE + (i << 11);
		p = flag;
		*flag++ = 0x02;  // grayscale-resize flag
		*flag++ = 0x02;  // resize-correct flag
		*flag++ = 0x02;  // correct-sobel flag
		*flag++ = 0x02;  // sobel-ascii flag
		*flag++ = 0x00;  // ascii-grayscale flag
		printf("%X  %X\n", *p, *(p+1));
	}
	flag = (unsigned char *) SHARED_ONCHIP_BASE + INITIAL_FLAG_ADDR;
	*flag = 0x0f;    // initial flag
}

int main(){
	/* flag pointer to shared memory */
	unsigned char *flag_1 = (unsigned char *) SHARED_ONCHIP_BASE;
	unsigned char *flag_2;
	unsigned char *flag_initial = flag_1 + INITIAL_FLAG_ADDR;
	
	int block = 0; // block index
	unsigned char origin_mat[WIDTH * HEIGHT]; // static array containing pixels of origin image
	
	initial_flag(); // initial function
	
	while (1){
		delay(1);
		if (*flag_initial == 0x0f) break; // wait until all flags are initialized to 0x0f
	}

	while (1){
		flag_1 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_1 + (block << 11); // block selection
		flag_2 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_2 + (block << 11); // block selection

		#if DEBUG == 1
		printf("\nsobel-ascii flag: %d, %d, %d, %d------Block%d\n", *((unsigned char *)SHARED_ONCHIP_BASE +3), *((unsigned char *)SHARED_ONCHIP_BASE +3 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +3+ 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +3 + 6144), block);
		printf("flag_1 addr: %d,  %d\n",flag_1, (unsigned char *)SHARED_ONCHIP_BASE +3);
		printf("flag_2 addr: %d,  %d\n",flag_2, (unsigned char *)SHARED_ONCHIP_BASE +4);		
		#endif


		while (1){
			delay(1);
			if (*flag_1 == 0x00){ // 00000000 image is not processed but available
				*flag_1 = 0x01; // 00000001 image is being processed and not available
				break;
			}
		}

		while (1){
			delay(1);
			if (*flag_2 == 0x02){
				*flag_2 = 0x03;
				break;
			}
		}

		mem_read_img(origin_mat, block); // read unprocessed image from memory
		*flag_1 = 0x02; // 00000010 image is processed and available
		

		/* image process */
		ascii(WIDTH, HEIGHT, origin_mat); // ascii function

		/* image save to shared memory */
		mem_write_img(origin_mat, block);  // write ascii 
		*flag_2 = 0x00;
		
		/* print current ascii image */
		print_img(WIDTH, HEIGHT, origin_mat);

		
		block = (block + 1) % 4; // switch to next block
	}
  
	return 0;
}
