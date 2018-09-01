
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "images.h"

double* matrix;
unsigned char ascii_gray[] = {32,46,59,42,115,116,73,40,106,117,86,119,98,82,103,64};
double * mat_double_gray; // matrix of double gray
double * mat_resize;  // resized matrix
double * h_bright;    // bright min & max
double * mat_sobel;   // matrix after sobel
unsigned char * mat_ascii; // matrix of unsigned char ascii
char control_signal;     // control signal
int stateSig[3] = {255, 255, 255}; // initial state signal (moore)

int lengthx, lengthy;
/*
double * rotateMatrix(int dx, int dy, int x, int y, double * origin_mat){
    int i, j;
	double * mat = malloc(sizeof(double) * x * y);
	for (i = 0; i < x; i++){
		for (j = 0; j < y; j++){
			mat[i + x * j] = origin_mat[i + x * j];
		}
	}

	if (dx == -1){
		// shift left
		for (i = 0; i < y; i++){ // which row
			double tmp = mat[0 + x * i];
			for (j = 0; j < x - 1; j++){
				mat[j + x * i] = mat[j + 1 + x * i];
			}
			mat[x - 1 + x * i] = tmp;
		}
	}
	else if (dx == 1){
		// shift right
		for (i = 0; i < y; i++){ // which row
			double tmp = mat[x - 1 + x * i];
			for (j = x - 1; j > 0; j--){
				mat[j + x * i] = mat[j - 1 + x * i];
			}
			mat[0 + x * i] = tmp;
		}
	}

    for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
		    printf("%d ", mat[i + x * j]);
		}
		printf("\n");
	}
	printf("\n");

	if (dy == -1){
		// shift up
		for (i = 0; i < x; i++) { // which column
			double tmp = mat[i + x * 0];
			for (j = 0; j < y - 1; j++){
				mat[i + x * j] = mat[i + x * (j + 1)];
			}
			mat[i + x * (y - 1)] = tmp;
		}
	}
	else if (dy == 1){
		// shift down
		for (i = 0; i < x; i++) { // which column
			double tmp = mat[i + x * (y - 1)];
			for (j = y - 1; j > 0; j--){
				mat[i + x * j] = mat[i + x * (j - 1)];
			}
			mat[i] = tmp;
		}
	}
	return mat;
}
*/

double * sobel(int x, int y, double * origin_mat){
	int i, j, m = 0;
	//double * origin_mat[8];
	double * g;
	double gx, gy;
	g = malloc(sizeof(double) * (x - 2) * (y - 2));
	//gx = malloc(sizeof(double) * x * y);
	//gy = malloc(sizeof(double) * x * y);

	/*
	for (i = -1; i < 2; i++){
		for (j = -1; j < 2; j++){
			if (!(i == 0 && j == 0)){
				origin_mat[m++] = rotateMatrix(i, j, x, y, origin_mat);
			}
		}
	}
	*/

	for (j = 0; j < y - 2; j++){
		for (i = 0; i < x - 2; i++){
            //printf("%d, %d\n", i , j);
			gx = origin_mat[i + j * x] * (-1) + origin_mat[i + (j + 1) * x] * (-2)
				+ origin_mat[i + (j + 2) * x] * (-1) + origin_mat[i + 2 + j * x] * 1
				+ origin_mat[i + 2 + (j + 1) * x] * 2 + origin_mat[i + 2 + (j + 2) * x] * 1;
			gy = origin_mat[i + j * x] * (-1) + origin_mat[i + 1 + j * x] * (-2)
				+ origin_mat[i + 2 + j * x] * (-1) + origin_mat[i + (j + 2) * x] * 1
				+ origin_mat[i + 1 + (j + 2) * x] * 2 + origin_mat[i + 2 + (j + 2) * x] * 1;
			g[i + j * (x - 2)] = sqrt(gx * gx + gy * gy) / 4;
            //printf("%f, %f\n", gx , gy);
		}
	}
	//x -= 2;
	//y -= 2;
	return g;
}

double * grayscale(int x, int y, unsigned char * origin_mat){
 	int i, j;
	double * gray_m = malloc(sizeof(double) * x * y);
	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			double r, g, b;
			r = origin_mat[3 * i + j * x];
			g = origin_mat[3 * i + j * x + 1];
			b = origin_mat[3 * i + j * x + 2];
			gray_m[i + j * x] = r * 0.3125 + g * 0.5625 + b * 0.125;
		}
	}
	return gray_m;
}

double * resize(int x, int y, double * origin_mat){
	int i, j;
	double * resize_m;
	x /= 2;
	y /= 2;
	resize_m = malloc(sizeof(double) * x * y);
	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			resize_m[i + j * x] = (origin_mat[i * 2 + j * 2 * x]
				+ origin_mat[i * 2 + 1 + j * 2 * x] + origin_mat[i * 2 + (j * 2 + 1) * x]
				+ origin_mat[i * 2 + 1 + (j * 2 + 1) * x]) / 4;
		}
	}
	return resize_m;
}

