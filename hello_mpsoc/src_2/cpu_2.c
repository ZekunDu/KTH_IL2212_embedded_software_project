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
#define CORRECT_MAT_ADDR    264 // 8 + 256
#define SOBEL_MAT_ADDR      520 // 264 + 256

/* width and height of origin images */
#define WIDTH  16
#define HEIGHT 16

#define DEBUG 0

/* Global variables */
int flag_offset_1 = RESIZE_CORRECT_FLAG_ADDR; //!> offset of flag before processing
int flag_offset_2 = CORRECT_SOBEL_FLAG_ADDR;  //!> offset of flag after processing
int mem_offset = RESIZE_MAT_ADDR;             //!> offset of resized image

extern void delay(int millisec);

int main(){
	/* flag pointer to shared memory */
	unsigned char *flag_1 = (unsigned char *) SHARED_ONCHIP_BASE;
	unsigned char *flag_2;
	unsigned char *flag_initial = flag_1 + INITIAL_FLAG_ADDR;
	
	int i, ave;
	int block = 0; // block index
	unsigned char enhance_index;
	unsigned char *image_mat;
	unsigned char h_bright[2];                // minimum and maximum grayscale, hmin and hmax
	unsigned char control_signal;             // control signal for correction
	int state_sig[3] = {255, 255, 255};        // initial state signal (moore)
	
	while (1){
		delay(1);
		if (*flag_initial == 0x0f) break; // wait until all flags are initialized to 0x0f
	}
	
	while (1){
		flag_1 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_1 + (block << 11); // block selection
		flag_2 = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_2 + (block << 11); // block selection

		#if DEBUG == 1
		printf("\nresize-correct flag: %d, %d, %d, %d--------Block\n", *((unsigned char *)SHARED_ONCHIP_BASE +1), *((unsigned char *)SHARED_ONCHIP_BASE +1 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +1+ 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +1 + 6144), block);
		printf("correct-sobel flag: %d, %d, %d, %d\n", *((unsigned char *)SHARED_ONCHIP_BASE +2), *((unsigned char *)SHARED_ONCHIP_BASE +2 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +2 + 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +2 + 6144));		
		printf("flag_1 addr: %d,  %d\n",flag_1, (unsigned char *)SHARED_ONCHIP_BASE +1);
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
			if (*flag_2 == 0x02){ // 00000010 image is processed and available
				*flag_2 = 0x03; // 00000011 image is being freshed and not available
				break;
			}
		}
		
		image_mat = (unsigned char *) SHARED_ONCHIP_BASE + mem_offset + (block << 11); // block selection
		
		/* initialize hmin and hmax */
		h_bright[0] = *image_mat;
		h_bright[1] = *image_mat;
		
		for (i = 0; i < WIDTH * HEIGHT; i++){
			if (*image_mat < h_bright[0]) h_bright[0] = *image_mat;
			else if (*image_mat > h_bright[1]) h_bright[1] = *image_mat;
			image_mat++;
		}
		
		/* update state signal with hmin and hmax */
		state_sig[2] = state_sig[1];
		state_sig[1] = state_sig[0];
		state_sig[0] = h_bright[1] - h_bright[0];
		ave = (state_sig[0] + state_sig[1] + state_sig[2]) / 3;
	
		if (ave < 128) {
			image_mat = (unsigned char *) SHARED_ONCHIP_BASE + mem_offset + (block << 11); // block selection
			
			/* enhance grayscale according to extremum */
			if (h_bright[1] - h_bright[0] > 127){}
			else if (h_bright[1] - h_bright[0] > 63){
				enhance_index = 1;
			}
			else if (h_bright[1] - h_bright[0] > 31){
				enhance_index = 2;
			}
			else if (h_bright[1] - h_bright[0] > 15){
				enhance_index = 3;
			}
			else{
				enhance_index = 4;
			} 
			for (i = 0; i < WIDTH * HEIGHT; i++) {
				*image_mat = (*image_mat - h_bright[0]) << enhance_index;
				image_mat++;
			}
		}

		*flag_1 = 0x02; // 00000010 image is processed and available
		*flag_2 = 0x00; // 00000000 image is not processed but available

		#if DEBUG == 1
		printf("resize-correct flag: %d, %d, %d, %d\n", *(unsigned char *)SHARED_ONCHIP_BASE +1, *((unsigned char *)SHARED_ONCHIP_BASE +1 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +1+ 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +1 + 6144));
		printf("correct-sobel flag: %d, %d, %d, %d\n", *(unsigned char *)SHARED_ONCHIP_BASE +2, *((unsigned char *)SHARED_ONCHIP_BASE +2 + 2048), *((unsigned char *)SHARED_ONCHIP_BASE +2 + 4096),
			 *((unsigned char *)SHARED_ONCHIP_BASE +2 + 6144));		
		#endif		
		
		block = (block + 1) % 4; // switch to next block
	}

	return 0;
}
