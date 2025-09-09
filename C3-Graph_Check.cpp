// svolto da Azad Ahmadi e Libaan Hassan Mohamed
// fonti utilizzate:
//      https://www.geeksforgeeks.org/detect-cycle-in-a-graph/ ;
//      https://stackoverflow.com/questions/31542031/detecting-a-cycle-in-a-directed-graph-using-dfs ;
//      https://www.codingdrills.com/tutorial/introduction-to-graph-algorithms/depth-first-search ;

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>

using namespace std;

// compilazione: g++ C3-Graph_Check.cpp -> ./a.out -graph
//
// Obiettivi CONSEGNA:
// 1) Visitare tutto il grafo, parti disconesse incluse:
/*
    Per fare ciò è stata introdotta la funzione has_cycle(), 
    la quale viene eseguita in modo ricorsivo per tutti i nodi esistenti.
*/
// 2) Calcolare la lunghezza del ciclo più lungo, fornendo anche l'elenco dei nodi coinvolti:
/*
    Nella funzione has_cycle() viene utilizzato un contatore che viene aumentato ogni volta che un nodo
    si aggiunge al ciclo attuale. Se si viene a trovare un nuovo ciclo massimo, allora viene sostituito.
*/

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step
((f(b)-f(a))/(b-a))*(x-xk)+f(xk)

// int ct_visit = 0; // contatore durante visita
// int max_cycle_length = 0; // lunghezza del ciclo più lungo
// vector<int> max_cycle_nodes; // per memorizzare i nodi del ciclo più lungo

//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node {
    int val;
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list {
    node *head;
    // node* tail; /// per lista doubly linked
} list_t;

//////////////////////////////////////////////////
/// Fine Definizione della st((f(b)-f(a))/(b-a))*(x-xk)+f(xk)
ruttura dati lista
//////////////////////////////////////////////////

int ct_visit = 0;           // contatore durante visita
int max_cycle_length = 0;   // lunghezza del ciclo più lungo
list_t *max_cycle_nodes;    // nodi del ciclo più lungo
list_t *visited_nodes;      // lista di nodi visitati (utilizzata come stack)

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato?

// list_t* E;  /// array con le liste di adiacenza per ogni nodo
list_t **E; /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati grafo
//////////////////////////////////////////////////

/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

list_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node) {
    return (int)((long)node - (long)global_ptr_ref);
}

void node_print(int n) {

    output_graph << "node_" << n << "_" << n_operazione << endl;
    output_graph << "[ shape = oval; ";
    if (V_visitato[n] == 1)
        output_graph << "fillcolor = \"#bbbbbb\"; style=filled; ";
    if (V_visitato[n] == 2)
        output_graph << "fillcolor = \"#555555\"; style=filled; ";
    output_graph << "label = "
                 << "\"Idx: " << n << ", val: " << V[n] << "\" ];\n";

    node_t *elem = E[n]->head;
    while (elem != NULL) { /// disegno arco
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << elem->val << "_" << n_operazione << " [  color=gray ]\n";
        elem = elem->next;
    }
}

void graph_print() {
    for (int i = 0; i < n_nodi; i++)
        node_print(i);
    n_operazione++;
}

void list_print(list_t *l) {
    // printf("Stampa lista\n");

    if (l->head == NULL) {
        printf("Lista vuota\n");
    } else {
        node_t *current = l->head;

        while (current != NULL) {
            if (!details)
                printf("%d, ", current->val);
            else { /// stampa completa
                if (current->next == NULL)
                    printf("allocato in %d [Val: %d, Next: NULL]\n",
                           get_address(current),
                           current->val);
                else
                    printf("allocato in %d [Val: %d, Next: %d]\n",
                           get_address(current),
                           current->val,
                           get_address(current->next));
            }
            current = current->next;
        }
        printf("\n");
    }
}

list_t *list_new() {
    list_t *l = new list;
    if (details) {
        printf("Lista creata\n");
    }

    l->head = NULL; //// perche' non e' l.head ?
    if (details) {
        printf("Imposto a NULL head\n");
    }

    return l;
}

void list_remove_front(list_t *l) {
    /// rimuove un elemento dal fondo della lista
    if (l->head == NULL)
        return;

    node_t *new_head = l->head->next;
    delete l->head;
    l->head = new_head;
}

void list_insert_front(list_t *l, int elem) {
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->val = elem;
    new_node->next = l->head;
    l->head = new_node;
}

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