double * brightness(int x, int y, double * origin_mat){
	int i, j;
	double *h = malloc(sizeof(double) * 2); // hmin, hmax
	h[0] = origin_mat[0];
	h[1] = origin_mat[0];
	for (j = 0; j < y; j++)
		for (i = 0; i < x; i++){
			if (origin_mat[i + j * x] < h[0])	h[0] = origin_mat[i + j * x];
			if (origin_mat[i + j * x] > h[1])	h[1] = origin_mat[i + j * x];
		}
	return h;
}

char moore(double hmin, double hmax, int * stateSig){
	// don't know if "inSig" is needed or not, I don't want to
	// delay here
	int ave;
	stateSig[2] = stateSig[1];
	stateSig[1] = stateSig[0];
	stateSig[0] = hmax - hmin;
	ave = (stateSig[0] + stateSig[1] + stateSig[2]) / 3;
	if (ave < 128) return 1;
	else return 0;
}

void correct(int x, int y, double* origin_mat,  char control_sig,
		double hmin, double hmax){
	// control_sig should be in the task and determine the semephor

	int i, j;

	if (control_sig == 0) return;
	if (hmax - hmin > 127){}
	else if (hmax - hmin > 63){
		for (j = 0; j < y; j++)
			for (i = 0; i < x; i++){
				origin_mat[i + j * x] = 2 * (origin_mat[i + j * x] - hmin);
			}
	}
	else if (hmax - hmin > 31){
		for (j = 0; j < y; j++)
			for (i = 0; i < x; i++){
				origin_mat[i + j * x] = 4 * (origin_mat[i + j * x] - hmin);
			}
	}
	else if (hmax - hmin > 15){
		for (j = 0; j < y; j++)
			for (i = 0; i < x; i++){
				origin_mat[i + j * x] = 8 * (origin_mat[i + j * x] - hmin);
			}
	}
	else{
		for (j = 0; j < y; j++)
			for (i = 0; i < x; i++){
				origin_mat[i + j * x] = 16 * (origin_mat[i + j * x] - hmin);
			}
	}
}

unsigned char * ascii(int x, int y, double * origin_mat){
	unsigned char * ascii_m = malloc(sizeof(unsigned char) * x * y);
	int i, j, n;
	for (j = 0; j < y; j++)
		for (i = 0; i < x; i++){
			n = (int) origin_mat[i + j * x] / 16;
       		ascii_m[i + j * x] = ascii_gray[n];
   		}
    return ascii_m;
}

void print_img(int x, int y, unsigned char * mat){
	int i, j;
	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++)
			printf("%c", mat[i + j * x]);
		printf("\n");
	}
}
void print_img_double(int x, int y, double * n){
  int i, j;
  printf("print-image-double\n");
    for (j = 0; j < y; j++){
      for (i = 0; i < x; i++){
        printf("%.2f ", n[i + x * j]);
      }
      printf("\n%d,%d\n\n", i, j);
    }
    printf("\n\n");
}

int main(){
	//double* om[3][3] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	int x = 4, y = 4;
	double om[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
	 12, 13, 14, 15, 16};
    int i, j;
	/*
	double* m = rotateMatrix(-1, 1, x, y, om);



	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
			printf("%d ", om[i + x * j]);
		}
		printf("\n");
	}
    printf("\n");

	for (j = 0; j < y; j++){
		for (i = 0; i < x; i++){
		    printf("%d ", m[i + x * j]);
		}
		printf("\n");
	}
	printf("\n");
	*/
/*
	double* n = sobel(x, y, om);
	for (j = 0; j < y - 2; j++){
		for (i = 0; i < x - 2; i++){
		    printf("%.3f ", n[i + (x - 2) * j]);
		}
		printf("\n");
	}
	printf("\n");

	unsigned char www = 20;
	unsigned char w = www * www;
	double qq = www * 0.033;
	printf("%d\n", w);
*/

    i = img1_24_24[0];
    j = img1_24_24[1];

	printf("before gray\n");
    mat_double_gray = grayscale(i, j, img1_24_24 + 3);
    printf("before resize\n");
    mat_resize = resize(i, j, mat_double_gray);
    printf("after resize\n");
    i /= 2;
    j /= 2;
    free(mat_double_gray);
      print_img_double(i, j, mat_resize);
    printf("before brightness\n");
    h_bright = brightness(i, j, mat_resize);
    control_signal = moore(h_bright[0], h_bright[1], stateSig);
    correct(i, j, mat_resize, stateSig, h_bright[0], h_bright[1]);
    mat_sobel = sobel(i, j, mat_resize);
          print_img_double(i, j, mat_sobel);
    i -= 2;
    j -= 2;

    free(mat_resize);
        printf("before ascii\n");
    mat_ascii = ascii(i, j, mat_sobel);
    //mat_ascii = ascii(i, j, mat_double_gray);
    free(mat_sobel);
    print_img(i, j, mat_ascii);


	getchar();
}
