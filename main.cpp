#include <stdio.h>
#include <cstring>
using namespace std;
#include "AvlTree.h"


struct Indice{
    char comando[100];
    int posicao;
    //Define os operadores para poder usar na AvlTree
    Indice(){
        comando[0] = '\0';
        for(int i=1;i<100;++i){
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



void gerarArquivosDat(int argc, char* argv[]);
void buscaConteudoPorComando();

static const int comandoTamMax = 100;
static const int conteudoTamMax = 149900;

int main(int argc, char* argv[]){
    int entrada;
    do{
        printf("\n");
        printf("(1) - Gerar arquivos .dat\n");
        printf("(2) - Buscar conteudo pelo nome da manpage\n");
        printf("(3) - Buscar manpages por uma palavra\n");
        printf("(4) - Buscar manpages por duas palavras\n");
        printf("\n");
        printf("(0) - Sair\n");
        scanf("%d",&entrada);
        switch(entrada){
            case 1: gerarArquivosDat(argc,argv); break;
            case 2: buscaConteudoPorComando(); break;
            case 3: break;
            case 4: break;
        }
    }while(entrada != 0);
}

void gerarArquivosDat(int argc, char* argv[]){
    //Fazendo a árvore em memória para não demorar tanto a geração...
    AvlTree<Indice> indices;
    
    ///////////////////////////////////
    ///// COMEÇA A MANPAGES.DAT
    ///////////////////////////////////
    //Abre para sobrescrever ou cria o arquivo manpages.dat
    FILE *manPagesDat;
    manPagesDat = fopen("..\\manpages.dat","wb");
    printf("Comecou a gerar manpages.dat\n");
    
    for(int i=1;i<argc;++i){
        //Abre para ler o arquivo de cada manpage
        FILE *manPageF;
        manPageF = fopen(argv[i],"r");
        if (manPageF == NULL) perror ("Erro abrindo arquivo");
        
        printf("%d - Abriu %s\n",i,argv[i]);
        
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
        for(aa=0;!feof(manPageF);++aa){
            conteudo[aa] = fgetc(manPageF);
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
        for(int a=0;a<conteudoTamMax;++a){
            fputc(conteudo[a],manPagesDat);
        }
        
        //Adiciona o arquivo e seu indice na árvore de indice principal
        //"i-1" corresponde a posição que foi salvo o registro do comando em manpages.dat
        Indice manPage;
        strcpy(manPage.comando,comando);
        manPage.posicao = i-1;
        indices.insert(manPage);
    }
    //fecha manPagesDat
    fclose(manPagesDat);
    printf("Terminou de gerar manpages.dat\n");
    
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
    indicesDat = fopen("..\\indices.dat","wb");
    
    printf("Comecou a gerar indices.dat\n");
    
    //percorre cada indice da lista
    for(int i=0;i<maxSize;++i){
        //coloca os valores que serão passado para o arquivo em "comando"
        char comando[100];
        for(int c=0;c<comandoTamMax;++c){
            comando[c] = indicesOrdenados[i].comando[c];
        }
        printf("%d - %s\n",(i+1),comando);
        //e em "posicao"
        int posicao = indicesOrdenados[i].posicao;
        //troca os 4 ultimos bytes de comando, guardando a posição.
        //*((int*)&a[96]) = 123;
        *((int*)&comando[96]) = posicao;
        
        for(int i=0;i<comandoTamMax;++i){
            fputc(comando[i],indicesDat);
        }
    }
    delete indicesOrdenados;
    
    //fecha o arquivo indices.dat
    fclose(indicesDat);
    printf("Terminou de gerar indices.dat\n");
    
    ///////////////////////////////////
    ///// TERMINOU A INDICES.DAT
    ///////////////////////////////////
}

void buscaConteudoPorComando(){
    //inicializa array onde será salvo o comando a ser pesquisado
    char comandoBusca[100];
    for(int i=0;i<comandoTamMax;++i){
        comandoBusca[i] = ' ';
    }
    //pede o comando ao usuário
    scanf("%s",comandoBusca);
    
    //abre o arquivo indices.dat
    FILE* indicesDat;
    indicesDat = fopen("..\\indices.dat","rb");
    
    //calcula a quantidade de itens na arvore e salva em "tamanho"
    fseek(indicesDat, 0, SEEK_END);
    int tamanho = ftell(indicesDat);
    fseek(indicesDat, 0, SEEK_SET);
    tamanho /= comandoTamMax; 
    
    //percorrimento da árvore em arquivo, buscando pelo item pedido
    int pos = 0;
    bool achou = false;
    char comandoAtual[100];
    while(!achou && pos<tamanho){
        //Posiciona o leitor no começo do registro
        fseek(indicesDat,pos*comandoTamMax,SEEK_SET);
        char comandoAtual[100];
        //lê o registro do arquivo
        fgets(comandoAtual,100,indicesDat);
        //compara com o procurado, a comparação para no caracter nulo '\0'
        //sem se importar com os bytes finais que são a posição
        int cmp = strcmp(comandoBusca,comandoAtual);
        //print percorrimento da arvore
        //printf("%s - %s -> %d\n",comandoBusca,comandoAtual,cmp);
        if(cmp < 0){//buscado maior que encontrado, pega o ramo da esquerda
            pos++;
            pos = (2*pos)-1;
        }else if(cmp > 0){//buscado menor que encontrado, pega o ramo da direita
            pos++;
            pos = (2*pos);
        }else{//encontrou!!!
            achou = true;
        }
    }
    //fecha indices.dat, mas deixa salvo o conteúdo pedido em "comandoAtual"
    fclose(indicesDat);
    
    if(achou){
        //retira a posicao dos ultimos 4 bytes de comando
        int posicao;
        //*pos = *((int*)&a[96]);
        posicao = *((int*)&comandoAtual[96]);
        
        //Abre manpages.dat, para pegar o conteudo pedido
        FILE* manPagesDat;
        manPagesDat = fopen("..\\manpages.dat","rb");
        //posiciona o leitor no começo do registro
        posicao *= comandoTamMax+conteudoTamMax;
        fseek(manPagesDat,posicao,SEEK_SET);
        //posiciona o leitor no começo do conteudo do registro
        fseek(manPagesDat,comandoTamMax,SEEK_CUR);
        //print a posição que será iniciada a leitura
        //printf("%d\n",ftell(manPagesDat));
        
        char lido = ' ';
        while(lido != '\0'){
            printf("%c",lido=fgetc(manPagesDat));
        }
        
        //Fecha manpages.dat
        fclose(manPagesDat);
    }else{
        printf("\nManPage nao encontrada!\n");
    }
}