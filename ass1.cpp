#include <iostream>
using namespace std;
#include "Image_Class.h"
#include <bits/stdc++.h>



Image loadimage(Image img) {
    return img;
}
Image Invert_image(Image img) {
    for (int i = 0; i<img.width ; i++) {
        for (int j=0; j<img.height ;j++) {
            
            img (i,j,0) = 255 - img(i,j,0);
            img (i,j,1) = 255 - img(i,j,1);
            img (i,j,2) = 255 - img(i,j,2); 



        }
      
    }
    return img;

}

Image Frame_image(Image img) {
    for (int i = 0; i<img.width ; i++) {
        for (int j=0; j<img.height ;j++) {
            if (i < 10 || i >= img.width - 10 ||j < 10 || j>=img.height - 10) {

            img (i,j,0) = 127;
            img (i,j,1) = 0;
            img (i,j,2) = 255;
            }


        }
      
    }
    return img;

}
Image blackandwhiteimage(Image& image) {
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
}

Image rotateimage(Image& image) {
    int angle;
    cout << "Enter the angle of rotation (90, 180, 270): ";
    cin >> angle;
    if (angle == 180) {
        Image newImage(image.width, image.height);
        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height; j++) {
                int new_i = image.width - 1 - i;
                int new_j = image.height - 1 - j;
                newImage(new_i, new_j, 0) = image(i, j, 0);
                newImage(new_i, new_j, 1) = image(i, j, 1);
                newImage(new_i, new_j, 2) = image(i, j, 2);
            }
        }
        image = newImage;
    }
    else if (angle == 90) {
        Image newImage(image.height, image.width);
        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height; j++) {
                int new_i = image.height - 1 - j;
                int new_j = i;
                newImage(new_i, new_j, 0) = image(i, j, 0);
                newImage(new_i, new_j, 1) = image(i, j, 1);
                newImage(new_i, new_j, 2) = image(i, j, 2);
            }
        }
        image = newImage;
    }
    else if (angle == 270) {
        Image newImage(image.height, image.width);
        for (int i = 0; i < image.width; i++) {
            for (int j = 0; j < image.height; j++) {
                int new_i = j;
                int new_j = image.width - 1 - i;
                newImage(new_i, new_j, 0) = image(i, j, 0);
                newImage(new_i, new_j, 1) = image(i, j, 1);
                newImage(new_i, new_j, 2) = image(i, j, 2);
            }
        }
        image = newImage;
    }
    else {
        cout << "Invalid angle!" << endl;
    }
}
Image Grey_scale(Image img) {

    for (int y = 0; y < img.height;y++) {
        for (int x = 0; x < img.width; x++) {


            int R = img.getPixel(x, y, 0);
            int G = img.getPixel(x, y, 1);
            int B = img.getPixel(x, y, 2);

            int grey = (R + G + B) / 3;

            img.setPixel(x, y, 0, grey);
            img.setPixel(x, y, 1, grey);
            img.setPixel(x, y, 2, grey);
        }
    }
    return img;
}


