#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include "compact.h"

res compress(int num, int target_num, int n_chunks)
{
    int* result = malloc(sizeof(int));
    if (num <= INT_MAX){
        float u, l;
        int n_red;
        int current = num/2;
        float chunk_size;
        
        n_red = 0;
        u = (float)num;
        l = 0.0;

        while (1)
        {   
            float chunk_size = (u-l)/n_chunks;
            int k = (int)floor((float)(target_num-l)/chunk_size);
            int ul = (k+1)*chunk_size+l;
            int il = k*chunk_size+l;

            u = (float)ul;
            l = (float)il;

            result = realloc(result, (n_red+1)*sizeof(int));
            result[n_red] = k;
            n_red++;

            current = (float)(ul + il)/2;

            if (ul == target_num){
                result = realloc(result, (n_red+1)*sizeof(int));
                result[n_red] = -1;
                break;
            } else if (il == target_num){
                result = realloc(result, (n_red+1)*sizeof(int));
                result[n_red] = -2;
                break;
            } else if (current == target_num){
                result = realloc(result, (n_red+1)*sizeof(int));
                result[n_red] = -3;
                break;
            }
        }
        res comp = {result, n_red+1};
        return comp;

    } perror("MAX_INT BUFFER EXCEEDED!\n");
    
}

res compressf(float num, float target_num, int n_chunks)
{
    int* result = malloc(sizeof(int));
    if (num <= FLT_MAX){
        float u, l;
        int n_red;
        float current = num/2;
        float chunk_size;
        
        n_red = 0;
        u = (float)num;
        l = 0.0;

        while (1)
        {   
            float chunk_size = (float)(u-l)/n_chunks;
            int k = (int)floor((float)(target_num-l)/chunk_size);
            float ul = (k+1)*chunk_size+l;
            float il = k*chunk_size+l;

            u = ul;
            l = il;

            //printf("%f %f %f\n", u, l, current);

            result = realloc(result, (n_red+1)*sizeof(int));
            result[n_red] = k;
            n_red++;

            current = (float)(ul + il)/2;

            if (ul == target_num){
                result = realloc(result, (n_red+1)*sizeof(int));
                result[n_red] = -1;
                break;
            } else if (il == target_num){
                result = realloc(result, (n_red+1)*sizeof(int));
                result[n_red] = -2;
                break;
            } else if (current == target_num){
                result = realloc(result, (n_red+1)*sizeof(int));
                result[n_red] = -3;
                break;
            }
        }
        res comp = {result, n_red+1};
        return comp;

    } perror("FLT_MAX BUFFER EXCEEDED!\n");
    
}



int decompress(res compressed, int offset, int base)
{
    int u = offset;
    int l = 0;
    float chunk_size;
    
    // Vamos percorrer todos os índices, EXCETO o último (que é -1, -2 ou -3)
    for (int step = 0; step < compressed.n - 1; step++) {
        int idx = compressed.result[step];
        chunk_size = (float)(u - l) / (float)base;
        
        int in = l + (int)floor(idx * chunk_size);
        int un = l + (int)floor((idx + 1) * chunk_size);
        
        // Atualiza o intervalo para o chunk escolhido
        u = un;
        l = in;
    }
    
    // Agora trata o último valor (condição de parada)
    int last = compressed.result[compressed.n - 1];
    int in = l;
    int un = u;
    int current = (in + un) / 2;
    
    switch(last) {
        case -1: // Estava no limite superior
            return un;
        case -2: // Estava no limite inferior
            return in;
        case -3: // Estava no meio
            return current;
        default: // Por segurança (nunca deve acontecer)
            return -1;
    }
}

float decompressf(res compressed, float offset,  int base){
    float u = offset;
    float l = 0.0;
    float chunk_size;

    for (int step = 0; step < compressed.n-1; step++){
        chunk_size = (u-l)/base;
        int idx = compressed.result[step];
        float il = l+idx*chunk_size;
        float ul = l+(idx+1)*chunk_size;

        l = il;
        u = ul;
    }

    int last = compressed.result[compressed.n - 1];
    float in = l;
    float un = u;
    float current = (in + un) / 2;
    
    switch(last) {
        case -1: // Estava no limite superior
            return un;
        case -2: // Estava no limite inferior
            return in;
        case -3: // Estava no meio
            return current;
        default: // Por segurança (nunca deve acontecer)
            return -1;
    }
}

int main(int argc, char** argv)
{
    int num;
    int offset;
    int base;

    float numf;
    float offsetf;
    
    int f = 0;

    for (int i = 0; i < argc-1; i++){
        if (strcmp(argv[i], "--f") == 0){
            f = 1;
        } else if(strcmp(argv[i], "--num") == 0){
            if (f){
                numf = atof(argv[i+1]);
                continue;
            } num = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "--base") == 0){
            base = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "--offset") == 0){
            if (f){
                offsetf = (float)atof(argv[i+1]);
                continue;
            } offset = atoi(argv[i+1]);
        }
    }
    if(!f){
        res r = compress(offset, num, base);
        for (int j = 0; j < r.n; j++){
            printf("%d ", r.result[j]);
        }

        int decompressed = decompress(r, offset, base);
        if (decompressed == num){
            printf("Success!!\n");
        } else{
            printf("Failure.\n");
            printf("%d %d\n", decompressed, num);
        }
    } else {
        res r = compressf(offsetf, numf, base);
        for (int j = 0; j < r.n; j++){
            printf("%d ", r.result[j]);
        }

        float decompressed = decompressf(r, offsetf, base);
        if (decompressed == numf){
            printf("Success!!\n");
        } else{
            printf("Failure.\n");
            printf("%f %f\n", decompressed, numf);
        }
    }
}