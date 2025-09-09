# LASD (Laboratorio di Algoritmi e Strutture Dati)
**I worked on this projects with [@Akmadan23](https://github.com/Akmadan23)**

Collection of assignements for "Laboratorio di Algoritmi e Strutture Dati", a subject for the CS Bachelor Degree at UniPR (University of Parma).
Each file is divided according to the topic of the course, in particular:

## CONSEGNA N.1 - SORTING ALGORITHM
Customized sorting algorithm used to sort a predefined dataset (data.txt).

## CONSEGNA N.2 - BINARY TREE
Edit the original assignement file and add the following functions:
  1) Tree-flip: for every node, the right subtree becomes the ledft subtree and viceversa
  2) Calculate the depth of every node of the tree
  3) isBalanced: return a flag that indicates if the tree is balanced or not
  4) isComplete: return a flag that indicates if the tree is complete or not, following the classic definition
  5) Lowest Common Ancestor: given two different values of the tree, return the value of the lowest node that contains both in his subtree

## CONSEGNA N.3 - CYCLIC GRAPH
Edit the original file including the following instruction:
  1) Visit the whole graph, even disconnected oarts
  2) Calculate the lenght of the longest cycle, and return its nodes

## CONSEGNA N.4 - SHORTEST PATH
Edit the original file, modifying the extraction of the minimum node using the heap, guaranteeing the log(n) complexity
and to correctly get back the heap's array index starting from the node code.
After this, study and implement Bellman-Ford's algorithm, using the shortest path code arleady written,
and test it with a fully negative graph and a graph with only one negative edge

## CONSEGNA N.5 - KRUSKAL'S ALGORITHM
Implement Kruskal's algorithm

## CONSEGNA N.6 - STRING AND FILE CHECK
Adapt the Needleman-Wunsch algorithm to obtain a comparison between two versions of the same source code, and test it
with a C++ file with at least 100 row's.
