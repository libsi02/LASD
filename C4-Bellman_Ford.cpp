// svolto con Libaan Hassan Mohamed
// Fonti utilizzate: 
//      https://it.wikipedia.org/wiki/Algoritmo_di_Bellman-Ford ;
//      https://www.geeksforgeeks.org/bellman-ford-algorithm-dp-23/;
//      https://stackoverflow.com/questions/41965431/dijkstra-algorithm-min-heap-as-a-min-priority-queue#42135298

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

using namespace std;

// compilazione: g++ C4-Bellman_ford.cpp -> ./a.out 

// Obiettivo:
// 1) Implementazione dell'heap per la ricerca del nodo minimo
// 2) implementazione algoritmo di Bellman-Ford (con relativo heap), e controllo di cicli negativi

#define INFTY 1000000
#define BEST_IDX 5000000
#define MAX_SIZE 256

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

int heap[MAX_SIZE][2];            // Colonna 0: distanza, Colonna 1: indice (visitato) nodo
int heap_size = 0;                   // dimensione attuale dell'heap

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step
int n_operazione2 = 0; /// """" ma per l'heap
int ct_visit = 0; // contatore durante visita


/// struct per il nodo della lista
typedef struct node {
    int val; /// prossimo nodo
    int w; /// peso dell'arco
    struct node *next;
} node_t;

/// struct per la lista
typedef struct list {
    node *head;
} list_t;

int *V;             // elenco dei nodi del grafo
int *V_visitato;    // nodo visitato?
int *V_prev;        // nodo precedente dalla visita
int *V_dist;        // distanza da sorgente

// list_t* E;  /// array con le liste di adiacenza per ogni nodo
list_t **E; // array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;

list_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista 

int parent_idx(int n) {
    if (n == 0)
        return -1;
    return (n - 1) / 2;
}

int child_L_idx(int n) {
    if (2 * n + 1 >= heap_size)
        return -1;
    return 2 * n + 1;
}

int child_R_idx(int n) {
    if (2 * n + 2 >= heap_size)
        return -1;
    return 2 * n + 2;
}

int is_leaf(int n) {
    return (child_L_idx(n) == -1); // non c'e' bisogno di controllare il figlio R

    /* versione equivalente
    if (child_L_idx(n)==-1)
      return 1;
    return 0;
    */
}

int get_address(void *node) {
    return (int)((long)node - (long)global_ptr_ref);
}

void node_print(int n) {

    /// calcolo massima distanza (eccetto infinito)
    int max_d = 0;
    for (int i = 0; i < n_nodi; i++)
        if (max_d < V_dist[i] && V_dist[i] < INFTY)
            max_d = V_dist[i];

    output_graph << "node_" << n << "_" << n_operazione << endl;
    output_graph << "[ shape = oval; ";

    if (V_visitato[n] == 1)
        output_graph << "penwidth = 4; ";

    int col = V[n]; /// distanza in scala 0..1
    // int col = V_dist[n] / max_d; /// distanza in scala 0..1

    output_graph << "fillcolor = \"0.0 0.0 " << col << "\"; style=filled; ";
    if (V_dist[n] < INFTY)
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: " << V_dist[n] << "\" ];\n";
    else
        output_graph << "label = "
                     << "\"Idx: " << n << ", dist: INFTY\" ];\n";

    node_t *elem = E[n]->head;
    while (elem != NULL) { /// disegno arco
        output_graph << "node_" << n << "_" << n_operazione << " -> ";
        output_graph << "node_" << elem->val << "_" << n_operazione << " [ label=\"" << elem->w << "\", len=" << elem->w / 100 * 10 << " ]\n";
        elem = elem->next;
    }

    if (V_prev[n] != -1) { // se c'e' un nodo precedente visitato -> disegno arco

        int len = 0;
        /*
        //cerco arco V_prev[n] --> V[n]
        node_t* elem=E[ V_prev[n] ]->head;
        while (elem!=NULL){
        int v=elem->val; /// arco u --> v
        if (v == V[n])
          len=elem->w;
        elem=elem->next;
          }
        */

            len = 1;
            output_graph << "node_" << n << "_" << n_operazione << " -> ";
            output_graph << "node_" << V_prev[n] << "_" << n_operazione << " [ color=blue, penwidth=5, len=" << len / 100 * 10 << " ]\n";

    }
}