void print_array_graph(int *A, int n, string c, int a, int l, int m, int r) {
    /// prepara il disegno dell'array A ed il suo contenuto (n celle)
    /// a e' il codice del nodo e c la stringa
    /// l,m,r i tre indici della bisezione

    // return ;

    output_graph << c << a << " [label=<" << endl;

    /// tabella con contenuto array
    output_graph << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" > " << endl;
    /// indici
    output_graph << "<TR  >";
    for (int j = 0; j < n; j++) {
        output_graph << "<TD ";
        output_graph << ">" << j << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "<TR>";
    // contenuto
    for (int j = 0; j < n; j++) {
        output_graph << "<TD BORDER=\"1\"";
        if (j == m)
            output_graph << " bgcolor=\"#00a000\""; /// valore testato
        else if (j >= l && j <= r)
            output_graph << " bgcolor=\"#80ff80\""; /// range di competenza
        output_graph << ">" << A[j] << "</TD>" << endl;
    }
    output_graph << "</TR>" << endl;
    output_graph << "</TABLE> " << endl;

    output_graph << ">];" << endl;
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

// Funzione di ricerca di cicli all'interno del grafo.
// Viene preso come argomento l'indice del nodo da analizzare
int has_cycle(int n) {
     graph_print();

    if (details)
        printf("DFS: lavoro sul nodo %d (visitato %d)\n", n, V_visitato[n]);

    if (V_visitato[n] == 1) {
        // Calcolo la lunghezza del ciclo
        int cycle_length = 0;
        // Creo lista in cui salvare i nodi del ciclo più lungo
        list_t *cycle_nodes = list_new();

        // itero lo stack di nodi visitati finché non individuo l'inizio del ciclo
        // contando e memorizzando i nodi di passaggio
        node_t *elem = visited_nodes->head;
        while (elem != NULL) {
            list_insert_front(cycle_nodes, elem->val);
            cycle_length++;

            // Se si raggiunge l'inizio del ciclo, usciamo dal while
            if (elem->val == n) {
                break;
            }

            elem = elem->next;
        }

        // Aggiorno il ciclo più lungo
        if (cycle_length > max_cycle_length) {
            max_cycle_length = cycle_length;
            max_cycle_nodes = cycle_nodes;
        }
        
        return 1;  // c'è un ciclo
    }

    if (V_visitato[n] == 2)
        return 0;  // trovato un nuovo percorso alternativo (non c'e' il ciclo)

    V_visitato[n] = 1; // prima volta che incontro questo nodo
    list_insert_front(visited_nodes, n);    // Inserisco il nodo nella lista di nodi visitati

    if (details)
        printf("Visito il nodo %d (val %d)\n", n, V[n]);

    /// esploro la lista di adiacenza
    int t = 0;
    node_t *elem = E[n]->head;

    // Se un nodo è disconnesso dal grafo viene segnalato
    if (elem == NULL) {
        printf("\nIl nodo %d è disconnesso.\n", n);
    }

    while (elem != NULL) { /// elenco tutti i nodi nella lista
        /// espando arco  n --> elem->val
        /// quindi DFS(elem->val)
        output_graph << "dfs_" << n << " -> dfs_" << elem->val;
        if (V_visitato[elem->val])
            output_graph << "[color=gray, label = \"" << ct_visit++ << "\"]";
        else
            output_graph << "[color=red, label = \"" << ct_visit++ << "\"]";
        output_graph << endl;

        t += has_cycle(elem->val);
        elem = elem->next;
    }

    V_visitato[n] = 2; // abbandono il nodo per sempre
    list_remove_front(visited_nodes);   // Dopo che il nodo viene abbandonato, viene rimosso anche dalla lista dei nodi visitati

    return t;
}

int main(int argc, char **argv) {
    srand((unsigned)time(NULL));

    if (parse_cmd(argc, argv))
        return 1;

    if (graph) {
        output_graph.open("graph.dot");
        /// preparo header
        output_graph << "digraph g" << endl;
        output_graph << "{ " << endl;
        output_graph << "node [shape=none]" << endl;
        output_graph << "rankdir=\"LR\"" << endl;
        //    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;
    }

    n_nodi = 10 + rand() % 11; // Numero di nodi randomico da 10 a 20
    V = new int[n_nodi];
    V_visitato = new int[n_nodi];
    E = new list_t *[n_nodi];

    // inizializzazione
    for (int i = 0; i < n_nodi; i++) {
        V[i] = 10 + rand() % 41; //Pesi randomici tra 10 e 50
        V_visitato[i] = 0; // flag = non visitato
        E[i] = list_new();

        if (i == 0)
            global_ptr_ref = E[i];

        for (int j = 0; j < n_nodi; j++) {
            // per ogni coppia di nodi, la probabilità che siano collegati è del ~14%
            if (rand() % 7 == 0)
                list_insert_front(E[i], j);
        }
    }

    graph_print();

    for (int i = 0; i < n_nodi; i++) {
        printf("\nIndice: %d\n", i);
        printf("Valore: %d\n", V[i]);
        printf("Lista di adiacenza: ");
        list_print(E[i]);
    }

    int t = 0;
    visited_nodes = list_new();

    for (int i = 0; i < n_nodi; i++) {
        if (V_visitato[i] == 0) {
            t += has_cycle(i);
        }
    }

    graph_print();
    
    printf("\nCicli trovati = %d\n", t);    // Vengono stampati gli n cicli presenti nel grafo
    printf("Lunghezza del ciclo più lungo: %d\n", max_cycle_length);
    printf("Nodi del ciclo più lungo: ");
    list_print(max_cycle_nodes);

    printf("\n");
    if (graph) {
        /// preparo footer e chiudo file
        output_graph << "}" << endl;
        output_graph.close();
        cout << " File graph.dot scritto" << endl
             << "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf" << endl;
    }

    return 0;
}
