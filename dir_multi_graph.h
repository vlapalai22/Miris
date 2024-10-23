#ifndef DIR_MULTI_GRAPH_H
#define DIR_MULTI_GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>

#define HASH_SIZE 1000000
#define USER_ID_LENGTH 7
#define DATE_LENGTH 12


extern size_t memCount;



// This struct defines each money transaction | The graph is directional so we need the source and the destination as well as the weight (amount of money)
typedef struct Edge {
    char source[USER_ID_LENGTH];       // store the source of each transaction     
    char destination[USER_ID_LENGTH];  // store the destination of each transaction
    double amount;  // store the amount of money for each transaction
    char date[DATE_LENGTH];  // store the date of every transaction like: (YYYY-MM-DD ~~ 2024-10-03)
    struct Edge* next;
} Edge;



// This struct will define each individual user with an ID
typedef struct Node {
    char UserID[USER_ID_LENGTH]; // store the ID of each user as a string like: "VP5581" (two letters and four digits)
    Edge* edges;        // store the edges of each user as a linked list
    struct Node* next;  // store the next user in the hash table
} Node;



typedef struct Graph {
    int numNodes;    // Number of Users
    int numEdges;    // Number of transactions
    Node** nodes;   // Im using a hash table to store the clients info (nodes) that has O(1) time complexity
    int hash_size;  
}Graph;



Graph* createGraph();
Node* addNode(Graph* graph, const char* userID);
Node* findNode(Graph* graph, const char* userID);
void addEdge(Graph* graph, const char* source, const char* destination, double amount, const char* date);
void deleteNode(Graph* graph, const char* userID);
void deleteEdge(Graph* graph, const char* source, const char* destination);
void modifyEdge(Graph* graph, const char* source, const char* destination,double old_amount, double new_amount, const char* old_date,const char* new_date);
void findOutgoingEdges(Graph* graph, const char* userID);
void findIncomingEdges(Graph* graph, const char* destination);
void findPath(Graph* graph, const char* source,const char* destination);
void Circlesfind(Graph* graph, const char* node);
void findCircles(Graph* graph,const char* node, double);
void traceFlow(Graph* graph, const char* node, int length);
void printGraph(Graph* graph);
void freeGraph(Graph* graph);

#endif 