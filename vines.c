#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct source{								//risorse
	int tipo;									//0=file, 1=directory
	char nome[255];								//nome
	struct source *next;						//puntatore al fratello successivo
	struct source *child;						//puntatore al primo figlio	
	int nchild;									//numero di figli
	int scritto;								//0=nessun testo, 1=testo presente
	char *text;									//testo
	int height;									//altezza dell'albero	
};
struct percorso{
	char path[255][255];
	int pos;
};
struct find{								//struttura supporto per la find
	int h;										//posizione nel vettore
	int cont;									//numero di risorse trovate
	char nome[255];								//risorsa da trovare
	char pv[255][255];							//vettore di stringhe
};
struct source lista;						//radice

int azione();
int create();
int create_dir();
int read();
int write();
int delete_f();
int delete_r();
int find();
int searchpath();					//salva percorso su array
struct source* esplora();			//scorre in profondità l'albero	
int inserisci();					//inserisce risorsa creata	
void snd();							//trova e cancella tutti i discendenti
void search();						//cerca e salva percorso
char* scrivi();						//salva il testo da scrivere in un file
int searchpath2();					//variante per la write

int main(){
	int n, ret;
	lista.tipo=1;
	lista.child=NULL;
	lista.nchild=0;
	lista.height=0;
	
	n=azione();
	while(n!=8){
		if(n==1){
			ret=create();
			//printf("%d", ret);
			if(ret==1) printf("ok\n");
			else if(ret==0) printf("no\n");
		}
		else if(n==2){
			ret=create_dir();
			if(ret==1) printf("ok\n");
			else if(ret==0) printf("no\n");
		}
		else if(n==3){
			ret=read();
			if(ret==0) printf("no\n");
		}
		else if(n==4){
			ret=write();
			if(ret==0) printf("no\n");
		}
		else if(n==5){
			ret=delete_f();
			if(ret==1) printf("ok\n");
			else if(ret==0) printf("no\n");
		}
		else if(n==6){
			ret=delete_r();
			if(ret==1) printf("ok\n");
			else if(ret==0) printf("no\n");
		}
		else if(n==7){
			ret=find();
			if(ret==0) printf("no\n");
		}
		else if(n<0) printf("ERRORE");
		fflush(stdin);
		n=azione();
	}
}

int azione(){
	char action[15];
	char c;
	int i;
	
	c=getchar();
	for(i=0; i<14&&c!=' '&&c!='\n'; i++){
		action[i]=c;
		c=getchar();
	}
	action[i]='\0';
	if(strcmp(action, "create")==0) return 1;
	if(strcmp(action, "create_dir")==0) return 2;
	if(strcmp(action, "read")==0) return 3;
	if(strcmp(action, "write")==0) return 4;
	if(strcmp(action, "delete")==0) return 5;
	if(strcmp(action, "delete_r")==0) return 6;
	if(strcmp(action, "find")==0) return 7;
	if(strcmp(action, "exit")==0){
		//printf("\n");
		return 8;
	}
	else return 0;
}

int create(){
	int check;
	struct percorso vet;
	struct source *e, *temp;
		
	check=searchpath(&vet);
	//printf("%d\n", check);
	if(check==0) return 0;
	//printf("%d\n", vet.pos);
	//printf("%s\n", vet.path[vet.pos]);
	temp=esplora(&vet);	
	if(temp==NULL) return 0;				//errori in esplora
	
	temp->nchild++;
	//printf("%d\n", temp->nchild);
	if(temp->nchild>1024){					//la cartella ha già 1024 figli
		temp->nchild--;
		return 0;
		}
	//printf("%d\n", temp->height);	
	if(temp->height>254) return 0;			//altezza massima raggiunta
	e=malloc(sizeof(struct source));		//alloca spazio per la risorsa da creare
	strcpy(e->nome, vet.path[vet.pos]);		//inizializzazione
	e->tipo=0;
	e->height=(temp->height)+1;
	e->scritto=0;
	
	if(inserisci(e, temp)==1) return 1;
	else{
		free(e);
		return 0;
	}
}

