/* Bompa Remus 335CB */.

1) Super Sampling Anti Aliasing

- pentru a citi imaginea am creat structura image avand campurile: type( tipul formatului pnm: 6
pentru cel color si 5 pentru cel grayscale), width si height( latimea si inaltimea imaginii), 
maxval( valoarea maxima a culorii, de maxim 255) si pixels( matricea in care se retin culorile
fiecarui pixel). Un pixel din cadrul matricei pixels poate fi color sau grayscale (in functie 
de tipul imaginii), motiv pentru care am definit inca 2 structuri: ColorP ( pentru un pixel 
color, avand culorile r(rosu), g(verde), b(albastru)) si GrayscaleP, avand doar o culoare.

- in functia readInput, sunt citite pe rand toate campurile structurii image: type, width, 
height, maxval din fisierul de intrare identificat prin numele fileName, dupa care in functie
de tipul imaginii se citesc fie pixelii ca structuri ColorP fie ca structuri GrayscaleP.

- functia resize primeste ca parametri un pointer catre imagnea initiala in si unul catre 
imaginea finala care trebuie contruita. Se initializeaza campurile imaginii out, pana la matricea
de pixeli, care va fi obtinuta prin paralelizare: noua inaltime si latime nheight si nwidth se
obtin impartind dimensiunile vechi la resize_factor, tipul type si valoarea maxima a culorii sunt
aceleasi ca in vechea imagine. Se aloca matricea de pixeli, fie ca o matrice de structuri ColorP
fie de structuri GrayscaleP, in functie de tipul type.
- pentru paralelizarea calculului matricei de pixeli, am impartit grupurile de resize_factor^2
pixeli (care vor reprezenta un singur pixel in noua matrice) in mai multe intervale (egal cu 
numarul de thread uri num_threads primit ca parametru in linia de comanda). Parametrii de care
are nevoie fiecare thread pentru a efectua calculele necesare pe un numar de grupuri de pixeli
aflate intr-un anumit interval i-am grupat in structura GrupT: start, end , in si out. In si
out reprezinta pointerii catre imaginea initiala si cea finala in vreme ce start si end reprezinta
numarul de ordinea al primului si al utimului grup de pixeli procesat in thread ul curent (am 
grupurile de pixeli ordonate de la 0 pana la nr_grupuriP-1, unde nr_grupuriP = nheight * nwidth).

- pentru a grupa grupurile de pixeli in thread uri determin: chunks - nr de de grupri dintr-un
thread in cazul in care nr de grupuri s-ar imparti exact la numarul de thread-uri, si reminder -
restul impartirii numarului de grupuri la numarul de thread uri. Restul este mai mic decat 
num_threads, deci il pot distribui la primele reminder thread uri pentru a avea un numar cat mai
egal de grupuri pe thread.Folosesc un indice p pentru a retine start ul urmatorului thread, initial
fiind 0. Cat timp am rest (reminder>0), thread-ul va evea chunks+1 grupuri, deci end = start +
chunks - 1 + 1, iar p se actualizeaza la end+1. Dupa ce nu mai am rest, un thread va avea doar
chunks grupuri, deci end va fi p + chunks -1. In final, creez cele num_threads thread uri, fiecare
dintre ele apeland functia calculMedie cu parametrul &group_threads[i], group_threads[i] continand
parametrii: start, end, in si out ptr thread ul i. Astept terminarea fiecarui thread i prin apelul
functiei pthread_join(tid[i], NULL). 

- functia calculMedie primeste ca parametru adresa unei structuri GrupT: &group_threads[i], din
care se determina intervalul [start,end] de grupuri de pixeli procesate de thread, imaginea de
intrare in si cea de iesire out. Un grup de pixeli reprezinta de fapt un pixel in noua matrice
out si va avea o linie si o coloana corespunzatoare in functie de numarul sau de ordine de la
start la end. Considerand ca numerele de ordine incep din coltul de stanga sus al matricei (nr 0) si
cresc la dreapta, pe linie si apoi in jos, pe verticala pana la coltul din dreapta jos al matricei
(nr nr_grupuriP - 1), atunci numarul de linie si coloana in noua matrice pentru grupurile start
si end sunt: 		int istart = start / nwidth, jstart = start % nwidth;
						int iend = end / nwidth, jend = end % nwidth;

