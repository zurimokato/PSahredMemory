#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAXDATOS 100

typedef struct f{
	float f1;
	float f2;
	int id;
	float fit;
}Especie;

float fitnes(float f1, float f2);
void burbuja(Especie *esp,int tam);
void manejadorSenial(int sig);
void llenarVector(Especie *esp,int tam);
void imprimirEspecies(Especie *esp, int tam);
void actualizar(Especie *esp,int desde, int hasta);
int main(){
	int numHijos=0,numIteraciones=0,tam=0,i=0;
	int shmid=0;
	signal(SIGUSR1,manejadorSenial);
	printf("Escriba el numero de iteraciones:\n");
	scanf("%d",&numIteraciones);
	printf("Escriba el numero de procesos:\n");
	scanf("%d",&numHijos);
	printf("Escriba el tamaño del vector de especies:\n");
	scanf("%d",&tam);
	pid_t padre=getpid(),hijos[numHijos];
	//vecEs=(Especie *)calloc(tam,sizeof(Especie));
	Especie *vecEs;
	shmid=shmget(IPC_PRIVATE,sizeof(Especie)*tam,IPC_CREAT|S_IRUSR|S_IWUSR);
	vecEs=shmat(shmid,0,0);
	

	llenarVector(vecEs,tam);
	printf("primer barrido antes de actualizar\n");
	//burbuja(vecEs,tam);
	imprimirEspecies(vecEs, tam);
	/*aleatoreo(vecEs,tam);
	actualizar(vecEs,tam);
	burbuja(vecEs,tam);
	aleatoreo(vecEs, tam);
	printf("despues de actualizar\n");
	imprimirEspecies(vecEs, tam);
	*/
	//Creando gerarquia
	printf("\n");
	for(i=0;i<numHijos;i++){
		if(!(hijos[i]=fork())){
			break;
		}
	}
	for (int x = 0; x< numIteraciones; x++){
		if(padre==getpid()){
			usleep(1500);
			kill(hijos[i-1],SIGUSR1);
			pause();
			//printf("antes\n");
			//imprimirEspecies(vecEs,tam);
			burbuja(vecEs,tam);
			printf("los dies mejores de la generacion %d:\n",x);
			imprimirEspecies(vecEs,10);
			
		}else{
			pause();
			int itePerHijos=tam/numHijos;
			int desde=i*itePerHijos;
			int hasta=(i*itePerHijos)+itePerHijos;
			for (desde; desde < hasta; desde++){
				//aleatoreo(vecEs, j,k);
				actualizar(vecEs,desde,hasta);
			}
			
			if(i==0){
				kill(getppid(),SIGUSR1);
			}else{
				kill(hijos[i-1],SIGUSR1);
			}
			
			
		}
		
	}

	shmdt(vecEs);
	if(padre==getpid()){
		for(int o=0;o<numHijos;o++){
			wait(NULL);
		}
		shmctl(shmid,IPC_RMID,0);
	}
	
	return 0;
}


float fitnes(float f1,float f2){
	float fit=0.0f;
	fit=0.5*(f1*f2);
	return fit;
}


void burbuja(Especie *esp, int tam){
	Especie *temp=(Especie*)malloc(sizeof(Especie));
	for(int i=0;i<tam;i++){
		for(int j=0;j<tam-1;j++){
			if(esp[j].fit<esp[j+1].fit){
				temp->f1=esp[j].f1;
				temp->f2=esp[j].f2;
				temp->fit=esp[j].fit;
				esp[j].f1=esp[j+1].f1;
				esp[j].f2=esp[j+1].f2;
				esp[j].fit=esp[j+1].fit;
				esp[j+1].f1=temp->f1;
				esp[j+1].f2=temp->f2;
				esp[j+1].fit=temp->fit;
			}
		}
		
	}

} 

void manejadorSenial(int sig){

}


void llenarVector(Especie *esp,int tam){
	
	for (int i = 0; i < tam; i++){
		Especie *es=(Especie*)malloc(sizeof(Especie));
		es->f1=rand()%20 +1;
		es->f2=rand()%15 + 1;
		es->fit=fitnes(es->f1,es->f2);
		esp[i].f1=es->f1;
		esp[i].f2=es->f2;
		esp[i].fit=es->fit;
		
	}

}

void imprimirEspecies(Especie *esp, int tam){
	for (int k = 0; k < tam; k++){
		printf(" [%f]\n",esp[k].fit);
	}
}
void actualizar(Especie *esp,int desde, int hasta){
	for(int i=desde;i<hasta;i++){
		esp[i].f1=rand()%20+1;
		esp[i].f2=rand()%10+1;
		esp[i].fit=fitnes(esp[i].f1,esp[i].f2);
	}
}