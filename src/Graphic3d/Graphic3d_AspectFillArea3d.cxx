// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Aspect_PolygonOffsetMode.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>

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

const Graphic3d_MaterialAspect& Graphic3d_AspectFillArea3d::BackMaterial() const
{
  return MyBackMaterial;
}

const Graphic3d_MaterialAspect& Graphic3d_AspectFillArea3d::FrontMaterial() const
{
  return MyFrontMaterial;
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

void Graphic3d_AspectFillArea3d::SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram)
{
  MyShaderProgram = theProgram;
}

const Handle(Graphic3d_ShaderProgram)& Graphic3d_AspectFillArea3d::ShaderProgram() const
{
  return MyShaderProgram;
}
