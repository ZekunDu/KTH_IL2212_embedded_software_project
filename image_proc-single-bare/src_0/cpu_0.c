#include <stdio.h>
#include <math.h>
#include "altera_avalon_performance_counter.h"
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 0

/* Definition of time delay (ms) */
#define DELAY_PERIOD 2000

/* Definition of performance counter section */
#define SECTION_1 1
#define SECTION_2 2
#define SECTION_3 3
#define SECTION_4 4
#define SECTION_5 5

/* Global variables */
unsigned char ascii_gray[] = {' ', '.', ':', '-', '=', '+', '/', 't', 'z', 'U', 'w', '*', '0', '#', '%', '@'};
unsigned char xdim, ydim;          //!> width and height of matrix
unsigned char * mat_double_gray;   //!> matrix of grayscale
unsigned char * mat_resize;        //!> matrix of resized grayscale
unsigned char * h_bright;          //!> minimum and maximum grayscale, hmin and hmax
unsigned char * mat_sobel;         //!> matrix of gradient
unsigned char * mat_ascii;         //!> matrix of ascii character
unsigned char control_signal;      //!> control signal for correction
unsigned char exe_time_size = 20;  //!> execution time loop size for throughput calculation  
double exe_time = 0;               //!> execution time 
int stateSig[3] = {255, 255, 255}; //!> initial state signal (moore)

//! A function converting an image from RGB to grayscale.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \return Pointer of the processed array containing pixels of grayscale image.
*/
unsigned char * grayscale(int x, int y, unsigned char * origin_mat){
	int i, j;
	unsigned char r, g, b; // RGB colormap components
	unsigned char * gray_m = malloc(sizeof(unsigned char) * x * y); // allocate memory for output image
	unsigned char * gray_m_head = gray_m; // save the head of the new array

	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			/* extract RGB components successively */
			r = *origin_mat++;
			g = *origin_mat++;
			b = *origin_mat++;
			/* convert by given formula gray = r * 0.3125 + g * 0.5625 + b * 0.125 */
			*gray_m = (r >> 4) + (r >> 2) + (g >> 1) + (g >> 4) + (b >> 3);
			gray_m++;
		}
	}

	return gray_m_head;
}

//! A function shrinking an image into a quarter size of original one.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \return Pointer of the array containing pixels of resized image.
*/
unsigned char * resize(int x, int y, unsigned char * origin_mat){
	int i, j;
	unsigned char * resize_m, * resize_m_head;
	int new_x = x >> 1; // half the width of image
	y = y >> 1; // half the height of image
	resize_m = malloc(sizeof(unsigned char) * new_x * y); // allocate memory for output image
	resize_m_head = resize_m; // save the head of new array

	for (j = 0; j < y; j++){
		for (i = 0; i < new_x; i++){
			/* get new grayscale from average of corresponding four pixels */
			*resize_m = *origin_mat >> 2;
			*resize_m += *(origin_mat + 1) >> 2;
			*resize_m += *(origin_mat + x) >> 2;
			*resize_m += *(origin_mat + x + 1) >> 2;
			resize_m++;
			origin_mat += 2;
		}
		origin_mat += x;
	}
  
  return resize_m_head;
}

//! A function shrinking an image into a quarter size of original one.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \return Pointer of the array containing pixels of resized image.
*/
unsigned char * brightness(int x, int y, unsigned char * origin_mat){
	int i, j;
	unsigned char *h = malloc(sizeof(unsigned char) * 2); // allocate memory for hmin and hmax
	/* initialize hmin and hmax */
	h[0] = origin_mat[0];
	h[1] = origin_mat[0];

	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			if (*origin_mat < h[0]) h[0] = *origin_mat;
			else if (*origin_mat > h[1]) h[1] = *origin_mat;
			origin_mat++;
		}
	}
	
  return h;
}

//! A function deciding whether an image needs to be corrected.
/*!
  \param hmin minimum grayscale of processing image.
  \param hmax maximum grayscale of processing image.
  \param state_sig pointer of original state signal.
  \return A control signal in which 0 represents DISABLE and 1 represents ENABLE.
*/
unsigned char moore(unsigned char hmin, unsigned char hmax, int * state_sig){
	int ave;

	/* update state signal with hmin and hmax */
	state_sig[2] = state_sig[1];
	state_sig[1] = state_sig[0];
	state_sig[0] = hmax - hmin;
	ave = (state_sig[0] + state_sig[1] + state_sig[2]) / 3;

	if (ave < 128) return 1;
	else return 0;
}

//! A function deciding whether an image needs to be corrected.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \param control_sig decide whether input image needs to be corrected or not.
  \param hmin minimum grayscale of input image.
  \param hmax maximum grayscale of input image.
  \return A control signal in which 0 represents DISABLE and 1 represents ENABLE.
