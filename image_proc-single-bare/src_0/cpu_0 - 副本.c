#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "altera_avalon_performance_counter.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 0

#define HW_TIMER_PERIOD 100 /* 100ms */

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    StartTask_Stack[TASK_STACKSIZE]; 

/* Definition of Task Priorities */
#define STARTTASK_PRIO      1
#define TASK1_PRIORITY      10

/* Definition of Task Periods (ms) */
#define TASK1_PERIOD 10000

#define SECTION_1 1

/*
 * Global variables
 */
int delay;									//!> Delay of HW-timer
// unsigned char ascii_gray[] = {32,46,59,42,115,116,73,40,106,117,86,119,98,82,103,64};
unsigned char ascii_gray[] = {' ', '.', ':', '-', '=', '+', '/', 't', 'z', 'U', 'w', '*', '0', '#', '%', '@'};
double * mat_double_gray;		//!> matrix of grayscale
double * mat_resize;				//!> matrix of resized grayscale
double * h_bright;					//!> mimimum and maximum grayscale, hmin and hmax
double * mat_sobel;					//!> matrix of gradient
unsigned char * mat_ascii;	//!> matrix of ascii character
char control_signal;				//!> control signal for correction
int stateSig[3] = {255, 255, 255}; //!> initial state signal (moore)

/*
 * ISR for HW Timer
 */
alt_u32 alarm_handler(void* context)
{
  OSTmrSignal(); /* Signals a 'tick' to the SW timers */
  
  return delay;
}

// Semaphores
OS_EVENT *Task1TmrSem;

// SW-Timer
OS_TMR *Task1Tmr;

/* Timer Callback Functions */ 
void Task1TmrCallback (void *ptmr, void *callback_arg){
  OSSemPost(Task1TmrSem);
}

//! A function converting an image from RGB to grayscale.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \return Pointer of the processed array containing pixels of grayscale image.
*/
double * grayscale(int x, int y, unsigned char * origin_mat){
  int i, j;
  double r, g, b; // RGB colormap components
  double * gray_m = malloc(sizeof(double) * x * y); // allocate memory for output image
  
  for (j = 0; j < y; j++){
    for (i = 0; i < x; i++){
			/* extract RGB components successively */
      r = *origin_mat++;
      g = *origin_mat++;
      b = *origin_mat++;
      gray_m[i + j * x] = r * 0.3125 + g * 0.5625 + b * 0.125; // convert by given formula
    }
  }
  
  return gray_m;
}

//! A function shrinking an image into a quarter size of original one.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \return Pointer of the array containing pixels of resized image.
*/
double * resize(int x, int y, double * origin_mat){
  int i, j;
  double * resize_m;
  x /= 2; // half the width of image
  y /= 2; // half the height of image
  resize_m = malloc(sizeof(double) * x * y); // allocate memory for output image
	
  for (j = 0; j < y; j++){
		/* get new grayscale from average of corresponding four pixels */
    for (i = 0; i < x; i++){
      resize_m[i + j * x] = (origin_mat[i * 2 + j * 2 * x * 2]
        + origin_mat[i * 2 + 1 + j * 2 * x * 2] + origin_mat[i * 2 + (j * 2 + 1) * x * 2]
        + origin_mat[i * 2 + 1 + (j * 2 + 1) * x * 2]) / 4;
    }
  }
	
  return resize_m;
}


