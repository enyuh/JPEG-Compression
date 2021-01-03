#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979323846
#include <fcntl.h>

int _setmode (
   int fd,
   int mode
);
void readheader();
void writeheader();
void zigzag(int arr[8][8]);
void inverse_zigzagtable(int arr[8][8]);
void inverse_zigzag_quantization();
void RGBtoYCbCr();
void YCbCrtoRGB();
void YCbCr_DCT_Quantization_zigzag();
void IDCT();
void pre_RLE_DPCM();
void RLE_codebook(int arr[8][8]);
void RLE_DPCM_compression(int arr[8][8]);
void RLE_huffman_encode();
void DPCM_huffman_encode();
void compression_txt();
void RLE_huffman_decode();
void DPCM_huffman_decode();
void bubblesort();
void swap();
void bubblesort1();
void swap1();
int arr_R[2080][1560],arr_G[2080][1560],arr_B[2080][1560];
int arr_Y[2080][1560],arr_Cb[2080][1560],arr_Cr[2080][1560];
int DCT_Y[8][8]={0},DCT_Cb[8][8]={0},DCT_Cr[8][8]={0};
int temp_Y[8][8]={0},temp_Cb[8][8]={0},temp_Cr[8][8]={0};
int temp_RLE[8][8]={0};
int IDCT_Y=0,IDCT_Cb=0,IDCT_Cr=0;
int i,j,u,v,temp,x,q,m,n,r,s,R,G,B,k=1,DPCM_index=0,num=0,queue[100],ch,ch1,ch2,queuelength=0;
float Cu,Cv;
int zigzag_Y[8][8]={0},zigzag_Cb[8][8]={0},zigzag_Cr[8][8]={0};
int Y_Quantization[8][8]={
        {16,11,10,16,24,40,51,61},
        {12,12,14,19,26,58,60,55},
        {14,13,16,24,40,57,69,56},
        {14,17,22,29,51,87,80,62},
        {18,22,37,56,68,109,103,77},
        {24,35,55,64,81,104,113,92},
        {49,64,78,87,103,121,120,101},
        {72,92,95,98,112,100,103,99}
        };
int CbCr_Quantization[8][8]={
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}
    };

 struct {
        unsigned short type;
        unsigned int filesize;/* File size in bytes,*/
        unsigned int reserved;
        unsigned int offset;/* Offset to image data, bytes */
        unsigned int size;/* Info Header size in bytes */
        int width, height;/* Width and height of image */
        unsigned short planes;/* Number of colors planes */
        unsigned short bits_perpixel; /* Bits per pixel */
        unsigned int compression; /* Compression type */
        unsigned int imagesize; /* Image size in bytes */
        int xresolution, yresolution; /* Pixels per meter */
        unsigned int usedcolors; /* Number of colors */
        unsigned int importantcolors; /* Important colors */
    } h;
 struct RR{
     int R;
     int L;
     int count_RL;
     int pos;//record the position when sorting
     int cod[100];//store the sorting sequence of the RLE[i].byte1
     int encode[100];
     int codebyte;//total number of the char's encode
     int codelength;
 } RLE[200000],c[64];
 struct DPCM{
     int count;
     int dif;
     int pos;//record the position when sorting
     int cod[100];//store the sorting sequence of the RLE[i].byte1
     int encode[100];
     int codebyte;
     int codelength;//total number of the char's encode
 } d[152100];
FILE *fin = NULL;    // source file handler
FILE *fout = NULL;
FILE *ftxt = NULL;    // target file handler
FILE *fRLEcodebook = NULL;
FILE *fDPCMcodebook = NULL;
FILE *fRLE_compression = NULL;
FILE *fDPCM_compression = NULL;

int main() {
     _setmode( _fileno( stdin ), _O_BINARY );
    printf("start jpeg compress\n");
    jpeg("little_girl.bmp", "output.bmp");

    return 0;
}

