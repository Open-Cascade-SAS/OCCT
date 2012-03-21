// Created on: 2011-09-20
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
