// svolto da Azad Ahmadi e Libaan Hassan Mohamed

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// g++ consegna2.cpp -> ./a.out

//Obbiettivi CONSEGNA:
/*
1) Flip albero: per ogni nodo il sottoalbero destro diventa il sottoalbero sinistro e viceversa.
2) Calcolo profondità di ciascun nodo dell'albero
3) Funzione isBalanced: restituire un flag che indichi se l'albero è bilanciato o meno. Bonus: l'algoritmo è O ( n ) con n nodi
4) Funzione isComplete: restituire un flag che indiche se l'albero è completo secondo la definizione classica
5) Funzione Lowest Common Ancestor: dati due valori presenti nell'albero, restituire il valore del nodo piu' basso che contiene entrambi nel suo sottoalbero
*/

int details = 0;
int graph = 1;

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

ofstream output_visit;
ifstream input_visit;

//////////////////////////////////////////////////
/// Definizione della struttura dati tree
//////////////////////////////////////////////////

/// struct per il nodo dell'albero
struct node {
    int data;
    node *L;
    node *R;
};

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati tree
//////////////////////////////////////////////////

/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

node *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node) {
    if (node == NULL)
        return 0;
    return (int)((long)node - (long)global_ptr_ref);
}

/// stampa il codice del nodo per dot
void print_node_code(node *n) {
    output_graph << "node_" << get_address(n) << "_" << n_operazione;
}

void node_print_graph(node *n) {
    print_node_code(n);
    output_graph << "\n[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" >\n<TR> <TD CELLPADDING=\"3\" BORDER=\"0\"  ALIGN=\"LEFT\" bgcolor=\"#f0f0f0\" PORT=\"id\">";
    output_graph << get_address(n) << "</TD> </TR><TR>\n<TD PORT=\"val\" bgcolor=\"#a0FFa0\">";
    output_graph << n->data << "</TD>\n <TD PORT=\"L\" ";
    if (n->L == NULL)
        output_graph << "bgcolor=\"#808080\"> NULL";
    else
        output_graph << "> " << get_address(n->L);
    output_graph << "</TD>\n <TD PORT=\"R\" ";
    if (n->R == NULL)
        output_graph << "bgcolor=\"#808080\"> NULL";
    else
        output_graph << "> " << get_address(n->R);
    output_graph << "</TD>\n</TR></TABLE>>];\n";

    /// visualizzazione figli sullo stesso piano
    if (n->L != NULL && n->R != NULL) {
        output_graph << "rank = same; ";
        print_node_code(n);
        output_graph << ";";
        print_node_code(n->L);
        output_graph << ";\n";
    }

    // mostro archi uscenti

    if (n->L != NULL) { /// disegno arco left
        print_node_code(n);
        output_graph << ":L:c -> ";
        print_node_code(n->L);
        output_graph << ":id ;\n";
    }

    if (n->R != NULL) { /// disegno arco R
        print_node_code(n);
        output_graph << ":R:c -> ";
        print_node_code(n->R);
        output_graph << ":id ;\n";
    }
}

void tree_print_rec_graph(node *n) {
    if (n != NULL) {
        node_print_graph(n);
        tree_print_rec_graph(n->L);
        tree_print_rec_graph(n->R);
    }
}

void tree_print_graph(node *n) {
    /// stampa ricorsiva del nodo
    tree_print_rec_graph(n);
    n_operazione++;
}

void node_print(node *n) {
    if (n == NULL)
        printf("Puntatore vuoto\n");
    else
        printf("allocato in %d [Val: %d, L: %d, R: %d]\n",
               get_address(n),
               n->data,
               get_address(n->R),
               get_address(n->L));
}

node *node_new(int elem) { /// crea nuovo nodo
    node *t = new node;
    if (details) {
        printf("nodo creato\n");
    }

    t->data = elem;
    t->L = NULL;
    t->R = NULL;
    if (details) {
        printf("Imposto a NULL children\n");
    }

    return t;
}

void tree_insert_child_L(node *n, int elem) {
    /// inserisco il nuovo nodo con contenuto elem
    /// come figlio Left del nodo n

    /// creo nodo
    n->L = node_new(elem);
}

void tree_insert_child_R(node *n, int elem) {
    /// inserisco il nuovo nodo con contenuto elem
    /// come figlio Right del nodo n
    n->R = node_new(elem);
}

int max_nodes = 10;
int n_nodes = 0;

