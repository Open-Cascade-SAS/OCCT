// File:      OpenGl_Matrix.hxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_Matrix_Header
#define OpenGl_Matrix_Header

#include <InterfaceGraphic_telem.hxx>

struct OpenGl_Matrix
{
  float mat[4][4];
  DEFINE_STANDARD_ALLOC
};

Standard_EXPORT void OpenGl_Multiplymat3 (OpenGl_Matrix *c, const OpenGl_Matrix *a, const OpenGl_Matrix *b);
Standard_EXPORT void OpenGl_Transposemat3 (OpenGl_Matrix *c, const OpenGl_Matrix *a);

#endif //OpenGl_Matrix_Header
