#include "utils/compact.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#define MIN(a,b) (((b) > (a)) ? (a) : (b))

typedef struct {
    res* encodeds;
    int n_encoded;
    int index;
} items;

items helper_function_sort(items its, unsigned int depth)
{
    //printf("DEPTH %d %d\n", depth, its.n_encoded);
    if (its.n_encoded == 1){
        return its;
    } else if (its.n_encoded < 1){
        items result = {NULL, 0, -1};
        return result;
    }

    int n_encoded_1, n_encoded_0, idx;
    n_encoded_1 = 0;
    n_encoded_0 = 0;
    idx = its.index;

    res* encoded_1 = calloc(its.n_encoded, sizeof(res));
    res* encoded_0 = calloc(its.n_encoded, sizeof(res));
    if (encoded_1 == NULL){
        fprintf(stderr, "FAILED TO CALLOC ENCODED_1\n");
    } else if (encoded_0 == NULL) {
        fprintf(stderr, "FAILED TO CALLOC ENCODED_0\n");
    }
    
    res inner;
    int last = 0;
    for (int i = 0; i < its.n_encoded; i++){
        inner = its.encodeds[i];
        int idx_inner = MIN(depth, inner.n-1);
        int bit = inner.result[idx_inner];
    //  printf("%d %d\n", inner.n, depth);
        if (depth >= inner.n){
            last = 1;
        }

        switch (bit){
            case 1:
                encoded_1[n_encoded_1] = inner;
                n_encoded_1++;
                continue;
            case 0:
                encoded_0[n_encoded_0] = inner;
                n_encoded_0++;
                continue;
            default:
                fprintf(stderr, "(For loop line 38) Unespected bit at %d: %d\n", idx_inner, bit);
        }

    }

    res* tmp_1 = realloc(encoded_1, n_encoded_1*sizeof(res));
    res* tmp_0 = realloc(encoded_0, n_encoded_0*sizeof(res));

   // printf("PRINTF ERROR\n");
    encoded_1 = tmp_1;
    encoded_0 = tmp_0;

    items its_0 = {encoded_0, n_encoded_0, idx-n_encoded_0};
    items its_1 = {encoded_1, n_encoded_1, idx};
    
    items result;
    items h_0, h_1;

    if(n_encoded_1 == 0){
        if (last){
            return its_0;
        }
        h_0 = helper_function_sort(its_0, depth+1);
        return h_0;
    } else if(n_encoded_0 == 0){
        if (last){
            return its_1;
        }
        h_1 = helper_function_sort(its_1, depth+1);
        return h_1;
    }

    h_0 = helper_function_sort(its_0, depth+1);
    h_1 = helper_function_sort(its_1, depth+1);
    
    result.encodeds = calloc(its.n_encoded, sizeof(res));
    memcpy(result.encodeds, h_0.encodeds, h_0.n_encoded * sizeof(res));
    memcpy(result.encodeds + h_0.n_encoded, h_1.encodeds, h_1.n_encoded * sizeof(res));

    result.n_encoded = its.n_encoded;
    result.index = its.index;

   // printf("PASSED ERROR 1\n");
    return result;

}

int* sort(int* unsorted, unsigned int n, int max_num){
    res* encoded = calloc(n, sizeof(res));
    int* decoded = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++){
        encoded[i] = encode(max_num+1, unsorted[i], 2);
    }
    items unsorted_its = {encoded, n, n-1};
    items its = helper_function_sort(unsorted_its, 0);
    for (int j = 0; j < its.n_encoded; j++){
        //printf("BEFORE CRASH\n");
        decoded[j] = decode(its.encodeds[j], max_num+1, 2);
    }
    return decoded;
}

int* read_file(char* filename, unsigned int* n, int* max_num){
    FILE* fptr = fopen(filename, "r");
    if (!fptr){
        fprintf(stderr, "Could not read sequence file.\n");
        return NULL;
    }

    char line[128];
    int* seq = NULL;
    int row = 0;

    while (fgets(line, sizeof(line), fptr) != NULL){
        char* end = strchr(line, '\n');
        if (end) *end = '\0';
        end = strchr(line, '\r');
        if (end) *end = '\0';

        if (line[0] == '\0') continue;

        if (row == 0){
            *n = (unsigned int)atoi(line);
            seq = calloc(*n, sizeof(int));
            row++;
            continue;

        } else if(row == 1){
            *max_num = atoi(line);
            row++;
            continue;
        }

        if (row <= (int)*n+1){
            seq[row-2] = atoi(line);
            row++;
        }
    }

    fclose(fptr);
    return seq;
}

int main(int argc, char** argv)
{   
    int* seq = NULL;
    unsigned int size = 0;
    int max_num = 0;
    for (int i = 1; i < argc - 1; i++){
        if(strcmp(argv[i], "-f") == 0){
            seq = read_file(argv[i+1], &size, &max_num);
            break;
        }
    }

    if (seq != NULL){
        int* sorted = sort(seq, size, max_num);

        for(int i = 0; i < size; i++){
            printf("%d ", sorted[i]);
        } printf("\n");
    }
}