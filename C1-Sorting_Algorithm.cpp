// svolto da Azad Ahmadi e Libaan Hassan Mohamed

#include <fstream>
#include <iostream>
using namespace std;

// g++ C1-Sorting-Algorithm.cpp -> ./a.out > consegna1.txt

// Obbiettivi CONSEGNA:
/*
Sviluppare un algoritmo di sorting dedicato per la serie di dati dati.txt (100 serie da 100 elementi ciascuna).
*/

int ct_swap = 0;
int ct_cmp = 0;
int ct_read = 0;

int max_dim = 0;
int ntests = 100;
int ndiv = 1;
int details = 0;
int graph = 0;

int n = 0; /// dimensione dell'array

void print_array(int *A, int dim) {
    for (int j = 0; j < dim; j++) {
        printf("%d ", A[j]);
    }
    printf("\n");
}

int parse_cmd(int argc, char **argv) {
    /// parsing argomento
    max_dim = 1000;

    for (int i = 1; i < argc; i++) {
        if (argv[i][1] == 'd')
            ndiv = atoi(argv[i] + 3);
        if (argv[i][1] == 't')
            ntests = atoi(argv[i] + 3);
        if (argv[i][1] == 'v')
            details = 1;
        if (argv[i][1] == 'g') {
            graph = 1;
            ndiv = 1;
            ntests = 1;
        }
    }

    return 0;
}

void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
}

void gnome_sort(int *A, int n) {
    int i = 1, j = 2;

    /// variabili di appoggio per risparmiare letture
    int v1 = A[0], v2 = A[1];
    ct_read += 2;

    while (i < n) {
        if (v1 > v2) {
            A[i] = v1;
            A[i - 1] = v2;

            if (--i) {
                /// finché v1 è maggiore di v2, v1 scala all'indietro
                /// fino a quando non saranno in ordine
                v1 = A[i - 1];
                ct_read++;
                continue;
            }
        }

        /// se si arriva a questo punto significa che v1 e v2 sono ordinati,
        /// dunque si procede alla riassegnazione degli stessi e l'aggiornamento
        /// degli indici i e j

        if (i == j - 1) {
            v1 = v2;
        } else {
            v1 = A[j - 1];
            ct_read++;
        }

        v2 = A[j];
        ct_read++;

        i = j++;
    }
}

void merge(int *A, int p, int q, int r) {
    int size_L = q - p, size_R = r - q;

    int *L = new int[size_L + 1];
    L[size_L] = 1000000000;

    for (int i = 0; i < size_L; i++) {
        L[i] = A[p + i];
        ct_read++;
    }

    int *R = new int[size_R + 1];
    R[size_R] = 1000000000;

    for (int i = 0; i < size_R; i++) {
        R[i] = A[q + i];
        ct_read++;
    }

    // if (details) {
    //     cout << "Array L (" << p << " .. " << q << "): ";
    //     print_array(L, q - p + 1);
    //     cout << "Array R (" << q + 1 << " .. " << r << "): ";
    //     print_array(R, r - q);
    // }

    /// flag utili per dimezzare le letture durante il merge:
    /// il valore tmp_L o tmp_R viene aggiornato solo se è stato utilizzato
    /// nell'iterazione precedente
    bool update_L = true, update_R = true;
    int tmp_L, tmp_R;

    for (int i = 0, j = 0, k = p; k < r; k++) {
        if (update_L) {
            tmp_L = L[i++];
            ct_read++;
        }

        if (update_R) {
            tmp_R = R[j++];
            ct_read++;
        }
// compilazione: g++ consegna3.cpp -> ./a.out -graph

        if (tmp_L <= tmp_R) {
            A[k] = tmp_L;
            update_L = true;
            update_R = false;
        } else {
            A[k] = tmp_R;
            update_L = false;
            update_R = true;
        }
    }

    delete[] L;
    delete[] R;
}

void custom_sort(int *A, int n) {
    for (int i = 500, j = 999; i < j; i++, j--) {
        swap(A[i], A[j]);
        ct_read += 2;
    }

    /// ottimizzato per il dataset
    gnome_sort(A, 325);         // 0-324
    gnome_sort(A + 463, 37);    // 463-499
    gnome_sort(A + 500, 374);   // 500-874
    gnome_sort(A + 955, 45);    // 955-999

    /// non ottimizzato ma funzionante su qualsiasi dataset
    // gnome_sort(A, 1000);

    merge(A, 0, 500, 1000);
}

int main(int argc, char **argv) {
    int i, test;
    int *A;

    if (parse_cmd(argc, argv))
        return 1;

    /// allocazione array
    A = new int[max_dim];

    n = max_dim;

    // srand((unsigned)time(NULL));
    // //creazione file input: NON USARE PIU' --> il file data.txt ufficiale è stato allegato, per permettere confronti equivalenti
    //  FILE *f = fopen("data.txt", "w+");
    //  int size=100;
    //  for (int j = 0; j < size; j++) {
    //      for (int i = 0; i < n; i++) {
    //          int v = 0;             
    //          v=(int)(100000*exp(-(0.0+i-n/2)*(0.0+i-n/2)/n/n*64));
    //          v+=rand()%(i+1);
    //          fprintf(f, "%d,", v);
    //      }
    //      fprintf(f, "\n");
    //  }
    //  fclose(f);

    ifstream input_data;
    input_data.open("data.txt");

    int read_min = -1;
    int read_max = -1;
    long read_avg = 0;

    /// lancio ntests volte per coprire diversi casi di input random
    for (test = 0; test < ntests; test++) {

        /// inizializzazione array: numeri random con range dimensione array
        for (i = 0; i < n; i++) {
            char comma;
            input_data >> A[i];
            input_data >> comma;
        }

        if (details) {
            printf("caricato array di dimensione %d\n", n);
            print_array(A, n);
        }

        ct_swap = 0;
        ct_cmp = 0;
        ct_read = 0;

        /// algoritmo di sorting
        custom_sort(A, n);

        if (details) {
            printf("Output:\n");
            print_array(A, n);
        }

        /// statistiche
        read_avg += ct_read;
        if (read_min < 0 || read_min > ct_read)
            read_min = ct_read;
        if (read_max < 0 || read_max < ct_read)
            read_max = ct_read;
        printf("Test %d %d\n", test, ct_read);
    }

    printf("N test: %d, Min: %d, Med: %.1f, Max: %d\n",
        ntests,
        read_min,
        (0.0 + read_avg) / ntests,
        read_max);

    delete[] A;
    return 0;
}
