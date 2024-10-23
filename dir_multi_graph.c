#include "dir_multi_graph.h"
   

size_t memCount = 0;


int hash(const char *str) {
    int hash = 58; // Starting value as an int
    int c;

    while ((c = *str++)) {
        // Update hash and keep it within the int range
        hash = ((hash << 5) + hash + c); // hash * 33 + c
    }
    return hash % HASH_SIZE;
}


Graph* createGraph(){
    int hash_size = HASH_SIZE;
    Graph* graph = malloc(hash_size * sizeof(Graph));
    memCount += hash_size * sizeof(Graph);
    graph->numNodes = 0;
    graph->hash_size = hash_size;
    graph->nodes = malloc(hash_size * sizeof(Node*));
    memCount += hash_size * sizeof(Node*);
    for(int i=0;i<hash_size;i++){
        graph->nodes[i] = NULL;
    }
    return graph;
}


Node* addNode(Graph* graph, const char* UserId){

    int hash_index = hash(UserId);
    Node* newNode = malloc(sizeof(Node));
    memCount += sizeof(Node);
    strcpy(newNode->UserID, UserId);
    newNode->edges = NULL;
    newNode->next = graph->nodes[hash_index];   // Seperate chaining method to deal with collisions set the new node as the head of the linked list

    graph->nodes[hash_index] = newNode; // Add the new node to the hash table and if there is a collision replace the head of the linked list with the new node
    graph->numNodes++;
    return newNode;
}


Node* findNode(Graph* graph, const char* node){
    unsigned long index = hash(node);
    int hash_index = index % graph->hash_size;

    Node* currentNode = graph->nodes[hash_index];
    while(currentNode != NULL){                         // Traverse the linked list to find the node
        if(strcmp(currentNode->UserID, node) == 0){     // If the node is found return it
            break;
        }
        currentNode = currentNode->next;
    }
    return currentNode;
}


void addEdge(Graph* graph, const char* source, const char* destination, double amount, const char* date){
   
    Node* sourceNode = findNode(graph,source);
    if(sourceNode == NULL){    // The source of the node was not found add it
        sourceNode = addNode(graph,source);
    }
    if(findNode(graph,destination) == NULL){    // The destination of the source was not found add it
        addNode(graph,destination);
    }

    Edge* newEdge = malloc(sizeof(Edge));
    memCount += sizeof(Edge);
    strcpy(newEdge->source, source);
    strcpy(newEdge->destination, destination);
    newEdge->amount = amount;
    strcpy(newEdge->date, date);
    newEdge->next = sourceNode->edges;
    sourceNode->edges = newEdge;
    graph->numEdges++;
}


void deleteNode(Graph* graph, const char* UserId) {
    // Step 1: Find the node to delete
    Node* currentNode = findNode(graph, UserId);
    if (currentNode == NULL) {
        printf("User ID %s not found.\n", UserId);
        return; // User does not exist
    }

    // Step 2: Remove all edges originating from this user
    Edge* currentEdge = currentNode->edges;
    while (currentEdge != NULL) {
        Edge* temp = currentEdge;
        currentEdge = currentEdge->next;
        free(temp); // Freeing each edge
        memCount -= sizeof(Edge);
    }
    currentNode->edges = NULL; // Set edges pointer to NULL after freeing

    // Step 3: Remove the node from the hash table
    int hash_index = hash(UserId) % graph->hash_size;
    Node* prevNode = NULL;
    Node* tempNode = graph->nodes[hash_index];

    while (tempNode != NULL) {
        if (strcmp(tempNode->UserID, UserId) == 0) {       // Found the node to delete
            if (prevNode == NULL) {
                // Deleting the first node in the list
                graph->nodes[hash_index] = tempNode->next;
            } else {
                // Deleting a node in the middle or end
                prevNode->next = tempNode->next;
            }
            free(tempNode); // Free the node itself
            memCount -= sizeof(Node);
            graph->numNodes--; 
            return;
        }
        prevNode = tempNode;
        tempNode = tempNode->next;
    }
    // If we reach this point, the node was not found
    printf("User ID %s was not found in the graph.\n", UserId);
}


