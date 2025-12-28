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

#include <Standard_Type.hxx>
#include <Vrml_SFImage.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_SFImage, Standard_Transient)

Vrml_SFImage::Vrml_SFImage()
{
  myArray     = new NCollection_HArray1<int>(1, 1);
  myArrayFlag = false;
}

Vrml_SFImage::Vrml_SFImage(const int                  aWidth,
                           const int                  aHeight,
                           const Vrml_SFImageNumber                aNumber,
                           const occ::handle<NCollection_HArray1<int>>& anArray)
{
  int size = aWidth * aHeight;
  if (anArray->Length() != size)
  {
    throw Standard_Failure("The size of Array is no equal (aWidth*aHeight)");
  }
  myWidth     = aWidth;
  myHeight    = aHeight;
  myNumber    = aNumber;
  myArray     = anArray;
  myArrayFlag = true;
}

void Vrml_SFImage::SetWidth(const int aWidth)
{
  myWidth = aWidth;
}

int Vrml_SFImage::Width() const
{
  return myWidth;
}

void Vrml_SFImage::SetHeight(const int aHeight)
{
  myHeight = aHeight;
}

int Vrml_SFImage::Height() const
{
  return myHeight;
}

void Vrml_SFImage::SetNumber(const Vrml_SFImageNumber aNumber)
{
  myNumber = aNumber;
}

Vrml_SFImageNumber Vrml_SFImage::Number() const
{
  return myNumber;
}

void Vrml_SFImage::SetArray(const occ::handle<NCollection_HArray1<int>>& anArray)
{
  int size = myWidth * myHeight;
  if (anArray->Length() != size)
  {
    throw Standard_Failure("The size of Array is no equal (aWidth*aHeight)");
  }
  myArray     = anArray;
  myArrayFlag = true;
}

bool Vrml_SFImage::ArrayFlag() const
{
  return myArrayFlag;
}

occ::handle<NCollection_HArray1<int>> Vrml_SFImage::Array() const
{
  return myArray;
}