Image Flip_Image(Image img)
{

    Image newiamge(img.width, img.height);
    char choice;
    cout << "if you want Horizontal Flip enter H and if veritcal enter V \n";
    cin >> choice;
    if (choice=='H') {
        for (int y = 0;y < img.height;y++){
            for (int x = img.width-1; 0 <= x;x--)
            {
                  int newX = img.width - 1 - x;

                int R = img.getPixel(x, y, 0);
                int G = img.getPixel(x, y, 1);
                int B = img.getPixel(x, y, 2);
                    
              
                newiamge.setPixel(newX, y, 0, R);
                newiamge.setPixel(newX, y, 1, G);
                newiamge.setPixel(newX, y, 2, B);

         
         } 
}
        return newiamge;



    }
    else {
        Image newiamge(img.width, img.height);
        for (int x = 0;x < img.width;x++) {

            for (int y = img.height - 1;y >= 0;y--) {

                int newy = img.height - 1 - y;

                int R = img.getPixel(x, y, 0);
                int G = img.getPixel(x, y, 1);
                int B = img.getPixel(x, y, 2);


                newiamge.setPixel(x, newy, 0, R);
                newiamge.setPixel(x, newy, 1, G);
                newiamge.setPixel(x, newy, 2, B);
           
  }
 }
        return newiamge;
           
    }

}
Image Brightness(Image img) {
    cout << "1- Darken \n";
    cout << "2- lighten \n";
    int b;
    cin >> b;
    if (b == 1) {
        cout << "\t write The level of Darkness (0-100)\n";
        double d;
        cin >> d;
        const double min_factor = 0.2;
        double darken = 1.0 - (d / 100.0) * (1.0 - min_factor);
        for (int i = 0; i < img.width; i++) {
            for (int j = 0; j < img.height; j++) {
                for (int k=0; k<3 ; k++){
                    img(i, j, k) *= darken ;
                }
            }
        }
    }
    else if (b == 2) {
        cout << "\t Write The level of Brightness (0-100)\n";
        double h;
        cin >> h;
        double lighten = 1 + (h/100.0);
        for (int i = 0; i < img.width; i++) {
            for (int j = 0; j < img.height; j++) {
                for (int k=0; k<3 ; k++){
                    img(i, j, k) = min(255.0, img(i, j, k)*lighten);    
                }
            }
        }
    }
return img;
}


    Image Resize(Image img) {
    cout << "1- Enter new dimensions\n";
    cout << "2- Enter a ratio\n";
    int r;
    cin >> r;

    int newWidth, newHeight;

    if (r == 1) {
        cout << "New width: ";
        cin >> newWidth;
        cout << "New height: ";
        cin >> newHeight;
    } else {
        double ratio;
        cout << "Ratio (e.g. 0.5 to reduce, 2 to increase): ";
        cin >> ratio;
        newWidth = max(1, (int)round(img.width * ratio));
        newHeight = max(1, (int)round(img.height * ratio));
    }

    Image result(newWidth, newHeight);

    for (int i = 0; i < newWidth; i++) {
        for (int j = 0; j < newHeight; j++) {

            
            double srcX = (newWidth > 1) ? i * (double)(img.width - 1) / (newWidth - 1) : 0;
            double srcY = (newHeight > 1) ? j * (double)(img.height - 1) / (newHeight - 1) : 0;

            int x0 = (int)srcX;
            int y0 = (int)srcY;
            int x1 = min(x0 + 1, img.width - 1);
            int y1 = min(y0 + 1, img.height - 1);

            double dx = srcX - x0;   
            double dy = srcY - y0;   

            for (int c = 0; c < 3; c++) {
                double top    = img(x0, y0, c) * (1 - dx) + img(x1, y0, c) * dx;
                double bottom = img(x0, y1, c) * (1 - dx) + img(x1, y1, c) * dx;
                result(i, j, c) = (unsigned char)round(top * (1 - dy) + bottom * dy);
            }
        }
    }
    return result;
}







int main() {
    Image img;

    bool r = true;
    while (r){

    cout << "\t========================================\n";
    cout << "\t           IMAGE FILTER MENU\n";
    cout << "\t========================================\n";
    cout << "\t 1.  Load a New Image\n";
    cout << "\t 2.  Grayscale Conversion\n";
    cout << "\t 3.  Black and White\n";
    cout << "\t 4.  Invert Image\n";
    cout << "\t 5.  Add Frame\n";
    cout << "\t 6.  Flip Image\n";
    cout << "\t 7.  Rotate Image\n";
    cout << "\t 8.  Darken / Lighten\n";
    cout << "\t 9.  Resize Image\n";
    cout << "\t 10. Save Image\n";
    cout << "\t 11. Exit\n";
    cout << "\t========================================\n";
    cout << "\n\tEnter your choice: ";

    
    int n;
    cin >> n;

   if (n == 1) {
    
    cout << "Loading a new image...\n";
    string imgname;
    cin >> imgname;
    img = loadimage(imgname);
    
}
else if (n == 2) {
    
    cout << "Applying grayscale conversion...\n";
    img = Grey_scale(img);
}
else if (n == 3) {
    
    cout << "Converting to black and white...\n";
    img = blackandwhiteimage(img);
}
else if (n == 4) {
    
    cout << "Inverting image colors...\n";
    img = Invert_image(img);
}
else if (n == 5) {
    
    cout << "Adding frame to image...\n";
    img = Frame_image(img);
    
}
else if (n == 6) {
   
    cout << "Flipping image...\n";
    img= Flip_Image(img);
}
else if (n == 7) {
    
    cout << "Rotating image...\n";
   img  =  rotateimage(img);
}
else if (n == 8) {
    cout << "Adjusting brightness...\n";
    img = Brightness(img);
    
}

else if (n == 9) {
    cout << "resizing image...\n";
    img = Resize(img);
    
}


else if (n == 10) {
   
    cout << "Saving image...\n";
    cout << "write image name\n";
    string filename;
    cin >> filename;
    img.saveImage(filename.c_str());

    
}
else if (n == 11) {
    
    cout << "Exiting program. Goodbye!\n";
    break;
    
}
else {
    cout << "Invalid choice! Please try again.\n";
}

}
return 0;
}