void deleteEdge(Graph* graph,const char* source, const char* destination){
    Node* sourceNode = findNode(graph, source);
    if (sourceNode == NULL) {
        printf("Source user ID: %s not found.\n", source);
        return;
    }

    Edge* currentEdge = sourceNode->edges;
    Edge* previousEdge = NULL;

    // Traverse the edge list
    while (currentEdge != NULL) {
        // Check if the current edge matches the destination
        if (strcmp(currentEdge->destination, destination) == 0) {
            // Found the edge to delete
            if (previousEdge == NULL) {
                // Edge to delete is the first edge in the list
                sourceNode->edges = currentEdge->next;
            } else {
                // Edge to delete is not the first one
                previousEdge->next = currentEdge->next;
            }
            free(currentEdge); 
            memCount -= sizeof(Edge);
            graph->numEdges--; 
            printf("Edge from %s to %s deleted successfully.\n", source, destination);
            return;
        }
        previousEdge = currentEdge;
        currentEdge = currentEdge->next;
    }

    // If we reach here, the edge was not found
    printf("Edge from %s to %s not found.\n", source, destination);
}


void modifyEdge(Graph* graph, const char* source, const char* destination,double old_amount, double new_amount, const char* old_date,const char* new_date){
    Node* sourceNode = findNode(graph, source);
    if (sourceNode == NULL) {
        printf("Source node %s not found!\n", source);
        return;
    }

    // Find the first matching edge and modify it
    Edge* currentEdge = sourceNode->edges;
    while (currentEdge != NULL) {
        if (strcmp(currentEdge->destination, destination) == 0 && currentEdge->amount == old_amount && strcmp(currentEdge->date, old_date) == 0) {
            // Modify the first matching edge
            currentEdge->amount = new_amount;
            strcpy(currentEdge->date, new_date);
            printf("Edge from %s to %s updated. New amount: %.2f, New date: %s\n", source, destination, new_amount, new_date);
            return; // Exit after modifying the first matching edge
        }
        currentEdge = currentEdge->next;
    }

    printf("No matching edge found from %s to %s with amount %.2f and date %s.\n", source, destination, old_amount, old_date);

}



void findOutgoingEdges(Graph* graph, const char* UserId){
    Node* sourceNode = findNode(graph, UserId);
    if (sourceNode == NULL) {
        printf("User ID %s not found.\n", UserId);
        return;
    }

    Edge* currentEdge = sourceNode->edges;
    if (currentEdge == NULL) {
        printf("User ID %s has no outgoing edges.\n", UserId);
        return;
    }

    printf("Outgoing edges from user ID %s:\n", UserId);
    while (currentEdge != NULL) {
        printf("  -> %s (Amount: %.2f, Date: %s)\n", currentEdge->destination, currentEdge->amount, currentEdge->date);
        currentEdge = currentEdge->next;
    }
}


void findIncomingEdges(Graph* graph, const char* destination) {
    // Check if the destination node exists
    Node* destinationNode = findNode(graph, destination);
    if (destinationNode == NULL) {
        printf("Non-existing node: %s\n", destination);
        return;
    }

    // Iterate through all nodes to find edges pointing to destination
    bool found = false; // keep a bool var to check if none where found
    for (int i = 0; i < graph->hash_size; i++) {
        Node* currentNode = graph->nodes[i];
        while (currentNode != NULL) {       // Iterate through all edges of each node
            Edge* currentEdge = currentNode->edges;
            while (currentEdge != NULL) {
                // Check if the edge's destination matches Ni
                if (strcmp(currentEdge->destination, destination) == 0) {   // If the edge points to the destination print it
                    printf("\t %s -> %s (Amount: %.2f, Date: %s)\n", currentEdge->source, currentEdge->destination, currentEdge->amount, currentEdge->date);
                    found = true;
                }
                currentEdge = currentEdge->next;
            }
            currentNode = currentNode->next;
        }
    }

    // If no edges were found, print nothing
    if (!found) {
        printf("none\n");
    }
}