void insert_random_rec(node *n) {
    //// inserisce in modo random un nodo L e R e prosegue ricorsivamente
    /// limito i nodi interni totali, in modo da evitare alberi troppo grandi

    printf("inserisco %d\n", n_nodes);

    if (n_nodes++ >= max_nodes) /// limito il numero di nodi
        return;
    printf("inserisco %d\n", n_nodes);

    float probabilita = 0.8; /// tra 0 e 1

    if (rand() % 100 < probabilita * 100) { // se numero random e' minore della probabilita' -> aggiungo nodo R con valore a caso
        tree_insert_child_R(n, rand() % 100);
    }
    if (rand() % 100 < probabilita * 100) { // se numero random e' minore della probabilita' -> aggiungo nodo L con valore a caso
        tree_insert_child_L(n, rand() % 100);
    }
    if (n->L != NULL)
        insert_random_rec(n->L);
    if (n->R != NULL)
        insert_random_rec(n->R);
}

int parse_cmd(int argc, char **argv) {
    /// controllo argomenti
    int ok_parse = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'v') {
            details = 1;
            ok_parse = 1;
        }
        if (argv[i][1] == 'g') {
            graph = 1;
            ok_parse = 1;
        }
    }

    if (argc > 1 && !ok_parse) {
        printf("Usage: %s [Options]\n", argv[0]);
        printf("Options:\n");
        printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
        printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
        return 1;
    }

    return 0;
}


// ----------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------


bool is_complete(node *tree) {
    if (tree->L == NULL && tree->R == NULL) return true;
    if (tree->L == NULL || tree->R == NULL) return false;

    bool l = is_complete(tree->L);
    bool r = is_complete(tree->R);
    return l && r;
}

int tree_height(node *tree, int h) {
    if (tree == NULL) return h;
    h++;

    int l = tree_height(tree->L, h);
    int r = tree_height(tree->R, h);
    return max(l, r);
}

bool is_balanced(node *tree) {
    if (tree == NULL) return true;

    int h_l = tree_height(tree->L, 0);
    int h_r = tree_height(tree->R, 0);

    /*
    if (abs(h_l - h_r) <= 1) {
        bool l = is_balanced(tree->L);
        bool r = is_balanced(tree->R);
        return l && r;
    }
    */
    return abs(h_l - h_r) <= 1;
}

bool is_equal(node *a, node *b) {
    if (a == NULL && b == NULL) return true;
    if (a == NULL || b == NULL) return false;
    if (a->data != b->data)     return false;
    if (!is_equal(a->L, b->L))  return false;
    if (!is_equal(a->R, b->R))  return false;
    return true;
}

// a sottoalbero di b
bool is_subtree(node *a, node *b) {
    if (a == NULL || b == NULL) return false;
    if (is_equal(a, b))         return true;
    if (is_subtree(a, b->L))    return true;
    if (is_subtree(a, b->R))    return true;
    return false;
}

int main(int argc, char **argv) {
    int i, test;

    if (parse_cmd(argc, argv))
        return 1;

    // init random
    srand((unsigned)time(NULL));

    if (graph) {
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g" << endl;
        output_graph << "{ " << endl;
        output_graph << "node [shape=none]" << endl;
        output_graph << "rankdir=\"TB\"" << endl;
        output_graph << "edge[tailclip=false,arrowtail=dot];" << endl;
    }

    /// creazione manuale degli alberi per testare l'effettiva funzionalità
    /// delle funzioni conoscendo il risultato corretto

    node *root_a = node_new(0);
    global_ptr_ref = root_a;

    tree_insert_child_L(root_a, 2);
    tree_insert_child_L(root_a->L, 4);
    tree_insert_child_L(root_a->L->L, 8);
    tree_insert_child_R(root_a->L, 6);
    tree_insert_child_R(root_a, 3);
    tree_insert_child_L(root_a->R, 5);
    tree_insert_child_R(root_a->R, 7);

    node *root_b = node_new(2);
    tree_insert_child_L(root_b, 4);
    tree_insert_child_L(root_b->L, 8);
    tree_insert_child_R(root_b, 4);

    // creo albero random
    // insert_random_rec(root);

    cout << "Balanced: " << is_balanced(root_a) << endl;
    cout << "Subtree: " << is_subtree(root_b, root_a) << endl;

    // stampa albero
    if (graph)
        tree_print_graph(root_a);
    n_operazione++;

    if (graph) {
        /// preparo footer e chiudo file
        output_graph << "}" << endl;
        output_graph.close();
        cout
            << " File graph.dot scritto" << endl
            << "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf" << endl;
    }

    return 0;
}
