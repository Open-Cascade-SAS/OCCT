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

#define PRO17334       //GG_050199
//                      Ne pas raiser si l'index du marker est NULL

#define OCC451        // SAV DrawVertex redefined.

#include <Graphic2d_Marker.ixx>
#include <Quantity_PlaneAngle.hxx>
#include <TShort_Array1OfShortReal.hxx>

Graphic2d_Marker::Graphic2d_Marker (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Quantity_Length X, const Quantity_Length Y)

	:Graphic2d_Line (aGraphicObject),
	 myMarkIndex (0),
	 myX (Standard_ShortReal (X)),
	 myY (Standard_ShortReal (Y)),
	 myWidth (0.0),
	 myHeight (0.0),
	 myAngle (0.0) {

	SetFamily(Graphic2d_TOP_MARKER);
	myMinX = myMaxX = myX; myMinY = myMaxY = myY ;

}

Graphic2d_Marker::Graphic2d_Marker (
	const Handle(Graphic2d_GraphicObject)& aGraphicObject,
	const Standard_Integer anIndex,
	const Quantity_Length X, const Quantity_Length Y,
	const Quantity_Length aWidth, const Quantity_Length anHeight,
	const Quantity_PlaneAngle anAngle)

	:Graphic2d_Line (aGraphicObject),
	 myMarkIndex (anIndex),
	 myX (Standard_ShortReal (X)),
	 myY (Standard_ShortReal (Y)),
	 myWidth (Standard_ShortReal(aWidth)),
	 myHeight (Standard_ShortReal(anHeight)),
	 myAngle (Standard_ShortReal(anAngle)) {

#ifdef PRO17334
        if( myMarkIndex < 0 )
                Graphic2d_MarkerDefinitionError::Raise
                        ("The marker index must be >= 0");
        else if( myMarkIndex > 0 ) {
#else
	if( myMarkIndex <= 0 )
		Graphic2d_MarkerDefinitionError::Raise
			("The marker index must be > 0");
#endif
	if (myWidth <= 0.0)
		Graphic2d_MarkerDefinitionError::Raise ("The width = 0.");

	if (myHeight <= 0.0)
		Graphic2d_MarkerDefinitionError::Raise ("The height = 0.");
#ifdef PRO17334
        } else {
          myWidth = myHeight = 0.;      //POINT marker
        }
#endif

	SetFamily(Graphic2d_TOP_MARKER);
	myMinX = myX - Standard_ShortReal(myWidth/2.); myMinY = myY - Standard_ShortReal(myHeight/2.);
	myMaxX = myX + Standard_ShortReal(myWidth/2.); myMaxY = myY + Standard_ShortReal(myHeight/2.);

}


void Graphic2d_Marker::DrawVertex( const Handle(Graphic2d_Drawer)& aDrawer,
				   const Standard_Integer anIndex )
{
#ifdef OCC451
  Draw( aDrawer );
#endif
}

void Graphic2d_Marker::Draw (const Handle(Graphic2d_Drawer)& aDrawer) {

Standard_Boolean IsIn = Standard_False;

  if (! myGOPtr->IsTransformed ())
    IsIn = aDrawer->IsIn (myMinX,myMaxX,myMinY,myMaxY);
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax(minx,maxx,miny,maxy);
    IsIn = aDrawer->IsIn (minx,maxx,miny,maxy);
  }

	if (IsIn) {

Standard_ShortReal a, b;
		a = myX; b = myY;

		if (myGOPtr->IsTransformed ()) {
gp_GTrsf2d aTrsf = myGOPtr->Transform ();
Standard_Real A, B;
			A = Standard_Real (a); B = Standard_Real (b);
			aTrsf.Transforms (A, B);
			a = Standard_ShortReal (A); b = Standard_ShortReal (B);
		}

		DrawMarkerAttrib (aDrawer);
		aDrawer->MapMarkerFromTo
			(myMarkIndex, a, b, myWidth, myHeight, myAngle);
	}

}

Standard_Boolean Graphic2d_Marker::Pick (
	const Standard_ShortReal X,
	const Standard_ShortReal Y,
	const Standard_ShortReal aPrecision,
	const Handle(Graphic2d_Drawer)& aDrawer) {

//	return IsInMinMax (X, Y, aPrecision);
  if ( IsInMinMax (X, Y, aPrecision) ) {
    SetPickedIndex( -1 );
    return Standard_True;
  }
  else {
    SetPickedIndex( 0 );
    return Standard_False;
  }
}
	
void Graphic2d_Marker::Position(Quantity_Length& X,Quantity_Length& Y) const {
  
  X = Quantity_Length( myX );
  Y = Quantity_Length( myY );
}

void Graphic2d_Marker::Size(Quantity_Length& aW,Quantity_Length& aH) const {
    
  aW = Quantity_Length( myWidth );
  aH = Quantity_Length( myHeight );
}

Standard_Integer Graphic2d_Marker::Index() const {
   return myMarkIndex;
}

Quantity_PlaneAngle Graphic2d_Marker::Angle() const {
   return Quantity_PlaneAngle( myAngle );
}

void Graphic2d_Marker::Save(Aspect_FStream& aFStream) const
{
}
