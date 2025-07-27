#include <stdio.h>
#include "moving-avg.h"

const float test_sig[64] = 
{
	0.8660, -0.5000, -0.0000, 0.5000, -0.8660, -2.0000, -0.8660, 0.5000, 0.0000, -0.5000, 
    0.8660, 2.0000, 0.8660, -0.5000, 0.0000, 0.5000, -0.8660, -2.0000, -0.8660, 0.5000, 
    -0.0000, -0.5000, 0.8660, 2.0000, 0.8660, -0.5000, -0.0000, 0.5000, -0.8660, -2.0000, 
    -0.8660, 0.5000, 0.0000, -0.5000, 0.8660, 2.0000, 0.8660, -0.5000, 0.0000, 0.5000, 
    -0.8660, -2.0000, -0.8660, 0.5000, -0.0000, -0.5000, 0.8660, 2.0000, 0.8660, -0.5000, 
    -0.0000, 0.5000, -0.8660, -2.0000, -0.8660, 0.5000, 0.0000, -0.5000, 0.8660, 2.0000, 
    0.8660, -0.5000, 0.0000, 0.5000
};

constexpr size_t test_sig_sz = sizeof(test_sig) / sizeof(test_sig[0]);
constexpr size_t avg_num_elements = test_sig_sz / 8;

int main(int argc, char **argvp)
{
    FILE *f = fopen("avg.txt", "w");

    auto avg = dsp::average<float>(avg_num_elements);

    for (size_t i=0;i < test_sig_sz;i++)
        fprintf(f, "%.4f ", avg.insert(test_sig[i]));

    fclose(f);

    return 0;
}