int jpeg(const char *fname_s, const char *fname_t) {
    fin = fopen(fname_s, "rb");
    readheader();
    fout = fopen(fname_t, "wb");
    writeheader();
    fRLEcodebook = fopen("RLEcodebook.txt", "wb");
    fDPCMcodebook = fopen("DPCMcodebook.txt", "wb");
    fRLE_compression = fopen("RLEcompression.txt", "wb");
    fDPCM_compression = fopen("DPCMcompression.txt", "wb");
    // ftxt = fopen("test.txt", "wb");
/*****RGB -> YCbCr*****/
    printf("set RGB to YCbCr\n");
    RGBtoYCbCr();
/*****YCbCr -> DCT -> Quantization -> zigzag*****/
    printf("do DCT & Quantization & zigzag\n");
    YCbCr_DCT_Quantization_zigzag();
/*****zigzag -> DPCM & RLE*****/
    printf("do huffman encode\n");
    pre_RLE_DPCM();
    compression_txt();
/*****DPCM & RLE -> huffman*****/
    RLE_huffman_encode();
    DPCM_huffman_encode();
/*****DPCM & RLE decode*****/
    printf("decode from compression.txt\n");
    RLE_huffman_decode();
    DPCM_huffman_decode();
/*****inverse zigzag -> inverse quantization*****/
    printf("Inverse zigzag & Quantization\n");
    inverse_zigzag_quantization();
/******IDCT*****/
    printf("do IDCT\n");
    IDCT();
/*****YCbCr -> RGB******/
    printf("set YCbCr to RGB\n");
    YCbCrtoRGB();
    printf("Finished.....\n");

       fclose(fin);
       fclose(fout);
      // fclose(ftxt);
       fclose(fRLEcodebook);
       fclose(fDPCMcodebook);
       fclose(fDPCM_compression);
       fclose(fRLE_compression);

    return 0;
}
void readheader()
{
    fread(&h.type, sizeof(short), 1, fin);
    fread(&h.filesize, sizeof(int), 1, fin);
    fread(&h.reserved, sizeof(int), 1, fin);
    fread(&h.offset, sizeof(int), 1, fin);
    fread(&h.size, sizeof(int), 1, fin);
    fread(&h.width, sizeof(int), 1, fin);
    fread(&h.height, sizeof(int), 1, fin);
    fread(&h.planes, sizeof(short), 1, fin);
    fread(&h.bits_perpixel, sizeof(short), 1, fin);
    fread(&h.compression, sizeof(int), 1, fin);
    fread(&h.imagesize, sizeof(int), 1, fin);
    fread(&h.xresolution, sizeof(int), 1, fin);
    fread(&h.yresolution, sizeof(int), 1, fin);
    fread(&h.usedcolors, sizeof(int), 1, fin);
    fread(&h.importantcolors, sizeof(int), 1, fin);
}
void writeheader()
{
    fwrite(&h.type,sizeof(short),1,fout);
    fwrite(&h.filesize,sizeof(int),1,fout);
    fwrite(&h.reserved,sizeof(int),1,fout);
    fwrite(&h.offset,sizeof(int),1,fout);
    fwrite(&h.size,sizeof(int),1,fout);
    fwrite(&h.width,sizeof(int),1,fout);
    fwrite(&h.height,sizeof(int),1,fout);
    fwrite(&h.planes,sizeof(short),1,fout);
    fwrite(&h.bits_perpixel,sizeof(short),1,fout);
    fwrite(&h.compression,sizeof(int),1,fout);
    fwrite(&h.imagesize,sizeof(int),1,fout);
    fwrite(&h.xresolution,sizeof(int),1,fout);
    fwrite(&h.yresolution,sizeof(int),1,fout);
    fwrite(&h.usedcolors,sizeof(int),1,fout);
    fwrite(&h.importantcolors,sizeof(int),1,fout);
}
void zigzag(int arr[8][8])
{
    int i,j;
    int arr_zz[64]={
    arr[0][0],arr[0][1],arr[1][0],arr[2][0],arr[1][1],arr[0][2],arr[0][3],arr[1][2],
    arr[2][1],arr[3][0],arr[4][0],arr[3][1],arr[2][2],arr[1][3],arr[0][4],arr[0][5],
    arr[1][4],arr[2][3],arr[3][2],arr[4][1],arr[5][0],arr[6][0],arr[5][1],arr[4][2],
    arr[3][3],arr[2][4],arr[1][5],arr[0][6],arr[0][7],arr[1][6],arr[2][5],arr[3][4],
    arr[4][3],arr[5][2],arr[6][1],arr[7][0],arr[7][1],arr[6][2],arr[5][3],arr[4][4],
    arr[3][5],arr[2][6],arr[1][7],arr[2][7],arr[3][6],arr[4][5],arr[5][4],arr[6][3],
    arr[7][2],arr[7][3],arr[6][4],arr[5][5],arr[4][6],arr[3][7],arr[4][7],arr[5][6],
    arr[6][5],arr[7][4],arr[7][5],arr[6][6],arr[5][7],arr[6][7],arr[7][6],arr[7][7],
    };
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            arr[i][j]=arr_zz[i*8+j];
        }
    }
}
void inverse_zigzagtable(int arr[8][8])
{
    int i,j;
    int arr_zz[64]={
    arr[0][0],arr[0][1],arr[0][5],arr[0][6],arr[1][6],arr[1][7],arr[3][3],arr[3][4],
    arr[0][2],arr[0][4],arr[0][7],arr[1][5],arr[2][0],arr[3][2],arr[3][5],arr[5][2],
    arr[0][3],arr[1][0],arr[1][4],arr[2][1],arr[3][1],arr[3][6],arr[5][1],arr[5][3],
    arr[1][1],arr[1][3],arr[2][2],arr[3][0],arr[3][7],arr[5][0],arr[5][4],arr[6][5],
    arr[1][2],arr[2][3],arr[2][7],arr[4][0],arr[4][7],arr[5][5],arr[6][4],arr[6][6],
    arr[2][4],arr[2][6],arr[4][1],arr[4][6],arr[5][6],arr[6][3],arr[6][7],arr[7][4],
    arr[2][5],arr[4][2],arr[4][5],arr[5][7],arr[6][2],arr[7][0],arr[7][3],arr[7][5],
    arr[4][3],arr[4][4],arr[6][0],arr[6][1],arr[7][1],arr[7][2],arr[7][6],arr[7][7],
    };
    for(i=0;i<8;i++)
    {
        for(j=0;j<8;j++)
        {
            arr[i][j]=arr_zz[i*8+j];
        }
    }
}
void inverse_zigzag_quantization() //after decode ,we do inverse zigzag & quantization
{
    for(u=0;u<h.height/8;u++)
{
    for(v=0;v<h.width/8;v++)
    {
        for(r=0;r<8;r++)
        {
            for(s=0;s<8;s++)
            {
               temp_Y[r][s]=arr_Y[8*u+r][8*v+s];
               temp_Cb[r][s]=arr_Cb[8*u+r][8*v+s];
               temp_Cr[r][s]=arr_Cr[8*u+r][8*v+s];
            }
        }
        inverse_zigzagtable(temp_Y);
        inverse_zigzagtable(temp_Cb);
        inverse_zigzagtable(temp_Cr);
        for (m=0;m<8;m++)
        {
            for (n=0;n<8;n++)
            {
                 arr_Y[8*u+m][8*v+n]= temp_Y[m][n]*Y_Quantization[m][n];     //use different table to quantization
                 arr_Cb[8*u+m][8*v+n]= temp_Cb[m][n]*CbCr_Quantization[m][n];
                 arr_Cr[8*u+m][8*v+n]= temp_Cr[m][n]*CbCr_Quantization[m][n];
            }
        }
    }
}

}
void RGBtoYCbCr()
{
      for(j=0;j<h.height;j++)
 {
    for(i=0;i<h.width;i++)
     {
        fread(&arr_B[j][i],1,1,fin);
        fread(&arr_G[j][i],1,1,fin);
        fread(&arr_R[j][i],1,1,fin);
        arr_Y[j][i] = 0.299*arr_R[j][i]+0.587*arr_G[j][i]+0.114*arr_B[j][i];
        arr_Cb[j][i] = -0.169*arr_R[j][i]-0.331*arr_G[j][i]+0.499*arr_B[j][i]+128;
        arr_Cr[j][i] =0.499*arr_R[j][i]-0.418*arr_G[j][i]-0.0813*arr_B[j][i]+128;
     }
 }
 for(j=0;j<h.height;j++)  //subtract 128 before doing DCT
 {
    for(i=0;i<h.width;i++)
     {
        arr_Y[j][i]  -=128;
        arr_Cb[j][i] -=128;
        arr_Cr[j][i] -=128;
     }
 }

}
void YCbCrtoRGB()
{

for(j=0;j<h.height;j++)
 {
    for(i=0;i<h.width;i++)  //add 128 before IDCT
     {
           arr_Y[j][i]+=128;
           arr_Cb[j][i]+=128;
           arr_Cr[j][i]+=128;
     }
 }
for(j=0;j<h.height;j++)
 {
    for(i=0;i<h.width;i++)
     {
        arr_R[j][i]=(arr_Y[j][i])+1.402*(arr_Cr[j][i]-128);
        arr_G[j][i]=(arr_Y[j][i])-0.344*(arr_Cb[j][i]-128)-0.714*(arr_Cr[j][i]-128);
        arr_B[j][i]=(arr_Y[j][i])+1.772*(arr_Cb[j][i]-128);
        fwrite(&arr_B[j][i],1,1,fout);
        fwrite(&arr_G[j][i],1,1,fout);
        fwrite(&arr_R[j][i],1,1,fout);
     }
 }
}
void YCbCr_DCT_Quantization_zigzag()
{
        for (u=0;u<h.height/8;u++) //display oppositely position to create image
        {
           for (v=0;v<h.width/8;v++)
          {
             for(x=0;x<8;x++)//return to 0
              {
                for(q=0;q<8;q++)
               {
               DCT_Y[x][q]=0;
               DCT_Cb[x][q]=0;
               DCT_Cr[x][q]=0;
               }
              }
            for (m=0;m<8;m++) //display oppositely position to create image
            {
              for (n=0;n<8;n++)
              {
               if (m == 0)
                Cu = 1.0 / sqrt(2.0);
               else
                Cu = 1.0;
               if (n == 0)
                Cv = 1.0 / sqrt(2.0);
               else
                Cv = (1.0);
              for (r=0;r<8;r++)  //doing DCT
              {
                for (s=0;s<8;s++)
                {
                    DCT_Y[m][n]+=(Cu*Cv)/4*(arr_Y[8*u+r][8*v+s])*cos((2*r+1)*m*PI/16)*cos((2*s+1)*n*PI/16);
                    DCT_Cb[m][n]+=(Cu*Cv)/4*(arr_Cb[8*u+r][8*v+s])*cos((2*r+1)*m*PI/16)*cos((2*s+1)*n*PI/16);
                    DCT_Cr[m][n]+=(Cu*Cv)/4*(arr_Cr[8*u+r][8*v+s])*cos((2*r+1)*m*PI/16)*cos((2*s+1)*n*PI/16);
                }
               }
               temp_Y[m][n]=DCT_Y[m][n]/Y_Quantization[m][n];   //quantization after DCT
               temp_Cb[m][n]=DCT_Cb[m][n]/CbCr_Quantization[m][n];
               temp_Cr[m][n]=DCT_Cr[m][n]/CbCr_Quantization[m][n];
              }
            }
          zigzag(temp_Y);  //zigzag
          zigzag(temp_Cb);
          zigzag(temp_Cr);
          for(i=0;i<8;i++)
           {
             for(j=0;j<8;j++)
              {
                    arr_Y[8*u+i][8*v+j] = temp_Y[i][j];
                    arr_Cb[8*u+i][8*v+j] = temp_Cb[i][j];
                    arr_Cr[8*u+i][8*v+j] = temp_Cr[i][j];
              }
           }

          }
        }
}
void IDCT()
{
     for (u=0;u<h.height/8;u++) //display oppositely position to create image
        {
           for (v=0;v<h.width/8;v++)
          {
              for(x=0;x<8;x++)//return to 0
              {
                for(q=0;q<8;q++)
               {
               DCT_Y[x][q]=0;
               DCT_Cb[x][q]=0;
               DCT_Cr[x][q]=0;
               }
              }
            for (r=0;r<8;r++) //display oppositely position to create image
            {
              for (s=0;s<8;s++)
              {

              for (m=0;m<8;m++)
              {
                for (n=0;n<8;n++)
                {
                    if (m == 0)
                    Cu = 1.0 / sqrt(2.0);
                   else
                    Cu = 1.0;
                    if (n == 0)
                    Cv = 1.0 / sqrt(2.0);
                   else
                    Cv = (1.0);
                    DCT_Y[r][s]+=(Cu*Cv)/4*arr_Y[8*u+m][8*v+n]*cos((2*r+1)*m*PI/16)*cos((2*s+1)*n*PI/16);
                    DCT_Cb[r][s]+=(Cu*Cv)/4*arr_Cb[8*u+m][8*v+n]*cos((2*r+1)*m*PI/16)*cos((2*s+1)*n*PI/16);
                    DCT_Cr[r][s]+=(Cu*Cv)/4*arr_Cr[8*u+m][8*v+n]*cos((2*r+1)*m*PI/16)*cos((2*s+1)*n*PI/16);
                }
               }
              }
            }
             for(i=0;i<8;i++)
               {
                   for(j=0;j<8;j++)
                   {
                      arr_Y[8*u+i][8*v+j]= DCT_Y[i][j];
                      arr_Cb[8*u+i][8*v+j]= DCT_Cb[i][j];
                      arr_Cr[8*u+i][8*v+j]= DCT_Cr[i][j];
                   }
               }
          }
        }
}
void pre_RLE_DPCM() //
{
    int DPCM_temp,DPCM_first=0,DPCM_k=0;
    RLE[0].R=15;
    RLE[0].L=0;
    RLE[0].count_RL=0;
    RLE[1].R=99;
    RLE[1].L=99;
    RLE[1].count_RL=0;
    for(i=2;i<200000;i++)
    RLE[i].R=0;
  for(u=0;u<h.height/8;u++)
    {
        for(v=0;v<h.width/8;v++)
        {
            for(r=0;r<8;r++)
            {
                for(s=0;s<8;s++)
                {
                     temp_RLE[0][0]=0;
                    if(r!=0||s!=0)    //the part of AC
                    temp_RLE[r][s]=arr_Y[u*8+r][v*8+s];
                   else if(r==0&&s==0) //the part of DC
                   {

                        if(DPCM_first==0)  //assign variable to first DC
                           {
                             DPCM_temp=arr_Y[8*u+r][8*v+s];
                             d[DPCM_index].dif=arr_Y[8*u+r][8*v+s];
                             d[DPCM_index].count++;
                             DPCM_first++;
                             DPCM_index++;
                           }
                        else if(DPCM_first!=0)
                           {
                              d[DPCM_index].dif=arr_Y[8*u+r][8*v+s]-DPCM_temp;
                              for(i=0;i<DPCM_index;i++)
                              {
                                  if(d[DPCM_index].dif==d[i].dif)
                                  {
                                      d[i].count++;
                                      DPCM_temp=arr_Y[8*u+r][8*v+s];
                                      d[DPCM_index].dif=99;
                                      d[DPCM_index].count=0;
                                      DPCM_k=1;
                                      break;
                                  }
                                  else
                                  continue;

                              }
                                if(DPCM_k==0)
                                  {
                                    d[DPCM_index].count++;
                                    //d[DPCM_index].dif=arr_Y[8*u+r][8*v+s]-DPCM_index;
                                    DPCM_temp=arr_Y[8*u+r][8*v+s];
                                    DPCM_index++;
                                  }
                                else if(DPCM_k==1)
                                  {
                                      DPCM_k=0;
                                  }
                           }
                   }
                 }
            }
            RLE_codebook(temp_RLE);
            }
    }


}
void RLE_codebook(int arr[8][8])
{
    int EOB=0;
    int same=0,a;
    for(i=0;i<8;i++)
    {
         for(j=0;j<8;j++)
         {
             if(i==0&&j==0) //skip if it's DC
                continue;
            if(i==0&&j==1)
            {
                k++;
                if(arr[i][j]==0)  //count R
                RLE[k].R++;
                else
                {
                    RLE[k].R=0;
                    RLE[k].L=arr[i][j];
                }
                for(a=0;a<k;a++)  //compare whether there is the same value before
                {
                    if(RLE[a].L==RLE[k].L&&RLE[a].R==RLE[k].R)
                    {
                        RLE[a].count_RL++;
                        same=1;
                        break;
                    }
                    else
                        continue;
                }
                if(same==1)
                {
                    same=0;
                }
                else
                {
                RLE[k].count_RL=1;
                k++;
                }
            }
                else
                {
                     if(arr[i][j]==0)
                    {
                        RLE[k].R++;//cont++
                        if(RLE[k].R>=16)
                        {
                           RLE[0].count_RL++;
                            EOB++;
                            RLE[k].R=0;
                        }
                        else
                            continue;
                    }
                    else
                    {
                        RLE[k].L=arr[i][j];
                        EOB=0;
                        for(a=0;a<k;a++)
                        {
                            if(RLE[a].L==RLE[k].L&&RLE[a].R==RLE[k].R)
                            {
                                RLE[a].count_RL++;
                                same=1;
                                break;
                            }
                            else
                            {
                                same=0;
                            }
                        }
                        if(same==1)
                        {
                            same =0 ;
                        }
                        else
                        {
                            RLE[k].count_RL=1;
                            k++;
                        }
                        //new symbol
                        RLE[k].R=0;
                    }
                }
         }
    }
    if(EOB!=0)
    {
        RLE[0].count_RL-=EOB;
        RLE[1].count_RL++;
        RLE[k].R=0;
        k--;
    }
    else
    {
        RLE[k].count_RL=1;
        RLE[k].L=0;
        for(a=0;a<k;a++)
        {
        if(RLE[a].L==RLE[k].L&&RLE[a].R==RLE[k].R)
        {
            RLE[a].count_RL++;
            same=1;
            break;
        }
        else
        {
            same=0;
        }
        }
        if(same==1)
        {
            RLE[k].R=0;
            k--;
            same=0;
        }

    }
}
void RLE_huffman_encode()
{
    int symbol[k],code[5000];  //symbol[] is used to sorting, code[] is used to store the sorting sequence
    int l,getbyte,total=0,index=0,weight,zero=0,biggest; //biggest is for the last position of the symbol when sorting
    for(i=0;i<=k;i++) //let the element be -1a
 {

     for(j=0;j<=100;j++)
     {
         RLE[i].cod[j]=-1;
         RLE[i].encode[j]=-1;

     }
 }
  bubblesort(RLE,0,k+1); //sorting d.count_RL

//give the position to the symbol[]
   for(i=0;i<=k;i++)
    {
     if(RLE[i].count_RL!=0)
     {
      symbol[num]=RLE[i].count_RL;
      num++;
     }
    }
//printf("\n num:%d   k:%d",num,k);
//give the initial position of the RLE[i].byte1
  for(i=0;i<num;i++)
    {
       RLE[i].pos=i;
    }

    for(i=0;i<num-1;i++)  //get the sorting sequence
    {

      weight=symbol[0]+symbol[1];

      for(j=2;j<num-i;j++)
              {
                  if(weight>symbol[j])
                  {
                     code[i]++;
                  }
                  else
                    break;

              }

      if(weight>symbol[2])
       {
                  biggest=symbol[num-i-1];

                  if(weight> biggest)
                  {
                      for(q=0;q<(num-i-2);q++)
                      {
                          symbol[q]=symbol[q+2];
                      }
                      symbol[num-i-2]=weight;
                  }
                  else if(weight<= biggest)
                  {
                      for(q=0;q<code[i];q++)
                      {
                          symbol[q]=symbol[q+2];
                      }
                      for(q=code[i]+1;q<num-1;q++)
                      {
                          symbol[q]=symbol[q+1];
                      }
                      symbol[code[i]]=weight;
                  }

       }

      else if(weight<=symbol[2])
       {
           code[i]=0;
           symbol[0]=weight;
           for(q=1;q<num;q++)
           {
               symbol[q]=symbol[q+1];
           }
       }

      symbol[num-i-1]=0;

    }


    //use sorting sequence to create the codebook
   for(i=0;i<num;i++)
    {
        l=0;
      for(j=0;j<num-1;j++)
      {

        if(RLE[i].pos==0)
        {
        RLE[i].cod[l]=0;
        RLE[i].pos=code[j];
        l++;
        }
        else if(RLE[i].pos==1)
        {
        RLE[i].cod[l]=1;
        RLE[i].pos=code[j];
        l++;
        }
        else if(RLE[i].pos>code[j]+1)
        {
        RLE[i].pos=RLE[i].pos-1;
        }
        else if(RLE[i].pos<=code[j]+1)
        {
        RLE[i].pos=RLE[i].pos-2;
        }
      }
      RLE[i].pos=0;

    }

//store the pass which is 0 or 1
for(i=k;i>=0;i--)
{
    if(RLE[i].count_RL!=0)
    {
     l=0;
     for(j=100;j>=0;j--)
     {
        if(RLE[i].cod[j]==1||RLE[i].cod[j]==0)
        {
          RLE[i].encode[l]=RLE[i].cod[j];
          l++;
        }
     }
     for(q=0;q<=100;q++)
     {
         if(RLE[i].encode[q]!=-1)
         {
             RLE[i].codebyte++;
         }
     }
    }
}

//using bubble sort to let the lowest byte display first

  for(j=0;j<k;j++)
{
     for(i=0;i<k-j-1;i++)
     {
        if(RLE[i+1].codebyte==RLE[i].codebyte&&RLE[i].codebyte!=0)
         {
            if(RLE[i+1].L>RLE[i].L)
            {
                swap(&RLE[i],&RLE[i+1]);
            }
         }
     }
}


   //output the codebook
 fprintf(fRLEcodebook," ");
for(i=k;i>=0;i--)
{

    fprintf(fRLEcodebook,"%d,%d ",i,RLE[i].R,RLE[i].L);
    for(j=RLE[i].codebyte-1;j>=0;j--)
    {
       fprintf(fRLEcodebook,"%d",RLE[i].encode[j]);
    }
    fprintf(fRLEcodebook,"\r\n ");

}
}
void DPCM_huffman_encode()
{
    int symbol[DPCM_index],code[5000];  //symbol[] is used to sorting, code[] is used to store the sorting sequence
    int l,getbyte,total=0,index=0,num1=0,weight,zero=0,biggest; //biggest is for the last position of the symbol when sorting
    for(i=0;i<DPCM_index;i++) //let the element be -1a
 {

     for(j=0;j<100;j++)
     {
         d[i].cod[j]=-1;
         d[i].encode[j]=-1;

     }
 }

//give the position to the symbol[]




   for(i=0;i<=DPCM_index;i++)
    {
     if(d[i].count!=0)
     {
      symbol[num1]=d[i].count;
      num1++;
     }
    }
    bubblesort1(d,0,num1); //sorting d.count_RL
   // printf("\nDPCM num1:%d   DPCM_index:%d",num1,DPCM_index);
     /*for(u=0;u<h.height/8*h.width/8;u++)
    {

            fprintf(ftxt,"%d   count:%d\r\n",u,d[u].dif,d[u].count);

    }*/

//give the initial position of the d[i].byte1
  for(i=0;i<num1;i++)
    {
       d[i].pos=i;
    }

    for(i=0;i<num1;i++)  //get the sorting sequence
    {

      weight=symbol[0]+symbol[1];

      for(j=2;j<=num1-i;j++)
              {
                  if(weight>symbol[j])
                  {
                     code[i]++;
                  }
                  else
                    break;
              }
      if(weight>symbol[2])
       {
                  biggest=symbol[num1-i-1];

                  if(weight> biggest)
                  {
                      for(q=0;q<(num1-i-2);q++)
                      {
                          symbol[q]=symbol[q+2];
                      }
                      symbol[num1-i-2]=weight;
                  }
                  else if(weight<= biggest)
                  {
                      for(q=0;q<code[i];q++)
                      {
                          symbol[q]=symbol[q+2];
                      }
                      for(q=code[i]+1;q<num1-1;q++)
                      {
                          symbol[q]=symbol[q+1];
                      }
                      symbol[code[i]]=weight;
                  }
       }
      else if(weight<=symbol[2])
       {
           code[i]=0;
           symbol[0]=weight;
           for(q=1;q<num1;q++)
           {
               symbol[q]=symbol[q+1];
           }
       }
      symbol[num1-i-1]=0;
    }
    //use sorting sequence to create the codebook
   for(i=0;i<num1;i++)
    {
        l=0;
      for(j=0;j<num1-1;j++)
      {
        if(d[i].pos==0)
        {
        d[i].cod[l]=0;
        d[i].pos=code[j];
        l++;
        }
        else if(d[i].pos==1)
        {
        d[i].cod[l]=1;
        d[i].pos=code[j];
        l++;
        }
        else if(d[i].pos>code[j]+1)
        {
        d[i].pos=d[i].pos-1;
        }
        else if(d[i].pos<=code[j]+1)
        {
        d[i].pos=d[i].pos-2;
        }
      }
      d[i].pos=0;
    }
//store the pass which is 0 or 1
for(i=DPCM_index;i>=0;i--)
{
    if(d[i].count!=0)
    {
     l=0;
     for(j=100;j>=0;j--)
     {
        if(d[i].cod[j]==1||d[i].cod[j]==0)
        {
          d[i].encode[l]=d[i].cod[j];
          l++;
        }
     }
     for(q=0;q<100;q++)
     {
         if(d[i].encode[q]!=-1)
         {
             d[i].codebyte++;
         }
     }
    }
}
//using bubble sort to let the lowest byte display first
 /* for(j=0;j<DPCM_index;j++)
{
     for(i=0;i<DPCM_index-j-1;i++)
     {
        if(d[i+1].codebyte==d[i].codebyte&&d[i].codebyte!=0)
         {
            if(d[i+1].count>d[i].count)
            {
                swap1(&d[i],&d[i+1]);
            }
         }
     }
}*/
   //output the codebook
   fprintf(fDPCMcodebook," ");
for(i=DPCM_index-1;i>=0;i--)
{
    fprintf(fDPCMcodebook,"%d ",d[i].dif);
      //fprintf(fDPCMcodebook,"  count:%d ",d[i].count);
    for(j=d[i].codebyte-1;j>=0;j--)
    {
       fprintf(fDPCMcodebook,"%d",d[i].cod[j]);
    }

    fprintf(fDPCMcodebook,"\r\n ");

}
}
void compression_txt()
{
      for(u=0;u<h.height/8;u++)
    {
        for(v=0;v<h.width/8;v++)
        {
            for(r=0;r<8;r++)
            {
                for(s=0;s<8;s++)
                {
                    if(r!=0||s!=0)
                    temp_RLE[r][s]=arr_Y[u*8+r][v*8+s];

                 }
            }
            RLE_DPCM_compression(temp_RLE);

        }
    }


}
void RLE_DPCM_compression(int arr[8][8])
{

    int EOB=0;
    int same=0,a;
    q=0;

    for(i=0;i<8;i++)
    {
         for(j=0;j<8;j++)
         {
             if(i==0&&j==0)  //do DPCM (other parts is for RLE )
                continue;
            if(i==0&&j==1)
            {

                if(arr[i][j]==0)
                c[q].R++;
                else
                {
                    c[q].R=0;
                    c[q].L=arr[i][j];
                    q++;
                }

            }
                else
                {
                     if(arr[i][j]==0)
                    {
                        c[q].R++;//cont++
                        if(c[q].R==16)
                        {
                            c[q].R--;
                            EOB++;
                            q++;
                            c[q].R=1;
                            c[q].L=0;
                        }
                        else
                            continue;
                    }
                    else
                    {
                        c[q].L=arr[i][j];
                        EOB=0;
                        q++;
                    }
                }
         }
    }
    if(EOB!=0)
    {
       for(i=q;i>q-EOB;i--)
       {
           c[i].R=0;
           c[i].L=0;
       }
       c[q-EOB].R=99;
       c[q-EOB].L=99;
    }
    else
    {
        c[q].L=0;
    }

    //compress

    for(i=0;i<=q;i++)
    {
        for(j=0;j<num;j++)
        {
            if(c[i].R==RLE[j].R&&c[i].L==RLE[j].L)
            {
                break;
            }
            else
                continue;
        }
        for(m=RLE[j].codebyte;m>=0;m--)
        {
            queue[queuelength]=RLE[m].encode[j];
            queuelength++;
        }
        if(queuelength>=8)
        {
            ch=0;
            for(n=0;n<8;n++)
            {
              ch+=(queue[n]-48)*pow(2,(7-n));
            ch1+=2;
            ch2++;
            }
            fprintf(fRLE_compression,"%c",ch1);
            ch2+=2;
            fprintf(fDPCM_compression,"%c",ch1);
            fprintf(fDPCM_compression,"%c",ch2);

                    if(queuelength>=8)
                    {
                        for(m=0; m<16; m++) //choosing 16 is because the waiting queue won't greater than 16
                            queue[m]=queue[m+8];
                    }
                    queuelength-=8;
        }

    }

            for(i=0; i<m; i++) //display the last elements in queue
            {
                ch1++;
                    fprintf(fRLE_compression,"%c",ch1);
                     fprintf(fRLE_compression,"%c",ch2);
            }


}
void RLE_huffman_decode()
{
    int getbyte,b1,total=0,index=0,num,cod[8]={-1},ch,que[40]={-1},cou,e;
  m=0;r=0;k=0;
  if(getbyte=fgetc(fRLE_compression))
    {
        ch=getbyte;
         for(i=0;i<8;i++) //change the char back to 0 or 1
         {
             cod[7-i]=ch%2;
             ch/=2;
         }
         m+=8;
         for(j=0;j<8;j++) //put 0 or 1 into que[] and waiting for working
         {
             que[k]=cod[j];
             k++;
         }
     while(m>=24) // if elements in que[] more than 24 then ready to printf
    {
        for(i=0;i<num;i++)
        {
        	cou=0;
        	for(j=0;j<RLE[i].codelength;j++)
          {
            if(que[j]==(RLE[i].encode[j]-48))
             cou++;

          }
          if(cou==RLE[i].codelength)
          {
        	//fprintf(fout,"%c",RLE[i].byte1);
        	break;
		  }
        }
        for(j=0;j<k-RLE[i].codelength;j++)//shift workarr ntimes left
        {
            que[j]=que[j+RLE[i].codelength];
        }
        k-=RLE[i].codelength;//because que[] have been printf out , subtract 1byte
        m-=RLE[i].codelength;
    }
 }
 for(r=0;r<3;r++) //because the last element in que won't be more than 3
        {
           for(i=0;i<num;i++)
        {
        	cou=0;
        	for(j=0;j<RLE[i].codelength;j++)
          {
            if(que[j]==(RLE[i].encode[j]-48))
             cou++;
          }
          if(cou==RLE[i].codelength)
          {
        	break;
		  }
        }
        for(j=0;j<k-RLE[i].codelength;j++)//shift the que
        {
            que[j]=que[j+RLE[i].codelength];
        }
        k-=RLE[i].codelength;//because que[] have been printf out , subtract 1byte
        m-=RLE[i].codelength;
        if(RLE[i].L==10)
            break;
        }

}
void DPCM_huffman_decode()
{
     int getbyte,b1,total=0,index=0,num,cod[8]={-1},ch,que[40]={-1},cou,e;
  m=0;r=0;k=0;
  if((getbyte=fgetc(fDPCM_compression))!=EOF)
    {
        ch=getbyte;
         for(i=0;i<8;i++) //change the char back to 0 or 1
         {
             cod[7-i]=ch%2;
             ch/=2;
         }
         m+=8;
         for(j=0;j<8;j++) //put 0 or 1 into que[] and waiting for working
         {
             que[k]=cod[j];
             k++;
         }
     while(m>=24) // if elements in que[] more than 24 then ready to printf
    {
        for(i=0;i<num;i++)
        {
        	cou=0;
        	for(j=0;j<d[i].codelength;j++)
          {
            if(que[j]==(d[i].encode[j]-48))
             cou++;

          }
          if(cou==d[i].codelength)
          {
        	//fprintf(fout,"%c",d[i].byte1);
        	break;
		  }
        }
        for(j=0;j<k-d[i].codelength;j++)//shift workarr ntimes left
        {
            que[j]=que[j+d[i].codelength];
        }
        k-=d[i].codelength;//because que[] have been printf out , subtract 1byte
        m-=d[i].codelength;
    }
 }
 for(r=0;r<3;r++) //because the last element in que won't be more than 3
        {
           for(i=0;i<num;i++)
        {
        	cou=0;
        	for(j=0;j<d[i].codelength;j++)
          {
            if(que[j]==(d[i].encode[j]-48))
             cou++;
          }
          if(cou==d[i].codelength)
          {
        	break;
		  }
        }
        for(j=0;j<k-d[i].codelength;j++)//shift the que
        {
            que[j]=que[j+d[i].codelength];
        }
        k-=d[i].codelength;//because que[] have been printf out , subtract 1byte
        m-=d[i].codelength;
        if(d[i].dif==10)
            break;
        }

}
void bubblesort(struct RR *RLE,int head,int tail)
{
     for(j=head;j<tail;j++)
    {
     for(i=head;i<tail-j-1;i++)
     {
        if(RLE[i+1].count_RL<RLE[i].count_RL)
         {
          swap(&RLE[i],&RLE[i+1]);
         }
     }
    }
}
void bubblesort1(struct DPCM *d,int head,int tail)
{
     for(j=head;j<tail;j++)
    {
     for(i=head;i<tail-j-1;i++)
     {
        if(d[i+1].count<d[i].count)
         {
          swap1(&d[i],&d[i+1]);
         }
     }
    }
}
void swap(struct RR *x,struct RR *y)//¥æ´«struct¦ì§}
{
struct RR RLE=*x;
*x=*y;
*y=RLE;
}
void swap1(struct DPCM *x,struct DPCM *y)//¥æ´«struct¦ì§}
{
struct DPCM d=*x;
*x=*y;
*y=d;
}

