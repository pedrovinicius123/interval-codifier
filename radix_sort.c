#include "utils/compact.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#define MIN(a,b) (((b) > (a)) ? (a) : (b))

typedef struct {
    res* encodeds;
    int n_encoded;
    int index;
} items;

typedef struct{
    items its;
    int n;
} items_full;


items_full helper_function_sort(items its, unsigned int depth){
    printf("ITS %d\n", its.n_encoded);
    for (int i = 0; i < its.n_encoded; i++){
        printf("ENCODED %d: ", i);
        for (int j = 0; j < its.encodeds[i].n; j++){
            printf("%d ", its.encodeds[i].result[j]);
        } printf("\n");
    }
    if (its.n_encoded <= 1){
        items_full result;
        if (its.n_encoded == 0) {
            result.its = NULL;
            result.n = 0;
        } else {
            result.its = malloc(sizeof(items));
            result.its[0] = its;
            result.n = 1;
        }
        return result;
    }

    items items_1 = {NULL, 0, its.index};
    items items_0 = {NULL, 0, its.index};
    int n_eqs_1 = 0;
    int n_eqs_0 = 0;

    for (int j = 0; j < its.n_encoded; j++){
        int max_idx = its.encodeds[j].n - 1;
        int idx = (depth < its.encodeds[j].n) ? depth : max_idx;
        int bit = its.encodeds[j].result[idx];

        if (bit == 1) {
            items_1.encodeds = realloc(items_1.encodeds, (items_1.n_encoded + 1) * sizeof(res));
            items_1.encodeds[items_1.n_encoded] = its.encodeds[j];
            items_1.n_encoded++;
        } else if (bit < 0) {
            switch (bit) {
                case -1:
                    n_eqs_1++;
                    items_1.encodeds = realloc(items_1.encodeds, (items_1.n_encoded + 1) * sizeof(res));
                    items_1.encodeds[items_1.n_encoded] = its.encodeds[j];
                    items_1.n_encoded++;
                    break;
                case -2:
                    n_eqs_0++;
                    items_0.encodeds = realloc(items_0.encodeds, (items_0.n_encoded + 1) * sizeof(res));
                    items_0.encodeds[items_0.n_encoded] = its.encodeds[j];
                    items_0.n_encoded++;
                    break;
                default:
                    fprintf(stderr, "Unexpected value in encoded result: %d\n", bit);
                    exit(EXIT_FAILURE);
            }
        }

        else {
            items_0.encodeds = realloc(items_0.encodeds, (items_0.n_encoded + 1) * sizeof(res));
            items_0.encodeds[items_0.n_encoded] = its.encodeds[j];
            items_0.n_encoded++;
        }
    }

    if (n_eqs_1 == its.n_encoded || n_eqs_0 == its.n_encoded) {
        items_0.index = its.index;
        items_1.index = its.index;
        items_full fr;

        if (n_eqs_1 == its.n_encoded) {
            fr.n = items_1.n_encoded;
            fr.its = items_1;
        } else {
            fr.n = items_0.n_encoded;
            fr.its = items_0;
        }

        return fr;
    }
    items_0.index = its.index - items_1.n_encoded;

    items_full fr = helper_function_sort(items_0, depth + 1);
    items_full sr = helper_function_sort(items_1, depth + 1);
    items_full fin;

    fin.n = fr.n + sr.n;
    fin.its = malloc(fin.n * sizeof(items));

    for (int m = 0; m < fr.n; m++){
        fin.its[m] = fr.its[m];
    }

    for (int n = 0; n < sr.n; n++){
        fin.its[fr.n + n] = sr.its[n];
    }

    free(fr.its);
    free(sr.its);

    return fin;
}

items_full sort(int* original, unsigned int original_size){
    res* encodeds = malloc(original_size*sizeof(res));
    int offset = 1000;
    for (size_t i = 0; i < original_size; i++){
        encodeds[i] = encode(offset, original[i], 2);
    }

    items it = {encodeds, original_size, original_size-1};
    return helper_function_sort(it, 0); 

}

int* read_input_text_file(char* filename, int* size)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    char line[256];
    *size = 0;
    printf("First line of file: %s\n", filename);
    while (fgets(line, sizeof(line), file)) {
        char* trimmed = line;
        while (*trimmed && isspace((unsigned char)*trimmed)) {
            trimmed++;
        }
        if (*trimmed == '\0' || *trimmed == '\n') {
            continue;
        }

        char* endptr;
        long count = strtol(trimmed, &endptr, 10);
        if (trimmed == endptr || count <= 0 || count > INT_MAX) {
            fprintf(stderr, "Invalid array size in %s\n", filename);
            fclose(file);
            return NULL;
        }

        *size = (int)count;
        break;
    }
    printf("Array size read from %s: %d\n", filename, *size);

    if (*size == 0) {
        fprintf(stderr, "No size found in %s\n", filename);
        fclose(file);
        return NULL;
    }

    int* array = malloc(*size * sizeof(int));
    if (array == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    printf("Reading %d numbers from %s\n", *size, filename);
    int index = 0;
    while (index < *size && fgets(line, sizeof(line), file)) {
        char* token = strtok(line, " \t\r\n");
        while (token != NULL && index < *size) {
            char* endptr;
            long value = strtol(token, &endptr, 10);
            if (token == endptr) {
                fprintf(stderr, "Invalid integer in %s: %s\n", filename, token);
                free(array);
                fclose(file);
                return NULL;
            }
            array[index++] = (int)value;
            token = strtok(NULL, " \t\r\n");
        }
    }

    printf("Read %d numbers from %s\n", index, filename);
    if (index != *size) {
        fprintf(stderr, "Expected %d numbers in %s, but found %d\n", *size, filename, index);
        free(array);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return array;
}

int main(int argc, char** argv)
{
    int* array;
    int size;
    for (int j = 0; j < argc-1; j++){
        if (strcmp(argv[j], "-f") == 0 && j + 1 < argc) {
            printf("Reading from file: %s\n", argv[j + 1]);
            array = read_input_text_file(argv[j + 1], &size);
            printf("Array read from file: ");
            for (int k = 0; k < size; k++) {
                printf("%d ", array[k]);
            }
            printf("\n");
            if (array == NULL) {
                return EXIT_FAILURE;
            }
        }
    } printf("\n");

    items_full response = sort(array, size);
    printf("BEFORE FINAL CRASH\n");
    for (int j = 0; j < response.n; j++)
    {
        res resp = (*response.its[j].encodeds);
        printf("%d ", decode(resp, 1000, 2));
    } printf("\n");

    return 0;
}