- cerinta specifica procesarea pixelilor in 2 cazuri: resize_factor este par sau este 3, pentru
fiecare din aceste cazuri existand 2 subcazuri corespunzatoare tipului imaginii: imaginea este 
color sau alb-negru. In fiecare din cele 4 cazuri, va exista o bucla infinita, in care se trece 
de la un grup de pixeli la altul din intervalul [start,end] si care se termina dupa ce s-au procesat
pixelii corespunzatori grupului end (de pe (iend,jend) in noua matrice). De asemenea, pentru fiecare
caz, se vor calcula coordonatele corespunzatoare primului (starth,startw) si ultimului pixel (endh,endw)
din veachea matrice pentru grupul de coordonate (i,j) in noua matrice.Deci, pentru toate cele 4 cazuri 
bucla va arata asa:
	while(1)
			{
				int starth = resize_factor * i;
				int endh = resize_factor * (i+1) - 1;
				int startw = resize_factor * j;
				int endw = resize_factor * (j+1) - 1;

				//procesare pixeli din grupul de pe pozitia (i,j)

				if( i == iend && j == jend) break;
				j++;
				if(j == nwidth)
				{
					j=0;
					i++;
				}
			}
- procesare pixeli din grupul de pe pozitia (i,j)
	*)pentru resize_factor par si imagine color:
	-se parcurg pixelii din grup si se acumuleaza suma pentru fiecare culoare in variabilele aux_r, 
aux_g, aux_b si numarul in vaiabila nr (resize_factor^2). Se impart aux_r, aux_g si aux_b la nr, 
aflandu-se media iar aceasta se salveaza in pixelul (i,j) din imaginea de iesire.
	*)pentru resize_factor par si imagine alb-negru
	-se parcurg pixelii din grup si se acumuleaza suma in variabila aux si numarul lor in vaiabila nr 
(resize_factor^2). Se imparte aux la nr, aflandu-se media iar aceasta se salveaza in pixelul (i,j) 
din imaginea de iesire.
	*)pentru resize_factor 3 si imagine color
	-se defineste kernel-ul gausian: int kernel[3][3] = { {1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
	-se parcurg pixelii din grup si se acumuleaza suma pentru fiecare culoare in variabilele aux_r,
 	aux_g, aux_b si numarul in vaiabila nr (resize_factor^2). Suma pe o culare se face adunand culoarea
	inmultita cu valoarea din matricea kernelului aflata pe pozitia pe care se afla pixelulul in grup. 
	(culorile pentru pixelul (ip,jp) se inmultesc cu k = kernel[ip - starth][jp - startw] ). Se impart 
	aux_r, aux_g si aux_b la nr, aflandu-se media iar aceasta se salveaza in pixelul (i,j) din imaginea 
	de iesire.
	*)pentru resize_factor 3 si imagine alb-negru
	-se foloseste acelasi kernel gausian
	-se parcurg pixelii din grup si se acumuleaza suma pentru in variabila aux si numarul in vaiabila 
	nr (resize_factor^2). Suma se face adunand culoarea inmultita cu valoarea din matricea kernelului 
	aflata pe pozitia pe care se afla pixelulul in grup. (culoarea pentru pixelul (ip,jp) se inmulteste
	cu k = kernel[ip - starth][jp - startw] ). Se imparte aux la nr, aflandu-se media iar aceasta se 
	salveaza in pixelul (i,j) din imaginea de iesire.

- functia writeData scrie pe rand in fisierul de iesire identificat prin numele fileName, primit
ca parametru in linia de comanda: type, width si height, maxval ale imginii obtinute. Dupa tipul
imaginii de iesire, se scriu pixelii fie ca structuri ColorP fie ca structuri GrayscaleP.

2) Micro renderer