void graph_print() {
    for (int i = 0; i < n_nodi; i++)
        node_print(i);
    n_operazione++;
}

void list_print(list_t *l) {
    printf("Stampa lista\n");

    if (l->head == NULL) {
        printf("Lista vuota\n");
    } else {
        node_t *current = l->head;

        while (current != NULL) {
            if (!details)
                printf("%d w:%d, ", current->val, current->w);
            else { /// stampa completa
                if (current->next == NULL)
                    printf("allocato in %d [Val: %d, W: %d, Next: NULL]\n",
                        get_address(current),
                        current->val,
                        current->w);
                else
                    printf("allocato in %d [Val: %d, W: %d, Next: %d]\n",
                        get_address(current),
                        current->val,
                        current->w,
                        get_address(current->next));
            }
            current = current->next;
        }
        printf("\n");
    }
}

list_t *list_new(void) {
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

void list_insert_front(list_t *l, int elem, int w) {
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->val = elem;
    new_node->w = w;
    new_node->next = l->head;
    l->head = new_node;
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
    /// aggiorno contatore globale di swap
    ct_swap++;
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

// Funzione di inserimento di un elemento all'interno dell'heap + heapify
void heap_insert(int distanza, int nodo) {
    if (details)
        printf("Inserisco elemento %d in posizione %d\n", distanza, heap_size);

    if (heap_size < MAX_SIZE) {
        int i = heap_size++;  // Incremento la grandezza dopo il controllo

        // Inserisco il nuovo elemento (con distanza, indice (visita)) all'interno dell'heap
        heap[i][0] = distanza;
        heap[i][1] = nodo;

        // Ripristino la proprietà (heapify) dell'heap con il "bubbling up"
        while (i != 0) {
            int par_idx = parent_idx(i);  // Trovo l'indice del genitore

            if (heap[par_idx][0] <= heap[i][0]) {
                return;  // Proprietà dell'heap soddisfatta
            }

            // Scambia con il genitore
            swap(heap[par_idx][0], heap[i][0]);
            swap(heap[par_idx][1], heap[i][1]);

            i = par_idx;
        }
    } else {
        printf("Heap pieno!\n");
    }
}

// Funzione di rimozione radice dell'heap che restituisce un array con il peso (distanza) e indice (visita) 
int *heap_remove_min() {
    // Controllo se l'heap è vuoto
    if (heap_size <= 0) {  
        printf("Errore: heap vuoto\n");
        return nullptr;  
    }

    // Estraggo il nodo minimo (radice)
    int *minimo = new int[2];  // Array che contiene distanza e indice
    minimo[0] = heap[0][0];  // Distanza
    minimo[1] = heap[0][1];  // Indice del nodo

    // Sposto l'ultimo nodo alla radice e diminuisco la grandezza dell'heap
    heap[0][0] = heap[heap_size - 1][0];
    heap[0][1] = heap[heap_size - 1][1];
    heap_size--;

    // Ripristino la proprietà (heapify) dell'heap 
    int i = 0;
    while (!is_leaf(i)) {
        int con_chi_mi_scambio = -1;
        if (heap[i][0] > heap[child_L_idx(i)][0]) {
            con_chi_mi_scambio = child_L_idx(i);
            if (child_R_idx(i) >= 0 && heap[child_L_idx(i)][0] > heap[child_R_idx(i)][0]) {
                con_chi_mi_scambio = child_R_idx(i);
            }
        } else {
            if (child_R_idx(i) >= 0 && heap[i][0] > heap[child_R_idx(i)][0]) {
                con_chi_mi_scambio = child_R_idx(i);
            } else {
                break; // Proprietà dell'heap rispettata
            }
        }

        // Scambio con il figlio più piccolo
        if (con_chi_mi_scambio != -1) {
            swap(heap[i][0], heap[con_chi_mi_scambio][0]);
            swap(heap[i][1], heap[con_chi_mi_scambio][1]);
            i = con_chi_mi_scambio;
        }
    }

    return minimo;
}

/*
//Funzione SP (Dijkstra) + heap per la ricerca del nodo minimo 
 void sp_dijkstra(int n) {
     V_dist[n] = 0;  // La distanza a sé stesso è 0
     int q_size = n_nodi;

     // Inizializza l'heap con le distanze
     for (int i = 0; i < n_nodi; i++) {
        if (V_dist[i]>=0)
            heap_insert(V_dist[i], i);  // Inserisce il nodo con la sua distanza nell'heap
     }

     // Esegui l'algoritmo
     while (q_size > 0) {
         int* best_dist = heap_remove_min();
         int best_dist_value = best_dist[0];                     // Il peso migliore           
         int best_idx = best_dist[1];             // Indice (visitato) castato in "int"

         // Se non ci sono nodi da processare, esci
         if (best_idx == -1) break;

         V_visitato[best_idx] = 1;
         q_size--;

         // Esplora i nodi adiacenti
         node_t* elem = E[best_idx]->head;
         while (elem != NULL) {
             int v = elem->val;
             float alt = V_dist[best_idx] + elem->w + 1000*pow((V[best_idx]-V[v]), 2);

             // Se la nuova distanza è migliore, aggiornala anche all'interno dell'heap
             if (alt < V_dist[v]) {
                 V_dist[v] = alt;
                 V_prev[v] = best_idx;
                 heap_insert(alt, v);
             }
             elem = elem->next;
         }
     }

     // Stampa il grafo finale (visualizzazione)
     graph_print();
 }
*/

// Funzione SP (Bellman-Ford) + heap per la ricerca del nodo minimo + controllo di cicli negativi all'interno del grafo
void sp_bellman_ford(int n) {
    V_dist[n] = 0;  // La distanza a sé stesso è 0
    int q_size = n_nodi;

    // Inizializza l'heap con le distanze
    for (int i = 0; i < n_nodi; i++) {
        heap_insert(V_dist[i], i);  // Inserisce il nodo con la sua distanza nell'heap
    }

    while (q_size > 0) {
        int *best_dist = heap_remove_min();
        int best_dist_value = best_dist[0];            // Il peso migliore           
        int best_idx = best_dist[1];                   // Indice (visitato) castato in "int"

        // Interrompe il ciclo non ci sono nodi da processare
        if (best_idx == -1) break;

        V_visitato[best_idx] = 1;
        q_size--;

        // Esplora i nodi adiacenti
        node_t *elem = E[best_idx]->head;
        while (elem != NULL) {
            int v = elem->val;
            //int alt = V_dist[best_idx] + elem->w + 1000 * pow((V[best_idx] - V[v]), 2);
            int alt = V_dist[best_idx] + elem->w;

            // Se la nuova distanza è migliore, aggiorno anche all'interno dell'heap
            if (alt < V_dist[v]) {
                V_dist[v] = alt;
                V_prev[v] = best_idx;
                heap_insert(alt, v);
            }
            elem = elem->next;
        }
    }

    printf("\nCONTROLLO CICLO NEGATIVO\n\n");

    // Controllo per i cicli/archi con soli pesi negativi uando una variabile booleana
    bool negative_cycle_detected = false;
    int rilas = 0;
    for (int u = 0; u < n_nodi; u++) {
        node_t *elem = E[u]->head; // Lista di adiacenza del nodo u
        while (elem != NULL) {
            int v = elem->val;    // Nodo adiacente v
            int weight = elem->w; // Peso dell'arco u -> v

            // Se è possibile rilassare ancora un arco, c'è un ciclo negativo
            printf("Controllo i nodi %d -> %d.\n", u, v);
            printf("Valori: %d + %d < %d\n", V_dist[u], weight, V_dist[v]);

            if (V_dist[v] != INFTY && V_dist[u] + weight < V_dist[v]) {
                printf("Ciclo negativo che comprende i nodi %d -> %d.\n", u, v);
                // Rilassamento numero n_nodi o maggiore ---> CICLO NEGATIVO
                negative_cycle_detected = true; 
            }
            elem = elem->next;
        }
    }

    if (negative_cycle_detected){
        // Questo messaggio viene mostrato solo se tutti gli archi in un ciclo sono negativi
        printf("\nIl grafo contiene un ciclo/i negativo/i trovato.\n");
        printf(":::::::::::\n");
    } else {
        printf("\nNessun ciclo negativo trovato.\n");
        printf(":::::::::::\n");
    }    

    // graph_print();
}

// All'interno del main ci sono 2 test: un solo arco negativo, tutti gli archi negativi
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

    int N = 5;    // discretizzazione di N
    n_nodi = N * N;  // n nodi nella matrice

    //// init nodi
    V = new int[n_nodi];
    V_visitato = new int[n_nodi];
    V_prev = new int[n_nodi];
    V_dist = new int[n_nodi];  // costo per raggiungere il nodo

    //// init archi
    E = new list_t *[n_nodi]; //(list_t**)malloc(n_nodi*sizeof(list_t*));

    for (int i = 0; i < n_nodi; i++) {
        V_visitato[i] = 0; // flag = non visitato
        V_prev[i] = -1;    // non c'è precedente
        V_dist[i] = INFTY;
        heap[i][0] = INFTY; 
        heap[i][1] = INFTY;

        E[i] = list_new();
    }

    // **Test 1: Un solo arco negativo**
    printf("TEST 1: Un solo arco negativo\n");

    list_insert_front(E[0], 1, 1);    // 0 -> 1, peso +1
    list_insert_front(E[1], 2, -5);   // 1 -> 2, peso -5
    list_insert_front(E[2], 3, 2);    // 2 -> 3, peso +2
    list_insert_front(E[3], 2, 4);    // 2 -> 3, peso +4

    sp_bellman_ford(0);  // Partenza dal nodo 0

    // Reset nodi e archi
    for (int i = 0; i < n_nodi; i++) {
        V_visitato[i] = 0;
        V_prev[i] = -1;
        V_dist[i] = INFTY;
        E[i]->head = NULL;
    }

    for (int i = 0; i < MAX_SIZE; i++) {
        heap[i][0] = 0;
        heap[i][1] = 0;
    }

    // Test 2: Tutti gli archi negativi**
    printf("\n\nTEST 2: Tutti gli archi negativi\n");

    list_insert_front(E[0], 1, -1); // 0 -> 1, peso -1
    list_insert_front(E[1], 2, -2); // 1 -> 2, peso -2
    list_insert_front(E[2], 3, -3); // 2 -> 3, peso -3
    list_insert_front(E[3], 0, -4); // 3 -> 0, peso -4 

    sp_bellman_ford(0);  // Partenza dal nodo 0

     // Reset nodi e archi
    for (int i = 0; i < n_nodi; i++) {
        V_visitato[i] = 0;
        V_prev[i] = -1;
        V_dist[i] = INFTY;
        E[i]->head = NULL;
    }

    for (int i = 0; i < MAX_SIZE; i++) {
        heap[i][0] = 0;
        heap[i][1] = 0;
    }

    printf("\n");

    // printf("distanza %d\n",V_dist[99]);

    if (graph) {
        /// preparo footer e chiudo file
        output_graph << "}" << endl;
        output_graph.close();
        cout << " File graph.dot scritto" << endl
             << "****** Creare il grafo con: neato graph.dot -Tpdf -o graph.pdf" << endl;
    }

    // Delete 
    for (int i = 0; i < n_nodi; i++) {
        delete E[i];
    }

    delete[] E;
    delete[] V;
    delete[] V_visitato;
    delete[] V_prev;
    delete[] V_dist;

    return 0;
}
