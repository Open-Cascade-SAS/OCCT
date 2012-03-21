// Copyright (c) 1995-1999 Matra Datavision
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

#define GER61351		//GG_171199     Enable to set an object RGB color
//						  instead a restricted object NameOfColor.

#include <Prs3d_PointAspect.ixx>

#ifdef GER61351
Prs3d_PointAspect::Prs3d_PointAspect (const Aspect_TypeOfMarker aType,
				  const Quantity_Color &aColor,
				  const Standard_Real aScale) {
  myAspect = new Graphic3d_AspectMarker3d(aType,aColor,aScale);
}
#endif

Prs3d_PointAspect::Prs3d_PointAspect (const Aspect_TypeOfMarker aType,
				  const Quantity_NameOfColor aColor,
				  const Standard_Real aScale) {
  myAspect = new Graphic3d_AspectMarker3d
    (aType,Quantity_Color(aColor),aScale);
}

Prs3d_PointAspect::Prs3d_PointAspect (const Quantity_Color &aColor,
				      const Standard_Real anId,
                                      const Standard_Integer aWidth,
                                      const Standard_Integer aHeight,
				      const Handle(TColStd_HArray1OfByte)& aTexture
                                      ) 
{
  myAspect = new Graphic3d_AspectMarker3d
    (Aspect_TOM_USERDEFINED,aColor,anId,aWidth,aHeight,aTexture);
}

#ifdef GER61351
void Prs3d_PointAspect::SetColor(const Quantity_Color &aColor) {
  myAspect->SetColor(aColor);
}
#endif

void Prs3d_PointAspect::SetColor(const Quantity_NameOfColor aColor) {
  myAspect->SetColor(Quantity_Color(aColor));
}

void Prs3d_PointAspect::SetTypeOfMarker(const Aspect_TypeOfMarker aType){
  myAspect->SetType(aType);
}

void Prs3d_PointAspect::SetScale(const Standard_Real aScale){
  myAspect->SetScale(aScale);
}

Handle (Graphic3d_AspectMarker3d) Prs3d_PointAspect::Aspect () const {
  return myAspect;
}

void Prs3d_PointAspect::GetTextureSize(Standard_Integer& AWidth, Standard_Integer& AHeight)
{
  myAspect->GetTextureSize( AWidth, AHeight);
}

const Handle(TColStd_HArray1OfByte)& Prs3d_PointAspect::GetTexture()
{
  return myAspect->GetTexture();
}
