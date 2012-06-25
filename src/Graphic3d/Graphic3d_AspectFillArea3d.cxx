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

// Modified     1/08/97 ; PCT : Ajout texture mapping
//		26/01/00 ; EUG/GG degeneration management (G003)
//			Add SetDegenerateModel() and DegenerateModel() methods
// JR 02.01.100 : Initialization order of fields in contructors
//		29/09/00 ; GG Add SetDefaultDegerateModel() and
//			DefaultDegenerateModel() class methods
//              22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets 


//-Version	

//-Design	Declaration of variables specific to the context
//		of tracing of facets 3D

//-Warning	Ccontext of tracing of facets 3d inherits the context
//		defined by :
//		- the style of the interior of the facet
//		- the style of the facet border
//		- the color
//		Additionally, it has more than one definition of material
//		for internal and external faces.

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_AspectFillArea3d.ixx>

// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
#include <Aspect_PolygonOffsetMode.hxx>
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 

#include <Standard_Boolean.hxx>

//-Aliases

//-Global data definitions
static Aspect_TypeOfDegenerateModel theDefaultDegenerateModel = Aspect_TDM_WIREFRAME;
static Quantity_Ratio theDefaultDegenerateRatio = 0.0;

//	-- la matiere
//	MyFrontMaterial		:	MaterialAspect;
//	MyBackMaterial		:	MaterialAspect;

//	-- flag de distinction entre faces internes et externes
//	DistinguishModeActive	:	Standard_Boolean;

//	-- flag de trace des aretes
//	EdgeModeActive		:	Standard_Boolean;

//	-- flag d'affichage des polygones tournant le dos
//	BackFaceRemovalActive	:	Standard_Boolean;

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_AspectFillArea3d::Graphic3d_AspectFillArea3d ():
DistinguishModeActive (Standard_False), EdgeModeActive (Standard_False), BackFaceRemovalActive (Standard_False),  MyTextureMapState(Standard_False), MyFrontMaterial (), MyBackMaterial () {
  MyDegenerateModel = theDefaultDegenerateModel;
  MyDegenerateRatio = theDefaultDegenerateRatio;

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets
  // By default, aspect do not change current polygon offset parameters
  MyPolygonOffsetMode   = Aspect_POM_Fill;
  MyPolygonOffsetFactor = 1.;
  MyPolygonOffsetUnits  = 0.;
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
}

// (InteriorStyle, InteriorColor, EdgeColor, EdgeLineType, EdgeLineWidth)
// because AspectFillArea3d inherits AspectFillArea and it is necessary to call
// initialisation of AspectFillArea with InteriorStyle, InteriorColor,
// EdgeColor, EdgeLineType and EdgeLineWidth.

Graphic3d_AspectFillArea3d::Graphic3d_AspectFillArea3d (const Aspect_InteriorStyle InteriorStyle, const Quantity_Color& InteriorColor, const Quantity_Color& EdgeColor, const Aspect_TypeOfLine EdgeLineType, const Standard_Real EdgeLineWidth, const Graphic3d_MaterialAspect& FrontMaterial, const Graphic3d_MaterialAspect& BackMaterial):
Aspect_AspectFillArea (InteriorStyle, InteriorColor, EdgeColor, EdgeLineType, EdgeLineWidth), DistinguishModeActive (Standard_False), EdgeModeActive (Standard_False), BackFaceRemovalActive (Standard_False), MyTextureMap(), MyTextureMapState(Standard_False), MyFrontMaterial (FrontMaterial), MyBackMaterial (BackMaterial) {
  MyDegenerateModel = theDefaultDegenerateModel;
  MyDegenerateRatio = theDefaultDegenerateRatio;

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
  // By default, aspect do not change current polygon offset parameters
  MyPolygonOffsetMode   = Aspect_POM_Fill;
  MyPolygonOffsetFactor = 1.;
  MyPolygonOffsetUnits  = 0.;
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
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

void Graphic3d_AspectFillArea3d::SetDefaultDegenerateModel(
	const Aspect_TypeOfDegenerateModel aModel,
	const Quantity_Ratio aRatio) {
  theDefaultDegenerateModel = aModel;
  theDefaultDegenerateRatio = aRatio;
}

void Graphic3d_AspectFillArea3d::SetDegenerateModel(
	const Aspect_TypeOfDegenerateModel aModel,
	const Quantity_Ratio aRatio) {

  MyDegenerateModel = aModel;
  MyDegenerateRatio = aRatio;
}

Aspect_TypeOfDegenerateModel Graphic3d_AspectFillArea3d::DefaultDegenerateModel(
			Quantity_Ratio& aRatio) {
  aRatio = theDefaultDegenerateRatio;
  return theDefaultDegenerateModel;
}

Aspect_TypeOfDegenerateModel Graphic3d_AspectFillArea3d::DegenerateModel(
			Quantity_Ratio& aRatio) const {
  aRatio = MyDegenerateRatio;
  return MyDegenerateModel;
}

// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
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
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
