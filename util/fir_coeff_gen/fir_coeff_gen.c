#include <stdio.h>
#include <stdint.h>

#define SAMPLEFILTER_TAP_NUM 249
static int16_t filter_taps[SAMPLEFILTER_TAP_NUM] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  1,
  2,
  2,
  2,
  2,
  2,
  1,
  1,
  0,
  -1,
  -3,
  -4,
  -5,
  -5,
  -6,
  -5,
  -4,
  -3,
  -1,
  2,
  5,
  8,
  10,
  12,
  13,
  12,
  11,
  8,
  3,
  -2,
  -8,
  -14,
  -19,
  -23,
  -25,
  -25,
  -22,
  -17,
  -9,
  1,
  12,
  23,
  33,
  41,
  45,
  46,
  42,
  33,
  19,
  2,
  -17,
  -36,
  -54,
  -68,
  -77,
  -79,
  -73,
  -59,
  -37,
  -10,
  22,
  54,
  85,
  109,
  125,
  130,
  122,
  101,
  68,
  24,
  -27,
  -80,
  -130,
  -172,
  -200,
  -211,
  -202,
  -171,
  -119,
  -50,
  32,
  118,
  202,
  275,
  327,
  351,
  342,
  297,
  216,
  102,
  -35,
  -188,
  -341,
  -480,
  -589,
  -654,
  -660,
  -598,
  -461,
  -248,
  38,
  386,
  784,
  1212,
  1649,
  2072,
  2457,
  2783,
  3031,
  3185,
  3238,
  3185,
  3031,
  2783,
  2457,
  2072,
  1649,
  1212,
  784,
  386,
  38,
  -248,
  -461,
  -598,
  -660,
  -654,
  -589,
  -480,
  -341,
  -188,
  -35,
  102,
  216,
  297,
  342,
  351,
  327,
  275,
  202,
  118,
  32,
  -50,
  -119,
  -171,
  -202,
  -211,
  -200,
  -172,
  -130,
  -80,
  -27,
  24,
  68,
  101,
  122,
  130,
  125,
  109,
  85,
  54,
  22,
  -10,
  -37,
  -59,
  -73,
  -79,
  -77,
  -68,
  -54,
  -36,
  -17,
  2,
  19,
  33,
  42,
  46,
  45,
  41,
  33,
  23,
  12,
  1,
  -9,
  -17,
  -22,
  -25,
  -25,
  -23,
  -19,
  -14,
  -8,
  -2,
  3,
  8,
  11,
  12,
  13,
  12,
  10,
  8,
  5,
  2,
  -1,
  -3,
  -4,
  -5,
  -6,
  -5,
  -5,
  -4,
  -3,
  -1,
  0,
  1,
  1,
  2,
  2,
  2,
  2,
  2,
  1,
  1,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

int main()
{
    FILE* out = fopen("fir_coeffs.memh", "w");

    fprintf(out, "// Coef size (%u)\n", SAMPLEFILTER_TAP_NUM);

    for(int i = 0; i < 256; i++)
    {
        if(i < SAMPLEFILTER_TAP_NUM)
        {
            fprintf(out, "%04hX\n", filter_taps[i]);
        }
        else if(i == SAMPLEFILTER_TAP_NUM)
        {
            fprintf(out, "// Padding\n");
            fprintf(out, "0000\n");
        }
        else
        {
            fprintf(out, "0000\n");
        }
    }

    fclose(out);

    return 0;
}
