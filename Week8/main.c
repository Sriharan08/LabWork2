#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

// Function to insert an edge at the front of the edge list
void insert_at_front(EdgeList* list, int to, int weight) {
    EdgeNodePtr newNode = (EdgeNodePtr)malloc(sizeof(struct edgeNode));
    newNode->edge.to_vertex = to;
    newNode->edge.weight = weight;
    newNode->next = list->head;
    list->head = newNode;
}

// Function to add an edge to the graph
void add_edge(Graph* self, int from, int to, int w) {
    if (from >= self->V || to >= self->V) {
        return;
    }
    insert_at_front(&(self->edges[from]), to, w);
}

// Function to initialize edge list
void init_edge_list(EdgeList* list) {
    list->head = NULL;
}

// Function to calculate in-degrees of all vertices
int* calculate_indegrees(Graph* graph) {
    int* indegrees = (int*)calloc(graph->V, sizeof(int));
    
    for (int i = 0; i < graph->V; i++) {
        EdgeNodePtr current = graph->edges[i].head;
        while (current != NULL) {
            indegrees[current->edge.to_vertex]++;
            current = current->next;
        }
    }
    
    return indegrees;
}

// Function to print in-degrees
void print_indegrees(int* indegrees, int V) {
    printf("In-degrees of vertices:\n");
    for (int i = 0; i < V; i++) {
        printf("Vertex %d: %d\n", i, indegrees[i]);
    }
}

// Function to print top vertices with highest in-degrees
void print_top_indegrees(int* indegrees, int V, int top_n) {
    int* indices = (int*)malloc(V * sizeof(int));
    for (int i = 0; i < V; i++) {
        indices[i] = i;
    }
    
    // Sort indices by in-degree (descending)
    for (int i = 0; i < V - 1; i++) {
        for (int j = 0; j < V - i - 1; j++) {
            if (indegrees[indices[j]] < indegrees[indices[j + 1]]) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
    
    printf("\nTop %d vertices by in-degree:\n", top_n);
    for (int i = 0; i < top_n && i < V; i++) {
        printf("Vertex %d: %d\n", indices[i], indegrees[indices[i]]);
    }
    
    free(indices);
}

// Function to find the maximum vertex number in the edge file
int find_max_vertex(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open %s\n", filename);
        return -1;
    }
    
    int max_vertex = -1;
    int from, to;
    char line[256];
    
    // Check for header (first line might be "from,to" or similar)
    if (fgets(line, sizeof(line), file)) {
        // If first line doesn't start with a digit, it's a header
        if (line[0] < '0' || line[0] > '9') {
            // Header line, skip it
        } else {
            // First line is data, rewind
            rewind(file);
        }
    }
    
    // Read all edges
    while (fscanf(file, "%d,%d", &from, &to) == 2) {
        if (from > max_vertex) max_vertex = from;
        if (to > max_vertex) max_vertex = to;
    }
    
    fclose(file);
    return max_vertex + 1; // Return number of vertices
}

// Function to load unweighted graph from file
void load_unweighted_graph(Graph* G, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    
    char line[256];
    int from, to;
    
    // Check for and skip header
    if (fgets(line, sizeof(line), file)) {
        if (line[0] < '0' || line[0] > '9') {
            // Header line, skip it
        } else {
            // First line is data, rewind
            rewind(file);
        }
    }
    
    int edge_count = 0;
    // Read all edges with weight 0
    while (fscanf(file, "%d,%d", &from, &to) == 2) {
        if (from < G->V && to < G->V) {
            add_edge(G, from, to, 0);
            edge_count++;
        }
    }
    
    printf("Loaded %d edges\n", edge_count);
    fclose(file);
}

// Function to free graph memory
void free_graph(Graph* graph) {
    for (int i = 0; i < graph->V; i++) {
        EdgeNodePtr current = graph->edges[i].head;
        while (current != NULL) {
            EdgeNodePtr temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(graph->edges);
}

int main() {
    Graph G;
    int choice;
    
    printf("Choose graph to load:\n");
    printf("1. Sample graph (resources/graph.txt)\n");
    printf("2. GitHub Social Network (resources/github_social_network/musae_git_edges.csv)\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    
    if (choice == 1) {
        // Load weighted graph from graph.txt in resources folder
        FILE* file = fopen("resources/graph.txt", "r");
        if (file == NULL) {
            printf("Error opening resources/graph.txt!\n");
            printf("Make sure graph.txt is in the 'resources' folder\n");
            return 1;
        }
        
        fscanf(file, "%d", &G.V);
        printf("Loading graph with %d vertices\n", G.V);
        
        G.edges = (EdgeList*)malloc(G.V * sizeof(EdgeList));
        for (int i = 0; i < G.V; i++) {
            init_edge_list(&G.edges[i]);
        }
        
        int from, to, weight;
        int edge_count = 0;
        while (fscanf(file, "%d,%d,%d", &from, &to, &weight) == 3) {
            add_edge(&G, from, to, weight);
            edge_count++;
        }
        
        printf("Loaded %d edges\n", edge_count);
        fclose(file);
    } 
    else if (choice == 2) {
        // First find the maximum vertex to determine number of vertices
        const char* filename = "resources/github_social_network/musae_git_edges.csv";
        
        printf("Scanning %s to find maximum vertex number...\n", filename);
        G.V = find_max_vertex(filename);
        if (G.V == -1) {
            printf("Error: Could not open %s\n", filename);
            printf("Make sure the file exists in: resources/github_social_network/\n");
            return 1;
        }
        printf("Graph has %d vertices (0 to %d)\n", G.V, G.V - 1);
        
        // Allocate and initialize edge lists
        G.edges = (EdgeList*)malloc(G.V * sizeof(EdgeList));
        for (int i = 0; i < G.V; i++) {
            init_edge_list(&G.edges[i]);
        }
        
        // Load the unweighted graph
        load_unweighted_graph(&G, filename);
    }
    else {
        printf("Invalid choice!\n");
        return 1;
    }
    
    // Calculate and print in-degrees
    printf("\nCalculating in-degrees...\n");
    int* indegrees = calculate_indegrees(&G);
    
    // For large graphs, print top 10 instead of all
    if (G.V > 100) {
        print_top_indegrees(indegrees, G.V, 10);
    } else {
        print_indegrees(indegrees, G.V);
    }
    
    // Free memory
    free(indegrees);
    free_graph(&G);
    
    printf("\nDone! Memory freed successfully.\n");
    
    return 0;
}