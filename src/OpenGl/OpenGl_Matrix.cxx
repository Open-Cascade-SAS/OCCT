// File:      OpenGl_Matrix.cxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_Matrix.hxx>

/*----------------------------------------------------------------------*/

void OpenGl_Multiplymat3 (OpenGl_Matrix *c, const OpenGl_Matrix *a, const OpenGl_Matrix *b)
{
  int row, col, i;
  for (row = 0; row < 4; row++) {
    for (col = 0; col < 4; col++) {
      float sum = 0.F;
      for (i = 0; i < 4; i++)
        sum += a->mat[row][i] * b->mat[i][col];
      c->mat[row][col] = sum;
    }
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_Transposemat3 (OpenGl_Matrix *c, const OpenGl_Matrix *a)
{
  Tint row, col;
  for (row = 0; row < 4; row++)
    for (col = 0; col < 4; col++)
      c->mat[row][col] = a->mat[col][row];
}
