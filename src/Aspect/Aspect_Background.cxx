
// File		Aspect_Background.cxx
// Created	Fevrier 1992
// Author	NW,JPB,CAL

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques aux fonds d'ecran.

//-Warning	Un fond d'ecran est defini par une couleur.

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_Background.ixx>

//-Aliases

//-Global data definitions

//	-- la couleur associee au fond d'ecran
//	MyColor	:	Color;

//-Constructors

//-Destructors

//-Methods, in order

Aspect_Background::Aspect_Background () {

Quantity_Color MatraGray (Quantity_NOC_MATRAGRAY);

	MyColor	= MatraGray;

}

Aspect_Background::Aspect_Background (const Quantity_Color& AColor) {

	MyColor	= AColor;

}

void Aspect_Background::SetColor (const Quantity_Color& AColor) {

	MyColor	= AColor;

}

Quantity_Color Aspect_Background::Color () const {

	return (MyColor);

}
