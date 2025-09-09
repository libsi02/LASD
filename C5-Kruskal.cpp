// svolto da Azad Ahmadi e Libaan Hassan Mohamed
// Ispirato dalla seguente implementazione: https://www.geeksforgeeks.org/prims-minimum-spanning-tree-mst-greedy-algo-5/

#include <bits/stdc++.h>
#include <cstdio>
using namespace std;

// g++ C5-Kruskal.cpp -> ./a.out -g

//Obbiettivi CONSEGNA:
/*
Implementazione dell'algoritmo di Kruskal
*/

FILE *output_graph;
bool draw_graph = true;

// numero di vertici nel grafo
#define V 6

// alias per la coppia di int
typedef pair<int, int> int_int;

void prim(int graph[V][V]) {
    // array che tiene traccia dei pesi per raggiungere ogni nodo del MST
    int key[V];

    // array che tiene traccia del nodo genitore di ogni nodo del MST
    int parent[V];

    // array che tiene traccia dei nodi già visitati
    bool vis[V];

    for (int i = 0; i < V; i++) {
        key[i] = INT_MAX;
        vis[i] = false;
    }

    // il nodo 0 è aggiunto di default
    key[0] = 0;
    parent[0] = -1;

    // priority queue per la gestione degli archi da analizzare
    // ogni elemento della coda è una coppia di interi che rappresentano un arco
    // il primo valore è il peso e il secondo è il nodo di arrivo
    priority_queue<int_int, vector<int_int>, greater<int_int>> pq;

    // Vertice iniziale
    pq.push({0, 0});

    while (!pq.empty()) {
        // top() prende dalla pq l'elemento in cima, nel nostro caso prendiamo l'indice del nodo col peso più grande
        int node = pq.top().second;
        pq.pop();
        vis[node] = true;

        for (int i = 0; i < V; i++) {
            // Se il nodo i non è ancora stato visitato e il peso
            // dell'arco che lo raggiunge è minore degli altri raggiungibili,
            // allora vengono aggiornati i valori degli array key e parent
            // e viene aggiunto alla priority queue
            if (!vis[i] && graph[node][i] != 0 && graph[node][i] < key[i]) {
                pq.push({graph[node][i], i});
                key[i] = graph[node][i];
                parent[i] = node;
            }
        }
    }

    cout << "Arco\tPeso\n";
    for (int i = 1; i < V; i++) {
        printf("%d-%d\t%d\n", parent[i], i, graph[i][parent[i]]);
    }

    if (draw_graph) {
        for (int i = 0; i < V; i++) {
            if (parent[i] != -1) {
                fprintf(output_graph, "node_%d -> node_%d [label=%d color=green]\n", parent[i], i, graph[i][parent[i]]);
            }

            for (int j = i + 1; j < V; j++) {
                int weight = graph[i][j];
                if (j == parent[i] || i == parent[j]) {
                    // l'arco è stato già disegnato
                    continue;
                }

                if (weight > 0) {
                    fprintf(output_graph, "node_%d -> node_%d [label=%d]\n", i, j, weight);
                }
            }
        }
    }
}

int main() {
    int graph[V][V] = {
        
        {0, 2, 0, 6, 0, 4},
        {2, 0, 3, 8, 5, 0},
        {0, 3, 0, 0, 7, 6},
        {6, 8, 0, 0, 9, 0},
        {0, 5, 7, 9, 0, 0},
        {4, 0, 6, 0, 0, 0},

        // {0, 1, 3, 2, 0, 0},
        // {1, 4, 0, 6, 3, 5},
        // {3, 0, 7, 0, 2, 7},
        // {2, 6, 0, 6, 0, 3},
        // {0, 3, 2, 0, 7, 1},
        // {0, 5, 7, 3, 1, 7},

        // {4, 0, 1, 2, 4, 0},
        // {0, 6, 3, 6, 7, 6},
        // {1, 3, 4, 1, 3, 0},
        // {2, 6, 1, 4, 0, 4},
        // {4, 7, 3, 0, 2, 1},
        // {0, 6, 0, 4, 1, 6},
    };

    if (draw_graph) {
        output_graph = fopen("graph.dot", "w");
        fprintf(output_graph, "digraph G {\n\
            node [shape=circle]\n\
            edge [color=grey dir=none fontsize=8]\n\
            rankdir=LR\n\n");

        for (int i = 0; i < V; i++) {
            fprintf(output_graph, "node_%d [label=%d]\n", i, i);
        }
    }

    prim(graph);

    if (draw_graph) {
        fprintf(output_graph, "}\n");
        fclose(output_graph);
        printf("Grafico salvato nel file 'graph.dot'");
    }

    return 0;
}