- pentru acest punct am folosit aceleasi structuri ca la punctul precedent: image, ColorP, GrayscaleP si
GrupT (care are campurile start,end si in , nu mai are out)
- functia initialize primeste ca parametru un poiinter im catre imaginea care trebuie construita si 
initializeaza imaginea: se atribuie tipul (5 pentru imagine alb-negru), width si height = resolution,
primit ca parametru in linia de comanda, maxval de 255 (culoarea maxima este alb) si se aloca memorie
pentru matricea de pixeli pixels pentru a avea resolution^2 structuri de tip GrayscaleP.

- functia render paralelizeaza construirea matricei prin impartirea celor resolution^2 pixeli, ce trebuie
colorati pentru a construi imaginea, la num_threads thread uri. Impartirea am realizat-o in acelasi mod
in care am impartit, la punctul precedent, cele nr_grupuriP grupuri la num_threads thread-uri, construind
un vector GrupT group_threads [num_threads], in care salvez inceputul si sfarsitul (inclusiv) al intervalului
procesat de un thread. In final creez cele num_threads thread-uri prin instructiunea:
	pthread_create(&(tid[i]), NULL, calculVal, &group_threads[i]) pentru i=0:num_threads-1
si astept terminarea fiecarui thread creat prin instructiunea:
	pthread_join(tid[i], NULL) pentru i=0:num_threads-1

- fiecare thread i va apela functia calculVal cu parametru &group_threads[i]. Folosind informatiile salvate
in parametru &group_threads[i], se determina start,end si in. Considerand ca numerele de ordine incep din 
coltul de stanga sus al matricei (nr 0) si cresc la dreapta, pe linie si apoi in jos, pe verticala pana la 
coltul din dreapta jos al matricei (nr resolution*resolution - 1), atunci numarul de linie si coloana in 
noua matrice pentru grupurile start si end sunt: 	int istart = start / resolution, jstart = start % resolution;
																	int iend = end / resolution, jend = end % resolution;

- exista o bucla while(1), in care se trece de la un pixel la altul din intervalul [start,end] si care se termina
dupa ce s-au procesat pixelul end (iend,jend). Initial i =istart si j = jstart. Deoarece, coordonatele imaginii 
sunt (0,0) is stanga jos si (max,max) in dreapta sus, inseamna ca i trebuie sa creasca de jos in sus iar j se la
stanga la drapta, deci ip = resolution - 1 - i si jp = j, sunt coordonatele pixelului (i,j) daca mut originea de 
stanga sus, stanga jos. Deoarece distanta se calculeaza fata de centrul pixelului, pe verticala pixelul se afla
la ip = reolution - 1 - i + 0.5 iar pe orizontala la jp = j + 0.5 fata de (0,0). Pentru dreapta d: a*x + b*y +c = 0,
distanta de la (x0,y0) la d este: abs(a*x0 + b*y0 + c)/sqrt(a^2 + b^2). In acest caz d: -x + 2*y + 0 = 0, distanta
dist in pixeli este abs(2*ip - jp) iar distanta in cm va fi dist = abs(2*ip - jp) * 100 / resolution ( la resolution
pixeli corespund 100cm deci unui pixel ii corespunde 100/resolution cm). Daca distanta dist obtinuta este <= 3, 
atunci pixelul face parte din dreapta si va fi negru (0), altfel va fi alb (255).

- functia writeData primeste ca parametru un pointer catre imaginea construita img si numele fisierului
in care se va scrie imaginea, primit ca parametru in linia de comanda. Sunt scrise tipul, width, height
maxval si matricea de pixeli corespunzatoare pointerului img de tip image*.  
