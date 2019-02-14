#include "homework.h"

int num_threads;
int resize_factor;

int nheight, nwidth;

void readInput(const char * fileName, image *img) {
	FILE* fin = fopen( fileName, "r");
	fscanf(fin, "P%d\n", &img -> type);
	fscanf(fin, "%d %d\n", &img -> width, &img -> height);
	fscanf(fin, "%hhu\n", &img -> maxval);
	if(img -> type == 6)
	{
		img -> pixels = malloc( sizeof(ColorP*) * img -> height);
		int i;
		for( i=0; i< img -> height; i++ ){
			((ColorP**)img -> pixels)[i] = (ColorP*)malloc( sizeof(ColorP) * img -> width);
			int j;
			for( j=0; j< img -> width; j++){
				ColorP* pixel= &((ColorP**)img -> pixels)[i][j];
				unsigned char culori[3];
				fread( culori, 1, 3, fin);
				pixel -> r = culori[0]; 
				pixel -> g = culori[1];
				pixel -> b = culori[2];
			}
		}
	}
	else if(img -> type == 5)
	{
		img -> pixels = malloc( sizeof(GrayscaleP*) * img -> height);
		int i;
		for( i=0; i< img -> height; i++ ){
			((GrayscaleP**)img -> pixels)[i] = (GrayscaleP*)malloc( sizeof(GrayscaleP) * img -> width);
			int j;
			for( j=0; j< img -> width; j++){
				GrayscaleP* pixel= &((GrayscaleP**)img -> pixels)[i][j];
				fread( pixel, 1, 1, fin);
			}
		}
	}
}

void writeData(const char * fileName, image *img) {

	FILE* fout = fopen( fileName, "wb");
	
	fprintf(fout, "P%d\n", img -> type);
	fprintf(fout, "%d %d\n", img -> width, img -> height);
	fprintf(fout, "%d\n", img -> maxval);
	int i,j;

	if( img -> type == 6)
	{
		for(i= 0; i< img -> height; i++){
			for( j=0; j< img -> width; j++){
				ColorP* pixel= &((ColorP**)img -> pixels)[i][j];
				unsigned char culori[3];
				culori[0] = pixel -> r; 
				culori[1] = pixel -> g; 
				culori[2] = pixel -> b; 
				fwrite( culori, 1, 3, fout);
			}
		}
	}
	else if( img -> type == 5){
		for(i= 0; i< img -> height; i++){
			for( j=0; j< img -> width; j++){
				GrayscaleP* pixel= &((GrayscaleP**)img -> pixels)[i][j];
				fwrite( pixel, 1, 1, fout);
			}
		}
	}
}

