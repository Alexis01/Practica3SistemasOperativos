#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define N 3
/** Ejercicio fork
 ** Objetivos:
	-> entender como codificar la creacion de procesos con fork
	-> entender que TODAS las regiones de datos se duplican (pila, heap, .data, .bss...)
	-> entender lo que pasa al usar ficheros entre procesos "familares"
	
 ** Cambios/propuestas:
	-> Eliminar el "wait" final 
	-> Hacer un "open" siempre al crear un proceso (en la rama "hijo") 
	-> Cambiar el malloc de sitio 
 
** Preguntas
	-> Cuantos procesos se crean. Arbol de procesos
	-> Seria el mismo comportamiento con "hilos"? 
 
** Fuero del "scope"
	-> strcat
	-> setvuf
 **/

 int a_global=0;
char tab[N]="\0";

int main() {
	pid_t pid;
	int b_local = 0;
	int * p_heap;
	int fd;
	int i;
	FILE* file_desc;
	
	p_heap = (int*) malloc (sizeof(int));
	*p_heap = 0;
	
	fd=open("fich.txt",O_CREAT | O_RDWR | O_TRUNC,0666);
	file_desc = fdopen(fd,"r+");
	
	setvbuf(file_desc,(char*)NULL,_IOLBF, 0);
	
	for (i=0;i<N;i++) {
		if ( (pid=fork()) == -1) {
			printf("ERROR. Process %u could not fork. Exit\n",pid);
			exit(-1);
		}
		else if (pid==0) {
			//Child

#ifdef EXERCISE_OPEN
			fd=open("fich.txt",O_CREAT | O_RDWR ,0666);
			file_desc = fdopen(fd,"r+");
			setvbuf(file_desc,(char*)NULL,_IOLBF, 0);
#endif
			strcat(tab,"\t");
			
			a_global+=2; b_local+=2; (*p_heap) +=2;
			fprintf(file_desc,"%s Child. pid=%u ppid=%u. i=%d a=%d b=%d *p=%d\n",tab,getpid(),getppid(),i,a_global,b_local,*p_heap);
			fflush(NULL);

		}
		else {
			// Parent.
			a_global++; b_local++; (*p_heap)++;
			fprintf(file_desc,"%s Parent. pid=%u ppid=%u. i=%d a=%d b=%d *p=%d\n",tab,getpid(),getppid(),i,a_global,b_local,*p_heap);
			fflush(NULL);
		}
		
	}
	fclose(file_desc);

	while (wait(NULL) != -1) ;
	if (errno != ECHILD) {
		printf("ERROR when waiting for childs to finish\n");
		exit(-1);
	}

	exit(0);
	
	
	
}
