/*
 *  Matrix task processor - matrix module
 *  Based on Operating Systems: Three Easy Pieces by R. Arpaci-Dusseau and A. Arpaci-Dusseau
 * 
 *  Wes J. Lloyd
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

int SumMatrix(int ** matrix, const int height, const int width);

// MATRIX ROUTINES
int ** AllocMatrix(int r, int c) 
{
  int ** a;
  int i;
  a = (int**) malloc(sizeof(int *) * r);
  assert(a != 0);
  for (i = 0; i < r; i++)
  {
    a[i] = (int *) malloc(c * sizeof(int));
    assert(a[i] != 0);
  }
  return a;
}

void FreeMatrix(int ** a, int r, int c)
{
  int i;
  for (i=0; i<r; i++)
  {
    free(a[i]);
  }
  free(a);
}

void GenMatrixType(int ** matrix, const int height, const int width, int type)
{
  int i, j;
  if (type > 100)
    type = 100;
  if (type < 1)
    type = 1;
  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      int * mm = matrix[i];
      switch (type)
      {
        case 1:
        mm[j] = 1;
        break;
        case 2:
        mm[j] = j;
        break;
        default:
        mm[j] = rand() % type;
      }
#if OUTPUT
      printf("matrix[%d][%d]=%d \n",i,j,mm[j]);
#endif
    }
  }
}

void GenMatrix(int ** matrix, const int height, const int width)
{
  GenMatrixType(matrix, height, width, 1);
}

// TO DO
// Implement the AvgElement function
int AvgElement(int ** matrix, const int height, const int width)
{
    int sum = SumMatrix(matrix, height, width);
    int num = height * width;
    int avg = sum / num;

  return avg;

}

int SumMatrix(int ** matrix, const int height, const int width)
{
  int sum=0;
  int y=0;
  int i, j;
  for (i=0; i<height; i++)
  {
    int *mm = matrix[i];
    for (j=0; j<width; j++)
    {
      y=mm[j];  
      sum=sum+y;
    }
  }
  return sum;
}

void DisplayMatrix(int ** matrix, const int height, const int width, FILE *stream)
{
  int y=0;
  int i, j;
  for (i=0; i<height; i++)
  { 
    int *mm = matrix[i];
    fprintf(stream, "|");
    for (j=0; j<width; j++)
    {
      y=mm[j];
      if (j==0)  
        fprintf(stream, "%3d",y);
      else
        fprintf(stream, " %3d",y);
    }
    fprintf(stream, "|\n");
  }
}

