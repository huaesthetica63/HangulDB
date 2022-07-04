//Resizing each dataset-image to 50x50 pixels format with inversing values (255->0, 0->255)
//It may be useful for creating your own neural network with Hangul-dataset
#include <opencv2/opencv.hpp>//I use opencv for image processing
#define SQUARE_SIZE 50 //size of image sample in my dataset is 50x50 pixels
#define CHARACTERS_COUNT 2350 //There are 2350 different hangul characters in dataset
#include <stdio.h>
#include <iostream>
#include <vector>
#define TRUE 1
#define FALSE 0
typedef struct {
    char m_code[3];
    unsigned char m_width, m_height;
    unsigned char m_type;            // 0: unsigned char image, 1: float image
    unsigned char m_reserved;

    unsigned char m_data[100][100];
} Image;
int ReadHGU1(FILE *fp, Image *pImage);
int WriteHGU1(FILE *fp, Image *pImage);
void DisplayImage(Image *pImage);
void LogImage(Image *pImage, char *fileName);
using namespace std;
int main()
{
    char header[9] = { 0 };
    int countFiles=CHARACTERS_COUNT;
    FILE *files = fopen("files.txt","rb");//I use files.txt that contains
    //list of filenames from train path those I need in my project.
    for(int indFile=0;indFile<countFiles;indFile++){
        char*curfilename = new char[100];
        fscanf(files, "%s",curfilename);
        char*path =new char[100];
        strcat(path,"PE92_train//");//open original file from train directory
        strcat(path,curfilename);
        FILE *fp = fopen(path, "rb");
        path =new char[100];//create new file with prefix "New-" that contains processed image from original
        strcat(path,"PE92_train//New");
        strcat(path,curfilename);
        FILE *fout = fopen(path, "wb");
        fread(header, 1, 8, fp);
        fwrite(header, 1, 8, fout);//copy header from original
        Image image;//original image
        while(ReadHGU1(fp, &image)){
            int n = image.m_height;//get size
            int m = image.m_width;
            cv::Mat orig = cv::Mat::zeros(n, m, CV_8U);//create opencv-matrix that contains
            //pixels of 256 grayscale image
            for(int i=0;i<n;i++){
                for(int j=0;j<m;j++){
                    orig.at<uchar>(i,j) = image.m_data[i][j];//copy pixel matrix
                }
            }
            cv::Mat resized = cv::Mat::zeros(SQUARE_SIZE, SQUARE_SIZE, CV_8U);
            //create resized image with size 50*50 pixels for my neural network
            cv::resize(orig, resized, cv::Size(SQUARE_SIZE,SQUARE_SIZE),cv::INTER_AREA);
            //I use inter_area interpolation because resized is smaller than original
            Image newimage=image;//writing result
            newimage.m_height=50;
            newimage.m_width=50;
            for(int i=0;i<50;i++){
                for(int j=0;j<50;j++){
                    newimage.m_data[i][j] = 255-resized.at<uchar>(i,j);
                    //for my neural network I decided to use inversed pixels
                    //white (255) pixels will be black (0)
                    //black (0) pixels will be white (255)
                }
            }
            WriteHGU1(fout, &newimage);//write new format of image in new file
        }
        fclose(fp);
    }
    system("pause");
    return 0;
}
//original functions from DisplayHGU1.cpp aren't changed
void DisplayImage(Image *pImage)
{
    printf("code = [%s], width = %d, height = %d, type = %d, reserved = %d\n", pImage->m_code, pImage->m_width, pImage->m_height, pImage->m_type, pImage->m_reserved);

    for(int y = 0; y < pImage->m_height; y++){
        for(int x = 0; x < pImage->m_width; x++)
            printf("%3d ", pImage->m_data[y][x]);
        printf("\n");
    }
    
    printf("\n");
}

void LogImage(Image *pImage, char *fileName)
{
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL){
        printf("Failed to open %s to write.\n", fileName);
        return;
    }

    fprintf(fp, "code = [%s], width = %d, height = %d, type = %d, reserved = %d\n", pImage->m_code, pImage->m_width, pImage->m_height, pImage->m_type, pImage->m_reserved);

    for (int y = 0; y < pImage->m_height; y++){
        for (int x = 0; x < pImage->m_width; x++)
            fprintf(fp, "%3d,", pImage->m_data[y][x]);
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");

    fclose(fp);
}

int ReadHGU1(FILE *fp, Image *pImage)
{
    int ret = fread(pImage->m_code, 1, 2, fp);
    if(ret < 2)
        return FALSE;
        
    pImage->m_code[2] = 0;
    fread(&pImage->m_width, 1, 1, fp);
    fread(&pImage->m_height, 1, 1, fp);
    fread(&pImage->m_type, 1, 1, fp);
    fread(&pImage->m_reserved, 1, 1, fp);

    for(int y = 0; y < pImage->m_height; y++)
        fread(pImage->m_data[y], 1, pImage->m_width, fp);

    return TRUE;
}

int WriteHGU1(FILE *fp, Image *pImage)
{
    fwrite(pImage->m_code, 1, 2, fp);
    fwrite(&pImage->m_width, 1, 1, fp);
    fwrite(&pImage->m_height, 1, 1, fp);
    fwrite(&pImage->m_type, 1, 1, fp);
    fwrite(&pImage->m_reserved, 1, 1, fp);

    for(int y = 0; y < pImage->m_height; y++)
        fwrite(pImage->m_data[y], 1, pImage->m_width, fp);

    return TRUE;
}