//! A function extracting the minimum and maximum grayscale from an image.
/*!
  \param x width of input image.
  \param y height of input image.
  \param origin_mat pointer of an array containing pixels of image.
  \return Pointer of the array containing the minimum and maximum grayscale.
*/
double * brightness(int x, int y, double * origin_mat){
  int i, j;
  double *h = malloc(sizeof(double) * 2); // allocate memory for hmin and hmax
	/* initialize hmin and hmax */
  h[0] = origin_mat[0];
  h[1] = origin_mat[0];
	
  for (j = 0; j < y; j++){
    for (i = 0; i < x; i++){
      if (origin_mat[i + j * x] < h[0]) h[0] = origin_mat[i + j * x];
      if (origin_mat[i + j * x] > h[1]) h[1] = origin_mat[i + j * x];
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
char moore(double hmin, double hmax, int * state_sig){
  /* 
	 * do not know if "inSig" is needed or not,
   * I do not want to delay here.
	*/
  int ave;
	
	/* renew state signal with hmin and hmax */
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
void correct(int x, int y, double * origin_mat,  char control_sig, double hmin, double hmax){
  int i, j;

  if (control_sig == 0) return; // do not need to correct
	
	/* enhance grayscale according to extremum */
  if (hmax - hmin > 127){}
  else if (hmax - hmin > 63){
    for (j = 0; j < y; j++){
      for (i = 0; i < x; i++){
        origin_mat[i + j * x] = 2 * (origin_mat[i + j * x] - hmin);
      }
		}
  }
  else if (hmax - hmin > 31){
    for (j = 0; j < y; j++){
      for (i = 0; i < x; i++){
        origin_mat[i + j * x] = 4 * (origin_mat[i + j * x] - hmin);
      }
		}
  }
  else if (hmax - hmin > 15){
    for (j = 0; j < y; j++){
      for (i = 0; i < x; i++){
        origin_mat[i + j * x] = 8 * (origin_mat[i + j * x] - hmin);
      }
		}
  }
  else{
    for (j = 0; j < y; j++){
      for (i = 0; i < x; i++){
        origin_mat[i + j * x] = 16 * (origin_mat[i + j * x] - hmin);
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
double * sobel(int x, int y, double * origin_mat){
  int i, j = 0;
  double * g;
  double gx, gy; // horizontal and vertical gradient
  g = malloc(sizeof(double) * (x - 2) * (y - 2));  // allocate memory for output gradient image

  for (j = 0; j < y - 2; j++){
    for (i = 0; i < x - 2; i++){
			/* calculate horizontal gradient by convolution */
      gx = origin_mat[i + j * x] * (-1) + origin_mat[i + (j + 1) * x] * (-2)
        + origin_mat[i + (j + 2) * x] * (-1) + origin_mat[i + 2 + j * x] * 1
        + origin_mat[i + 2 + (j + 1) * x] * 2 + origin_mat[i + 2 + (j + 2) * x] * 1;
			/* calculate vertical gradient by convolution */
      gy = origin_mat[i + j * x] * (-1) + origin_mat[i + 1 + j * x] * (-2)
        + origin_mat[i + 2 + j * x] * (-1) + origin_mat[i + (j + 2) * x] * 1
        + origin_mat[i + 1 + (j + 2) * x] * 2 + origin_mat[i + 2 + (j + 2) * x] * 1;
			/* merge horizontal and vertical gradient */
      g[i + j * (x - 2)] = sqrt(gx * gx + gy * gy) / 4;
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
unsigned char * ascii(int x, int y, double * origin_mat){
  unsigned char * ascii_m = malloc(sizeof(unsigned char) * x * y); // allocate memory for output ascii image
  int i, j, n;
	
  for (j = 0; j < y; j++)
    for (i = 0; i < x; i++){
			/* convert gradient into corresponding ascii character */
       n = ((int) origin_mat[i + j * x]) / 16; 
       ascii_m[i + j * x] = ascii_gray[n];
    } 
		
  return ascii_m;
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
void print_img_double(int x, int y, double * n){
  int i, j;
	
    for (j = 0; j < y; j++){
      for (i = 0; i < x; i++){
        printf("%.2f ", *n++);
      }
    }
    printf("\n\n");
}
#endif

void task1(void* pdata)
{
	INT8U err;
	INT8U value = 0;
	char current_image = 0;
  int loop_times = 0;
	
	#if DEBUG == 1
	/* Sequence of images for testing if the system functions properly */
	char number_of_images=10;
	unsigned char* img_array[10] = {img1_24_24, img1_32_22, img1_32_32, img1_40_28, img1_40_40, 
			img2_24_24, img2_32_22, img2_32_32, img2_40_28, img2_40_40 };
	#else
	/* Sequence of images for measuring performance */
	char number_of_images=3;
	unsigned char* img_array[3] = {img1_32_32, img2_32_32, img3_32_32};
	#endif

	while (1)
	{ 
		/* Extract the x and y dimensions of the picture */
		unsigned char i = *img_array[current_image];
		unsigned char j = *(img_array[current_image]+1);
    int p;
		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		
		/* Measurement here */
		sram2sm_p3(img_array[current_image]);
		PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1);  

    /* image process */
    mat_double_gray = grayscale(i, j, img_array[current_image] + 3);
    mat_resize = resize(i, j, mat_double_gray);
    i /= 2;
    j /= 2;
    free(mat_double_gray);
    h_bright = brightness(i, j, mat_resize);
    control_signal = moore(h_bright[0], h_bright[1], stateSig);
    correct(i, j, mat_resize, control_signal, h_bright[0], h_bright[1]);
    mat_sobel = sobel(i, j, mat_resize);
    i -= 2;
    j -= 2;
    free(mat_resize);
    mat_ascii = ascii(i, j, mat_sobel);
    free(mat_sobel);
    print_img(i, j, mat_ascii);

		/* Just to see that the task compiles correctly */
		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_GREEN_BASE,value++);

    /* Print report */
    perf_print_formatted_report
    (PERFORMANCE_COUNTER_0_BASE,            
    ALT_CPU_FREQ,        // defined in "system.h"
    1,                   // How many sections to print
    "Section 1"        // Display-name of section(s).
    );   


		OSSemPend(Task1TmrSem, 0, &err);


		/* Increment the image pointer */
		current_image=(current_image+1) % number_of_images;


	}
}

void StartTask(void* pdata)
{
  INT8U err;
  void* context;

  static alt_alarm alarm;     /* Is needed for timer ISR function */
  
  /* Base resolution for SW timer : HW_TIMER_PERIOD ms */
  delay = alt_ticks_per_second() * HW_TIMER_PERIOD / 1000; 
  printf("delay in ticks %d\n", delay);

  /* 
   * Create Hardware Timer with a period of 'delay' 
   */
  if (alt_alarm_start (&alarm,
      delay,
      alarm_handler,
      context) < 0)
      {
          printf("No system clock available!n");
      }

  /* 
   * Create and start Software Timer 
   */

   //Create Task1 Timer
   Task1Tmr = OSTmrCreate(0, //delay
                            TASK1_PERIOD/HW_TIMER_PERIOD, //period
                            OS_TMR_OPT_PERIODIC,
                            Task1TmrCallback, //OS_TMR_CALLBACK
                            (void *)0,
                            "Task1Tmr",
                            &err);
                            
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if creation successful
      printf("Task1Tmr created\n");
    }
   }
   

   /*
    * Start timers
    */
   
   //start Task1 Timer
   OSTmrStart(Task1Tmr, &err);
   
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if start successful
      printf("Task1Tmr started\n");
    }
   }


   /*
   * Creation of Kernel Objects
   */

  Task1TmrSem = OSSemCreate(0);   

  /*
   * Create statistics task
   */

  OSStatInit();

  /* 
   * Creating Tasks in the system 
   */

  err=OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task1 created\n");
    }
   }  

  printf("All Tasks and Kernel Objects generated!\n");

  /* Task deletes itself */

  OSTaskDel(OS_PRIO_SELF);
}


int main(void) {

  printf("MicroC/OS-II-Vesion: %1.2f\n", (double) OSVersion()/100.0);
     
  OSTaskCreateExt(
	 StartTask, // Pointer to task code
         NULL,      // Pointer to argument that is
                    // passed to task
         (void *)&StartTask_Stack[TASK_STACKSIZE-1], // Pointer to top
						     // of task stack 
         STARTTASK_PRIO,
         STARTTASK_PRIO,
         (void *)&StartTask_Stack[0],
         TASK_STACKSIZE,
         (void *) 0,  
         OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
         
  OSStart();
  
  return 0;
}