bool DFS(Graph* graph, const char* current, const char* destination, bool* visited, char** path, int* pathIndex) {
    int currentHashIndex = hash(current) % graph->hash_size;

    visited[currentHashIndex] = true;
    path[(*pathIndex)++] = strdup(current);  // Add current node to the path

    if (strcmp(current, destination) == 0) {
        return true;  // Path found
    }

    Node* currentNode = findNode(graph, current);
    if (currentNode == NULL) {
        return false;
    }

    Edge* currentEdge = currentNode->edges;
    while (currentEdge != NULL) {
        int nextHashIndex = hash(currentEdge->destination) % graph->hash_size;
        if (!visited[nextHashIndex]) {
            if (DFS(graph, currentEdge->destination, destination, visited, path, pathIndex)) {
                return true;  // Path found in recursion
            }
        }
        currentEdge = currentEdge->next;
    }

    (*pathIndex)--;  // Backtrack
    return false;  // No path found
}


void findPath(Graph* graph,const char* source,const char* destination){
    Node* nodeNi = findNode(graph, source);
    Node* nodeNj = findNode(graph, destination);

    if (nodeNi == NULL) {
        printf("Non-existing node: %s\n", source);
        return;
    }
    if (nodeNj == NULL) {
        printf("Non-existing node: %s\n", destination);
        return;
    }

    // Prepare visited array and path tracking
    bool* visited = malloc(graph->hash_size * sizeof(bool));
    char** path = malloc(graph->hash_size * sizeof(char*));
    int pathIndex = 0;

    for (int i = 0; i < graph->hash_size; i++) {
        visited[i] = false;
    }

    // Perform DFS to find a path from Ni to Nj
    if (DFS(graph, source, destination, visited, path, &pathIndex)) {
        // Print the path
        for (int i = 0; i < pathIndex; i++) {
            printf("%s", path[i]);
            if (i < pathIndex - 1) {
                printf(" -> ");
            }
            free(path[i]);  // Free strdup memory after use
        }
        printf("\n");
    } else {
        printf("(none)\n");  // No path found
    }

    // Free allocated memory
    free(visited);
    free(path);
}



// function to print the circle 
void printCircle(char** path, int pathIndex, const char* startNode) {
    bool cycleStarted = false;
    for (int i = 0; i < pathIndex; i++) {
        if (strcmp(path[i], startNode) == 0) {
            cycleStarted = true;
        }
        if (cycleStarted) {
            printf("%s -> ", path[i]);
        }
    }
    printf("%s\n", startNode);  // Close the cycle
}


// DFS function to detect circles (used in Circlesfind)
bool Circles_dfs(Graph* graph, const char* start, const char* current, bool* visited, bool* stack, char** path, int* pathIndex, bool* foundCircle) {
    int currentHashIndex = hash(current) % graph->hash_size;

    visited[currentHashIndex] = true;
    stack[currentHashIndex] = true;
    path[(*pathIndex)++] = strdup(current);  // Add current node to the path

    Node* currentNode = findNode(graph, current);
    if (currentNode == NULL) {
        return false;
    }

    Edge* currentEdge = currentNode->edges;
    while (currentEdge != NULL) {
        int nextHashIndex = hash(currentEdge->destination) % graph->hash_size;

        if (!visited[nextHashIndex]) {
            // Recursive DFS call
            if (Circles_dfs(graph, start, currentEdge->destination, visited, stack, path, pathIndex, foundCircle)) {
                return true;
            }
        } else if (stack[nextHashIndex]) {
            // Found a circle
            if (strcmp(currentEdge->destination, start) == 0) {
                printCircle(path, *pathIndex, start);
                *foundCircle = true;
            }
        }
        currentEdge = currentEdge->next;
    }

    stack[currentHashIndex] = false;
    (*pathIndex)--;  // Remove current node from path when backtracking
    return false;
}



