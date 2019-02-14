#ifndef HOMEWORK_H1
#define HOMEWORK_H1

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

typedef struct {
	int type;
	int width;
	int height;
	unsigned char maxval;
	void *pixels;
}image;

typedef struct{
	unsigned char r,g,b;
}ColorP;

typedef struct{
	int start;
	int end;
	image *in;
}GrupT;

typedef unsigned char GrayscaleP;

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);

#endif /* HOMEWORK_H1 */
