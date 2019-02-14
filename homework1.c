#include "homework1.h"

int num_threads;
int resolution;

image* img;

void initialize(image *im) {
	im -> type = 5;
	im -> width = resolution;
	im -> height = resolution;
	im -> maxval = 255;
	
	im -> pixels = malloc( sizeof(GrayscaleP*) * im -> height);
	int i;
	for( i=0; i< im -> height; i++ ){
		((GrayscaleP**)im -> pixels)[i] = (GrayscaleP*)malloc( sizeof(GrayscaleP) * im -> width);	
	}
}

void* calculVal(void *var)
{
	GrupT* groupT = (GrupT*)var;
	int start = groupT -> start;
	int end = groupT -> end;
	image *in = groupT -> in;
	
	int istart = start / resolution, jstart = start % resolution;
	int iend = end / resolution, jend = end % resolution;


	//parcurgere pixeli de la pixelul (istart,jstart) la (iend,jend)
	int i=istart, j=jstart;
	GrayscaleP** pixels_in = ((GrayscaleP**)in -> pixels);

	while(1)
		{
			
			double ip = ( (resolution  - i -1 + 0.5 ) ), jp = (j + 0.5);
			double dist = (2 * ip - jp);

			dist *= 100;
			dist /= resolution;
			
			dist = abs(dist);

			dist /= sqrt(5);
			
			if( dist <= 3 ) pixels_in[i][j] = 0; //negru
			else pixels_in[i][j] = 255; //alb

			if( i == iend && j == jend) break;
			j++;
			if(j == resolution)
			{
				j=0;
				i++;
			}
		}
	return 0;
}

void render(image *im) {

	int nr_grupuriP = resolution * resolution;

	//grupare grupuri pixeli in thread-uri
	GrupT group_threads [num_threads];
	int chunks = nr_grupuriP / num_threads;
	int reminder = nr_grupuriP % num_threads;

	int p = 0;
	int i=0;
	while( reminder > 0 )
	{
		GrupT* agrup = &group_threads[i];
		agrup -> start = p;
		agrup -> end = p + chunks - 1 + 1;
		reminder--;
		p = agrup -> end + 1;

		agrup -> in = im;
		i++;

	}
	while( i < num_threads ){
		GrupT* agrup = &group_threads[i];
		agrup -> start = p;
		agrup -> end = p + chunks - 1;
		p = agrup -> end + 1;

		agrup -> in = im;

		i++;
	}
	
	//creez si astept sa se execute thread-urile
	pthread_t tid[num_threads];
	for( i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, calculVal, &group_threads[i]);
	}

	for( i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

}

void writeData(const char * fileName, image *img) {
	FILE* fout = fopen( fileName, "wb");
	
	fprintf(fout, "P%d\n", img -> type);
	fprintf(fout, "%d %d\n", img -> width, img -> height);
	fprintf(fout, "%d\n", img -> maxval);
	int i,j;

	
	for(i= 0; i< img -> height; i++){
		for( j=0; j< img -> width; j++){
			GrayscaleP* pixel= &((GrayscaleP**)img -> pixels)[i][j];
			fwrite( pixel, 1, 1, fout);
		}
	}
}

