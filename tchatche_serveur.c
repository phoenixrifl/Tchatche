#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#define TAILLE_MESSAGE 256

typedef struct utilisateur utilisateur;
struct utilisateur{
  int entree_tube;
  char pseudo[25];
  int id;
};
  
utilisateur liste[10];
int nbr=0;
int sortie_serveur;

int connexion(){
  if(access("serveur.fifo",F_OK)!=0)
    mkfifo("serveur.fifo",0644);
  sortie_serveur=open("serveur.fifo",O_RDONLY);
}


int nvl_connexion(int l){
  int t;
  read(sortie_serveur,&t,4);
  char pseudo[25];
  read(sortie_serveur,pseudo,t);
  read(sortie_serveur,&t,4);
  char* tube=malloc(25*sizeof(char));
  read(sortie_serveur,tube,t);
  
  int i;
  int b=1;
  for(i=0;i<nbr;i++){
    if(strcmp(liste[i].pseudo,pseudo)==0)
      b= 0;
    /*else{
      if(access(tube,F_OK)==0)
	b= 0;
	}*/
  }
  liste[nbr].entree_tube=open(tube,O_WRONLY);
  if(b=1){
    strcpy(liste[nbr].pseudo,pseudo);
    liste[nbr].id=nbr;
    int i=12;
    printf("%s\n",tube);
    write(liste[nbr].entree_tube,&i,4);
    write(liste[nbr].entree_tube,"OKOK",4);
    write(liste[nbr].entree_tube,&nbr,4);
    
    //printf("%s ajouter\n",tube);
    nbr++;
  }else{
    t=8;
    write(liste[nbr].entree_tube,&t,4);
    write(liste[nbr].entree_tube,"BADD",4);
  }
}
     
int deconnexion(int l){
  int i;
  read(sortie_serveur,&i,l-8);
  int t=12;
  write(i,&t,4);
  write(i,"BYEE",4);
  write(i,&i,4);
  for(i++;i<nbr;i++){
    if(i<=nbr)
      liste[i--]=liste[i];
  }
  nbr--;
}

int message_public(int l){
  int id;
  read(sortie_serveur,&id,4);
  char* msg=malloc(l-8);
  int t_msg;
  read(sortie_serveur,&t_msg,4);
  read(sortie_serveur,msg,t_msg);
  printf("mesage recu\n");
  int i;
  for(i=0;i<nbr;i++){
    int t=16+strlen(liste[i].pseudo)+t_msg;
    int t_psd=strlen(liste[i].pseudo);
    write(liste[i].entree_tube,&t,4);
    write(liste[i].entree_tube,"BCST",4);
    write(liste[i].entree_tube,&t_psd,4);
    write(liste[i].entree_tube,liste[i].pseudo,strlen(liste[i].pseudo));
    write(liste[i].entree_tube,&t_msg,4);
    write(liste[i].entree_tube,msg,strlen(msg));
    //printf("message envoyer: %s envoyer a %d",msg,i);

  }
}



int message(){
  int l;
  read(sortie_serveur,&l,sizeof(int));
 
  char type[4];
  read(sortie_serveur,type,4);
  if(strcmp(type,"HELO")==0){
    nvl_connexion(l);
  }else if(strcmp(type,"BYEE")==0)
    deconnexion(l);
  else if(strcmp(type,"BCST")==0)
    message_public(l);
}




  









/*int message(){
  char* msg = (char*) malloc(TAILLE_MESSAGE*sizeof(char));
  read(sortie_serveur,msg,TAILLE_MESSAGE*sizeof(char));
  if(strstr(msg,"HELO")!=NULL){
    char* mess=(char*) malloc(25*sizeof(char));
    mess=strtok(msg," ");
    printf("%s",mess);
    fflush(stdout);
    (liste[nbr]).entree_tube=open(mess,O_WRONLY);
    strcpy((liste[nbr]).pseudo,mess);
    nbr=nbr+1;
    printf("ajouter");
    fflush(stdout);
  }else{

    int i;
    for(i=0;i<nbr;i++){
      printf("%s\n",msg);
      write((liste[i]).entree_tube,msg,100*sizeof(char));
    }
  }
  }*/

int main(){
  connexion();
  int i=0;
  while(i==0)
    message();
}
