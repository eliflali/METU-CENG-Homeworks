/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
/*
 * Please fill in the following team_t struct
 */
team_t team = {
};


/********************
 * CONVOLUTION KERNEL
 ********************/

/***************************************************************
 * Your different versions of the convolution functions  go here
 ***************************************************************/

/*
 * naive_conv - The naive baseline version of convolution
 */
char naive_conv_descr[] = "naive_conv: Naive baseline implementation";
void naive_conv(int dim, pixel *src, pixel *ker, unsigned *dst) {
    int i,j,k,l;

    for(i = 0; i < dim-8+1; i++)
        for(j = 0; j < dim-8+1; j++) {
            dst[RIDX(i, j, dim)] = 0;
            for(k = 0; k < 8; k++)
            {
                for(l = 0; l < 8; l++) {
                    dst[RIDX(i, j, dim)] += src[RIDX((i+k),(j+l), dim)].red * ker[RIDX(k, l, 8)].red;
                    dst[RIDX(i, j, dim)] += src[RIDX((i+k),(j+l), dim)].green * ker[RIDX(k, l, 8)].green;
                    dst[RIDX(i, j, dim)] += src[RIDX((i+k),(j+l), dim)].blue * ker[RIDX(k, l, 8)].blue;
                }
            }

        }
}

char myconv_descr[] = "First try";
void myconv(int dim, pixel *src, pixel *ker, unsigned *dst)
{
        int i,j,k,l;
    int loopvar = dim-8+1;
    for(i = 0; i < loopvar; i++)
    {
        int idim = i*dim;
        for(j = 0; j < loopvar; j++) {
            unsigned value = 0;
            int idimpj = idim+j;
            dst[idimpj] = 0;
            for(k = 0; k < 8; k++)
            {
                int kdimpidimpj = k*dim + idimpj; // i*dim + k*dim +j
                int k8 = k*8;
                for(l = 0; l < 8; l++) {
                        //#define RIDX(i,j,n) ((i)*(n)+(j))
                        //(i+k)*(dim) + (j+l) = i*dim +k *dim + j+l
                        //k*8 + l
                    pixel srcval = src[kdimpidimpj + l];
                    pixel kerval = ker[k8 + l];
                    value += srcval.red * kerval.red;
                    value += srcval.green * kerval.green;
                    value += srcval.blue * kerval.blue;
                }
                dst[idimpj] = value;
            }

        }
     }
}


