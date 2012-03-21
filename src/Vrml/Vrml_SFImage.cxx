// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <Vrml_SFImage.ixx>

Vrml_SFImage::Vrml_SFImage()
{
  myArray = new TColStd_HArray1OfInteger(1,1);
  myArrayFlag = Standard_False;
}
 Vrml_SFImage::Vrml_SFImage(const Standard_Integer aWidth, 
			    const Standard_Integer aHeight, 
			    const Vrml_SFImageNumber aNumber,
			    const Handle(TColStd_HArray1OfInteger)& anArray)
{
  Standard_Integer size = aWidth*aHeight;
  if (anArray->Length() != size)
    {
      Standard_Failure::Raise("The size of Array is no equal (aWidth*aHeight)");
    }
  myWidth  = aWidth;
  myHeight = aHeight;
  myNumber = aNumber;
  myArray  = anArray;
  myArrayFlag = Standard_True;
}

void Vrml_SFImage::SetWidth(const Standard_Integer aWidth)
{
  myWidth  =  aWidth;
}

Standard_Integer Vrml_SFImage::Width() const 
{
  return myWidth;
}

void Vrml_SFImage::SetHeight(const Standard_Integer aHeight)
{
  myHeight = aHeight;
}

Standard_Integer Vrml_SFImage::Height() const 
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

void Vrml_SFImage::SetArray(const Handle(TColStd_HArray1OfInteger)& anArray)
{
  Standard_Integer size = myWidth*myHeight;
  if (anArray->Length() != size)
    {
      Standard_Failure::Raise("The size of Array is no equal (aWidth*aHeight)");
      }
  myArray = anArray; 
  myArrayFlag = Standard_True;
}

Standard_Boolean Vrml_SFImage::ArrayFlag() const
{
 return myArrayFlag;
}
Handle(TColStd_HArray1OfInteger) Vrml_SFImage::Array() const 
{
 return myArray;
}
