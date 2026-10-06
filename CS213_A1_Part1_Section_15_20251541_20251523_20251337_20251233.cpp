//====================================================================================
//======================================Part 1========================================
//Mohamed Tamer : 20251523 |||| filter: 3, 7
//Mohamed Assem:20251337  ||| filter: 2, 6
//Abdalrahman Mohamed Wahba: 20251541 |||| filter: 4, 8
//Abdlruhman Mohamed: 20251233 |||| filter: 1, 5
//section: S15
//===================================================================================
//===================================================================================
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
    char flip;
    cout << "if you want Horizontal Flip enter H and if veritcal enter V \n";
    cout << "Your choice: ";
    cin >> flip;
    if (flip=='H') {
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

Image CropImage(Image img){









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
    int width,height;
    cout<<"write dimensions\n";
        cout<< "\nwidth: ";
        cin>>width;
        cout <<"\nheight: ";
        cin>>height;
        img = resize(img,width ,height); 
    
    
}


else if (n == 10) {
   
   
    
    
}
else if (n == 11) {
    
   
    
}
else if (n == 12) {
    
    
}
else if (n == 13) {
    
  
    
}
else if (n == 14) {
    
    
    
}
else if (n == 15) {
    
   
    
}
else if (n == 16) {

    cout<<" applying purple effect.....\n";
    img = Purple(img);
   
    
}

else if (n == 17) {
    
   
}
else if (n == 18) {
    
  
}

else if (n == 19) {
    
 
    
}
else if (n == 20) {
    
    cout << "Saving image...\n";
    cout << "write image name\n";
    string filename;
    cin >> filename;
    img.saveImage(filename.c_str());

    
}


else if (n == 21) {
    
    cout << "Exiting program. Goodbye!\n";
    break;
    
}


else {
    cout << "Invalid choice! Please try again.\n";
}

}
return 0;
}
