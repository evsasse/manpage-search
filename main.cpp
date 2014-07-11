#include <stdio.h>
#include <cstring>
#include <iostream>
using namespace std;
#include "ManPage.h"
#include "AvlTree.h"


struct Indice{
    char comando[100];
    int posicao;
    //Define os operadores para poder usar na AvlTree
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
    
    AvlTree<char> a;
    a.insert('a');
    a.insert('b');
    a.insert('c');
    a.insert('d');
    a.insert('e');
    a.insert('f');
    a.insert('g');
    a.insert('h');
    cout << a.toString();
    int maxSize = a.getMaxSizeByHeight();
    char* b;
    b = a.getByLevel();
    for(int i=0; i<maxSize;++i){
        cout << '\n';
        cout << b[i];
    }
    
    //gerarManPagesDat(argc,argv);
}

void gerarManPagesDat(int argc, char* argv[]){
    //Fazendo a árvore em memória para não demorar tanto a geração...
    AvlTree<Indice> indices;
    
    //Abre para sobrescrever ou cria o arquivo manpages.dat
    FILE *manPagesDat;
    manPagesDat = fopen("..\\manpages.dat","w");
    
    for(int i=1;i<argc;++i){
        //Abre para ler o arquivo de cada manpage
        FILE *manPageF;
        manPageF = fopen(argv[i],"r");
        if (manPageF == NULL) perror ("Erro abrindo arquivo");
        
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
        //Finalizou a escrita em manPagesDat
        
        //Adiciona o arquivo e seu indice na árvore de indice principal
        //"i-1" corresponde a posição que foi salvo o registro do comando em manpages.dat
        Indice manPage;
        *manPage.comando = *comando;
        manPage.posicao = i-1;
        indices.insert(manPage);
    }
    
    //A arvore "indices" deve conter todos os indices ordenados em forma de árvore para serem salvos em disco
    //Ela é retornada em ordem de nível para ser escrita nessa ordem
    //tree.root->height
    Indice indicesOrdenados[indices.getMaxSizeByHeight()];
    //indices.getByLevel(indicesOrdenados);
    
    fclose(manPagesDat);
}