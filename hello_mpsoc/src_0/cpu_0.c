#include <stdio.h>
#include "system.h"
#include "altera_avalon_performance_counter.h"
#include "images.h"

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

/* width and height of images */
#define WIDTH  32
#define HEIGHT 32
#define WIDTH_ASCII  14
#define HEIGHT_ASCII 14

/* Definition of performance counter section */
#define SECTION_1 1
#define SECTION_2 2
#define SECTION_3 3

#define DEBUG 0

/* Global variables */
int flag_offset = GRAYSCALE_RESIZE_FLAG_ADDR; //!> offset of flag after processing
int flag_offset_asc = ASCII_GRAYSCALE_FLAG_ADDR;
int mem_offset = GRAYSCALE_MAT_ADDR;          //!> offset of grayscale image
double exe_time = 0;                          //!> execution time 
char exe_time_size = 20;                      //!> execution time loop size for throughput calculation
unsigned char * ascii_img[4][WIDTH_ASCII * HEIGHT_ASCII];

extern void delay (int millisec);

int main(){
	unsigned char current_image = 0;        // index for processing image
	unsigned char number_of_images = 4;     // total number of tested images
	unsigned char ** img_array = sequence1; // array of tested images
	unsigned char loop_times = 0;           // index for loop
	alt_u64 total_clock;
	
	/* flag pointer to shared memory */
	unsigned char *flag = (unsigned char *) SHARED_ONCHIP_BASE;
	unsigned char *flag_asc;
	unsigned char *flag_initial = flag + INITIAL_FLAG_ADDR;
	
	int i;
	int block = 0; // block index
	unsigned char *origin_mat;
	unsigned char *shared;
	
	while (1){
		delay(1);
		if (*flag_initial == 0x0f) break; // wait until all flags are initialized to 0x0f
	}
	
	while (1){
		/* Start performance counter */
		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING(PERFORMANCE_COUNTER_0_BASE);
		
		#if DEBUG == 1
		printf("\ngray-resize flag: %d, %d, %d, %d---Block%d\n", *((unsigned char *)SHARED_ONCHIP_BASE), *((unsigned char *)SHARED_ONCHIP_BASE + 2048), 
			*((unsigned char *)SHARED_ONCHIP_BASE + 4096), *((unsigned char *)SHARED_ONCHIP_BASE + 6144), block);
		printf("flag_1 addr: %d,  %d\n",flag, (unsigned char *)SHARED_ONCHIP_BASE);
		#endif

		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		flag = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset + (block << 11); // block selection
		flag_asc = (unsigned char *) SHARED_ONCHIP_BASE + flag_offset_asc + (block << 11);
		while (1){
			delay(1);
			if (*flag == 0x02){ // 00000010 image is processed and available
				*flag = 0x03; // 00000011 image is being freshed and not available
				break;
			}
		}
				
		while (1){
			delay(1);
			if (*flag_asc == 0x00){ // 00000010 image is processed and available
				*flag = 0x01; // 00000011 image is being freshed and not available
				break;
			}
		}

		/* save ascii image from shared memory to SRAM */
		shared = (unsigned char *) SHARED_ONCHIP_BASE + mem_offset + (block << 11); 
		for (i = 0; i < WIDTH_ASCII * HEIGHT_ASCII; i++){
			*shared++ = *base++; 	
		}

		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_2);

		/* image process and write img to sm*/
		origin_mat = img_array[current_image] + 3;
		shared = (unsigned char *) SHARED_ONCHIP_BASE + mem_offset + (block << 11); 
		for (i = 0; i < WIDTH * HEIGHT; i++){
			/* convert by given formula gray = r * 0.3125 + g * 0.5625 + b * 0.125 */
			*shared++ = ((*origin_mat) >> 4) + ((*origin_mat++) >> 2) + ((*origin_mat) >> 1) + ((*origin_mat++) >> 4) + ((*origin_mat++) >> 3);
		}
		
		*flag = 0x00; // 00000000 image is not processed but available
		*flag_asc = 0x02; 
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_2);

		#if DEBUG == 1
		printf("gray-resize flag: %d, %d, %d, %d\n", *(unsigned char *)SHARED_ONCHIP_BASE, *((unsigned char *)SHARED_ONCHIP_BASE + 2048), 
			*((unsigned char *)SHARED_ONCHIP_BASE + 4096), *((unsigned char *)SHARED_ONCHIP_BASE + 6144));		
		#endif
		
		/* Increment the image pointer */
		current_image = (current_image + 1) % number_of_images;
		
		block = (block + 1) % 4; // switch to next block
		
		/* Print general report */ 
		/*
		exe_time = 0.0;
		total_clock = perf_get_total_time((void *)PERFORMANCE_COUNTER_0_BASE);
		exe_time += ((double)total_clock) / ALT_CPU_FREQ;
		printf("%d\n", PERFORMANCE_COUNTER_0_BASE);
		printf("%d\n", ALT_CPU_FREQ);
		printf("%d\n", perf_get_total_time((void *)PERFORMANCE_COUNTER_0_BASE));
		*/
		perf_print_formatted_report
		(PERFORMANCE_COUNTER_0_BASE,            
		ALT_CPU_FREQ,       // defined in "system.h"
		2,                  // How many sections to print
		"delay", "exe"
		);
		/*
		if (++loop_times >= exe_time_size){
			printf("|**************************************\n");
			printf("|** %d images done \n", loop_times);
			printf("|** throughput is %f images/second\n", (double)loop_times / exe_time);
			printf("|**************************************\n");
			loop_times = 0;
		}
		*/
	}
  
	return 0;
}