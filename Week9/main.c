#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

// Function to calculate out-degrees for all vertices
int* calculate_out_degrees(Graph* graph) {
    int* out_degrees = (int*)calloc(graph->V, sizeof(int));
    
    for (int i = 0; i < graph->V; i++) {
        EdgeNodePtr current = graph->edges[i].head;
        while (current != NULL) {
            out_degrees[i]++;  // Count outgoing edges
            current = current->next;
        }
    }
    
    return out_degrees;
}

// Function to calculate PageRank following the pseudocode requirements
// Each loop must be kept separate - no combining steps!
void calculate_pagerank(Graph* graph, double damping_factor, int iterations) {
    int V = graph->V;
    
    // Create required arrays
    double* sums = (double*)calloc(V, sizeof(double));      // Sigma part of equation
    double* PR = (double*)malloc(V * sizeof(double));       // PageRank values
    double* new_PR = (double*)malloc(V * sizeof(double));   // New PageRank values
    int* out_degrees = calculate_out_degrees(graph);        // Out-degrees L(p_j)
    
    // Initialize PageRanks to 1.0
    for (int i = 0; i < V; i++) {
        PR[i] = 1.0;
    }
    
    printf("\nInitial PageRank values: 1.0 for all %d vertices\n", V);
    printf("Running %d iterations with damping factor = %.2f\n\n", iterations, damping_factor);
    
    // Main iteration loop
    for (int iter = 0; iter < iterations; iter++) {
        
        // STEP 1: Loop through all vertices and set sums to zero
        // This loop MUST be separate from other loops
        for (int i = 0; i < V; i++) {
            sums[i] = 0.0;
        }
        
        // STEP 2: Loop through all edges adding PR(p_j)/L(p_j) to vertex i
        // where i is the to-vertex and j is the from-vertex
        // This loop MUST be separate from other loops
        for (int j = 0; j < V; j++) {
            // Skip vertices with no outgoing edges to avoid division by zero
            if (out_degrees[j] == 0) continue;
            
            double contribution = PR[j] / out_degrees[j];
            
            // Add contribution to all vertices that j points to
            EdgeNodePtr current = graph->edges[j].head;
            while (current != NULL) {
                int to_vertex = current->edge.to_vertex;
                sums[to_vertex] += contribution;
                current = current->next;
            }
        }
        
        // STEP 3: Loop through all vertices and calculate new PageRanks
        // This loop MUST be separate from other loops
        for (int i = 0; i < V; i++) {
            new_PR[i] = (1 - damping_factor) + damping_factor * sums[i];
        }
        
        // Copy new PageRanks to PR for next iteration
        // (This could be considered part of step 3 or a separate loop)
        for (int i = 0; i < V; i++) {
            PR[i] = new_PR[i];
        }
        
        // Print progress every 10 iterations or for the last iteration
        if (iter % 10 == 0 || iter == iterations - 1) {
            printf("Iteration %d/%d completed\n", iter + 1, iterations);
        }
    }
    
    // Print final PageRank values
    printf("\n=== Final PageRank Values ===\n");
    
    // For large graphs, print top 10
    if (V > 50) {
        // Create array of indices and sort by PageRank
        int* indices = (int*)malloc(V * sizeof(int));
        for (int i = 0; i < V; i++) {
            indices[i] = i;
        }
        
        // Sort indices by PageRank (descending)
        for (int i = 0; i < V - 1; i++) {
            for (int j = 0; j < V - i - 1; j++) {
                if (PR[indices[j]] < PR[indices[j + 1]]) {
                    int temp = indices[j];
                    indices[j] = indices[j + 1];
                    indices[j + 1] = temp;
                }
            }
        }
        
        printf("Top 10 vertices by PageRank:\n");
        for (int i = 0; i < 10 && i < V; i++) {
            printf("Vertex %d: %.10f\n", indices[i], PR[indices[i]]);
        }
        free(indices);
    } else {
        // For small graphs, print all
        for (int i = 0; i < V; i++) {
            printf("Vertex %d: %.10f\n", i, PR[i]);
        }
    }
    
    // Free allocated memory
    free(sums);
    free(PR);
    free(new_PR);
    free(out_degrees);
}

