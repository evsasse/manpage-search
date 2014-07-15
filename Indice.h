#ifndef INDICE_H
#define	INDICE_H

/**
 * Struct utilizada para organizar os índices na árvore, para que possam ser retirados por níveis.
 */
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

#endif	/* INDICE_H */

