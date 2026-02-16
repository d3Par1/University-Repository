/*
 * Variant 15: Cyclic references preventing memory deallocation
 *
 * Implements a dynamic data structure (doubly-linked graph nodes)
 * where additional cross-references create cycles. A naive free
 * strategy cannot handle cycles — memory is leaked.
 *
 * The program demonstrates:
 *   1. Creating a graph with cyclic references
 *   2. Naive free attempt (leaks memory due to cycles)
 *   3. Correct two-phase free (break cycles, then free nodes)
 *
 * Compile: gcc -Wall -Wextra -std=c11 -g cyclic_refs.c -o cyclic_refs
 * Run:     ./cyclic_refs
 * Valgrind: valgrind --leak-check=full ./cyclic_refs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Data structures --- */

typedef struct Node {
    int id;
    char *data;            /* dynamically allocated payload */
    struct Node *next;     /* linked list pointer */
    struct Node *prev;     /* doubly-linked back pointer */
    struct Node *ref;      /* cross-reference that may create a cycle */
} Node;

typedef struct Graph {
    Node *head;
    int size;
} Graph;

/* --- Graph operations --- */

static Node *create_node(int id, const char *data) {
    Node *n = malloc(sizeof(Node));
    if (!n) {
        perror("malloc(Node)");
        exit(EXIT_FAILURE);
    }
    n->id = id;
    n->data = strdup(data);
    if (!n->data) {
        perror("strdup");
        free(n);
        exit(EXIT_FAILURE);
    }
    n->next = NULL;
    n->prev = NULL;
    n->ref = NULL;
    return n;
}

static Graph *create_graph(void) {
    Graph *g = malloc(sizeof(Graph));
    if (!g) {
        perror("malloc(Graph)");
        exit(EXIT_FAILURE);
    }
    g->head = NULL;
    g->size = 0;
    return g;
}

static void add_node(Graph *g, int id, const char *data) {
    Node *n = create_node(id, data);
    if (!g->head) {
        g->head = n;
    } else {
        Node *cur = g->head;
        while (cur->next)
            cur = cur->next;
        cur->next = n;
        n->prev = cur;
    }
    g->size++;
}

