#ifndef PALAVRA_H
#define	PALAVRA_H

#include <deque>
using std::deque;


/**
 * Struct utilizada para conter as palavras que estão contidas no arquivo
 */
struct Palavra{
    char palavra[100];
    deque<int> posicoes;
    
    Palavra(){
        palavra[0] = '\0';
        for(int i=1;i<100;++i){
            palavra[i] = ' ';
        }
    }
    
    Palavra(char* p, int a){
        for(int i=1;i<100;++i){
            palavra[i] = ' ';
        }
        strcpy(palavra,p);
        posicoes.push_back(a);
    }
    
    bool operator==(const Palavra& rhs) const
    {
        if (strcmp(palavra,rhs.palavra) == 0) return true;
        return false;
    }
    bool operator<(const Palavra& rhs) const
    {
        if (strcmp(palavra,rhs.palavra) < 0) return true;
        return false;
    }
    
    bool operator>(const Palavra& rhs) const
    {
        if (strcmp(palavra,rhs.palavra) > 0) return true;
        return false;
    }
};

#endif	/* PALAVRA_H */

