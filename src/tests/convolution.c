#include <stdio.h>
#include <stdlib.h>
#include <types/matrix.h>
#include <types/image.h>
#include "../app/convolution.h"

int main(int argc, char **argv)
{
    // Matrix creation test
    struct matrix_t *mat;
    mat = mat_make(3, 3);
    if (mat == NULL) {
        printf("Failed to initialize matrix!\n");
        return 1;
    }

    // Set embass filter values
    mat->values[0][0] = -2;
    mat->values[0][1] = -1;
    mat->values[0][2] = 0;
    mat->values[1][0] = -1;
    mat->values[1][1] = 1;
    mat->values[1][2] = 1;
    mat->values[2][0] = 0;
    mat->values[2][1] = 1;
    mat->values[2][2] = 2;

    // Image creation test
    struct image_t *img, *filteredImg;
    FILE *file;
    file = fopen("../test_datasets/in/colored.raw", "r");
    img = img_makeFromFile(file, 1920, 2520, 3);
    fclose(file);
    if (img == NULL) {
        printf("Failed to initialize image!\n");
        mat_destroy(mat);
        return 1;
    }
    file = fopen("../test_datasets/out/st-same.raw", "w");
    img_writeToFile(img, file);
    fclose(file);

    // Serial convolution test
    filteredImg = conv_run(img, mat);
    if (filteredImg == NULL) {
        printf("Failed to run image convolution!\n");
        img_destroy(img);
        mat_destroy(mat);
        return 1;
    }
    file = fopen("../test_datasets/out/st-filtered.raw", "w");
    img_writeToFile(filteredImg, file);
    fclose(file);

    // Clean
    img_destroy(filteredImg);
    img_destroy(img);
    mat_destroy(mat);

    return 0;
}