// Function to handle 'circlefind Ni' command (uses dfs)
void Circlesfind(Graph* graph, const char* Ni) {
    // Find if node Ni exists
    Node* startNode = findNode(graph, Ni);
    if (startNode == NULL) {
        printf("Non-existing node: %s\n", Ni);
        return;
    }

    bool* visited = malloc(graph->hash_size * sizeof(bool));
    bool* stack = malloc(graph->hash_size * sizeof(bool));
    char** path = malloc(graph->numNodes * sizeof(char*));  // Track the current path
    int pathIndex = 0;
    bool foundCircle = false;

    // Initialize visited and stack arrays
    for (int i = 0; i < graph->hash_size; i++) {
        visited[i] = false;
        stack[i] = false;
    }

    // Perform DFS to find cycles
    Circles_dfs(graph, Ni, Ni, visited, stack, path, &pathIndex, &foundCircle);

    if (!foundCircle) {
        printf("(none)\n");
    }

    // Free allocated memory
    for (int i = 0; i < pathIndex; i++) {
        free(path[i]);
    }
    free(path);
    free(visited);
    free(stack);
}



// DFS Function that finds circles modified to check for minimum transaction amount (used in findCircles) 
bool Circles_mod_dfs(Graph* graph, const char* start,const char* current, bool* visited, bool* stack, char** path, int* pathIndex, double* minAmountInCircle, double amountThreshold, bool* foundCircle){
    unsigned long index = hash(current);
    int currentHashIndex = index % graph->hash_size;

     visited[currentHashIndex] = true;
    stack[currentHashIndex] = true;
    path[(*pathIndex)++] = strdup(current);  // Add current node to the path

    Node* currentNode = findNode(graph, current);
    if (currentNode == NULL) {
        return false;
    }

    Edge* currentEdge = currentNode->edges;
    while (currentEdge != NULL) {
        int nextHashIndex = hash(currentEdge->destination) % graph->hash_size;
        
        // Only proceed if the amount is at least the threshold
        if (currentEdge->amount >= amountThreshold) {

            if (*minAmountInCircle == 0.0 || *minAmountInCircle > currentEdge->amount) {
                *minAmountInCircle = currentEdge->amount;  // Update minimum amount in circle
            }

            if (!visited[nextHashIndex]) {
                // Recursive DFS call
                if (Circles_mod_dfs(graph, start, currentEdge->destination, visited, stack, path, pathIndex, minAmountInCircle, amountThreshold, foundCircle)) {
                    return true;
                }
            } 
            else if (stack[nextHashIndex]) {
                // Found a circle
                if (strcmp(currentEdge->destination, start) == 0) {
                    *foundCircle = true;
                    return true;
                }
            }
        }
        currentEdge = currentEdge->next;
    }

    stack[currentHashIndex] = false;
    (*pathIndex)--;  // Remove current node from path when backtracking
    return false;
}

// Function that finds a circle for the given node with a minimum transaction amount involved in the circle (uses mod_dfs)
void findCircles(Graph* graph,const char* node, double amount){
    Node* startNode = findNode(graph, node);
    if (startNode == NULL) {
        printf("Non-existing node: %s\n", node);
        return;
    }

    // Initialize visited and stack arrays
    bool* visited = malloc(graph->hash_size * sizeof(bool));
    bool* stack = malloc(graph->hash_size * sizeof(bool));

    // Path array to store nodes in the current path
    char** path = malloc(graph->hash_size * sizeof(char*));
    int pathIndex = 0;
    bool foundCircle = false;
    double minAmountInCircle = DBL_MAX;

    // Initialize visited and stack arrays
    for (int i = 0; i < graph->hash_size; i++) {
        visited[i] = false;
        stack[i] = false;
    }

    // Start DFS from node 
    if (Circles_mod_dfs(graph, node, node, visited, stack, path, &pathIndex, &minAmountInCircle, amount, &foundCircle)) {
        // If a circle was found, print the circle
        if (foundCircle) {      // print the path of nodes involved in the circle
            printCircle(path,pathIndex, node);
            printf("\tmin transaction: %.2f\n",minAmountInCircle);
        }
    } 
    else {
        printf("(none)\n");  // No circle found
    }

    // Free allocated memory
    for (int i = 0; i < pathIndex; i++) {
        free(path[i]);
    }
    free(path);
    free(visited);
    free(stack);
}

