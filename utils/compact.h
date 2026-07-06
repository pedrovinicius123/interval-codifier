#ifndef COMPACT
#define COMPACT

typedef struct {
    int* result;
    int n;
} res;

res compress(int num, int target_num, int n_chunks);
res compressf(int num, float target_num, int n_chunks);
int decompress(res compressed, int offset, int base);
float decompressf(res compressed, float offset, int base);

#endif