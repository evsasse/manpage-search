#include <stdio.h>
#include <cstring>
#include <deque>
using namespace std;
#include "AvlTree.h"
#include "Palavra.h"

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
deque<int> posicaoDasManPagesCom(char* palavra);
void buscaPorUmaPalavra();
void buscaPorDuasPalavras();

static const int palavraTamMax = 100;
static const int comandoTamMax = 100;
static const int conteudoTamMax = 149900;

static const char* conectivos[] = {
    "a","the","and","or","if","of","to","into","by","for","on","at","in","next",
    "then","than","more","only","always","never","all","off","with","that",
    "this",""
};


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
            case 3: buscaPorUmaPalavra();break;
            case 4: buscaPorDuasPalavras();break;
        }
    }while(entrada != 0);
}

void gerarArquivosDat(int argc, char* argv[]){
    //Fazendo as árvores em memória para não demorar tanto a geração...
    AvlTree<Indice> indices;
    AvlTree<Palavra> palavras;
    
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
                    fputc('\0',manPagesDat);
                    //fputc(' ',manPagesDat);
                }else
                    fputc(comando[a],manPagesDat);
            }
        }
        //Escreve o conteúdo em manpages.dat
        for(int a=0;a<conteudoTamMax;++a){
            fputc(conteudo[a],manPagesDat);
        }
        
        ///////////////////////////////////////
        // Montagem da árvore de palavras
        //char *palavra = strtok(conteudo," ");
        char *palavra = strtok(conteudo," \n.,|`´^~<>:;_-+=()[]\"'/*!@#");
        //char *palavra = strtok(conteudo," ,.;:[]{}<>()_-=+\\/?!@#$%&*|\"'`´^~");
        while(palavra != 0){
            Palavra atual(palavra,i-1);
            palavras.insert(atual);
            //palavra = strtok(0," ");
            palavra = strtok(0," \n.,|`´^~<>:;_-+=()[]\"'/*!@#");
            //palavra = strtok(conteudo," ,.;:[]{}<>()_-=+\\/?!@#$%&*|\"'`´^~");
        }
        ///////////////////////////////////////
        
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
    printf("Organizou por nivel");
    
    //Abre ou cria o arquivo indices.dat para escrita
    FILE* indicesDat;
    indicesDat = fopen("..\\indices.dat","wb");
    
    printf("Abriu e comecou a gerar indices.dat\n");
    
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
    ///// COMEÇOU O PALAVRAS.DAT
    ///////////////////////////////////
    
    FILE* palavrasDat;
    palavrasDat = fopen("..\\palavras.dat","wb");
    printf("Abriu e comecou a gerar palavras.dat\n");
    printf("%d|%d\n",palavras.getSize(),palavras.getMaxSizeByHeight());
    
    //Exclui conectivos da árvore
    //palavras.remove(Palavra("if",0));
    
    //char* conectivoC = "if";
    //Palavra conectivo(conectivoC,0);
    //palavras.remove(conectivo);
    
    //Gera palavras ordenadas para inserção
    Palavra* palavrasOrdenadas = palavras.getByLevel();
    printf("Gerou palavras ordenadas");
    
    //Escreve qtd de palavras
    maxSize = palavras.getMaxSizeByHeight();
    char maxSizeW[4];
    *((int*)&maxSizeW[0]) = maxSize;
    for(int i=0;i<4;++i){
        fputc(maxSizeW[i],palavrasDat);
    }
    //Escreve palavras seguidas do inicio de local das posicoes,e a qtd de posicoes
    //"wait                         0 4"
    int posicao = 0;
    for(int i=0;i<maxSize;++i){
        Palavra atual = palavrasOrdenadas[i];
        printf("%d - %s\n",i,atual.palavra);
        *((int*)&atual.palavra[92]) = posicao;
        int qtd = atual.posicoes.size();
        *((int*)&atual.palavra[96]) = qtd;
        posicao += qtd;
        for(int i=0;i<100;++i){
            fputc(atual.palavra[i],palavrasDat);
        }
    }
    //Escreve as posicoes das manpages que contem as palavras
    char pos[4]; //converter o int para binário
    for(int i=0;i<maxSize;++i){
        Palavra atual = palavrasOrdenadas[i];
        while(!atual.posicoes.empty()){
            *((int*)&pos[0]) = atual.posicoes.front();
            atual.posicoes.pop_front();
            for(int i=0;i<4;++i){
                fputc(pos[i],palavrasDat);
            }
        }
    }
    
    fclose(palavrasDat);
    
    printf("Terminou de gerar palavras.dat\n");
    
    ///////////////////////////////////
    ///// TERMINOU A PALAVRAS.DAT
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
        /*
         for 100
         fgetc
         */
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
        
        //print o conteudo até encontrar um caractere nulo, que deve ser o último
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

