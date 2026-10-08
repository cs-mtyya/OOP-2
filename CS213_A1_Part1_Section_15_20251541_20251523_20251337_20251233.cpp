//====================================================================================
//======================================Part 1========================================
//Mohamed Tamer : 20251523 |||| filter: 3, 7
//Mohamed Assem:20251337  ||| filter: 2, 6, 10, 14, 18
//Abdalrahman Mohamed Wahba: 20251541 |||| filter: 4, 8, 12, 16
//Abdlruhman Mohamed: 20251233 |||| filter: 1, 5 , 9 , 13
//section: S15
//===================================================================================
//===================================================================================
#include <iostream>
using namespace std;
#include "Image_Class.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
void oilPaintingFilter(Image& image) {
    for (int i = 0; i < image.width; i++) {
        for (int j = 0; j < image.height; j++) {
            image(i, j, 0) = (image(i, j, 0) / 32) * 32 ;
            image(i, j, 1) = (image(i, j, 1) / 32) * 32 ;
            image(i, j, 2) = (image(i, j, 2) / 32) * 32 ;
            
        }
    }
}
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
    int f;
    cout << "Choose frame style:\n";
    cout << "1) Simple\n";
    cout << "2) Fancy\n";
    cout << "Your choice: ";
    cin >> f;

    for (int i = 0; i < img.width; i++) {
        for (int j = 0; j < img.height; j++) {

            
            int d = min(min(i, img.width - 1 - i),
                        min(j, img.height - 1 - j));

            if (f == 1) {
                if (d < 10) {
                    img(i, j, 0) = 255;
                    img(i, j, 1) = 255;
                    img(i, j, 2) = 255;
                }
            }
            else {
                
                if (d < 10) {
                    img(i, j, 0) = 255;
                    img(i, j, 1) = 255;
                    img(i, j, 2) = 255;
                }
                else if (d >= 15 && d < 20) {
                    img(i, j, 0) = 255;
                    img(i, j, 1) = 255;
                    img(i, j, 2) = 255;
                }
            }
        }
    }
    return img;
}


Image blackandwhiteimage(Image img) {
    for (int i = 0; i < img.width; i++) {
        for (int j = 0; j < img.height; j++) {
            int red = img(i, j, 0);
            int green = img(i, j, 1);
            int blue = img(i, j, 2);
            int average = (red + green + blue) / 3;
            if (average < 127) {
                img(i, j, 0) = 0;
                img(i, j, 1) = 0;
                img(i, j, 2) = 0;
            } else {
                img(i, j, 0) = 255;
                img(i, j, 1) = 255;
                img(i, j, 2) = 255;
            }
        }
    }
    return img;
}

Image rotateimage(Image img) {

    int angle;
    cout << "Enter the angle of rotation (90, 180, 270): ";
    cin >> angle;
    if (angle == 180) {
        Image newImage(img.width, img.height);
        for (int i = 0; i < img.width; i++) {
            for (int j = 0; j < img.height; j++) {
                int new_i = img.width - 1 - i;
                int new_j = img.height - 1 - j;
                newImage(new_i, new_j, 0) = img(i, j, 0);
                newImage(new_i, new_j, 1) = img(i, j, 1);
                newImage(new_i, new_j, 2) = img(i, j, 2);
            }
        }
        img = newImage;
    }
    else if (angle == 90) {
        Image newImage(img.height, img.width);
        for (int i = 0; i < img.width; i++) {
            for (int j = 0; j < img.height; j++) {
                int new_i = img.height - 1 - j;
                int new_j = i;
                newImage(new_i, new_j, 0) = img(i, j, 0);
                newImage(new_i, new_j, 1) = img(i, j, 1);
                newImage(new_i, new_j, 2) = img(i, j, 2);
            }
        }
        img = newImage;
    }
    else if (angle == 270) {
        Image newImage(img.height, img.width);
        for (int i = 0; i < img.width; i++) {
            for (int j = 0; j < img.height; j++) {
                int new_i = j;
                int new_j = img.width - 1 - i;
                newImage(new_i, new_j, 0) = img(i, j, 0);
                newImage(new_i, new_j, 1) = img(i, j, 1);
                newImage(new_i, new_j, 2) = img(i, j, 2);
            }
        }
        img = newImage;
    }
    else {
        cout << "Invalid angle!" << endl;
    }
    return img;
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


    Image resize(Image img, int width,int height) {

        Image img2(width,height);
        double w = img.width,h = img.height;
        double step1,step2;
        step1 = w/width;
        step2= h/height;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++){
                for (int k=0; k<3 ; k++){
                    img2(i,j,k) = img(round(i*step1),round(j * step2),k);
                }
            }
        }
        return img2;
}