int create_dir(){
	int check;
	struct percorso vet;
	struct source *e, *temp;
	
	check=searchpath(&vet);
	if(check==0) return 0;					//errori in path
	temp=esplora(&vet);	
	if(temp==NULL) return 0;				//errori in esplora
	temp->nchild++;
	if(temp->nchild>1024){					//la cartella ha già 1024 figli
		temp->nchild--;
		return 0;
		}	
	if(temp->height>254) return 0;			//altezza massima raggiunta
	e=malloc(sizeof(struct source));		//alloca spazio per la risorsa da creare
	strcpy(e->nome, vet.path[vet.pos]);		//inizializzazione
	e->tipo=1;
	e->height=(temp->height)+1;
	e->child=NULL;
	e->nchild=0;

	if(inserisci(e, temp)==1) return 1;
	else{
		free(e);
		return 0;
	}
}

int read(){
	int check, trovato;
	struct percorso vet;
	struct source *temp;
	
	check=searchpath(&vet);
	if(check==0) return 0;					//errori in path
	temp=esplora(&vet);	
	if(temp==NULL) return 0;				//errori in esplora
	
	temp=temp->child;
	trovato=0;
	while(temp!=NULL&&trovato==0){
		if(strcmp(temp->nome, vet.path[vet.pos])==0) trovato=1;
		else temp=temp->next;
	}
	if(trovato==0) return 0;				//la risorsa cercata non esiste
	if(temp->tipo==1) return 0;				//la risorsa è una cartella
	
	if(temp->scritto==0) printf("contenuto \n");
	if(temp->scritto==1) printf("contenuto %s\n", temp->text);
	return 1;
}

int write(){
	int check, trovato;
	struct percorso vet;
	struct source *temp;
	
	check=searchpath2(&vet);
	if(check==0) return 0;					//errori in path
	temp=esplora(&vet);	
	if(temp==NULL) return 0;				//errori in esplora
	
	temp=temp->child;
	trovato=0;
	while(temp!=NULL&&trovato==0){
		if(strcmp(temp->nome, vet.path[vet.pos])==0) trovato=1;
		else temp=temp->next;
	}
	if(trovato==0) return 0;				//la risorsa cercata non esiste
	if(temp->tipo==1) return 0;				//la risorsa è una cartella
	
	if(temp->scritto==1){
		free(temp->text);					//liberare spazio allocato
		temp->text=scrivi();	
	}
	if(temp->scritto==0){
		temp->scritto=1;
		temp->text=scrivi();	
	}
	return 1;	
}

int delete_f(){
	int check, trovato;
	struct percorso vet;
	struct source *h, *temp, *temp2;
	
	check=searchpath(&vet);
	if(check==0) return 0;					//errori in path
	temp=esplora(&vet);	
	if(temp==NULL) return 0;				//errori in esplora
	
	h=temp;
	temp2=temp->child;
	trovato=0;
	while(temp2!=NULL&&trovato==0){
			if(strcmp(temp2->nome, vet.path[vet.pos])==0) trovato=1;
			else{
				temp=temp2;
				temp2=temp2->next;
			}
		}
	if(trovato==0) return 0;						//la risorsa cercata non esiste
	if(temp2->tipo==1&&temp2->nchild!=0) return 0;	//la risorsa è una cartella con figli
	
	if(h==temp) temp->child=temp2->next;
	else temp->next=temp2->next;
		
	h->nchild--;
	if(temp2->tipo==0&&temp2->scritto==1) free(temp2->text);
	free(temp2);
	return 1;	
}

int delete_r(){
	int check, trovato;
	struct percorso vet;
	struct source *h, *temp, *temp2;
	
	check=searchpath(&vet);
	if(check==0) return 0;					//errori in path
	temp=esplora(&vet);	
	if(temp==NULL) return 0;				//errori in esplora
	
	h=temp;
	temp2=temp->child;
	trovato=0;
	while(temp2!=NULL&&trovato==0){
			if(strcmp(temp2->nome, vet.path[vet.pos])==0) trovato=1;
			else{
				temp=temp2;
				temp2=temp2->next;
			}
		}
	if(trovato==0) return 0;						//la risorsa cercata non esiste
	if(temp2->tipo==1) snd(temp2);					//libera il contenuto della cartella
	
	if(h==temp) temp->child=temp2->next;
	else temp->next=temp2->next;
		
	h->nchild--;
	if(temp2->tipo==0&&temp2->scritto==1) free(temp2->text);
	free(temp2);
	return 1;
}

int find(){
	struct find a;
	char c;
	int i;
	
	a.cont=0;
	a.h=0;
	c=getchar();
	for(i=0; c!='\n'; i++){
		if(i==254) return 0;																			//return 254!
		a.nome[i]=c;
		c=getchar();
	}
	a.nome[i]='\0';
	
	search(lista.child, &a);
	if(a.cont==0) return 0;
	return 1;
}

