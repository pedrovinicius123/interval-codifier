#ifndef COMPACT
#define COMPACT

typedef struct {
    int* result;
    int n;
} res;

res encode(int num, int target_num, int n_chunks);
res encodef(float num, float target_num, int n_chunks);
int decode(res compressed, int offset, int base);
float decodef(res compressed, float offset, int base);

#endif