char myconv2_descr[] = "my second try, moving the square; error in definition, not working.";
void myconv2(int dim, pixel *src, pixel *ker, unsigned *dst) {

    int i,j,k,l;
    unsigned square;
    unsigned column;
    for(i = 0; i < dim-8+1; i++)
    {
        square = 0;
        column = 0;
        for(j = 0; j < dim-8+1; j++) {
            dst[RIDX(i, j, dim)] = 0;
            //temp = 0;
            if(j == 0)
            {
                for(k = 0; k < 8; k++)
                {
                    column += src[RIDX((i+k),(j), dim)].red * ker[RIDX(k, 0, 8)].red;
                    column += src[RIDX((i+k),(j), dim)].green * ker[RIDX(k, 0, 8)].green;
                    column += src[RIDX((i+k),(j), dim)].blue * ker[RIDX(k, 0, 8)].blue;

                    for(l = 0; l < 8; l++)
                    {

                        square += src[RIDX((i+k),(j+l), dim)].red * ker[RIDX(k, l, 8)].red;
                        square += src[RIDX((i+k),(j+l), dim)].green * ker[RIDX(k, l, 8)].green;
                        square += src[RIDX((i+k),(j+l), dim)].blue * ker[RIDX(k, l, 8)].blue;
                    }


                }
                dst[i*dim +j] = square;
            }
            else
            {
                square -= column;
                //since we'll define column, 
                column = 0;
                for(k = 0; k<8; k++)
                {
                    square += src[RIDX((i+k),(j+7), dim)].red * ker[RIDX(k, 7, 8)].red;
                    square += src[RIDX((i+k),(j+7), dim)].green * ker[RIDX(k, 7, 8)].green;
                    square += src[RIDX((i+k),(j+7), dim)].blue * ker[RIDX(k, 7, 8)].blue;
                    column += src[RIDX((i+k),j, dim)].red * ker[RIDX(k, 0, 8)].red +
                            src[RIDX((i+k),j, dim)].green * ker[RIDX(k, 0, 8)].green +
                            src[RIDX((i+k),j, dim)].blue * ker[RIDX(k, 0, 8)].blue;
                }
                //the needed column added. new column value also defined.
                dst[RIDX(i, j, dim)] = square;
            }

        }
    }

}
char myconv3_descr[] = "Convolution: Current working version";
void myconv3(int dim, pixel *src, pixel *ker, unsigned *dst)
{
    int i,j,value,loopvariable = dim - 7;
    pixel *tempsource, *source;
    unsigned  *destination;

    for(i = 0; i < loopvariable ; ++i)
    {
        tempsource = src;
        destination = dst;
        for(j = 0; j < loopvariable ; ++j)
        {
            source = tempsource;
            value = source[0].red * ker[0].red + source[0].green * ker[0].green + source[0].blue * ker[0].blue  ;
            value += (source[1].red * ker[1].red + source[1].green * ker[1].green + source[1].blue * ker[1].blue ) +
                   (source[2].red * ker[2].red + source[2].green * ker[2].green + source[2].blue * ker[2].blue ) +
                   (source[3].red * ker[3].red + source[3].green * ker[3].green + source[3].blue * ker[3].blue )
                 + (source[4].red * ker[4].red + source[4].green * ker[4].green + source[4].blue * ker[4].blue )
                 + (source[5].red * ker[5].red + source[5].green * ker[5].green + source[5].blue * ker[5].blue ) +
                   (source[6].red * ker[6].red + source[6].green * ker[6].green + source[6].blue * ker[6].blue )
                 + (source[7].red * ker[7].red + source[7].green * ker[7].green + source[7].blue * ker[7].blue );
            source += dim;
            value += (source[0].red * ker[8].red + source[0].green * ker[8].green + source[0].blue * ker[8].blue) +
                   (source[1].red * ker[9].red + source[1].green * ker[9].green + source[1].blue * ker[9].blue)
                 + (source[2].red * ker[10].red + source[2].green * ker[10].green + source[2].blue * ker[10].blue) +
                   (source[3].red * ker[11].red + source[3].green * ker[11].green + source[3].blue * ker[11].blue  )
                 + (source[4].red * ker[12].red + source[4].green * ker[12].green + source[4].blue * ker[12].blue)
                 + (source[5].red * ker[13].red + source[5].green * ker[13].green + source[5].blue * ker[13].blue) +
                   (source[6].red * ker[14].red + source[6].green * ker[14].green+ source[6].blue * ker[14].blue)
                 + (source[7].red* ker[15].red + source[7].green* ker[15].green + source[7].blue* ker[15].blue);
            source += dim;
            value += (source[0].red * ker[16].red + source[0].green * ker[16].green + source[0].blue * ker[16].blue) +
                    (source[1].red * ker[17].red + source[1].green * ker[17].green + source[1].blue * ker[17].blue)
                  + (source[2].red * ker[18].red + source[2].green * ker[18].green + source[2].blue * ker[18].blue) +
                   (source[3].red * ker[19].red + source[3].green * ker[19].green + source[3].blue * ker[19].blue  )
                 + (source[4].red * ker[20].red + source[4].green * ker[20].green + source[4].blue * ker[20].blue)
                 + (source[5].red * ker[21].red + source[5].green * ker[21].green + source[5].blue * ker[21].blue) +
                   (source[6].red * ker[22].red + source[6].green * ker[22].green+ source[6].blue * ker[22].blue)
                 + (source[7].red* ker[23].red + source[7].green* ker[23].green + source[7].blue* ker[23].blue);
            source += dim;
            value += (source[0].red * ker[24].red + source[0].green * ker[24].green + source[0].blue * ker[24].blue) +
                    (source[1].red * ker[25].red + source[1].green * ker[25].green + source[1].blue * ker[25].blue)
                 + (source[2].red * ker[26].red + source[2].green * ker[26].green + source[2].blue * ker[26].blue) +
                   (source[3].red * ker[27].red + source[3].green * ker[27].green + source[3].blue * ker[27].blue  )
                 + (source[4].red * ker[28].red + source[4].green * ker[28].green + source[4].blue * ker[28].blue)
                 + (source[5].red * ker[29].red + source[5].green * ker[29].green + source[5].blue * ker[29].blue) +
                   (source[6].red * ker[30].red + source[6].green * ker[30].green+ source[6].blue * ker[30].blue)
                 + (source[7].red* ker[31].red + source[7].green* ker[31].green + source[7].blue* ker[31].blue);
            source += dim;
            value += (source[0].red * ker[32].red + source[0].green * ker[32].green + source[0].blue * ker[32].blue) +
                    (source[1].red * ker[33].red + source[1].green * ker[33].green + source[1].blue * ker[33].blue)
                  + (source[2].red * ker[34].red + source[2].green * ker[34].green + source[2].blue * ker[34].blue) +
                    (source[3].red * ker[35].red + source[3].green * ker[35].green + source[3].blue * ker[35].blue  )
                  + (source[4].red * ker[36].red + source[4].green * ker[36].green + source[4].blue * ker[36].blue)
                  + (source[5].red * ker[37].red + source[5].green * ker[37].green + source[5].blue * ker[37].blue) +
                    (source[6].red * ker[38].red + source[6].green * ker[38].green+ source[6].blue * ker[38].blue)
                  + (source[7].red* ker[39].red + source[7].green* ker[39].green + source[7].blue* ker[39].blue);
            source += dim;
            value += (source[0].red * ker[40].red + source[0].green * ker[40].green + source[0].blue * ker[40].blue) +
                    (source[1].red * ker[41].red + source[1].green * ker[41].green + source[1].blue * ker[41].blue)
                  + (source[2].red * ker[42].red + source[2].green * ker[42].green + source[2].blue * ker[42].blue) +
                   (source[3].red * ker[43].red + source[3].green * ker[43].green + source[3].blue * ker[43].blue  )
                 + (source[4].red * ker[44].red + source[4].green * ker[44].green + source[4].blue * ker[44].blue)
                 + (source[5].red * ker[45].red + source[5].green * ker[45].green + source[5].blue * ker[45].blue) +
                   (source[6].red * ker[46].red + source[6].green * ker[46].green+ source[6].blue * ker[46].blue)
                 + (source[7].red* ker[47].red + source[7].green* ker[47].green + source[7].blue* ker[47].blue);
            source += dim;
            value += (source[0].red * ker[48].red + source[0].green * ker[48].green + source[0].blue * ker[48].blue) +
                    (source[1].red * ker[49].red + source[1].green * ker[49].green + source[1].blue * ker[49].blue)
                  + (source[2].red * ker[50].red + source[2].green * ker[50].green + source[2].blue * ker[50].blue) +
                   (source[3].red * ker[51].red + source[3].green * ker[51].green + source[3].blue * ker[51].blue  )
                 + (source[4].red * ker[52].red + source[4].green * ker[52].green + source[4].blue * ker[52].blue)
                 + (source[5].red * ker[53].red + source[5].green * ker[53].green + source[5].blue * ker[53].blue) +
                   (source[6].red * ker[54].red + source[6].green * ker[54].green+ source[6].blue * ker[54].blue)
                 + (source[7].red* ker[55].red + source[7].green* ker[55].green + source[7].blue* ker[55].blue);
            source += dim;
            value += (source[0].red * ker[56].red + source[0].green * ker[56].green + source[0].blue * ker[56].blue) +
                   (source[1].red * ker[57].red + source[1].green * ker[57].green + source[1].blue * ker[57].blue)
                 + (source[2].red * ker[58].red + source[2].green * ker[58].green + source[2].blue * ker[58].blue) +
                  (source[3].red * ker[59].red + source[3].green * ker[59].green + source[3].blue * ker[59].blue  )
                + (source[4].red * ker[60].red + source[4].green * ker[60].green + source[4].blue * ker[60].blue)
                + (source[5].red * ker[61].red + source[5].green * ker[61].green + source[5].blue * ker[61].blue) +
                  (source[6].red * ker[62].red + source[6].green * ker[62].green+ source[6].blue * ker[62].blue)
                + (source[7].red* ker[63].red + source[7].green* ker[63].green + source[7].blue* ker[63].blue);
            ++tempsource;
            *destination = value;
            destination++;
        }
        src += dim;
        dst += dim;
    }

}


