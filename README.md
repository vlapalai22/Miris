

# Program - Miris
---------------------------------------------------------------------------------------------------------------------
Miris is an app that manages and tracks small money transfers between users.

This program implements a directed multi-graph to model transactions between users. Each user is represented by a 
unique node (UserID with length of 7), and each transaction between two users is represented as a directional edge. 
The graph is implemented using a hash table to store nodes (users) for efficient lookups.

The primary application of this graph is to track and analyze financial transactions, including finding paths 
between users, detecting cycles, and managing a list of transactions for each user.


# Structs - dir_multi_graph.h
---------------------------------------------------------------------------------------------------------------------
- Edges:
    - source[userId_length] : Stores the source (UserID) of the directed edge (transaction)
    - destination[userId_length] : Stores the Destination (UserID) of the transaction
    - amount : Stores the amount of money of the transaction
    - date[Date_length] : Stores the date of the transaction (12 digits <YYYY-MM-DD>)

- Node:
    - UserID[userId_length] : Stores the ID of the user as string (7 digits: <AAAAAAA>/<0000000>)
    - edges : Stores the edges of each user as a linked list
    - next : Store the next user in the hash table

- Graph:
    - numNodes : Number of Users
    - numEdges : Number of Edges
    - nodes : Hash table to store the users
    - hash_size : size of hash table


# Functions - dir_multi_graph.c
---------------------------------------------------------------------------------------------------------------------
In this file some basic functions are beeing developed for the Graph creation and managment,
Quering, Analysis, Utility

> Graph* createGraph();
> Node* addNode(Graph* graph, const char* userID);
> Node* findNode(Graph* graph, const char* userID);
> void addEdge(Graph* graph, const char* source, const char* destination,double amount,const char* date);
> void deleteNode(Graph* graph, const char* userID);
> void deleteEdge(Graph* graph, const char* source, const char* destination);
> void modifyEdge(Graph* graph,const char* source,const char* destination,double old_amount,
                                double new_amount,const char* old_date,const char* new_date);
> void findOutgoingEdges(Graph* graph, const char* userID);
> void findIncomingEdges(Graph* graph, const char* destination);
> void findPath(Graph* graph, const char* source,const char* destination);
> void Circlesfind(Graph* graph, const char* node);
> void findCircles(Graph* graph,const char* node, double);
> void traceFlow(Graph* graph, const char* node, int length);
> void printGraph(Graph* graph);
> void freeGraph(Graph* graph);

- Also some helping functions that are not declared in the .h file:

> DFS : A DFS function to find a path between two nodes (Users)
                used in findPath()
> Circles_dfs : DFS to detect circles from given node 
                used in Circlesfind()
> printCircle : Func that prints a circle given a path matrix
                used in Circles_dfs , findCircles
> Circles_mod_dfs : Find circles of transactions with a minimum
                    amount of money in it. Used in dindCircles
> traceFlowUtility : A util function that trace the flow of money 
                    transactions until a certain depth. 
                    Used is traceflow

        
# Main Program - main.c
---------------------------------------------------------------------------------------------------------------------
In this file is the main loop, the command prompt (miris>) and the functions that load the program from an input file 
and write to an output file.

## Dependencies

- Standard C libraries: `stdio.h`, `stdlib.h`, `string.h`,  `float.h`.

## Compilation

1) Use Makefile:
    - make all : compiles the program
    - make run : runs program (make changes at: /Makefile line 18 ./$(TARGET) `input.txt` `output.txt`)
    - make leaks : runs with valgrind (make changes at: /Makefile line 22 
                                        valgrind --leak-check=full ./$(TARGET) `input.txt` `output.txt`)
    - make clean : removes .o & .exe files 

2) Use gcc compiler through terminal:
    - gcc -c main.c dir_multi_graph.c
    - gcc main.o dir_multi_graph.o -o miris
    - ./miris `input.txt` `output.txt`