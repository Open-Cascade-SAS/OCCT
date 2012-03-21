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

#include <Prs2d_Point.ixx>

Prs2d_Point::Prs2d_Point( const Handle(Graphic2d_GraphicObject)& aGO,
                          const gp_Pnt2d& aPnt, 
                          const Aspect_TypeOfMarker aTOM, 
                          const Quantity_Length aWSize,
                          const Quantity_Length aHSize,
                          const Quantity_PlaneAngle anAngle ) 
    
  :	Graphic2d_Line( aGO ),
    myPoint( aPnt ),
    myTOM( aTOM ),
    myIndMark( 0 ),
    myWSize( aWSize ),
    myHSize( aHSize ),
    myAngle( anAngle )
{
  
    if ( ( myWSize <= 0.0 ) || ( myHSize <= 0.0 ) )
          myWSize = myHSize = 0.0;      

 	myMinX = Standard_ShortReal(myPoint.X() - myWSize/2.); 
    myMinY = Standard_ShortReal(myPoint.Y() - myHSize/2.);
	myMaxX = Standard_ShortReal(myPoint.X() + myWSize/2.); 
    myMaxY = Standard_ShortReal(myPoint.Y() + myHSize/2.);

}

void Prs2d_Point::Draw( const Handle(Graphic2d_Drawer)& aDrawer ) {

  Standard_Boolean IsIn = Standard_False;

  if ( !myGOPtr->IsTransformed() )
    IsIn = aDrawer->IsIn( myMinX, myMaxX, myMinY, myMaxY );
  else {
    Standard_ShortReal minx, miny, maxx, maxy;
    MinMax( minx, maxx, miny, maxy );
    IsIn = aDrawer->IsIn( minx, maxx, miny, maxy );
  }

  if ( IsIn ) {

    Standard_ShortReal a = Standard_ShortReal(myPoint.X()); 
    Standard_ShortReal b = Standard_ShortReal(myPoint.Y());

	if ( myGOPtr->IsTransformed() ) {

        gp_GTrsf2d aTrsf = myGOPtr->Transform();
        Standard_Real A = Standard_Real( a ); 
        Standard_Real B = Standard_Real( b );
		aTrsf.Transforms( A, B );
		a = Standard_ShortReal( A ); 
        b = Standard_ShortReal( B );
	}

    DrawMarkerAttrib( aDrawer );
	aDrawer->MapMarkerFromTo( myIndMark, a, b, Standard_ShortReal(myWSize), 
          Standard_ShortReal(myHSize), Standard_ShortReal(myAngle) );
  }

}

void Prs2d_Point::DrawElement( const Handle(Graphic2d_Drawer)& /*ADrawer*/,
                               const Standard_Integer /*anIndex*/ ) {
  
}

void Prs2d_Point::DrawVertex( const Handle(Graphic2d_Drawer)& /*aDrawer*/,
                              const Standard_Integer /*anIndex*/ ) {
  
}

Standard_Boolean Prs2d_Point::Pick( const Standard_ShortReal X,
                                    const Standard_ShortReal Y,
                                    const Standard_ShortReal aPrecision,
                                    const Handle(Graphic2d_Drawer)& /*aDrawer*/ ) {

	return IsInMinMax( X, Y, aPrecision );

}

void Prs2d_Point::SetPoint( const gp_Pnt2d& aPnt ) {
  myPoint = aPnt;
}

void Prs2d_Point::SetMarker( const Aspect_TypeOfMarker aTOM ) {
  myTOM = aTOM;
}

void Prs2d_Point::SetIndex( const Standard_Integer anInd ) {
  myIndMark = anInd;    
}

gp_Pnt2d Prs2d_Point::Point() const {
  return myPoint;
}

Aspect_TypeOfMarker Prs2d_Point::Marker() const {
  return myTOM;
}

void Prs2d_Point::Save(Aspect_FStream& aFStream) const
{
}