// Function to calculate in-degrees
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

// Function to print top vertices by in-degree
void print_top_indegrees(int* indegrees, int V, int top_n) {
    int* indices = (int*)malloc(V * sizeof(int));
    for (int i = 0; i < V; i++) {
        indices[i] = i;
    }
    
    // Sort by in-degree (descending)
    for (int i = 0; i < V - 1; i++) {
        for (int j = 0; j < V - i - 1; j++) {
            if (indegrees[indices[j]] < indegrees[indices[j + 1]]) {
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
    
    printf("Top %d vertices by in-degree:\n", top_n);
    for (int i = 0; i < top_n && i < V; i++) {
        printf("Vertex %d: %d\n", indices[i], indegrees[indices[i]]);
    }
    
    free(indices);
}

// Function to find maximum vertex number in edge file
int find_max_vertex(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    
    int max_vertex = -1;
    int from, to;
    char line[256];
    
    if (fgets(line, sizeof(line), file)) {
        if (line[0] < '0' || line[0] > '9') {
            // Header, skip
        } else {
            rewind(file);
        }
    }
    
    while (fscanf(file, "%d,%d", &from, &to) == 2) {
        if (from > max_vertex) max_vertex = from;
        if (to > max_vertex) max_vertex = to;
    }
    
    fclose(file);
    return max_vertex + 1;
}

// Function to load unweighted graph
void load_unweighted_graph(Graph* G, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    
    char line[256];
    int from, to;
    
    if (fgets(line, sizeof(line), file)) {
        if (line[0] < '0' || line[0] > '9') {
            // Header, skip
        } else {
            rewind(file);
        }
    }
    
    int edge_count = 0;
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
    int iterations;
    double damping_factor = 0.85;
    
    printf("=== Graph PageRank Calculator ===\n\n");
    printf("Choose graph to load:\n");
    printf("1. Sample graph (resources/graph.txt)\n");
    printf("2. GitHub Social Network (resources/github_social_network/musae_git_edges.csv)\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    
    if (choice == 1) {
        FILE* file = fopen("resources/graph.txt", "r");
        if (file == NULL) {
            printf("Error opening resources/graph.txt!\n");
            return 1;
        }
        
        fscanf(file, "%d", &G.V);
        printf("\nLoading graph with %d vertices\n", G.V);
        
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
        const char* filename = "resources/github_social_network/musae_git_edges.csv";
        
        printf("\nScanning %s...\n", filename);
        G.V = find_max_vertex(filename);
        if (G.V == -1) {
            printf("Error: Could not open file\n");
            return 1;
        }
        printf("Graph has %d vertices (0 to %d)\n", G.V, G.V - 1);
        
        G.edges = (EdgeList*)malloc(G.V * sizeof(EdgeList));
        for (int i = 0; i < G.V; i++) {
            init_edge_list(&G.edges[i]);
        }
        
        load_unweighted_graph(&G, filename);
    }
    else {
        printf("Invalid choice!\n");
        return 1;
    }
    
    // Calculate and display in-degree analysis
    printf("\n=== In-degree Analysis ===\n");
    int* indegrees = calculate_indegrees(&G);
    if (G.V > 100) {
        print_top_indegrees(indegrees, G.V, 10);
    } else {
        printf("In-degrees:\n");
        for (int i = 0; i < G.V; i++) {
            printf("Vertex %d: %d\n", i, indegrees[i]);
        }
    }
    
    // Get number of iterations from user
    printf("\n=== PageRank Settings ===\n");
    printf("Enter number of iterations for PageRank: ");
    scanf("%d", &iterations);
    
    // Calculate PageRank
    calculate_pagerank(&G, damping_factor, iterations);
    
    // Free memory
    free(indegrees);
    free_graph(&G);
    
    printf("\n✓ Done! All memory freed successfully.\n");
    
    return 0;
}