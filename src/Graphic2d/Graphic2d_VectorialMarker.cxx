#include <Graphic2d_VectorialMarker.ixx>

Graphic2d_VectorialMarker::Graphic2d_VectorialMarker (const Handle(Graphic2d_GraphicObject)& aGraphicObject, const Quantity_Length aXPosition, const Quantity_Length aYPosition)

	:Graphic2d_Line (aGraphicObject),
	 myXPosition (Standard_ShortReal (aXPosition)),
	 myYPosition (Standard_ShortReal (aYPosition)) {

	SetFamily(Graphic2d_TOP_MARKER);
}

void Graphic2d_VectorialMarker::SetPosition (const Quantity_Length aXPosition, const Quantity_Length aYPosition) { 

	myXPosition	= Standard_ShortReal (aXPosition);
	myYPosition	= Standard_ShortReal (aYPosition);
	ResetIndex ();

}

Quantity_Length Graphic2d_VectorialMarker::XPosition () const {

Standard_Real X = Standard_Real (myXPosition);
Standard_Real Y = Standard_Real (myYPosition);

	if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = myGOPtr->Transform ();
		aTrsf.Transforms (X, Y);
	}

	return Quantity_Length (X);

}

Quantity_Length Graphic2d_VectorialMarker::YPosition () const {

Standard_Real X = Standard_Real (myXPosition);
Standard_Real Y = Standard_Real (myYPosition);

	if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = myGOPtr->Transform ();
		aTrsf.Transforms (X, Y);
	}

	return Quantity_Length (Y);

}

