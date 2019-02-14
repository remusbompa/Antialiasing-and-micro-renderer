#ifndef HOMEWORK_H
#define HOMEWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

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
	image *in, *out;
}GrupT;

typedef unsigned char GrayscaleP;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

#endif /* HOMEWORK_H */
