#include <stdio.h>
#include <cstring>
#include <iostream>
using namespace std;
#include "AvlTree.h"


struct Indice{
    char comando[100];
    int posicao;
    //Define os operadores para poder usar na AvlTree
    Indice(){
        for(int i=0;i<100;++i){
            comando[i] = ' ';
        }
        posicao = 0;
    }
    bool operator==(const Indice& rhs) const
    {
        if (strcmp(comando,rhs.comando) == 0) return true;
        return false;
    }
    bool operator<(const Indice& rhs) const
    {
        if (strcmp(comando,rhs.comando) < 0) return true;
        return false;
    }
    
    bool operator>(const Indice& rhs) const
    {
        if (strcmp(comando,rhs.comando) > 0) return true;
        return false;
    }
};



void gerarManPagesDat(int argc, char* argv[]);

static const int comandoTamMax = 100;
static const int conteudoTamMax = 149900;

int main(int argc, char* argv[]){
    printf("Começou");
    char* rgv[5];
    for(int i=0;i<5;++i){
        strcpy(rgv[i],"locale.5.txt");
    }
    printf("Terminou");
    gerarManPagesDat(argc,rgv);
}

void gerarManPagesDat(int argc, char* argv[]){
    //Fazendo a árvore em memória para não demorar tanto a geração...
    AvlTree<Indice> indices;
    
    ///////////////////////////////////
    ///// COMEÇA A MANPAGES.DAT
    ///////////////////////////////////
    //Abre para sobrescrever ou cria o arquivo manpages.dat
    FILE *manPagesDat;
    manPagesDat = fopen("..\\manpages.dat","w");
    printf("Abriu manpages.dat");
    
    for(int i=1;i<argc;++i){
        //Abre para ler o arquivo de cada manpage
        FILE *manPageF;
        manPageF = fopen(argv[i],"r");
        if (manPageF == NULL) perror ("Erro abrindo arquivo");
        
        printf("Abriu %s",argv[1]);
        
        //Gera o nome do comando a partir da entrada de argv[i]], tirando a extensão .txt, e coloca em "comando"
        char comando[comandoTamMax];
        *strrchr(argv[i],'.') = '\0';
        strcpy(comando,argv[i]);
        
        //Lê o conteúdo do arquivo correspondente e coloca em em "conteudo"
        char conteudo[conteudoTamMax];
        for(int a=0; a<conteudoTamMax; ++a){
            conteudo[a] = ' ';
        }
        int aa;
        int linhas = 0;
        for(aa=0;!feof(manPageF);++aa){
            //printf("AAA");
            conteudo[aa] = fgetc(manPageF);
            if(conteudo[aa] == '\n') ++linhas;
        }
        conteudo[aa-1] = '\0';
        //Fecha o arquivo que estava lendo
        fclose(manPageF);
        
        bool charFinal = false;
        //Escreve o comando em manpages.dat
        for(int a=0;a<comandoTamMax;++a){
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
        //Escreve o conteúdo em manpages.dat
        //Diminui a quantidade de linhas pois os \n contam como 2 caracteres, então 2 bytes, "pula a linha, e retorno do carro"
        for(int a=0;a<conteudoTamMax-linhas;++a){
            fputc(conteudo[a],manPagesDat);
        }
        
        //Adiciona o arquivo e seu indice na árvore de indice principal
        //"i-1" corresponde a posição que foi salvo o registro do comando em manpages.dat
        Indice manPage;
        //printf("%s\n",comando);
        strcpy(manPage.comando,comando);
        //printf("%s\n",manPage.comando);
        manPage.posicao = i-1;
        indices.insert(manPage);
    }
    //fecha manPagesDat
    fclose(manPagesDat);
    ///////////////////////////////////
    ///// TERMINOU A MANPAGES.DAT
    ///////////////////////////////////
    ///// COMEÇA A INDICES.DAT
    ///////////////////////////////////
    
    //A arvore "indices" deve conter todos os indices ordenados em forma de árvore para serem salvos em disco
    //Ela é retornada em ordem de nível para ser escrita nessa ordem
    int maxSize = indices.getMaxSizeByHeight();
    Indice* indicesOrdenados;
    indicesOrdenados = indices.getByLevel();
    
    //Abre ou cria o arquivo indices.dat para escrita
    FILE* indicesDat;
    indicesDat = fopen("..\\indices.dat","w");
    
    //percorre cada indice da lista
    for(int i=0;i<maxSize;++i){
        //coloca os valores que serão passado para o arquivo em "comando"
        char comando[100];
        //strcpy(comando,indicesOrdenados[i].comando);
        for(int c=0;c<comandoTamMax;++c){
            comando[c] = indicesOrdenados[i].comando[c];
        }
        //printf("%s\n",indicesOrdenados[i].comando);
        //printf("%s\n",comando);
        //e em "posicao"
        int posicao = indicesOrdenados[i].posicao;
        //troca os 4 ultimos bytes de comando, guardando a posição.
        //*((int*)&a[96]) = 123;
        *((int*)&comando[96]) = posicao;
        //printf("%s\n",comando);
        
        for(int i=0;i<comandoTamMax;++i){
            fputc(comando[i],indicesDat);
        }
    }
    delete indicesOrdenados;
    
    //fecha o arquivo indices.dat
    fclose(indicesDat);
    
    ///////////////////////////////////
    ///// TERMINOU A INDICES.DAT
    ///////////////////////////////////
}