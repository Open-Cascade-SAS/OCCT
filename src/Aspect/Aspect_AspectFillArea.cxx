
// File		Aspect_AspectFillArea.cxx
// Created	Mars 1992
// Author	NW,JPB,CAL
// Modified:    15/01/98 ; FMN : Ajout Hidden Line

//-Copyright	MatraDatavision 1991,1992

//-Version	

//-Design	Declaration des variables specifiques au contexte
//		de trace des facettes

//-Warning	Un contexte de trace de facette est defini par :
//		- le style de l'interieur de la facette
//		- le style du bord de la facette
//		- la couleur

//-References	

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Aspect_AspectFillArea.ixx>

//-Aliases

//-Global data definitions

//	-- l'interieur
//	MyInteriorStyle	:	InteriorStyle;
//	MyInteriorColor	:	Color;

//	-- le bord
//	MyEdgeColor	:	Color;
//	MyEdgeType	:	TypeOfLine;
//	MyEdgeWidth	:	Standard_Real;

//	-- les hachures
//	MyHatchStyle	:	HatchStyle;


#define OCC1174  // SAV : 08/01/03 Added back face interior color

//-Constructors

//-Destructors

//-Methods, in order

Aspect_AspectFillArea::Aspect_AspectFillArea () {

	MyInteriorStyle	= Aspect_IS_EMPTY;
	MyInteriorColor	= Quantity_NOC_CYAN1;
#ifdef OCC1174
	MyBackInteriorColor = Quantity_NOC_CYAN1;
#endif
	MyEdgeColor	= Quantity_NOC_WHITE;
	MyEdgeType	= Aspect_TOL_SOLID;
	MyEdgeWidth	= 1.0;
	MyHatchStyle	= Aspect_HS_VERTICAL;

}

Aspect_AspectFillArea::Aspect_AspectFillArea (const Aspect_InteriorStyle InteriorStyle, const Quantity_Color& InteriorColor, const Quantity_Color& EdgeColor, const Aspect_TypeOfLine EdgeLineType, const Standard_Real EdgeLineWidth) {

	if (EdgeLineWidth <= 0.0)
		Aspect_AspectFillAreaDefinitionError::Raise
			("Bad value for EdgeLineWidth");

	MyInteriorStyle	= InteriorStyle;
	MyInteriorColor	= InteriorColor;
	MyEdgeColor	= EdgeColor;
	MyEdgeType	= EdgeLineType;
	MyEdgeWidth	= EdgeLineWidth;
	MyHatchStyle	= Aspect_HS_VERTICAL;

}

void Aspect_AspectFillArea::SetInteriorStyle (const Aspect_InteriorStyle AStyle) {

	MyInteriorStyle	= AStyle;

}

void Aspect_AspectFillArea::SetInteriorColor (const Quantity_Color& AColor) {

	MyInteriorColor	= AColor;

}

void Aspect_AspectFillArea::SetBackInteriorColor( const Quantity_Color& color )
{
#ifdef OCC1174
  MyBackInteriorColor = color;
#endif
}

void Aspect_AspectFillArea::SetEdgeColor (const Quantity_Color& AColor) {

	MyEdgeColor	= AColor;

}

void Aspect_AspectFillArea::SetEdgeLineType (const Aspect_TypeOfLine AType) {

	MyEdgeType	= AType;

}

void Aspect_AspectFillArea::SetEdgeWidth (const Standard_Real AWidth) {

	if (AWidth <= 0.0)
		Aspect_AspectFillAreaDefinitionError::Raise
			("Bad value for EdgeLineWidth");

	MyEdgeWidth	= AWidth;

}

void Aspect_AspectFillArea::SetHatchStyle (const Aspect_HatchStyle AStyle) {

	MyHatchStyle	= AStyle;

}

Aspect_HatchStyle Aspect_AspectFillArea::HatchStyle () const {

	return (MyHatchStyle);

}

void Aspect_AspectFillArea::Values (Aspect_InteriorStyle& AStyle, Quantity_Color& AIntColor, Quantity_Color& AEdgeColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const {

	AStyle		= MyInteriorStyle;
	AIntColor	= MyInteriorColor;
	AEdgeColor	= MyEdgeColor;
	AType		= MyEdgeType;
	AWidth		= MyEdgeWidth;

}

void Aspect_AspectFillArea::Values (Aspect_InteriorStyle& AStyle, Quantity_Color& AIntColor, Quantity_Color& BackIntColor, Quantity_Color& AEdgeColor, Aspect_TypeOfLine& AType, Standard_Real& AWidth) const {

	AStyle		= MyInteriorStyle;
	AIntColor	= MyInteriorColor;
#ifdef OCC1174
	BackIntColor    = MyBackInteriorColor;
#endif
	AEdgeColor	= MyEdgeColor;
	AType		= MyEdgeType;
	AWidth		= MyEdgeWidth;

}
