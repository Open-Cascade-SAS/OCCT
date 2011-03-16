// File		Graphic3d_AspectFillArea3d.cxx
// Created	Mars 1992
// Author	NW,JPB,CAL
// Modified     1/08/97 ; PCT : Ajout texture mapping
//		26/01/00 ; EUG/GG degeneration management (G003)
//			Add SetDegenerateModel() and DegenerateModel() methods
// JR 02.01.100 : Initialization order of fields in contructors
//		29/09/00 ; GG Add SetDefaultDegerateModel() and
//			DefaultDegenerateModel() class methods
//              22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets 

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques au contexte
//		de trace des facettes 3d

//-Warning	Un contexte de trace de facette 3d herite du contexte
//		defini par :
//		- le style de l'interieur de la facette
//		- le style du bord de la facette
//		- la couleur
//		Il possede en plus d'une definition de matiere
//		pour les faces internes et externes.

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
  MyPolygonOffsetMode   = Aspect_POM_None;
  MyPolygonOffsetFactor = 1.;
  MyPolygonOffsetUnits  = 0.;
  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
}

// le :(InteriorStyle, InteriorColor, EdgeColor, EdgeLineType, EdgeLineWidth)
// car AspectFillArea3d herite de AspectFillArea et il faut appeler
// l'initialisation de AspectFillArea avec InteriorStyle, InteriorColor,
// EdgeColor, EdgeLineType et EdgeLineWidth.

Graphic3d_AspectFillArea3d::Graphic3d_AspectFillArea3d (const Aspect_InteriorStyle InteriorStyle, const Quantity_Color& InteriorColor, const Quantity_Color& EdgeColor, const Aspect_TypeOfLine EdgeLineType, const Standard_Real EdgeLineWidth, const Graphic3d_MaterialAspect& FrontMaterial, const Graphic3d_MaterialAspect& BackMaterial):
Aspect_AspectFillArea (InteriorStyle, InteriorColor, EdgeColor, EdgeLineType, EdgeLineWidth), DistinguishModeActive (Standard_False), EdgeModeActive (Standard_False), BackFaceRemovalActive (Standard_False), MyTextureMap(), MyTextureMapState(Standard_False), MyFrontMaterial (FrontMaterial), MyBackMaterial (BackMaterial) {
  MyDegenerateModel = theDefaultDegenerateModel;
  MyDegenerateRatio = theDefaultDegenerateRatio;

  // OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
  // By default, aspect do not change current polygon offset parameters
  MyPolygonOffsetMode   = Aspect_POM_None;
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
void Graphic3d_AspectFillArea3d::SetPolygonOffsets(const Standard_Integer aMode,
                                                   const Standard_Real    aFactor,
                                                   const Standard_Real    aUnits) {
  MyPolygonOffsetMode   = ( aMode & Aspect_POM_Mask );
  MyPolygonOffsetFactor = aFactor;
  MyPolygonOffsetUnits  = aUnits;
}

void Graphic3d_AspectFillArea3d::PolygonOffsets(Standard_Integer& aMode,
                                                Standard_Real&    aFactor,
                                                Standard_Real&    aUnits) const {
  aMode   = MyPolygonOffsetMode;
  aFactor = MyPolygonOffsetFactor;
  aUnits  = MyPolygonOffsetUnits;
}
// OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets 
