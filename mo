#include<iostream> 
#include "Image_Class.h"
using namespace std;
void imageedges(Image& image){
        
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            int red = image(i, j, 0);
            int green = image(i, j, 1);
            int blue = image(i, j, 2);
            int average = (red + green + blue) / 3;
            if (average < 127) {
                image(i, j, 0) = 0;
                image(i, j, 1) = 0;
                image(i, j, 2) = 0;
            } else {
                image(i, j, 0) = 255;
                image(i, j, 1) = 255;
                image(i, j, 2) = 255;
            }
        }
    }
    Image newImage(image.width, image.height);
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            newImage(i, j, 0) = 255;
            newImage(i, j, 1) = 255;
            newImage(i, j, 2) = 255;
        }
    }
    for (int i = 0; i < image.width - 1; i++) {
        for (int j = 0; j < image.height - 1; j++) {
            if (image(i, j, 0) != image(i + 1, j, 0) || image(i, j, 0) != image(i, j + 1, 0)) {
                newImage(i, j, 0) = 0;
                newImage(i, j, 1) = 0;
                newImage(i, j, 2) = 0;
            }
        }
    }

    image = newImage;
}
void tvFilter(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            if (j % 2 != 0) {
                image(i, j, 0) = 0;
                image(i, j, 1) = 0;
                image(i, j, 2) = 0;
            }
        }
    }
}
