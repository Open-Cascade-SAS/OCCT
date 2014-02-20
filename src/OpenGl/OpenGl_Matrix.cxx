// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
