
#include <Graphic2d_InfiniteLine.ixx>

Graphic2d_InfiniteLine::Graphic2d_InfiniteLine (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Quantity_Length X, const Quantity_Length Y,
	const Quantity_Length DX, const Quantity_Length DY)

  : Graphic2d_Line (aGraphicObject) {

	myX = Standard_ShortReal(X);
        myY = Standard_ShortReal(Y);
        myDX = Standard_ShortReal(DX); 
	myDY = Standard_ShortReal(DY);

	//if ((myDX == 0.) && (myDY == 0.))
	if ((Abs (Standard_Real (DX)) <= RealEpsilon ()) &&
	    (Abs (Standard_Real (DY)) <= RealEpsilon ()))
                Graphic2d_InfiniteLineDefinitionError::Raise
			("The slope is undefined");

	//if (myDX != 0.) {
	if (Abs (Standard_Real (DX)) > RealEpsilon ()) {
	  myMinX = ShortRealFirst();
	  myMaxX = ShortRealLast();
	}
	else
	  myMinX = myMaxX = myX;

	//if (myDY != 0.) {
	if (Abs (Standard_Real (DY)) > RealEpsilon ()) {
	  myMinY = ShortRealFirst();
	  myMaxY = ShortRealLast();
	}
	else
	  myMinY = myMaxY = myY;
}

void Graphic2d_InfiniteLine::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

Standard_ShortReal a, b, c, d;

	DrawLineAttrib (aDrawer);

	a = myX; b = myY;
	c = myDX; d = myDY;

	if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = myGOPtr->Transform ();
Standard_Real A, B, C, D;
		// Calcul second point de passage
		C = Standard_Real (a) + Standard_Real (c);
		D = Standard_Real (b) + Standard_Real (d);
		// Calcul nouveau point de reference
		A = Standard_Real (a); B = Standard_Real (b);
		aTrsf.Transforms (A, B);
		a = Standard_ShortReal (A); b = Standard_ShortReal (B);
		// Calcul nouveau point de passage
		aTrsf.Transforms (C, D);
		// Calcul nouvelle pente
		c = Standard_ShortReal (C - A);
		d = Standard_ShortReal (D - B);
	}

	aDrawer->MapInfiniteLineFromTo (a, b, c, d);

}

Standard_Boolean Graphic2d_InfiniteLine::Pick (const Standard_ShortReal X,
					  const Standard_ShortReal Y,
					  const Standard_ShortReal aPrecision,
					  const Handle(Graphic2d_Drawer)& aDrawer)
{
   Standard_ShortReal SRX = X, SRY = Y;

   if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = (myGOPtr->Transform ()).Inverted ();
Standard_Real RX = Standard_Real (SRX), RY = Standard_Real (SRY);
	aTrsf.Transforms (RX, RY);
	SRX = Standard_ShortReal (RX); SRY = Standard_ShortReal (RY);
   }

   Standard_ShortReal lambda = ( myDX * (SRX-myX) + myDY * (SRY-myY) )
                              /  ( myDX * myDX + myDY * myDY);
   Standard_ShortReal Xproj = myX + lambda * myDX;
   Standard_ShortReal Yproj = myY + lambda * myDY;

//  On prend comme norme la somme des valeurs absolues:

   return ( Abs ( SRX - Xproj) + Abs ( SRY - Yproj ) ) < aPrecision;
}

void Graphic2d_InfiniteLine::Reference(Quantity_Length& X,Quantity_Length& Y) const {
    X = Quantity_Length( myX );
    Y = Quantity_Length( myY );
}

void Graphic2d_InfiniteLine::Slope(Quantity_Length& dX,Quantity_Length& dY) const {
    dX = Quantity_Length( myDX );
    dY = Quantity_Length( myDY );
}

void Graphic2d_InfiniteLine::Save(Aspect_FStream& aFStream) const
{
	*aFStream << "Graphic2d_InfiniteLine" << endl;
	*aFStream << myX << ' ' << myY << endl;
	*aFStream << myDX << ' ' << myDY << endl;
	Graphic2d_Line::Save(aFStream);
}

void Graphic2d_InfiniteLine::Retrieve(Aspect_IFStream& anIFStream,
			const Handle(Graphic2d_GraphicObject)& aGraphicObject)
{
	Quantity_Length X, Y, dX, dY;
	
	*anIFStream >> X >> Y;
	*anIFStream >> dX >> dY;
	Handle(Graphic2d_InfiniteLine)
		theILine = new Graphic2d_InfiniteLine(aGraphicObject, X, Y, dX, dY);
	((Handle (Graphic2d_Line))theILine)->Retrieve(anIFStream);
}

