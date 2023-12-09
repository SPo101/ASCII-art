#include <stdio.h>
#include <stdlib.h>


struct BITMAP_header{
    char name[2];
    unsigned int size;
    unsigned int trash;
    unsigned int image_offset;
};

struct DIB_header{
    unsigned int header_size;
    unsigned int width;
    unsigned int height;

    unsigned short color_planes;
    unsigned short bit_per_pixel;
    unsigned int compression_method;
    unsigned int data_size;
    unsigned int pwidth;
    unsigned int pheight;
    unsigned int colors_count;
    unsigned int imp_colors_count;
};

struct Trash{
    unsigned int start;
    char trash[512];
};

struct RGB{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct Image{
    int height;
    int width;
    struct RGB **rgb;
};

struct Image readImage(FILE *fp, int height, int width){
    struct Image pic;
    pic.rgb = (struct RGB**) malloc(height*sizeof(void*));
    pic.height = height;
    pic.width = width;
    int bytestored = ((24 * width + 31)/32)*4;
    int numOfrgb = bytestored/sizeof(struct RGB) + 1;

    for(int i = height - 1; i >= 0; i--){
        pic.rgb[i] = (struct RGB*) malloc(numOfrgb*sizeof(struct RGB));
        fread(pic.rgb[i], 1, bytestored, fp);
    }
    return pic;
}

void freeImage(struct Image pic){
    for(int i = 0; i < pic.height; i++)
        free(pic.rgb[i]);
    free(pic.rgb);
}

unsigned char grayscale(struct RGB rgb){
    return (rgb.red + rgb.blue + rgb.green)/3;
}

void ImageToText(struct Image pic){
    //0-31, 32-63, 64-95, 96-127, ...
    char textpixel[] = {'@', '%', 'g', 'a', 'O', '-', '.', ' '};

    printf("\n");
    for(int i = 0; i < pic.height; i++){
        for(int j = 0; j < pic.width; j++){
            unsigned char gs = grayscale(pic.rgb[i][j]);
            printf("%c%c%c", textpixel[7-gs/32], textpixel[7-gs/32], textpixel[7-gs/32]);
        }
        printf("\n");
    }
}

void openbmpfile(char name[]){
    FILE *fp = fopen(name, "rb");
   
    struct BITMAP_header header;
    struct DIB_header dibHeader;
    struct Trash Trash;

    fread(header.name, 2, 1, fp);
    fread(&header.size, sizeof(int), 3 , fp);

    fread(&dibHeader.header_size, sizeof(int),1 , fp);
    fread(&dibHeader.width, 9*sizeof(int) ,1 , fp);
    fread(&Trash.start, dibHeader.header_size - 10*sizeof(int), 1, fp);



    
    printf("File type: %c%c\n", header.name[0], header.name[1]);
    printf("Size: %d\n", header.size);
    printf("Offset: %d\n\n", header.image_offset);
     
    printf("head size: %d\n", dibHeader.header_size);
  
    printf("Width: %d\n", dibHeader.width);
    printf("Height: %d\n", dibHeader.height);
    printf("Bits per pixel: %d\n", dibHeader.bit_per_pixel);
    printf("Compression method: %d\n", dibHeader.compression_method);
    printf("Image size: %d", dibHeader.data_size);
    
    //fseek//for indentation
    
   
    fseek(fp, header.image_offset, SEEK_SET);
    struct Image image = readImage(fp, dibHeader.height, dibHeader.width);
    ImageToText(image);
    


    fclose(fp);
    freeImage(image);
    
}



int main(int argc, char* argv[]){
    
    if (argc < 2){
        printf("Lack of param");
        return 0;
    };
    


    printf("sizeof BITMAP_header: %lu\n", sizeof(struct BITMAP_header));
    printf("sizeof DIB_header: %lu\n\n", sizeof(struct DIB_header));

    openbmpfile(argv[1]);

    return 0;
}




