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
    manPagesDat = fopen("manpages.dat","w");
    
    for(int i;i<argc;++i){
        FILE *manPageF;
        manPageF = fopen(argv[i],"r");
        
        if (manPageF == NULL) perror ("Erro abrindo arquivo");
        
        char comando[comandoTamMax];
        *strrchr(argv[i],'.') = '\0';
        strcpy(comando,argv[i]);
        
        char conteudo[conteudoTamMax];
        for(int i=0; i<conteudoTamMax; ++i){
            conteudo[i] = ' ';
        }
        while(!feof(manPageF)){
            conteudo[i] = fgetc(manPageF);
        }
        conteudo[conteudoTamMax] = '\0';
        
        bool charFinal = false;
        for(int i=0;i<comandoTamMax;++i){//Escreve o comando
            if(charFinal)
                fputc(' ',manPagesDat);
            else{
                if(comando[i] == '\0'){
                    charFinal = true;
                    fputc(' ',manPagesDat);
                }else
                    fputc(comando[i],manPagesDat);
            }
        }
        for(int i=0;i<conteudoTamMax;++i){//Escreve o conteúdo
            if(charFinal)
                fputc(' ',manPagesDat);
            else{
                if(comando[i] == '\0'){
                    charFinal = true;
                    fputc(' ',manPagesDat);
                }else
                    fputc(conteudo[i],manPagesDat);
            }
        }
    }
    
    fclose(manPagesDat);
}