void buscaPorUmaPalavra(){
    char palavra[100];
    scanf("%s",palavra);
    
    deque<int> indices = posicaoDasManPagesCom(palavra);

    FILE* manpagesDat;
    manpagesDat = fopen("..\\manpages.dat","rb");
    
    printf("\n\n");
    
    while(!indices.empty()){
        fseek(manpagesDat,indices.front()*(comandoTamMax+conteudoTamMax),SEEK_SET);
        char comando[100];
        fgets(comando,100,manpagesDat);
        printf("%d - %s\n",indices.front(),comando);
        indices.pop_front();
    }
    
    fclose(manpagesDat);
    
}

void buscaPorDuasPalavras(){
    char palavra1[100];
    scanf("%s",palavra1);
    
    deque<int> indices1 = posicaoDasManPagesCom(palavra1);
    
    char palavra2[100];
    scanf("%s",palavra2);
    
    deque<int> indices2 = posicaoDasManPagesCom(palavra2);
    
    //Intersecção de indices1 e indices2
    deque<int> indices;
    while(!indices2.empty()){
        int atual = indices2.front();
        
        bool achou = false;
        for(int i=0;i<indices1.size() && !achou;++i){
            if(indices1.at(i) == atual){
                achou = true;
            }
        }
        
        if(achou){
            indices.push_back(atual);
        }
        indices2.pop_front();
    }
    
    FILE* manpagesDat;
    manpagesDat = fopen("..\\manpages.dat","rb");
    
    printf("\n\n");
    
    while(!indices.empty()){
        fseek(manpagesDat,indices.front()*(comandoTamMax+conteudoTamMax),SEEK_SET);
        char comando[100];
        fgets(comando,100,manpagesDat);
        printf("%d - %s\n",indices.front(),comando);
        indices.pop_front();
    }
    
    fclose(manpagesDat);
}

deque<int> posicaoDasManPagesCom(char* palavra){
    FILE* palavrasDat;
    palavrasDat = fopen("..\\palavras.dat","rb");
   
    //Lê a qtd de palavras dos primeiros 4 bytes e guarda em "tamanho";
    char tamanhoC[4];
    for(int i=0;i<4;++i){
        tamanhoC[i] = fgetc(palavrasDat);
    }
    int tamanho = *((int*)&tamanhoC[0]);
    
    int pos = 0;
    bool achou = false;
    char palavraAtual[100];
    while(!achou && pos<tamanho){
        //posiciona o leitor no inicio do registro da palavra
        fseek(palavrasDat,4+(pos*palavraTamMax),SEEK_SET);
        //pega a palavra do registro
        fgets(palavraAtual,100,palavrasDat);
        //compara
        int cmp = strcmp(palavra,palavraAtual);
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
    
    deque<int> indices;
    if(achou){
        pos = *((int*)&palavraAtual[92]);
        int qtd = *((int*)&palavraAtual[96]);
        fseek(palavrasDat,4+(tamanho*palavraTamMax)+pos*4,SEEK_SET);
        for(int i=0;i<qtd;++i){
            char indiceC[4];
            for(int a=0;a<4;++a){
                indiceC[a] = fgetc(palavrasDat);
            }
            int indice = *((int*)&indiceC[0]);
            indices.push_back(indice);
        }
    }else{
        printf("Nenhum arquivo encontrado com a palavra '%s'",palavra);
    }
    
    fclose(palavrasDat);
    
    return indices;
}