// 
void traceFlowUtility(Graph* graph, const char* node, bool* visited, char** path, int* pathIndex, int length){
    // Check if the maximum path length is reached
    if (*pathIndex >= length) {
        return;
    }

    // Mark the current node as visited
    visited[hash(node) % graph->hash_size] = true;
    path[*pathIndex] = strdup(node);    
    (*pathIndex)++;

    // Print the current path if it's not empty
    if (*pathIndex > 1) {
        // Output the path
        printf("%s", path[0]);
        for (int i = 1; i < *pathIndex; i++) {
            printf(", %s", path[i]);
        }
        printf("\n");
    }

    // Get the current node's edges
    Node* currentNode = findNode(graph, node);
    Edge* edge = currentNode->edges;

    // Explore each outgoing edge
    while (edge != NULL) {
        // If the destination node has not been visited, continue the path
        if (!visited[hash(edge->destination) % graph->hash_size]) {
            traceFlowUtility(graph, edge->destination, visited, path, pathIndex, length);
        }
        edge = edge->next;
    }

    // Backtrack
    visited[hash(node) % graph->hash_size] = false;
    (*pathIndex)--;  // Decrease the path index when backtracking
}

//  
void traceFlow(Graph* graph, const char* source, int length){
    // Find the starting node
    Node* start = findNode(graph, source);
    if (start == NULL) {
        printf("Non-existing node: %s\n", source);
        return;
    }

    bool* visited = malloc(graph->hash_size * sizeof(bool));
    char** path = malloc(length * sizeof(char*));
    int pathIndex = 0;

    // Initialize visited array
    for (int i = 0; i < graph->hash_size; i++) {
        visited[i] = false;
    }

    // Call the recursive helper function
    traceFlowUtility(graph, source, visited, path, &pathIndex, length);

    // Clean up
    for(int i=0;i<pathIndex;i++){
        free(path[i]);
    }
    free(visited);
    free(path);
}



void printGraph(Graph* graph){
    printf("Bank Details:\n");
    for(int i=0;i<graph->hash_size;i++){
        Node* currentNode = graph->nodes[i];
        while(currentNode != NULL){
            printf("User ID: %s\n", currentNode->UserID);
            Edge* currentEdge = currentNode->edges;
            while(currentEdge != NULL){
                printf("  -> %s (Amount: %.2f, Date: %s)\n", 
                currentEdge->destination, 
                currentEdge->amount, 
                currentEdge->date);
                currentEdge = currentEdge->next;
            }
            currentNode = currentNode->next;
        }
    }
}


void freeGraph(Graph* graph){
    for(int i=0;i<graph->hash_size;i++){
        Node* currentNode = graph->nodes[i];
        while(currentNode != NULL){
            Edge* currentEdge = currentNode->edges;
            while(currentEdge != NULL){
                Edge* temp = currentEdge;
                currentEdge = currentEdge->next;
                free(temp);
                memCount -= sizeof(Edge);
            }
            Node* temp = currentNode;
            currentNode = currentNode->next;
            free(temp);
            memCount -= sizeof(Node);
        }
    }
    free(graph->nodes);
    memCount -= sizeof(Node*);
    free(graph);
    memCount -= sizeof(Graph);
}