Image CropImage(Image img, int x , int y,int width,int height){
    if (x<0) x = 0;
    if (y<0) y = 0;
    if (x+width > img.width) width = img.width - x;
    if (y+height > img.height) height = img.width - y;
    Image img2 (width,height);
    for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++){
                for (int k=0; k<3 ; k++){
                    img2(i, j,k) = img (i+x,j+y,k);
                }
            }
        }
        return img2;
}


Image Purple(Image img){
    for (int i = 0; i < img.width;i++) {
        for (int j = 0; j < img.height; j++) {
            img(i,j,0) *= 0.8;
            img(i,j,1) *= 0.65;
            img(i,j,2) *= 0.95;

        }
    }
    return img;
}
//                                                         **                               ABDLRHAMN                        **

Image marge(Image img1) {
    string name;
    cout << "load the image u want to marge \n ";
    cin.ignore();
    getline(cin, name);
    Image img2;
    img2.loadNewImage(name);

    int w = min(img1.width, img2.width);
    int h = min(img1.height, img2.height);
  
        Image result(w,h);
        for (int x = 0;x < img1.width;x++){
         for (int y = 0;y < img1.height;y++){
             for (int k = 0;k < 3;k++) {

                 int avg = (img1.getPixel(x, y, k) + img2.getPixel(x, y, k)) / 2;
                 result.setPixel(x, y, k, avg);
             }

            }
        
}
        return result;
}

