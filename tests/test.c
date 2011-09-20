#include <stdio.h>
#include <stdlib.h>
#include "../include/bvgraph.h"
#include "../include/bvgraphfun.h"
#include "../include/bitfile.h"
#include <string.h>
#include <time.h>

#define MAX_PAIR_NUM 1000000

struct pair{
    int from;
    int to;
};

struct pair* ALL_PAIR = NULL;
int PAIR_SIZE = 0;

void load_all(const char* name){
    bvgraph g = {0};
    bvgraph_load(&g, name, strlen(name), 0);

    bvgraph_iterator git;
    int rval = bvgraph_nonzero_iterator(&g, &git);
    if (rval){
        printf("Construct nonzero iterator failed. Stop.\n");
        return;
    }

    ALL_PAIR = malloc(sizeof(int)*2*MAX_PAIR_NUM);

    for (; bvgraph_iterator_valid(&git); bvgraph_iterator_next(&git)) {
        int *links = NULL;
        unsigned int d = 0;
        
        bvgraph_iterator_outedges(&git, &links, &d);
        
        int i = 0;
        for (i; i<d; ++i) {
            ALL_PAIR[PAIR_SIZE].from = git.curr;
            ALL_PAIR[PAIR_SIZE].to = links[i];
            PAIR_SIZE++;
        }
    }
    bvgraph_iterator_free(&git);
}

int exist_pair(int from, int to){
    //use binary search to search pair
    int index = 0;
    int min = 0;
    int max = PAIR_SIZE;
    while (min <= max){
        index = (min + max) / 2;
        if (ALL_PAIR[index].from < from){
            min = index + 1;
        }
        else if (ALL_PAIR[index].from > from){
            max = index - 1;
        }
        else{
            // compare to
            if (ALL_PAIR[index].to == to){
                return 1;
            }
            else{
                while (ALL_PAIR[index].from == from){
                    if (ALL_PAIR[index].to > to){
                        index--;
                    }
                    else{
                        index++;
                    }
                    if (ALL_PAIR[index].to == to){
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

void head_tail_first_test(bvgraph g){
    //test random access from 0, n-1, 1, n-2, 2, n-3, ...
    int i, j;
    unsigned int d;

    for (i = 0; i <= g.n; i++){
        int node;
        if ( i % 2 == 0){
            node = i / 2;
        }
        else {
            node = g.n - 1 - i / 2;
        }
        //get successors example
        int *links;    
        bvgraph_successors(&g, node, &links, &d);

        int j = 0;
        for (j; j< d; j++){
            if (!exist_pair(node, links[j])){
                printf("Wrong links from node %d to node %d. Stop.\n", i, links[j]);
                //return;
            }
        }
    }

    printf("All links from head-tail test are tested and correct.\n");

}

void random_test(bvgraph g, int test_num){

    //randomly generate test case
    int i = 0;
    unsigned int d;
    srand(time(NULL));

    for (i; i < test_num; i++){
        int node = rand() % g.n;
        int *links;

        bvgraph_successors(&g, node, &links, &d);

        int j = 0;
        for (j; j< d; j++){
            if (!exist_pair(node, links[j])){
                printf("Wrong links from node %d to node %d. Stop.\n", i, links[j]);
                return;
            }
        }
    }
    printf("Total %d random nodes generated and tested and correct.\n", test_num);

}

void print_all(bvgraph g){
    int i = 0;
    unsigned int d = 0;

    bvgraph_random_iterator ri;
    int rval = bvgraph_random_access_iterator(&g, &ri);

    if (rval){
        printf ("Random access iterator allocation failed. Stop.\n");
        return;
    }
    
    for (i; i < g.n; i++){
        //get successors example
        int *links;
        bvgraph_random_successors(&ri, i, &links, &d);

        //printf ("node %d has degree %d\n", i, d);
        
        int j = 0;
        for (j; j< d; j++){
            //printf("node %i links to node %i\n", i, links[j]);
            if (!exist_pair(i, links[j])){
                printf("Wrong links from node %d to node %d. Stop.\n", i, links[j]);
                return;
            }
        }
    }

    bvgraph_random_free(&ri);

    printf("All links are tested and correct.\n");

}

void test_performance(bvgraph g, int test_num){

    //randomly generate test case
    int i = 0;
    unsigned int d;
    srand(time(NULL));
    int edge_count = 0;
    clock_t start, end;
    bvgraph_random_iterator ri;
    int rval = bvgraph_random_access_iterator(&g, &ri);

    if (rval){
        printf ("Random access iterator allocation failed. Stop.\n");
        return;
    }
 
    start = clock();
    for (i; i < test_num; i++){
        int node = rand() % g.n;
        int *links;
        bvgraph_random_successors(&ri, i, &links, &d);
        edge_count += d;
    }
    end = clock();
    double dif = ((double)end - (double)start) / CLOCKS_PER_SEC;
    double edge_per_sec = edge_count / dif;

    printf("Used %.2lf secs. Edges = %d. Edge per second = %.2lf\n", dif, edge_count, edge_per_sec);

    bvgraph_random_free(&ri);

}

void print_help(){
    printf("Use the following parameters to test random access: \n");
    printf("./a.out dataset param\n");
    printf("dataset: the graph dataset without extenion. Ex: harvard500\n");
    printf("param:\n");
    printf("\trandom    - to test by randomly generated nodes. Need a parameter for # of nodes.\n");
    printf("\thead-tail - to test from head and tail roundly.\n");
    printf("\tall       - to test all nodes in dataset\n");    
}

int main(int argc, char** argv){
    bvgraph g = {0};
    //const char* name = "libbvg/data/wb-cs.stanford";
    const char* name = argv[1];
    char* method = argv[2];

    if (method == NULL){
        print_help();
        return 0;
    }

    //load with offsets
    bvgraph_load(&g, name, strlen(name), 1);

    printf("Input file: %s\n", name);
    printf("nodes = %d\n", g.n);
    printf("edges = %lli\n", g.m);

    if (strcmp(method, "random") == 0){
        load_all(name);
        if (argv[3] == NULL){
            printf("Need node number. Stop\n");
            return 1;
        }
        int num = atoi(argv[3]);
        random_test(g, num);
    }
    else if (strcmp(method, "head-tail") == 0){
        load_all(name);
        head_tail_first_test(g);
    }
    else if (strcmp(method, "all") == 0){
        load_all(name);
        print_all(g);
    }
    else if (strcmp(method, "perform") == 0){
        // test performance
        if (argv[3] == NULL){
            printf("Need node number. Stop\n");
            return 1;
        }
        int num = atoi(argv[3]);
        test_performance(g, num);
    }
    else{
        print_help();
    }

    bvgraph_close(&g);

    return 0;
}