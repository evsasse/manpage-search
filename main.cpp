#include <stdio.h>
#include <cstring>

#include "ManPage.h"
using namespace std;

void gerarManPagesDat(int argc, char* argv[]);

static const int comandoTamMax = 100;
static const int conteudoTamMax = 140000;

int main(int argc, char* argv[]){
    
    gerarManPagesDat(argc,argv);
    
}

void gerarManPagesDat(int argc, char* argv[]){
    FILE *manPagesDat;
    manPagesDat = fopen("..\\manpages.dat","w");
    
    for(int i=1;i<argc;++i){
        FILE *manPageF;
        manPageF = fopen(argv[i],"r");
        
        if (manPageF == NULL) perror ("Erro abrindo arquivo");
        
        char comando[comandoTamMax];
        *strrchr(argv[i],'.') = '\0';
        strcpy(comando,argv[i]);
        
        char conteudo[conteudoTamMax];
        for(int a=0; a<conteudoTamMax; ++i){
            conteudo[i] = ' ';
        }
        int aa;
        for(aa=0;!feof(manPageF);++aa){
            //printf("AAA");
            conteudo[aa] = fgetc(manPageF);
        }
        conteudo[aa-1] = '\0';
        
        bool charFinal = false;
        for(int a=0;a<comandoTamMax;++a){//Escreve o comando
            if(charFinal)
                fputc(' ',manPagesDat);
            else{
                if(comando[a] == '\0'){
                    charFinal = true;
                    fputc(' ',manPagesDat);
                }else
                    fputc(comando[a],manPagesDat);
            }
        }
        charFinal = false;
        for(int a=0;a<conteudoTamMax;++a){//Escreve o conteúdo
            fputc(conteudo[a],manPagesDat);
        }
    }
    
    fclose(manPagesDat);
}
