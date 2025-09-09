// svolto da Azad Ahmadi e Libaan Hassan Mohamed

#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

// compilazione: g++ C6-File_Check.cpp -> ./a.out file1.cpp file2.ccp

//Obbiettivi CONSEGNA:
/*
Adattare l'algoritmo di Needleman-Wunsch per ottenere un confronto tra versioni diverse di un codice sorgente. 
Preparare un test con un file C++ da almeno 100 righe. 
L'obiettivo è quello di dare priorità alla struttura dei blocchi del codice e poi ai token. 
Gli spazi/tab invece possono essere aggiunti o tolti senza penalità. 
L'output deve indicare i cambiamenti effettuati, come lista di azioni o stringa di cambiamento (- per cancellazione e caratteri aggiunti o modificati) 
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

int ct_visit = 0; // contatore durante visita

typedef struct list my_stack;

my_stack *s;

int idx = 0;

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
/// Fine Definizione della struttura dati lista
//////////////////////////////////////////////////

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato?
int *V_lowlink;  //
int *V_onStack;

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
    if (V_visitato[n] == -1)
        output_graph << "fillcolor = \"#aaaaaa\"; style=filled; ";
    output_graph << "label = "
                 << "\"Idx: " << n << ", val: " << V[n] << " index: " << V_visitato[n] << " lowlink: " << V_lowlink[n] << "\" ];\n";

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
            if (!details) {
                char ch = current->val;

                if (ch == '\1') {
                    printf("\033[31m"); // colore rosso
                } else if (ch == '\2') {
                    printf("\033[32m"); // colore verde
                } else if (ch == '\3') {
                    printf("\033[33m"); // colore giallo
                } else if (ch == '\4') {
                    printf("\033[39m"); // default
                } else {
                    printf("%c", ch);
                }
            } else { /// stampa completa
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

void list_delete(list_t *l) {
    //// implementare rimozione dal fondo della lista
    //// deallocazione struct list
}

void list_insert_front(list_t *l, int elem) {
    /// inserisce un elemento all'inizio della lista
    node_t *new_node = new node_t;
    new_node->next = NULL;
    new_node->val = elem;
    new_node->next = l->head;
    l->head = new_node;
}

void list_delete_front(list_t *l) {
    /// elimina il primo elemento della lista
    node_t *node = l->head; // il nodo da eliminare

    if (node == NULL) // lista vuota
        return;

    l->head = node->next;

    // if (graph) print_status(l,node,"DEL FRONT: aggancio lista a nodo successivo");

    node->next = NULL;

    // if (graph) print_status(l,node,"DEL FRONT: sgancio puntatore da nodo da cancellare");

    delete node;

    //  if (graph) print_status(l,NULL,"DEL FRONT: cancello nodo");
}

////////// operazioni stack

my_stack *stack_new() {
    return list_new();
}

int stack_top(my_stack *s) {
    if (s->head != NULL)
        return s->head->val;
    printf("ERRORE: stack vuoto!\n");
    return -1;
}

int stack_pop(my_stack *s) {
    if (s->head != NULL) {
        int v = s->head->val;
        list_delete_front((list_t *)s);
        return v;
    }
    printf("ERRORE: stack vuoto!\n");
    return -1;
}

void stack_push(my_stack *s, int v) {
    list_insert_front((list_t *)s, v);
}

void stack_print(my_stack *s) {
    list_print((list_t *)s);
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

bool is_alpha(char c) {
    return c == '_' ||
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z');
}

char whitespaces[] = {' ', '\n', '\t'};
bool is_whitespace(char c) {
    for (char ws : whitespaces)
        if (c == ws)
            return true;
    
    return false;
}

int is_match(char c1, char c2) {
    if ((c1 == c2) || (is_alpha(c1) && is_alpha(c2)))
        return 1;

    return 0;
}

int fscore(char c1, char c2) {
    /// match
    if (c1 == c2) {
        if (
            c1 == '(' ||
            c1 == ')' ||
            c1 == '[' ||
            c1 == ']' ||
            c1 == '{' ||
            c1 == '}' ||
            c1 == ';'
        ) {
            return 15;
        }

        return 10;
    } else if (is_alpha(c1) && is_alpha(c2)) {
        // entrambi sono caratteri alfabetici (o _)
        return 5;
    }

    return 0;
}

// variabili per ricordare la stringa/sottosequenza più lunga
int max_str_len = 0, max_str_len_exact = 0;
int max_i = 0, max_j = 0, max_i_exact = 0;
list_t *longest_subseq, *longest_substr;

// void allinea(const char *str1, int len1, const char *str2, int len2) {
void allinea(string s1, string s2) {
    const char *str1 = s1.c_str();
    const char *str2 = s2.c_str();

    int len1 = s1.length() + 1;
    int len2 = s2.length() + 1;

    int costo_match = 1;
    int costo_mismatch = -1;
    int costo_indel = -1;

    int **exact_M = new int *[len1];
    int **tolerant_M = new int *[len1];
    int **subseq_M = new int *[len1];

    for (int i = 0; i < len1; i++) {
        exact_M[i] = new int[len2];
        tolerant_M[i] = new int[len2];
        subseq_M[i] = new int[len2];
    }

    for (int i = 0; i < len1; i++) {
        exact_M[i][0] = costo_indel;
        tolerant_M[i][0] = costo_indel;
        subseq_M[i][0] = costo_indel;
    }

    for (int j = 0; j < len2; j++) {
        exact_M[0][j] = costo_indel;
        tolerant_M[0][j] = costo_indel;
        subseq_M[0][j] = costo_indel;
    }

    // matrice previous
    char **subseq_P = new char *[len1];
    char **substr_P = new char *[len1];

    for (int i = 0; i < len1; i++) {
        subseq_P[i] = new char[len2];
        substr_P[i] = new char[len2];
    }

    /// 0: vengo da i-1 (sopra)
    /// 1: vengo da j-1 (sinistra)
    /// 2: vengo da i-1, j-1 (diag)

    subseq_P[0][0] = -1;
    subseq_P[0][0] = -1;

    for (int i = 1; i < len1; i++) {
        subseq_P[i][0] = 0;
        substr_P[i][0] = 0;
    }

    for (int j = 0; j < len2; j++) {
        subseq_P[0][j] = 1;
        substr_P[0][j] = 1;
    }

    for (int i = 1; i < len1; i++) {
        for (int j = 1; j < len2; j++) {
            // /// calcolo costo sottostringa (con maiuscole/minuscole)
            int match_exact = 0;
            if (str1[i - 1] == str2[j - 1]) {
                match_exact = 1;
            }

            /// sottostringa esatta
            if (match_exact) {
                /// M(i, j) ← 1 + M(i − 1, j − 1)
                exact_M[i][j] = 1 + exact_M[i - 1][j - 1];
                if (exact_M[i][j] > max_str_len_exact) {
                    max_str_len_exact = exact_M[i][j];
                    max_i_exact = i;
                }
            } else { // mismatch
                exact_M[i][j] = 0; // interrompo tutto perché cerco il match esatto
            }

            // flag utile per controllare ad ogni ciclo se bisogna aggiornare max_i e max_j
            bool check_max = true;

            // match con tolleranza per le variabili
            int match = is_match(str1[i - 1], str2[j - 1]);

            /// sottosequenza (sottostringa con tolleranza per variabili e spazi)
            if (match) { // match
                /// M(i, j) ← 1 + M(i − 1, j − 1)
                tolerant_M[i][j] = 1 + tolerant_M[i - 1][j - 1];
                substr_P[i][j] = 2;
            } else { // mismatch
                // // M(i, j) ← max(M(i − 1, j), M(i, j − 1))
                // int max = tolerant_M[i - 1][j];
                // if (max < tolerant_M[i][j - 1])
                //     max = tolerant_M[i][j - 1];
                // tolerant_M[i][j] = max;

                // se vengono aggiunti o cancellati caratteri di spaziatura
                if ((is_alpha(str1[i]) && is_alpha(str2[j - 1])) || (is_whitespace(str1[i]) && is_whitespace(str2[j - 1]))) {
                    tolerant_M[i][j] = tolerant_M[i][j - 1];
                    substr_P[i][j] = 1;
                } else if ((is_alpha(str1[i - 1]) && is_alpha(str2[j])) || (is_whitespace(str1[i - 1]) && is_whitespace(str2[j]))) {
                    tolerant_M[i][j] = tolerant_M[i - 1][j];
                    substr_P[i][j] = 0;
                } else {
                    tolerant_M[i][j] = 0;
                    substr_P[i][j] = -1;
                    check_max = false;
                }
            }

            if (check_max && tolerant_M[i][j] >= max_str_len) {
                max_str_len = tolerant_M[i][j];
                max_i = i;
                max_j = j;
            }

            /// sottosequenza
            int score = fscore(str1[i - 1], str2[j - 1]);

            if (score != 0) { // match/mismatch
                subseq_M[i][j] = score + subseq_M[i - 1][j - 1];
                subseq_P[i][j] = 2;
            } else { /// insertion/deletion
                char cur = str1[i - 1], prev = str1[i - 2];
                int max = subseq_M[i - 1][j];
                subseq_P[i][j] = 0;

                if (max < subseq_M[i][j - 1]) {
                    max = subseq_M[i][j - 1];
                    subseq_P[i][j] = 1;
                    cur = str2[j - 1];
                    prev = str2[j - 2];
                }

                costo_indel = 3;

                if (cur == prev && cur == ' ')
                    costo_indel = 5;

                subseq_M[i][j] = max + costo_indel;
            }

        }
    }

    // for (int i = 0; i < len1; i++) {
    //     // if (i == 0) {
    //     //     printf("   ");
    //     //     for (int j = 0; j < s2; j++)
    //     //         printf("%d ", j);
    //     //     printf("\n");
    //     // }

    //     printf("%2d %c: ", i, i > 0 ? str1[i - 1] : ' ');

    //     for (int j = 0; j < len2; j++) {
    //         printf("%2d ", M[i][j]);
    //     }
    //     printf("\n");
    // }

    // for (int i = 0; i < len1; i++) {
    //     // if (i == 0) {
    //     //     printf("   ");
    //     //     for (int j = 0; j < s2; j++)
    //     //         printf("%d ", j);
    //     //     printf("\n");
    //     // }

    //     printf("%2d %c: ", i, i > 0 ? str1[i - 1] : ' ');

    //     for (int j = 0; j < len2; j++) {
    //         printf("%d ", P[i][j]);
    //     }
    //     printf("\n");
    // }

    int i = max_i;
    int j = max_j;

    // ricostruzione sottosequenza per la stringa con tolleranza
    while (substr_P[i][j] != -1) {
        switch (substr_P[i][j]) {
            case 0:
                list_insert_front(longest_substr, '\4');
                list_insert_front(longest_substr, str1[--i]);
                list_insert_front(longest_substr, '\2');
                break;
            case 1:
                list_insert_front(longest_substr, '\4');
                list_insert_front(longest_substr, str2[--j]);
                list_insert_front(longest_substr, '\1');
                break;
            case 2:
                if (str1[--i] == str2[--j]) {
                    list_insert_front(longest_substr, str1[i]);
                } else {
                    list_insert_front(longest_substr, '_');
                }

                break;
        }
    }

    i = len1 - 1;
    j = len2 - 1;

    // ricostruzione sottosequenza per l'intero file
    while (i > 0 || j > 0) {
        switch (subseq_P[i][j]) {
            case 0:
                list_insert_front(longest_subseq, '\4');
                list_insert_front(longest_subseq, str1[--i]);
                list_insert_front(longest_subseq, '\2');
                break;
            case 1:
                list_insert_front(longest_subseq, '\4');
                list_insert_front(longest_subseq, str2[--j]);
                list_insert_front(longest_subseq, '\1');
                break;
            case 2:
                if (str1[--i] == str2[--j]) {
                    list_insert_front(longest_subseq, str1[i]);
                } else {
                    list_insert_front(longest_subseq, '\4');
                    list_insert_front(longest_subseq, '_');
                    list_insert_front(longest_subseq, '\3');
                }

                break;
        }
    }

    /// ricrea la stringa comune più lunga grazie alla lunghezza (max_str) e l'indice della fine (max_i)
    string longest_substr_exact = s1.substr(max_i_exact - max_str_len_exact + 1, max_str_len_exact - 1);

    printf("Sottostringa più lunga con tolleranza 0 (%i caratteri):\n%s", max_str_len_exact, longest_substr_exact.c_str());
    printf("\n############################################################\n");
    printf("Sottostringa più lunga con tolleranza per variabili e spazi:\n");
    list_print(longest_substr);
    printf("\n############################################################\n");
    printf("Panoramica delle differenze tra i due file: in verde le aggiunte nel primo file, in rosso le aggiunte nel secondo file.\n");
    list_print(longest_subseq);
}

int main(int argc, char **argv) {
    string str[2] = {"", ""};

    for (int i = 1; i <= 2; i++) {
        ifstream file(argv[i]);

        if (!file.is_open()) {
            fprintf(stderr, "File non valido: %s", argv[i]);
            return 1;
        }

        ostringstream stream;
        stream << file.rdbuf();
        str[i - 1] = stream.str();
    }

    longest_substr = list_new();
    longest_subseq = list_new();
    allinea(str[0], str[1]);

    return 0;
}
