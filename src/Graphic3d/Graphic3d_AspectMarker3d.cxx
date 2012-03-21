// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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




//-Version	

//-Design	Declaration of variables specific to the context
//		of trace of markers 3d

// Rappels	Context of trace of markers 3d inherits the context
//		defined by :
//		- the color
//		- the type of marker
//		- the scale

//-Warning	

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_AspectMarker3d.ixx>
#include <TColStd_Array1OfByte.hxx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d ()
     :Aspect_AspectMarker(), MyTextureWidth(0), MyTextureHeight(0)
{  
}

Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Aspect_TypeOfMarker AType, const Quantity_Color& AColor, const Standard_Real AScaleOrId ):
Aspect_AspectMarker( AColor, AType, AScaleOrId ), MyTextureWidth(0), MyTextureHeight(0)
{ 
}


Graphic3d_AspectMarker3d::Graphic3d_AspectMarker3d (const Aspect_TypeOfMarker AType, 
						    const Quantity_Color& AColor, 
						    const Standard_Real AScaleOrId, 
						    const Standard_Integer AWidth,
						    const Standard_Integer AHeight,
						    const Handle(TColStd_HArray1OfByte)& ATexture ):
Aspect_AspectMarker( AColor, AType, AScaleOrId ), MyTexture( ATexture ), MyTextureWidth(AWidth), MyTextureHeight(AHeight)
{
  /*for( Standard_Integer aIndex = ATexture.Lower(); aIndex <= ATexture.Upper(); aIndex++ )
    {
      MyTexture.SetValue( aIndex, ATexture.Value( aIndex ) );
    }
  */
  
}

void Graphic3d_AspectMarker3d::GetTextureSize(Standard_Integer& AWidth, Standard_Integer& AHeight)
{
  AWidth = MyTextureWidth;
  AHeight = MyTextureHeight;
}

const Handle(TColStd_HArray1OfByte)& Graphic3d_AspectMarker3d::GetTexture()
{
  return MyTexture;
}

void Graphic3d_AspectMarker3d::SetTexture (const Standard_Integer AWidth,
					   const Standard_Integer AHeight,
                                           const Handle(TColStd_HArray1OfByte)& ATexture )
{
   MyTextureWidth = AWidth;
   MyTextureHeight = AHeight;
     
   MyTexture = ATexture;
}
