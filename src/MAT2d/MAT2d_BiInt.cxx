// Created on: 1993-11-19
// Created by: Yves FRICAUD
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <MAT2d_BiInt.hxx>

MAT2d_BiInt::MAT2d_BiInt(const Standard_Integer I1,
			 const Standard_Integer I2)
     :i1(I1),i2(I2)
{
}


Standard_Integer  MAT2d_BiInt::FirstIndex()const 
{
  return i1;
}

Standard_Integer  MAT2d_BiInt::SecondIndex()const
{
  return i2;
}

void  MAT2d_BiInt::FirstIndex(const Standard_Integer I1)
{
  i1 = I1;
}

void  MAT2d_BiInt::SecondIndex(const Standard_Integer I2)
{
  i2 = I2;
}

Standard_Boolean MAT2d_BiInt::IsEqual(const MAT2d_BiInt& B)
const 
{
  return (i1 == B.FirstIndex() && i2 == B.SecondIndex());
}
     
