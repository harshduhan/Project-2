#include "seamcarving.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int d_y(struct rgb_img *im, int y, int x){
    if(y==0){
        int red = get_pixel(im,(im->height)-1,x,0) - get_pixel(im,y+1,x,0);
        int green = get_pixel(im, (im->height)-1,x,1) - get_pixel(im,y+1,x,1); 
        int blue = get_pixel(im,(im->height)-1,x,2) - get_pixel(im,y+1,x,2);
        return red*red + green*green + blue*blue;
    }
    else if(y==im->height-1){
        int red = get_pixel(im,0,x,0) - get_pixel(im,y-1,x,0);
        int green = get_pixel(im,0,x,1) - get_pixel(im,y-1,x,1);
        int blue = get_pixel(im,0,x,2) - get_pixel(im,y-1,x,2);
        return red*red + green*green + blue*blue;
    }
    else{
        int red = get_pixel(im,y+1,x,0) - get_pixel(im,y-1,x,0);
        int green = get_pixel(im,y+1,x,1) - get_pixel(im,y-1,x,1);
        int blue = get_pixel(im,y+1,x,2) - get_pixel(im,y-1,x,2);
        return red*red + green*green + blue*blue;
    }
}

int d_x(struct rgb_img *im, int y, int x){
    if(x==0){
        int red = get_pixel(im,y,(im->width)-1,0) - get_pixel(im,y,x+1,0);
        int green = get_pixel(im, y, (im->width)-1,1) - get_pixel(im,y,x+1,1);
        int blue = get_pixel(im, y,(im->width)-1,2) - get_pixel(im,y,x+1,2);
        return red*red + green*green + blue*blue;
    }
    else if(x==im->width-1){
        int red = get_pixel(im,y,0,0) - get_pixel(im,y,x-1,0);
        int green = get_pixel(im,y,0,1) - get_pixel(im,y,x-1,1);
        int blue = get_pixel(im,y,0,2) - get_pixel(im,y,x-1,2);
        return red*red + green*green + blue*blue;
    }
    else{
        int red = get_pixel(im,y,x+1,0) - get_pixel(im,y,x-1,0);
        int green = get_pixel(im,y,x+1,1) - get_pixel(im,y,x-1,1);
        int blue = get_pixel(im,y,x+1,2) - get_pixel(im,y,x-1,2);
        return red*red + green*green + blue*blue;
    }
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    create_img(grad,im->height,im->width);
    int y = 0;
    int x = 0;
    while (y<im->height){
        x =0;
        while(x<im->width){
            int x_delta = d_x(im,y,x);
            int y_delta = d_y(im,y,x);
            double grad_energy = ((double)sqrt((double)y_delta+(double)x_delta))/10;
            int energy_set = (uint8_t)(grad_energy);
            set_pixel(*grad,y,x,energy_set,energy_set,energy_set);
            x++;
        }
        y++;
    }
}

double min(int y, int x, double *array, int width){
    if(x==0){
        return array[(y-1)*width+x]>array[(y-1)*width+x+1] ? array[(y-1)*width+x+1] : array[(y-1)*width+x];
    }
    else if(x==width-1){
        return array[(y-1)*width + x] > array[(y-1)*width + x - 1] ? array[(y-1)*width + x - 1] : array[(y-1)*width + x];
    }
    else{
        double cur_min = array[(y-1)*width+x]>array[(y-1)*width+x+1] ? array [(y-1)*width+x+1] : array[(y-1)*width+x];
        return cur_min>array[(y-1)*width+x-1] ? array[(y-1)*width+x-1] : cur_min;
    }
}
void dynamic_seam(struct rgb_img *grad, double **best_arr){
    *best_arr = (double *)malloc(sizeof(double)* (grad->width) * (grad->height));
    int y = 0;
    int i = 0;
    int x = 0;
    while(y < grad->height){
        x = 0;
        while(x < grad->width){
            if(y==0){
                (*best_arr)[i] = get_pixel(grad,y,x,0);
                i++;
            }
            else if(y>0){
                (*best_arr)[i] = (double)get_pixel(grad,y,x,0) + (double)min(y,x,*best_arr,grad->width);
                i++;
            }
            x++;
        }
        y++;
    }
}

void recover_path(double *best, int height, int width, int **path){

    *path = (int *)malloc(sizeof(int) * height);
    int initial_min =  2147483647, y = 0, x = 0, i = 0, cur_col = 0;

    while(x < width){
        if(best[y*width + x] < initial_min){
            initial_min = (int)best[y*width + x], cur_col = x;
        }
        x++;
    }

    (*path)[i] = cur_col;
    i++;

    while(y < height - 1){
        if(cur_col == 0){
            if((int)(best[(y+1)*width + cur_col]) > (int)(best[(y+1)*width + cur_col + 1])){
                cur_col++, (*path)[i] = cur_col, i++;
            }
            else{
                (*path)[i] = cur_col, i++;
            }
        }
        else if(cur_col == width - 1){
            if((int)(best[(y+1)*width + cur_col]) > (int)(best[(y+1)*width + cur_col - 1])){
                cur_col--, (*path)[i] = cur_col, i++;
            }
            else{
                (*path)[i] = cur_col, i++;
            }
        }
        else{
            if(best[(y+1)*width + cur_col] > best[(y+1)*width + cur_col + 1]){
                int temp = cur_col + 1;
                if(best[(y+1)*width + temp] > best[(y+1)*width + cur_col - 1]){
                    cur_col--, (*path)[i] = cur_col, i++;
                }
                else{
                    cur_col++, (*path)[i] = cur_col, i++;
                }
            }
            else if((int)(best[(y+1)*width + cur_col]) < (int)(best[(y+1)*width + cur_col + 1])){
                int temp = cur_col;
                if((int)(best[(y+1)*width + temp]) > (int)(best[(y+1)*width + cur_col - 1])){
                    cur_col--, (*path)[i] = cur_col, i++;
                }
                else{
                    (*path)[i] = cur_col, i++;
                }
            }
        }
        y++;
    }
}
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int* path){
    create_img(dest, src->height, (src->width)- 1);
    int x = 0; 
    int y = 0;
    while (y < (*dest)->height)
    {
        x = 0;
        while (x < (*dest)->width)
        {
            if (x == path[x])
            {
                continue;
            }
            else
            {
                set_pixel(*dest, y, x, get_pixel(src, y, x, 0), get_pixel(src, y, x, 1), get_pixel(src, y, x, 2));
                x++;
            }
        }
        y++;
    }
}
