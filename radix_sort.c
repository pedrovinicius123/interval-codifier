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
    for (int i = 0; i < its.n_encoded; i++){
        inner = its.encodeds[i];
        int idx_inner = MIN(depth, inner.n);
        int bit = inner.result[idx_inner];
        //printf("A\n");
        
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

    //printf("PRINTF ERROR\n");
    encoded_1 = tmp_1;
    encoded_0 = tmp_0;

    items its_0 = {encoded_0, n_encoded_0, idx-n_encoded_0};
    items its_1 = {encoded_1, n_encoded_1, idx};
    
    items result;
    items h_0 = helper_function_sort(its_0, depth+1);
    items h_1 = helper_function_sort(its_1, depth+1);
    
    result.encodeds = calloc(its.n_encoded, sizeof(res));
    memcpy(result.encodeds, h_0.encodeds, h_0.n_encoded * sizeof(res));
    memcpy(result.encodeds + h_0.n_encoded, h_1.encodeds, h_1.n_encoded * sizeof(res));

    result.n_encoded = its.n_encoded;
    result.index = its.index;

    //printf("PASSED ERROR 1\n");
    return result;

}

int* sort(int* unsorted, int n){
    res* encoded = calloc(n, sizeof(res));
    int* decoded = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++){
        encoded[i] = encode(1000, unsorted[i], 2);
    }
    items unsorted_its = {encoded, n, n-1};
    items its = helper_function_sort(unsorted_its, 0);
    for (int j = 0; j < its.n_encoded; j++){
        //printf("BEFORE CRASH\n");
        decoded[j] = decode(its.encodeds[j], 1000, 2);
    }
    return decoded;
}

int main(int argc, char** argv)
{   
    int array[8] = {10, 30, 28, 2, 4, 11, 32, 99};
    int* sorted = sort(array, 8);

    for(int i = 0; i < 8; i++){
        printf("%d ", sorted[i]);
    } printf("\n");
    
}