void* calculMedie(void *var)
{
	GrupT* groupT = (GrupT*)var;
	int start = groupT -> start;
	int end = groupT -> end;
	image *in = groupT -> in, *out = groupT -> out;
	
	//determinare numar linie si coloana in noua imagine ptr grupurile start si end 
	int istart = start / nwidth, jstart = start % nwidth;
	int iend = end / nwidth, jend = end % nwidth;

	//parcurgere grupuri de pixeli de la grupul (istart,jstart) la (iend,jend)
	int i=istart, j=jstart;
		//aplicare operatie pe grupul de pixeli

		//cazul formatului pnm color

	if(resize_factor % 2 == 0){ 
		if( in -> type == 6){
			//resize_factor par si imagine color
			while(1)
			{
				ColorP** pixels_in = ((ColorP**)in -> pixels);
				ColorP** pixels_out = ((ColorP**)out -> pixels);
			
				//parcurgere pixeli din grupul (i,j) si acumulare suma in aux_r,aux_g 
				//si aux_b pentru toate cele 3 culori

				int starth = resize_factor * i;
				int endh = resize_factor * (i+1) - 1;
				int startw = resize_factor * j;
				int endw = resize_factor * (j+1) - 1;

				int ip,jp,nr=0;
				ColorP* culoriP = &pixels_out[i][j];
				int aux_r = 0, aux_g = 0, aux_b = 0;
				for( ip = starth; ip <= endh; ip++){
					for( jp = startw; jp <= endw; jp++){
						ColorP *culori = &pixels_in[ip][jp];
						aux_r += culori -> r;
						aux_g += culori -> g;
						aux_b += culori -> b;
						nr++;
					}
				}

				aux_r /= nr;
				aux_g /= nr;
				aux_b /= nr;

				culoriP -> r = aux_r;
				culoriP -> g = aux_g;
				culoriP -> b = aux_b;

				if( i == iend && j == jend) break;
				j++;
				if(j == nwidth)
				{
					j=0;
					i++;
				}
			}
		}
			//cazul formatului pnm grayscale
		else if ( in -> type == 5){
			//resize factor par si imagine alb-negru
			while(1)
			{
				GrayscaleP** pixels_in = ((GrayscaleP**)in -> pixels);
				GrayscaleP** pixels_out = ((GrayscaleP**)out -> pixels);
			
				//parcurgere pixeli din grupul (i,j) si acumulare suma in aux

				int starth = resize_factor * i;
				int endh = resize_factor * (i+1) - 1;
				int startw = resize_factor * j;
				int endw = resize_factor * (j+1) - 1;

				int ip,jp,nr=0;
				int aux = 0;
				for( ip = starth; ip <= endh; ip++){
					for( jp = startw; jp <= endw; jp++){
						GrayscaleP culoriP = pixels_in[ip][jp];
						aux += culoriP;
						nr++;
					}
				}
				aux /= nr;
				//plasare valoare medie a grupului in pixelul din noua imagine
				pixels_out[i][j] = (GrayscaleP)aux;

				if( i == iend && j == jend) break;
				j++;
				if(j == nwidth)
				{
					j=0;
					i++;
				}
			}
		}
	}
	else if(resize_factor == 3){
		int kernel[3][3] = { {1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
		if( in -> type == 6){
			//resize_factor 3 si imagine color
			while(1)
			{
				ColorP** pixels_in = ((ColorP**)in -> pixels);
				ColorP** pixels_out = ((ColorP**)out -> pixels);
			
				//parcurgere pixeli din grupul (i,j) si acumulare suma in aux_r,aux_g 
				//si aux_b pentru toate cele 3 culori

				int starth = resize_factor * i;
				int endh = resize_factor * (i+1) - 1;
				int startw = resize_factor * j;
				int endw = resize_factor * (j+1) - 1;

				int ip,jp;

			

				ColorP* culoriP = &pixels_out[i][j];
				int aux_r = 0, aux_g = 0, aux_b = 0;
				for( ip = starth; ip <= endh; ip++){
					for( jp = startw; jp <= endw; jp++){
						int k = kernel[ip - starth][jp - startw];
						ColorP *culori = &pixels_in[ip][jp];
						aux_r += culori -> r * k;
						aux_g += culori -> g * k;
						aux_b += culori -> b * k;
					}
				}

				culoriP -> r = aux_r / 16;
				culoriP -> g = aux_g / 16;
				culoriP -> b = aux_b / 16;

				if( i == iend && j == jend) break;
				j++;
				if(j == nwidth)
				{
					j=0;
					i++;
				}
			}
		}
		//cazul formatului pnm grayscale
		else if ( in -> type == 5){
			//resize_factor 3 si imagine alb-negru
			while(1)
			{
				GrayscaleP** pixels_in = ((GrayscaleP**)in -> pixels);
				GrayscaleP** pixels_out = ((GrayscaleP**)out -> pixels);
			
				//parcurgere pixeli din grupul (i,j) si acumulare suma in aux

				int starth = resize_factor * i;
				int endh = resize_factor * (i+1) - 1;
				int startw = resize_factor * j;
				int endw = resize_factor * (j+1) - 1;

				int ip,jp;
				int aux = 0;
				for( ip = starth; ip <= endh; ip++){
					for( jp = startw; jp <= endw; jp++){
						GrayscaleP culoriP = pixels_in[ip][jp];
						int k = kernel[ip - starth][jp - startw];
						aux += culoriP * k;
					}
				}

				aux /= 16;
				//plasare valoare medie a grupului in pixelul din noua imagine
				pixels_out[i][j] = (GrayscaleP)aux;

				if( i == iend && j == jend) break;
				j++;
				if(j == nwidth)
				{
					j=0;
					i++;
				}
			}
		}
	}

	return 0;
}

void resize(image *in, image * out) { 
	int i;
	//aflare inaltime si latime noua a imaginii
	nheight = in -> height / resize_factor;
	nwidth = in -> width / resize_factor;

	//initializare imagine pana la matricea de pixeli
	out -> height = nheight;
	out -> width = nwidth;
	out -> type = in -> type;
	out -> maxval = in -> maxval;
	if( in -> type == 6){
		out -> pixels = malloc( nheight * sizeof(ColorP*));
		ColorP** pixels = ((ColorP**)out -> pixels);
		for(  i = 0; i < nheight; i++){
			pixels[i] = (ColorP*) malloc( nwidth * sizeof(ColorP));
		}
	}
	else if( in -> type == 5){
		out -> pixels = malloc( nheight * sizeof(GrayscaleP*));
		GrayscaleP** pixels = ((GrayscaleP**)out -> pixels);
		for(  i = 0; i < nheight; i++){
			pixels[i] = (GrayscaleP*) malloc( nwidth * sizeof(GrayscaleP));
		}
	}

	int nr_grupuriP = nheight * nwidth;
	//grupare grupuri pixeli in thread-uri => det start si end 
	GrupT group_threads [num_threads];
	int chunks = nr_grupuriP / num_threads;
	int reminder = nr_grupuriP % num_threads;

	int p = 0;
	i=0;
	while( reminder > 0 )
	{
		GrupT* agrup = &group_threads[i];
		agrup -> start = p;
		agrup -> end = p + chunks - 1 + 1;
		reminder--;
		p = agrup -> end + 1;

		agrup -> in = in;
		agrup -> out = out;
		i++;

	}
	while( i < num_threads ){
		GrupT* agrup = &group_threads[i];
		agrup -> start = p;
		agrup -> end = p + chunks - 1;
		p = agrup -> end + 1;

		agrup -> in = in;
		agrup -> out = out;

		i++;
	}
	
	//creez si astept sa se execute thread-urile
	pthread_t tid[num_threads];
	for( i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, calculMedie, &group_threads[i]);
	}

	for( i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}