static Node *find_node(Graph *g, int id) {
    Node *cur = g->head;
    while (cur) {
        if (cur->id == id)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void create_cyclic_ref(Graph *g, int src_id, int dst_id) {
    Node *src = find_node(g, src_id);
    Node *dst = find_node(g, dst_id);
    if (src && dst) {
        src->ref = dst;
        printf("  Created reference: Node %d -> Node %d\n", src_id, dst_id);
    }
}

/* --- Display --- */

static void display_graph(Graph *g) {
    printf("\n  Graph (%d nodes):\n", g->size);
    printf("  %-6s %-15s %-8s %-8s %-8s\n",
           "ID", "Data", "Prev", "Next", "Ref");
    printf("  %-6s %-15s %-8s %-8s %-8s\n",
           "---", "------------", "----", "----", "---");

    Node *cur = g->head;
    while (cur) {
        printf("  %-6d %-15s %-8s %-8s",
               cur->id, cur->data,
               cur->prev ? "yes" : "-",
               cur->next ? "yes" : "-");
        if (cur->ref)
            printf(" -> %d", cur->ref->id);
        else
            printf(" -");
        printf("\n");
        cur = cur->next;
    }
    printf("\n");
}

/*
 * Visualize the cycle structure:
 *
 *   Node 1 --ref--> Node 3
 *     ^                |
 *     |               ref
 *     +--- Node 3 ----+   (cycle: 1 -> 3 -> 1)
 *
 *   Node 2 --ref--> Node 4
 *     ^                |
 *     |               ref
 *     +--- Node 4 ----+   (cycle: 2 -> 4 -> 2)
 */

/* --- Naive free: does NOT handle cycles --- */

static void naive_free_graph(Graph *g) {
    printf("  [Naive free] Attempting to free graph...\n");

    /*
     * Problem: if we try to follow 'ref' pointers to determine
     * if a node is "still in use", cycles make this impossible.
     * A reference-counting approach would never reach 0 for
     * nodes in a cycle.
     *
     * Simulated reference counting:
     */
    Node *cur = g->head;
    int freed = 0, leaked = 0;

    /* Count incoming references (simulated refcount) */
    while (cur) {
        int refcount = 0;
        /* Check if any other node's 'ref' points to this node */
        Node *scanner = g->head;
        while (scanner) {
            if (scanner->ref == cur)
                refcount++;
            scanner = scanner->next;
        }
        printf("  Node %d: refcount = %d", cur->id, refcount);
        if (refcount > 0) {
            printf(" → SKIPPED (still referenced — LEAKED!)\n");
            leaked++;
        } else {
            printf(" → would free\n");
            freed++;
        }
        cur = cur->next;
    }

    printf("  Result: %d freed, %d leaked (cycles prevent freeing)\n\n", freed, leaked);
    /* In reality we don't free anything here to avoid double-free */
}

/* --- Correct free: two-phase approach --- */

static void correct_free_graph(Graph *g) {
    printf("  [Correct free] Two-phase approach:\n");

    /* Phase 1: Break all cyclic references */
    printf("  Phase 1: Breaking all cyclic references...\n");
    Node *cur = g->head;
    while (cur) {
        if (cur->ref) {
            printf("    Node %d: ref -> Node %d → set to NULL\n",
                   cur->id, cur->ref->id);
            cur->ref = NULL;
        }
        cur = cur->next;
    }

    /* Phase 2: Free all nodes sequentially */
    printf("  Phase 2: Freeing all nodes...\n");
    cur = g->head;
    while (cur) {
        Node *next = cur->next;
        printf("    Freeing Node %d (data=\"%s\")\n", cur->id, cur->data);
        free(cur->data);
        free(cur);
        cur = next;
    }

    free(g);
    printf("  All memory freed successfully.\n\n");
}

/* --- Main --- */

int main(void) {
    printf("=== Variant 15: Cyclic References in Dynamic Data Structures ===\n\n");

    /* --- Part 1: Build graph with cycles --- */
    printf("--- Building graph with cyclic references ---\n");
    Graph *graph = create_graph();

    add_node(graph, 1, "Node Alpha");
    add_node(graph, 2, "Node Beta");
    add_node(graph, 3, "Node Gamma");
    add_node(graph, 4, "Node Delta");

    /* Create cycles: 1->3->1 and 2->4->2 */
    create_cyclic_ref(graph, 1, 3);
    create_cyclic_ref(graph, 2, 4);
    create_cyclic_ref(graph, 3, 1); /* creates cycle 1 <-> 3 */
    create_cyclic_ref(graph, 4, 2); /* creates cycle 2 <-> 4 */

    display_graph(graph);

    printf("  Cycle visualization:\n");
    printf("    Node 1 --ref--> Node 3 --ref--> Node 1  (cycle!)\n");
    printf("    Node 2 --ref--> Node 4 --ref--> Node 2  (cycle!)\n\n");

    /* --- Part 2: Show naive approach fails --- */
    printf("--- Part 2: Naive free (reference counting) ---\n");
    naive_free_graph(graph);

    /* --- Part 3: Correct two-phase free --- */
    printf("--- Part 3: Correct free (break cycles first) ---\n");
    correct_free_graph(graph);

    printf("=== Summary ===\n");
    printf("Cyclic references prevent simple reference-counting deallocation.\n");
    printf("Solution: two-phase free — first break all cycles, then free nodes.\n");
    printf("In real systems, use:\n");
    printf("  - Mark-and-sweep (like GC in Java/Go)\n");
    printf("  - Weak references (one direction is non-owning)\n");
    printf("  - Explicit cycle-breaking before cleanup\n");
    printf("\nVerify with: valgrind --leak-check=full ./cyclic_refs\n");

    return 0;
}
