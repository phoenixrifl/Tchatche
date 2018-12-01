 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define TAILLE_MESSAGE 256

char* pseudo;
int id;
int entree_serveur;
int sortie_tube;


int connexion(){
  entree_serveur=open("serveur.fifo",O_WRONLY);
  pseudo=malloc(25*sizeof(char));
  char type[5];
  char* tube=malloc(25*sizeof(char));
  while(strcmp(type,"OKOK")!=0){ 
    printf("Tapez votre pseudo:");
    scanf("%s",pseudo);
    int b=0;
    
    while(b==0){
      printf("votre tube?");// verifie si le tube existe
      scanf("%s",tube);
      tube=strcat(tube,".fifo");
      if(access(tube,F_OK)!=0)
	b=1;
      else
	printf("tube deja existant");
    }
    
    mkfifo(tube,0644);
    int t=16+strlen(pseudo)+strlen(tube);
    int t_psd=strlen(pseudo);
    int t_tube=strlen(tube);
    write(entree_serveur,&t,4);
    write(entree_serveur,"HELO",4);
    write(entree_serveur,&t_psd,4);
    write(entree_serveur,pseudo,strlen(pseudo));
    write(entree_serveur,&t_tube,4);
    write(entree_serveur,tube,strlen(tube));
    int l=-1;
         
    sortie_tube=open(tube,O_RDONLY);
    int x=read(sortie_tube,&l,4);
    read(sortie_tube,type,4);
    type[4]='\0';
  }
  read(sortie_tube,&id,4);
  fflush(stdout);
  
    
  

  
  printf("bienvenue %s !vous etes connecté \n",pseudo);
}

int message(){
  printf("[%s]" ,pseudo);
  char* msg=malloc(TAILLE_MESSAGE*sizeof(char));
  fgets(msg,sizeof msg,stdin);
  int l=16+strlen(msg);
  int t_msg=strlen(msg);
  write(entree_serveur,&l,4);
  write(entree_serveur,"BCST",4);
  write(entree_serveur,&id,4);
  write(entree_serveur,&t_msg,4);
  write(entree_serveur,msg,l-8);
  //printf("message envoyer\n");


  /*read(sortie_tube,&l,4);
  char type[4];
  read(sortie_tube,type,4);
  char psd[25];
  int t_psd;
  read(sortie_tube,&t_psd,4,);
  read(sortie_tube,psd,t_psd);
  read(sortie_tube,&t_msg,4);
  read(sortie_tube,msg,t_msg);
  printf("[%s] %s\n",psd,msg);
  int x=read(sortie_tube,msg,t_msg);*/
}

int reception_msg(){
  char* msg=malloc(TAILLE_MESSAGE*sizeof(char));
  int l;
  int t_msg;
  read(sortie_tube,&l,4);
  char type[4];
  read(sortie_tube,type,4);
  char psd[25];
  int t_psd;
  if(strcmp(type,"BCST")){
    read(sortie_tube,&t_psd,4);
    read(sortie_tube,psd,t_psd);
    read(sortie_tube,&t_msg,4);
    read(sortie_tube,msg,t_msg);
    printf("[%s] %s\n",psd,msg);
  }
}

int liaison(){
  int b=1;
  char* msg=malloc(TAILLE_MESSAGE*sizeof(char));
  while(b==1){
    fcntl(0,F_SETFL,O_NONBLOCK);
    int x=read(0,msg,TAILLE_MESSAGE);
    printf("x: %d",x);
    if(x>1){
      fcntl(0,F_SETFL,O_NONBLOCK);
      message();
    }
  }
}


int main(){
  connexion();
  int i=0;
  while(i==0){
	if(message())
	reception_msg();
  }
}