*/
void correct(int x, int y, unsigned char * origin_mat, char control_sig, unsigned char hmin, unsigned char hmax){
	int i, j; 
	if (control_sig == 0) return; // do not need to correct

	/* enhance grayscale according to extremum */
	if (hmax - hmin > 127){}
	else if (hmax - hmin > 63){
		for (j = 0; j < y; j++){
			for (i = 0; i < x; i++){
				*origin_mat = (*origin_mat - hmin) << 1;
				origin_mat++;
			}
		}
	}
	else if (hmax - hmin > 31){
		for (j = 0; j < y; j++){
			for (i = 0; i < x; i++){
				*origin_mat = (*origin_mat - hmin) << 2;
				origin_mat++;
			}
		}
	}
	else if (hmax - hmin > 15){
		for (j = 0; j < y; j++){
			for (i = 0; i < x; i++){
				*origin_mat = (*origin_mat - hmin) << 3;
				origin_mat++;
			}
		}
	}
	else{
		for (j = 0; j < y; j++){
			for (i = 0; i < x; i++){
				*origin_mat = (*origin_mat - hmin) << 4;
				origin_mat++;
			}
		}
	}
}

//! A function calculating gradient image by sobel operator.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \return Pointer of the array containing gradient values of original image.
*/
unsigned char * sobel(int x, int y, unsigned char * origin_mat){
	int i, j = 0;
	unsigned char * g = malloc(sizeof(unsigned char) * (x - 2) * (y - 2)); // allocate memory for output gradient image
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
	
  return g;
}

//! A function converting gradient values of an image into ascii characters.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing gradient values of image.
  \return Pointer of the array containing ascii characters of gradient image.
*/
unsigned char * ascii(int x, int y, unsigned char * origin_mat){
	unsigned char * ascii_m = malloc(sizeof(unsigned char) * x * y); // allocate memory for output ascii image
	unsigned char * ascii_m_head = ascii_m; // save the head of new array
	int i, j, n;
	
	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			/* convert gradient into corresponding ascii character */
			n = *origin_mat++ >> 4; 
			*ascii_m = ascii_gray[n];
			ascii_m++;
		}
	}
	
	return ascii_m_head;
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

#if DEBUG == 1
//! A function printing an image.
/*!
  \param x width of input image.
  \param y height of input image.
  \param mat pointer of an array containing pixels of image.
*/
void print_img_int(int x, int y, unsigned char * n){
	int i, j;
	
	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			printf("%d ", *n++);
		}
	}
	printf("\n\n");
}
#endif

int main(){
	unsigned char current_image = 0; // index for processing image
	unsigned char number_of_images = 4; // total number of tested images
	unsigned char ** img_array = sequence1; // array of tested images
	#if DEBUG == 0
	unsigned char loop_times = 0; // index for loop
	#endif  

	while (1){
		/* Extract the x and y dimensions of the picture */
		unsigned char i = *img_array[current_image];
		unsigned char j = *(img_array[current_image]+1);
		int p;
		
		/* performance counter, start section 1 */
		#if DEBUG == 0
		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		#endif
		
		/* image process */
		mat_double_gray = grayscale(i, j, img_array[current_image] + 3); // grayscale function
		
		/* performance counter, end section 1 and start section 2 */
		#if DEBUG == 0
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_2);	
		#endif
		
		mat_resize = resize(i, j, mat_double_gray); // resize function
		i /= 2; // update x dimension
		j /= 2; // update y dimention
		free(mat_double_gray); // free original grayscale image
		
		/* performance counter, end section 2 and start section 3 */
		#if DEBUG == 0
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_2);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_3);
		#endif
		
		h_bright = brightness(i, j, mat_resize); // brightness function
		control_signal = moore(h_bright[0], h_bright[1], stateSig); // moore function
		correct(i, j, mat_resize, control_signal, h_bright[0], h_bright[1]); // correct function
		
		/* performance counter, end section 3 and start section 4 */
		#if DEBUG == 0
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_3);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_4);
		#endif
		
		mat_sobel = sobel(i, j, mat_resize); // sobel function
		i -= 2; // update x dimension
		j -= 2; // update y dimention
		free(mat_resize); // free original resized image
		
		/* performance counter, end section 4 and start section 5 */
		#if DEBUG == 0
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_4);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_5);	
		#endif
		
		mat_ascii = ascii(i, j, mat_sobel); // ascii function
		free(mat_sobel); // free original gradient image
		
		/* performance counter, end section 5 */
		#if DEBUG == 0
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_5);  
		#endif
		
		/* print current ascii image */
		#if DEBUG == 1
		print_img(i, j, mat_ascii);
		
		OSTimeDlyHMSM(0, 0, 0, DELAY_PERIOD);
		#endif
		
		/* Print section report */
		#if DEBUG == 0
		exe_time += ((double)perf_get_total_time(PERFORMANCE_COUNTER_0_BASE)) / ALT_CPU_FREQ;
		
		perf_print_formatted_report
		(PERFORMANCE_COUNTER_0_BASE,            
		ALT_CPU_FREQ,				// defined in "system.h"
		5,           				// How many sections to print
		"Grayscale",  			    // Display-name of section(s).
		"Resize", "Brightness&Correct", "Sobel", "ASCII"
		);
		
		/* Print general report */
		if (++loop_times >= exe_time_size){
			printf("|**************************************\n");
			printf("|** %d images done \n", loop_times);
			printf("|** throughput is %f images/second\n", loop_times / exe_time);
			printf("|**************************************\n");
			getchar();
			loop_times = 0;
		}

		#endif
		
		/* Increment the image pointer */
		current_image = (current_image+1) % number_of_images;
	}
}
