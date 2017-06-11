/*
 *  Matrix task processor - matrix header file
 *  Based on Operating Systems: Three Easy Pieces by R. Arpaci-Dusseau and A. Arpaci-Dusseau
 * 
 *  Wes J. Lloyd
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2017
 */

#define ROW 5
#define COL 5

// MATRIX ROUTINES
int ** AllocMatrix(int r, int c);
void FreeMatrix(int ** a, int r, int c);
void GenMatrix(int ** matrix, const int height, const int width);
void GenMatrixType(int ** matrix, const int height, const int width, int type);
int AvgElement(int ** matrix, const int height, const int width);
int SumMatrix(int ** matrix, const int height, const int width);
void DisplayMatrix(int ** matrix, const int height, const int width, FILE *stream);