char convolution_descr[] = "Convolution: Current working version";
void convolution(int dim, pixel *src, pixel *ker, unsigned *dst)
{
        myconv3(dim,src,ker,dst);
}

/*********************************************************************
 * register_conv_functions - Register all of your different versions
 *     of the convolution functions  with the driver by calling the
 *     add_conv_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_conv_functions() {
    add_conv_function(&naive_conv, naive_conv_descr);
    add_conv_function(&convolution, convolution_descr);
    add_conv_function(&myconv, myconv_descr);
        add_conv_function(&myconv2, myconv2_descr);
        add_conv_function(&myconv3, myconv3_descr);
    /* ... Register additional test functions here */
}




/********* 
AVERAGE POOLING KERNEL
 ********/

/*******************
 * Your different versions of the average pooling  go here
 *******************/

/*
 * naive_average_pooling - The naive baseline version of average pooling
 */
char naive_average_pooling_descr[] = "Naive Average Pooling: Naive baseline implementation";
void naive_average_pooling(int dim, pixel *src, pixel *dst) {
    int i,j,k,l;

    for(i = 0; i < dim/2; i++)
        for(j = 0; j < dim/2; j++) {
            dst[RIDX(i, j, dim/2)].red = 0;
            dst[RIDX(i, j, dim/2)].green = 0;
            dst[RIDX(i, j, dim/2)].blue = 0;
            for(k = 0; k < 2; k++) {
                for (l = 0; l < 2; l++) {
                    dst[RIDX(i, j, dim/2)].red += src[RIDX(i*2 + k, j*2 + l, dim)].red;
                    dst[RIDX(i, j, dim/2)].green += src[RIDX(i*2 + k, j*2 + l, dim)].green;
                    dst[RIDX(i, j, dim/2)].blue += src[RIDX(i*2 + k, j*2 + l, dim)].blue;
                }
            }
            dst[RIDX(i, j, dim/2)].red /= 4;
            dst[RIDX(i, j, dim/2)].green /= 4;
            dst[RIDX(i, j, dim/2)].blue /= 4;
        }
}


