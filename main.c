#include "dir_multi_graph.h"


void readInputFile(char *input_file,Graph* graph){
    FILE* file = fopen(input_file, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }


    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* source = strtok(line, " \n");      
        char* destination = strtok(NULL, " \n"); 
        char* tmp_amount = strtok(NULL, " \n");  
        char* date = strtok(NULL, " \n");        

        if (source != NULL && destination != NULL && tmp_amount != NULL && date != NULL) {
            double amount = atof(tmp_amount);   // Convert string to double
            addEdge(graph, source, destination, amount, date);
        } else {
            printf("File format error: <source> <destination> <amount> <date>\n");
        }
    }
    fclose(file);
    return;
}


void WriteOutputFile(char* file_output, Graph * graph){
    FILE* file = fopen(file_output, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    // Write each node and its edges to the file
    for (int i = 0; i < graph->hash_size; i++) {
        Node* currentNode = graph->nodes[i];
        while (currentNode != NULL) {
            // Write all outgoing edges from each node (UserID)
            Edge* currentEdge = currentNode->edges;
            while (currentEdge != NULL) {
                fprintf(file, "%s %s %.2f %s\n", 
                    currentEdge->source, 
                    currentEdge->destination, 
                    currentEdge->amount, 
                    currentEdge->date);
                currentEdge = currentEdge->next;    //get the next edge from the current node 
            }

            currentNode = currentNode->next;    // iterate through all nodes
        }
    }
    fclose(file);  // Close the file after writing
}


void execute_commands(Graph* graph){
    char command[256];
    printf("miris> ");
    
    // Main loop
    while (fgets(command, sizeof(command), stdin)) {    // while we can read a line from the user
        // Tokenize the command
        char* cmd = strtok(command, " \n");

        // 1)   Handle the "insert / "i" command to add nodes
        if (strcmp(cmd, "insert") == 0 || strcmp(cmd, "i") == 0) {
            cmd = strtok(NULL, " \n");  // Tokenize the next word
            while (cmd != NULL) {   // While there are words="nodes"
                if (findNode(graph, cmd) == NULL) {
                    addNode(graph, cmd);
                    printf("Succ: %s\n", cmd);  
                } else {
                    printf("ERROR INDICATION:\n\t");
                    printf("IssueWith: %s (already exists)\n", cmd);
                }
                cmd = strtok(NULL, " \n");     // Tokenize the next word
            }
        }
        // 2)   Handle the "insert2" command to add  edges
        else if (strcmp(cmd, "insert2") == 0 || strcmp(cmd, "n") == 0) {
            char* source = strtok(NULL, " \n");
            char* destination = strtok(NULL, " \n");
            char* tmp_amount = strtok(NULL, " \n");  // Convert string to double
            char* date = strtok(NULL, " \n");
             // Check if the required parameters are provided
            if (source == NULL || destination == NULL || date == NULL || tmp_amount == NULL) {
                printf("Format error: insert2 <source> <destination> <amount> <date>\n");
                printf("miris> ");
                continue; // Early return to prevent further processing
            }
            double amount = atof(tmp_amount);

            addEdge(graph, source, destination, amount, date);
            printf("Edge added from %s to %s for %.2f on %s\n", source, destination, amount, date);
            
        }
        // 3)  Handle the delete node from graph cmd
        else if(strcmp(cmd, "delete") == 0 || strcmp(cmd, "d") == 0){
            cmd = strtok(NULL, " \n");
            while (cmd != NULL) {
                Node* node = findNode(graph, cmd);
                if (node != NULL) {
                    deleteNode(graph, node->UserID);
                    printf("Deleted: %s\n", cmd);
                } else {
                    printf("ERROR INDICATION:\n\t");
                    printf("Non-existing node: %s\n", cmd);
                }
                cmd = strtok(NULL, " \n");
            }

        }
        // 4) Handle delete Edges command
        else if(strcmp(cmd, "delete2") == 0 || strcmp(cmd, "l") == 0){
            char* user1 = strtok(NULL, " \n");
            char* user2 = strtok(NULL, " \n");
            if(user1 != NULL && user2 != NULL){
                deleteEdge(graph,user1,user2);
            }
            else{
                printf("ERROR INDICATION:\n\t");
                printf("Non-existing node(s): %s or %s\n", user1,user2);
            }
        }
        // 5) Handle modify edge command
        else if(strcmp(cmd, "modify") == 0 || strcmp(cmd, "m") == 0){
            char* source = strtok(NULL, " \n");
            char* destination = strtok(NULL, " \n");
            char* tmp_old_amount = strtok(NULL, " \n");
            char* tmp_new_amount = strtok(NULL, " \n");
            char* old_date = strtok(NULL, " \n");
            char* new_date = strtok(NULL, " \n");
            if (source == NULL || destination == NULL || tmp_old_amount == NULL || tmp_new_amount == NULL || old_date == NULL || new_date == NULL) {
                printf("Format error: modify <source> <destination> <old amount> <new amount> <old date> <new date>\n");
                printf("miris> ");
                continue;
            }
            double old_amount = atof(tmp_old_amount);
            double new_amount = atof(tmp_new_amount);
            modifyEdge(graph, source, destination, old_amount,new_amount, old_date, new_date);
        }
        // 6) Handle find outgoing edges command
        else if(strcmp(cmd, "find") == 0 || strcmp(cmd, "f") == 0){
            char* user = strtok(NULL, " \n");
            if(user != NULL){
                findOutgoingEdges(graph,user);
            }
            else{
                printf("ERROR INDICATION:\n\t");
                printf("Non-existing node: %s\n", user);
            }
        }
        // 7) Handle find all incoming to Ni edges
        else if(strcmp(cmd, "receiving") == 0 || strcmp(cmd, "r") == 0){
            char* node = strtok(NULL, " \n");
            if(node != NULL){
                findIncomingEdges(graph,node);
            }
            else{
                printf("ERROR INDICATION:\n\t");
                printf("Non-existing node: %s\n", node);
            }

        }
        // 8)  Handle find circles Ni is involved in if any
        else if(strcmp(cmd, "circlefind") == 0 || strcmp(cmd, "c") == 0){
            char* node = strtok(NULL, " \n");

            if (node != NULL) {
                Circlesfind(graph, node);
            } else {
                printf("Format error: circlefind / c <Ni>\n");
            }
        }
        // 9)  Handle find circular relationships in which Ni is involved and moves at least k units of funds.
        else if(strcmp(cmd, "findcircles") == 0 || strcmp(cmd, "fi") == 0){
            char* node = strtok(NULL, " \n");
            char* tmp_amount = strtok(NULL, " \n");

            if (node != NULL && tmp_amount != NULL) {
                double amount = atof(tmp_amount);
                findCircles(graph, node, amount);
            } else {
                printf("Format error: findcircles / fi <Ni> <k>\n");
            }
        }
        // 10) Handle Traceflow command to find all paths of length upto m for which a flow of transfers can be establsihed from Ni
        else if(strcmp(cmd, "traceflow") == 0 || strcmp(cmd, "t") == 0){
            char* node = strtok(NULL, " \n");
            char* tmp_length = strtok(NULL, " \n");
           if (node != NULL && tmp_length != NULL) {
                int length = atof(tmp_length);
                traceFlow(graph, node, length);
            } else {
                printf("Format error: traceflow / t <Ni> <m>\n");
            }
        }
        // 11) Handle If exist, a path between Ni and Nj
        else if(strcmp(cmd, "connected") == 0 || strcmp(cmd, "o") == 0){
            char* source = strtok(NULL, " \n");
            char* destination = strtok(NULL, " \n");
            if(source != NULL && destination != NULL){
                findPath(graph,source,destination);
            }
            else{
                printf("ERROR INDICATION:\n\t");
                printf("Format error: connected / o <Ni> <Nj>\n");
            }
        }
        // 12) Handle exit command
        else if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "e") == 0) {
            break;
        }
        // Unrecognized command
        else {
            printf("Unrecognized command: %s\n", cmd);
        }
        // Prompt again
        printf("miris> ");
    }
    return;
}


int main(int argc,char* argv[]){

    char * input_file;
    char * output_file;

    if(argc < 5){
        printf("Not proper call of the program!\n <file.exe>  -i <input.txt> -o <out.txt>\n");
        return (1);
    }
    if(strcmp(argv[1], "-i") == 0 && strcmp(argv[3], "-o") == 0){
        input_file = argv[2];
        output_file = argv[4];
    }
    else if(strcmp(argv[1], "-o") == 0 && strcmp(argv[3], "-i") == 0){
        input_file = argv[4];
        output_file = argv[2];
    }
    else{
        printf("Not proper call of the program!\n <file.exe>  -i <input.txt> -o <out.txt>\n");
        return (1);
    }
    
    Graph* graph = createGraph();
    readInputFile(input_file,graph);
    execute_commands(graph);
    WriteOutputFile(output_file, graph);    
    printf("%lld Bytes released\n",memCount);
    freeGraph(graph);

    return 0;
}