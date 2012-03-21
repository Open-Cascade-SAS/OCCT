// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