/*
 * average_pooling - Your current working version of average_pooling
 * IMPORTANT: This is the version you will be graded on
 */
char average_pooling_descr[] = "Average Pooling: Current working version";
void average_pooling(int dim, pixel *src, pixel *dst)
{

    //naive_average_pooling(dim,src,dst);
	//#define RIDX(i,j,n) ((i)*(n)+(j))
	int i,j;
	int half=dim/2;
	
	int blocksize=16;
	int ridx; //for i=0, j=0,n=half
	int src_index_1, src_index_2,src_index_3,src_index_4;
	int dest_index;
	for(i = 0; i < half; i++)
	{
		ridx=i*half;
		for(j=0;j<half;j+=blocksize)
		{
			dest_index=ridx+j; //destinationÄ± 0 dan baÅŸlattÄ±m 16ya kadar artmasÄ± lazÄ±m lazÄ±m
			src_index_1 =2*(2*ridx+j);//4*ridx;
			src_index_2=src_index_1+1; 
			src_index_3=4*ridx+dim+2*j;
			src_index_4=src_index_3+1;
			
			//j=0 iÃ§in
			// dest_index=0;
			//src_index_1=0
			//src_index_2=1
			//src_index_3=32
			//src_index_4=33
			dst[dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			//dst[dest_index].red += src[src_index_3].red +src[src_index_4].red;
			//dst[dest_index].green += src[src_index_3].green+src[src_index_4].green;
			//dst[dest_index].blue += src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			/*dst[dest_index].red =dst[dest_index].red / 4;
			dst[dest_index].green =dst[dest_index].green/   4;
			dst[dest_index].blue =dst[dest_index].blue / 4;*/
			
			
			src_index_1+=2; //2
			src_index_2+=2 ;//3
			src_index_3+=2; //34
			src_index_4+=2; //35
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			/*dst[dest_index].red += src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green += src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue += src[src_index_3].blue+src[src_index_4].blue;*/
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //4
			src_index_2+=2; //5
			src_index_3+=2; //36
			src_index_4+=2 ;//37
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //6
			src_index_2+=2 ;//7
			src_index_3+=2 ;//38
			src_index_4+=2; //39
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //8
			src_index_2+=2 ;//9
			src_index_3+=2 ;//40
			src_index_4+=2; //41
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //10
			src_index_2+=2; //11
			src_index_3+=2; //42
			src_index_4+=2; //43
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //12
			src_index_2+=2; //13
			src_index_3+=2 ;//44
			src_index_4+=2; //45
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //14
			src_index_2+=2; //15
			src_index_3+=2 ;//46
			src_index_4+=2 ;//47
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //16
			src_index_2+=2; //17
			src_index_3+=2 ;//48
			src_index_4+=2; //49
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //18
			src_index_2+=2 ;//19
			src_index_3+=2 ;//50
			src_index_4+=2; //51
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //20
			src_index_2+=2 ;//21
			src_index_3+=2 ;//52
			src_index_4+=2 ;//53
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //22
			src_index_2+=2 ;//23
			src_index_3+=2 ;//54
			src_index_4+=2; //55
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
		
			src_index_1+=2; //24
			src_index_2+=2; //25
			src_index_3+=2; //56
			src_index_4+=2; //57
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //26
			src_index_2+=2; //27
			src_index_3+=2; //58
			src_index_4+=2; //59
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //28
			src_index_2+=2; //29
			src_index_3+=2 ;//60
			src_index_4+=2 ;//61
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
			
			src_index_1+=2; //30
			src_index_2+=2; //31
			src_index_3+=2 ;//62
			src_index_4+=2; //63
			
			dst[++dest_index].red = src[src_index_1].red +src[src_index_2].red+src[src_index_3].red +src[src_index_4].red;
			dst[dest_index].green = src[src_index_1].green+src[src_index_2].green+src[src_index_3].green+src[src_index_4].green;
			dst[dest_index].blue = src[src_index_1].blue+src[src_index_2].blue+src[src_index_3].blue+src[src_index_4].blue;
			
			dst[dest_index].red /= 4;
			dst[dest_index].green /= 4;
			dst[dest_index].blue /= 4;
			
		}
		
	}
		
	
}

/**************************
 * register_average_pooling_functions - Register all of your different versions
 *     of the average pooling  with the driver by calling the
 *     add_average_pooling_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 **************************/

void register_average_pooling_functions() {
    add_average_pooling_function(&naive_average_pooling, naive_average_pooling_descr);
    add_average_pooling_function(&average_pooling, average_pooling_descr);
    /* ... Register additional test functions here */
}

