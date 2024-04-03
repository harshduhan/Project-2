#include "seamcarving.h"
#include "c_img.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *inputFilename = "HJoceanSmall.bin";
    char *outputFilename = "HJoceanSmall_Carved2.bin";
    int numSeams = 1000; // You can adjust the number of seams you want to remove

    // Step 1: Read the input image
    struct rgb_img *srcImage;
    read_in_img(&srcImage, inputFilename);

    // Loop to remove multiple seams
    for(int seam = 0; seam < numSeams; seam++) {
        // Step 2: Calculate energy map
        struct rgb_img *grad;
        calc_energy(srcImage, &grad);

        // Step 3: Find the seam to remove
        double *best_arr;
        dynamic_seam(grad, &best_arr);
        int *path;
        recover_path(best_arr, grad->height, grad->width, &path);

        // Step 4: Remove the seam from the source image
        struct rgb_img *destImage;
        remove_seam(srcImage, &destImage, path);

        // Clean up
        destroy_image(srcImage);
        free(best_arr);
        free(path);
        destroy_image(grad);

        // Prepare for the next iteration
        srcImage = destImage;
    }

    // Step 5: Write the modified image to a file
    write_img(srcImage, outputFilename);

    // Clean up
    destroy_image(srcImage);

    printf("Seam carving completed. Result saved to %s\n", outputFilename);

    return 0;
}
