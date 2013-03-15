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

//-Design	Declaration of variables specific to the context
//		of tracing of facets 3D

//-Warning	Ccontext of tracing of facets 3d inherits the context
//		defined by :
//		- the style of the interior of the facet
//		- the style of the facet border
//		- the color
//		Additionally, it has more than one definition of material
//		for internal and external faces.

// for the class
#include <Graphic3d_AspectFillArea3d.ixx>

#include <Aspect_PolygonOffsetMode.hxx>
#include <Standard_Boolean.hxx>

Graphic3d_AspectFillArea3d::Graphic3d_AspectFillArea3d ():
DistinguishModeActive (Standard_False), EdgeModeActive (Standard_False), BackFaceRemovalActive (Standard_False),  MyTextureMapState(Standard_False), MyFrontMaterial (), MyBackMaterial ()
{
  // By default, aspect do not change current polygon offset parameters
  MyPolygonOffsetMode   = Aspect_POM_Fill;
  MyPolygonOffsetFactor = 1.;
  MyPolygonOffsetUnits  = 0.;
}

// (InteriorStyle, InteriorColor, EdgeColor, EdgeLineType, EdgeLineWidth)
// because AspectFillArea3d inherits AspectFillArea and it is necessary to call
// initialisation of AspectFillArea with InteriorStyle, InteriorColor,
// EdgeColor, EdgeLineType and EdgeLineWidth.

Graphic3d_AspectFillArea3d::Graphic3d_AspectFillArea3d (const Aspect_InteriorStyle InteriorStyle, const Quantity_Color& InteriorColor, const Quantity_Color& EdgeColor, const Aspect_TypeOfLine EdgeLineType, const Standard_Real EdgeLineWidth, const Graphic3d_MaterialAspect& FrontMaterial, const Graphic3d_MaterialAspect& BackMaterial):
Aspect_AspectFillArea (InteriorStyle, InteriorColor, EdgeColor, EdgeLineType, EdgeLineWidth), DistinguishModeActive (Standard_False), EdgeModeActive (Standard_False), BackFaceRemovalActive (Standard_False), MyTextureMap(), MyTextureMapState(Standard_False), MyFrontMaterial (FrontMaterial), MyBackMaterial (BackMaterial) {
  // By default, aspect do not change current polygon offset parameters
  MyPolygonOffsetMode   = Aspect_POM_Fill;
  MyPolygonOffsetFactor = 1.;
  MyPolygonOffsetUnits  = 0.;
}

void Graphic3d_AspectFillArea3d::SetBackMaterial (const Graphic3d_MaterialAspect& AMaterial) {

	MyBackMaterial	= AMaterial;

}

void Graphic3d_AspectFillArea3d::SetFrontMaterial (const Graphic3d_MaterialAspect& AMaterial) {

	MyFrontMaterial	= AMaterial;

}

Graphic3d_MaterialAspect Graphic3d_AspectFillArea3d::BackMaterial () const {

	return (MyBackMaterial);

}

Graphic3d_MaterialAspect Graphic3d_AspectFillArea3d::FrontMaterial () const {

	return (MyFrontMaterial);

}

void Graphic3d_AspectFillArea3d::AllowBackFace () {

	BackFaceRemovalActive	= Standard_False;

}

void Graphic3d_AspectFillArea3d::SuppressBackFace () {

	BackFaceRemovalActive	= Standard_True;

}

Standard_Boolean Graphic3d_AspectFillArea3d::BackFace () const {

	return (BackFaceRemovalActive);

}

void Graphic3d_AspectFillArea3d::SetDistinguishOn () {

	DistinguishModeActive	= Standard_True;

}

void Graphic3d_AspectFillArea3d::SetDistinguishOff () {

	DistinguishModeActive	= Standard_False;

}

Standard_Boolean Graphic3d_AspectFillArea3d::Distinguish () const {

	return (DistinguishModeActive);

}

void Graphic3d_AspectFillArea3d::SetEdgeOn () {

	EdgeModeActive	= Standard_True;

}

void Graphic3d_AspectFillArea3d::SetEdgeOff () {

	EdgeModeActive	= Standard_False;

}


void Graphic3d_AspectFillArea3d::SetTextureMap(const Handle(Graphic3d_TextureMap)& ATexture)
{
  MyTextureMap = ATexture;
}


void Graphic3d_AspectFillArea3d::SetTextureMapOn()
{
  MyTextureMapState = Standard_True;
}


void Graphic3d_AspectFillArea3d::SetTextureMapOff()
{
  MyTextureMapState = Standard_False;
}


Standard_Boolean Graphic3d_AspectFillArea3d::Edge () const {

	return (EdgeModeActive);

}


Handle(Graphic3d_TextureMap) Graphic3d_AspectFillArea3d::TextureMap() const
{
  return MyTextureMap;
}


Standard_Boolean Graphic3d_AspectFillArea3d::TextureMapState() const
{
  return MyTextureMapState;
}

void Graphic3d_AspectFillArea3d::SetPolygonOffsets(const Standard_Integer    aMode,
                                                   const Standard_ShortReal  aFactor,
                                                   const Standard_ShortReal  aUnits) {
  MyPolygonOffsetMode   = ( aMode & Aspect_POM_Mask );
  MyPolygonOffsetFactor = aFactor;
  MyPolygonOffsetUnits  = aUnits;
}

void Graphic3d_AspectFillArea3d::PolygonOffsets(Standard_Integer&    aMode,
                                                Standard_ShortReal&  aFactor,
                                                Standard_ShortReal&  aUnits) const {
  aMode   = MyPolygonOffsetMode;
  aFactor = MyPolygonOffsetFactor;
  aUnits  = MyPolygonOffsetUnits;
}
