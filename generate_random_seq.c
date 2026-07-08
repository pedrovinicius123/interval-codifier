#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc, char** argv){
    char* filename;
    int n_items;
    int max_val;

    for (int i = 0; i < argc-1; i++){
        if (strcmp(argv[i], "-n") == 0){
            n_items = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-m") == 0){
            max_val = atoi(argv[i+1]);
        } else if(strcmp(argv[i], "-f") == 0){
            filename = argv[i+1];
        }
    }
    
    FILE* fptr = fopen(filename, "w");

    if (!fptr){
        fprintf(stderr, "File could not be created\n");
        return 1;
    }

    fprintf(fptr, "%d\n", n_items);
    fprintf(fptr, "%d\n", max_val);
    srand(time(NULL));

    for(int n = 0; n < n_items; n++){
        int random_num = (int)rand()%max_val+1;
        fprintf(fptr, "%d\n", random_num);

    }

    fclose(fptr);
    return 0;

}