Image sunlight(Image img) {
    for (int x = 0;x < img.width;x++)
    {   
        for (int y = 0; y < img.height;y++)
        {

            img.setPixel(x, y, 0, min(img.getPixel(x, y, 0) + 90, 255));
            img.setPixel(x, y, 1, min(img.getPixel(x, y, 1) + 75, 255));
            img.setPixel(x, y, 2, min(img.getPixel(x, y, 2) + 10, 255));
        }

    }
    return img;


}
Image Flip_Image(Image img)
{


    Image newiamge(img.width, img.height);
    char flip;
    cout << "if you want Horizontal Flip enter H and if veritcal enter V \n";
    cout << "Your choice: ";
    cin >> flip;
    if (flip == 'H') {
        for (int y = 0;y < img.height;y++) {
            for (int x = img.width - 1; 0 <= x;x--)
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
Image Grey_scale(Image img) {

    for (int y = 0; y < img.height;y++) {
        for (int x = 0; x < img.width; x++) {
            int grey = 0;
            for (int k = 0;k < 3;k++)
            {
                grey += img.getPixel(x, y, k);


            }
            grey = grey / 3;
            for (int k = 0;k < 3;k++)
            {

                img.setPixel(x, y, k, grey);

            }



        }
    }
    return img;
}

Image skew(Image img)
{
    int dgree;
    cout << "Enter skew angle from 0 to 50  in degrees : \n "; 
    cin >> dgree;
  double rad = ((dgree * 3.14) / 180);
    int maxshift = ((img.height - 1) * tan(rad));
        int newWidth = img.width + maxshift;


    Image result(newWidth, img.height);

    for (int x = 0; x < newWidth; x++){
        for (int y = 0; y < img.height; y++) {
            for (int k = 0; k < 3; k++){
                result.setPixel(x, y, k, 255);}
        }
    }
    for(int y=0;y<img.height;y++)
    {
        int shift = (img.height - 1 - y) * rad;
        for (int x=0; x < img.width;x++)
            for (int k = 0;k < 3;k++)
                result.setPixel(x + shift, y, k, img.getPixel(x, y, k));

    
    }
    return result;




}
























//                                                       **                               ABDLRHAMN                        **
Image Blur(Image img, int radius) {
    if (radius <= 0) {
        cout << "Invalid radius. Must be greater than 0." << endl;
        return img;
    }
    Image newImage(img.width, img.height);
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            int sumR = 0, sumG = 0, sumB = 0;
            int count = 0; // to make avg later on 
            for (int dx = -radius; dx <= radius; dx++) {
                for (int dy = -radius; dy <= radius; dy++) {
                    int newX = x + dx;
                    int newY = y + dy;
                    if (newX >= 0 && newX < img.width && newY >= 0 && newY < img.height) {
                        sumR += img.getPixel(newX, newY, 0);
                        sumG += img.getPixel(newX, newY, 1);
                        sumB += img.getPixel(newX, newY, 2);
                        count++;
                    }
                }
            }
            newImage.setPixel(x, y, 0, sumR / count);
            newImage.setPixel(x, y, 1, sumG / count);
            newImage.setPixel(x, y, 2, sumB / count);
        }
    }
    return newImage;
}




int main() {
    Image img;

    bool r = true;
    while (r) {
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
        cout << "\t 10.  Merge Images\n";
        cout << "\t 11.  Detect Image Edges\n";
        cout << "\t 12.  Crop Images\n";
        cout << "\t 13.  Blur Images\n";
        cout << "\t 14.  sunlight filter\n";
        cout << "\t 15.  TV filter\n";
        cout << "\t 16.  Purple filter\n";
        cout << "\t 17.  infrared filter\n";
        cout << "\t 18.  Skewing Filter\n";
        cout << "\t 19.  Oil painting\n";
        cout << "\t 20. Save Image\n";
        cout << "\t 21. Exit\n";
        cout << "\t========================================\n";
        cout << "\n\tEnter your choice: ";


        int n;
        cin >> n;

        switch (n) {

        case 1: {
            cout << "Loading a new image...";
            cout << "\t Enter the image name: ";
            string imgname;
            cin.ignore();
            getline(cin, imgname);
            img = loadimage(imgname);
            break;
        }

        case 2:

            cout << "Applying grayscale conversion...\n";
            img = Grey_scale(img);
            cout << "\nfilter applied\n";
            break;
        case 3:

            cout << "Converting to black and white...\n";
            img = blackandwhiteimage(img);
            cout << "\nfilter applied\n";
            break;
        case 4:

            cout << "Inverting image colors...\n";
            img = Invert_image(img);
            cout << "\nfilter applied\n";
            break;
        case 5:

            cout << "Adding frame to image...\n";
            img = Frame_image(img);
            cout << "\nfilter applied\n";
            break;

        case 6:

            cout << "Flipping image...\n";
            img = Flip_Image(img);
            cout << "\nfilter applied\n";
            break;

        case 7:

            cout << "Rotating image...\n";
            img = rotateimage(img);
            cout << "\nfilter applied\n";
            break;
        case 8:
            cout << "Adjusting brightness...\n";
            img = Brightness(img);
            cout << "\nfilter applied\n";
            break;


        case 9:{
            cout << "resizing image...\n";
            int width, height;
            cout << "write dimensions\n";
            cout << "\nwidth: ";
            cin >> width;
            cout << "\nheight: ";
            cin >> height;
            img = resize(img, width, height);
            cout << "\nfilter applied\n";
            break;
}



        case 10:
            cout << "merging iamge....\n";
            img = marge(img);
            cout << "\nfilter applied\n";
            break;


        case 11:

            cout << "  Detecting Image Edges \n";
            imageedges(img);
            cout << "filter applayed \n";



            break;
        case 12:
            cout << "cropping image.....\n";
            int x, y, width, height;
            cout << "Enter x (starting column): ";
            cin >> x;
            cout << "Enter y (starting row): ";
            cin >> y;
            cout << "Enter width of the crop: ";
            cin >> width;
            cout << "Enter height of the crop: ";
            cin >> height;

            img = CropImage(img, x, y, width, height);
            cout << "\nImage cropped.\n";
            break;
        case 13:
            cout << "Applying blur filter...\n";
            int percentage;
            cout << "Enter blur percentage (1-100): ";
            cin >> percentage; 
            img = Blur(img, percentage);
            cout << "\nfilter applied\n";
            break;
        case 14:

            cout << "applaying sunlihght filter....... \n";
            img = sunlight(img);
            cout << "\nfilter applied\n";
            break;
        case 15:

            cout << "applayin tv filter \n";
        tvFilter(img);
        cout << "filter applyed \n";

            break;
        case 16:

            cout << " applying purple effect.....\n";
            img = Purple(img);
            cout << "\nfilter applied\n";
            break;



        case 17:



            break;

        case 18:

            cout << "skewness.... \n";
            img = skew(img);
            cout << "filter applyed \n";
            break;

        case 19:



            break;
        case 20:{

            cout << "Saving image...\n";
            cout << "write image name\n";
            string filename;
            cin.ignore();
            getline(cin, filename);
            if (filename.find(".jpg") == string::npos)
                filename += ".jpg";
            img.saveImage(filename.c_str());


            break;}


        case 21:
        {
            int s;

            cout << "\t 1. save before exit\n";
            cout << "\t 2.discard\n";
            cout << "\n Enter choice: \n";
            cin >> s;
            if (s == 1) {
                cout << "Saving image...\n";
                cout << "write image name\n";
                string filename;
                cin.ignore();
                getline(cin, filename);
                img.saveImage(filename.c_str());
            }
            r = false;
            break;
}

        default:
            cout << "Invalid choice! Please try again.\n";
            break;

        }
     
    } 
    return 0;
}