int searchpath(struct percorso *p){
	int i;
	char c;
	char risorsa[255];
	p->pos=0;
	c=getchar();
	while(c!='/') c=getchar();
	while(c!='\n'){
		c=getchar();
		for(i=0; c!='\n'&&c!='/'; i++){
			if(i==254) return 0;																						//return 254
			risorsa[i]=c;
			c=getchar();
		}
		risorsa[i]='\0';
		strcpy(p->path[p->pos], risorsa);
		//printf("%s\n", p->path[p->pos]);
		p->pos++;																										//controllo p==255?
	}
	p->pos--;
	return 1;
}

struct source* esplora(struct percorso *p){
	struct source *temp;
	int i, trovato;
	temp=&lista;
	
	//printf("\n%d", p->pos);
	for(i=0; i<p->pos; i++){
		temp=temp->child;
		trovato=0;
		while(temp!=NULL&&trovato==0){
				if(strcmp(temp->nome, p->path[i])==0) trovato=1;
				else temp=temp->next;
			}
		if(trovato==0) return NULL;				//la risorsa cercata non esiste
		if(temp->tipo==0) return NULL;			//la risorsa  è file, non cartella
	}	
	return temp;	
}

int inserisci(struct source *e, struct source *temp){
	struct source *temp2;
	
	if((temp2=temp->child)==NULL){							//lista vuota
		e->next=NULL;
		temp->child=e;
		return 1;
		}
			
	if(strcmp(temp2->nome, e->nome)==0) return 0;			//esiste già una risorsa con lo stesso nome
	if(strcmp(temp2->nome, e->nome)>0){						//inserisci in testa
		temp->child=e;
		e->next=temp2;
		return 1;
		}
	temp=temp2;
	temp2=temp2->next;
		
	while(temp2!=NULL){										//scorro la directory
		if(strcmp(temp2->nome, e->nome)==0) return 0;		//esiste già una risorsa con lo stesso nome
		if(strcmp(temp2->nome, e->nome)>0){					//inserisci in testa
			temp->next=e;
			e->next=temp2;
			return 1;
			}
		temp=temp2;
		temp2=temp2->next;
		}
	temp->next=e;
	e->next=NULL;
	return 1;
}

void snd(struct source *a){
	struct source *b;
	b=a->child;
	
	while (b!=NULL){
		if(b->tipo==0){
			a->child=b->next;
			if(b->scritto==1) free(b->text);
			free(b);
			b=a->child;
		}
		else if(b->tipo==1){
			snd(b);
			a->child=b->next;
			free(b);
			b=a->child;
		}
	}
}

void search(struct source *temp, struct find *p){
	int i;
	while(temp!=NULL){
		if(strcmp(temp->nome, p->nome)==0){
			printf("ok ");
			for(i=0; i<p->h; i++){
				printf("/%s", p->pv[i]);	
			}
			printf("/%s\n", p->nome);
			p->cont++;
		}
		if(temp->tipo==1){
			strcpy(p->pv[p->h], temp->nome);
			p->h++;
			search(temp->child, p);
			p->h--;
		}
		temp=temp->next;
	}	
}

char* scrivi(){
	int i, j;
	char c;
	char *p, *p2;
	
	i=1;
	j=1;
	p=malloc(i*100*sizeof(char));
	p2=p;
	getchar();																											//controllo per " all'inizio
	while((c=getchar())!='\"'){
		*p2=c;
		p2++;
		j++;
		if(j==i*100){
			i++;
			p=realloc(p, i*100*sizeof(char));
		}
	}
	*p2='\0';
	printf("ok %d\n", j-1);
	//printf("%s", p);
	return p;
}

int searchpath2(struct percorso *p){
	int i;
	char c;
	char risorsa[255];
	p->pos=0;
	c=getchar();
	while(c!='/') c=getchar();
	while(c!=' '){
		c=getchar();
		for(i=0; c!=' '&&c!='/'; i++){
			if(i==254) return 0;																						//return 254
			risorsa[i]=c;
			c=getchar();
		}
		risorsa[i]='\0';
		strcpy(p->path[p->pos], risorsa);
		//printf("%s\n", p->path[p->pos]);
		p->pos++;																										//controllo p==255?
	}
	p->pos--;
	return 1;
}



















