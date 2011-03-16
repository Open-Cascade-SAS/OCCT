
// File		Graphic3d_AspectLine3d.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques au contexte
//		de trace des lignes 3d

//-Warning	Un contexte de trace de lignes 3d herite du contexte
//		defini par :
//		- la couleur
//		- le type de trait
//		- l'epaisseur

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_AspectLine3d.ixx>

//-Aliases

//-Global data definitions

//-Constructors

//-Destructors

//-Methods, in order

Graphic3d_AspectLine3d::Graphic3d_AspectLine3d () {
}

// le :(AColor, AType, AWidth)
// car AspectLine3d herite de AspectLine et il faut appeler
// l'initialisation de AspectLine avec AColor, AType, AWidth.

Graphic3d_AspectLine3d::Graphic3d_AspectLine3d (const Quantity_Color& AColor, const Aspect_TypeOfLine AType, const Standard_Real AWidth):Aspect_AspectLine (AColor, AType, AWidth) {}
