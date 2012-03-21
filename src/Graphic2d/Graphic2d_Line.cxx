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

// Modified     23/02/98 : FMN ; Remplacement PI par Standard_PI

//		10/11/98 : GG ; Protection sur methode IsOn() lorsque
//				les points sont confondus.

#define G002     //GG_100500
//              Change IsOn method with a best computation on short segments.

#include <Graphic2d_Line.ixx>

Graphic2d_Line::Graphic2d_Line (const Handle(Graphic2d_GraphicObject)& aGraphicObject)
         :Graphic2d_Primitive (aGraphicObject),
	  myTypeOfPolygonFilling (Graphic2d_TOPF_EMPTY),
          myDrawEdge (Standard_True),
          myWidthIndex	(0),
	  myTypeIndex	(0),
          myPatternIndex (0),
          myInteriorColorIndex (1)  {

	SetFamily(Graphic2d_TOP_LINE);
}

void Graphic2d_Line::SetWidthIndex (const Standard_Integer anIndex) {

	myWidthIndex	= anIndex;
	ResetIndex ();

}

void Graphic2d_Line::SetTypeIndex (const Standard_Integer anIndex) {

	myTypeIndex	= anIndex;
	ResetIndex ();

}

void Graphic2d_Line::SetInteriorColorIndex (const Standard_Integer anIndex) {

	myInteriorColorIndex	= anIndex;
	ResetIndex ();

}

void Graphic2d_Line::SetDrawEdge (const Standard_Boolean aDraw) {

	myDrawEdge = aDraw;
	ResetIndex ();

}

void Graphic2d_Line::SetInteriorPattern (const Standard_Integer anIndex) {
  
	myPatternIndex = anIndex;
	ResetIndex ();

}

void Graphic2d_Line::SetTypeOfPolygonFilling (const Graphic2d_TypeOfPolygonFilling aType) {

	myTypeOfPolygonFilling = aType;
	ResetIndex ();

}

Standard_Integer Graphic2d_Line::WidthIndex () const { 

	return myWidthIndex;

}

Standard_Integer Graphic2d_Line::InteriorColorIndex () const { 

	return myInteriorColorIndex;

}

Standard_Integer Graphic2d_Line::InteriorPattern () const { 

	return myPatternIndex;

}

Graphic2d_TypeOfPolygonFilling Graphic2d_Line::TypeOfPolygonFilling () const { 

	return myTypeOfPolygonFilling;

}

Standard_Integer Graphic2d_Line::TypeIndex () const { 

	return myTypeIndex;

}

void Graphic2d_Line::DrawLineAttrib (const Handle(Graphic2d_Drawer)& aDrawer) 
 const {

	aDrawer->SetLineAttrib (myColorIndex,myTypeIndex,myWidthIndex);

	switch (myTypeOfPolygonFilling) {
		case Graphic2d_TOPF_FILLED:
		aDrawer->SetPolyAttrib (myInteriorColorIndex,0,myDrawEdge);
		break;
		case Graphic2d_TOPF_PATTERNED:
		aDrawer->SetPolyAttrib (myInteriorColorIndex,
						myPatternIndex,myDrawEdge);
		break;
		default: break;
	}

}

void Graphic2d_Line::DrawMarkerAttrib (const Handle(Graphic2d_Drawer)& aDrawer) 
 const {

Standard_Boolean filled = (myTypeOfPolygonFilling != Graphic2d_TOPF_EMPTY);

	switch (myTypeOfPolygonFilling) {
		case Graphic2d_TOPF_FILLED:
		aDrawer->SetPolyAttrib (myInteriorColorIndex,0,Standard_False);
		break;
		case Graphic2d_TOPF_PATTERNED:
		aDrawer->SetPolyAttrib (myInteriorColorIndex,myPatternIndex,Standard_False);
		break;
		default: break;
	}

	aDrawer->SetMarkerAttrib (myColorIndex,myWidthIndex,filled);

}

Standard_Boolean Graphic2d_Line::IsIn ( const Standard_ShortReal aX,
					const Standard_ShortReal aY,
					const TShort_Array1OfShortReal&  X,
					const TShort_Array1OfShortReal&  Y,
					const Standard_ShortReal aPrecision)
{ 
  Standard_Integer i1=0,i2=0,n;
  Standard_Real dx1,dy1,dx2,dy2,anglesum=0.,angle;
  Standard_Real prosca,provec,norme1,norme2,cosin;
  n = X.Length ();
  for (Standard_Integer m = 1; m <= n; m++) {
    i1++; 
    i2 = (i1 == n) ? 1 : i1 + 1;
    dx1 = X(i1)  - aX; dy1 = Y(i1) - aY;
    dx2 = X(i2)  - aX; dy2 = Y(i2) - aY;
    prosca = dx1 * dx2 + dy1 * dy2;
    provec = dx1 * dy2 - dx2 * dy1;    
    norme1  = Sqrt ( dx1 * dx1 + dy1 * dy1 );
    norme2  = Sqrt ( dx2 * dx2 + dy2 * dy2 );
    if ( norme1 <= aPrecision || norme2 <= aPrecision ) return Standard_True;

    cosin = prosca / norme1 / norme2;
    if ( cosin >= 1 ) angle = 0.;
    else {
      if ( cosin <= -1) angle = - M_PI;
      else
	angle = Sign ( ACos ( cosin ) , provec );
	}
    anglesum = anglesum + angle;}
    return (Abs (anglesum) > 1.) ;
}

Standard_Boolean Graphic2d_Line::IsOn ( const Standard_ShortReal aX,
					const Standard_ShortReal aY,
					const Standard_ShortReal aX1,
					const Standard_ShortReal aY1,
					const Standard_ShortReal aX2,
					const Standard_ShortReal aY2,
					const Standard_ShortReal aPrecision)
{

Standard_ShortReal DX = aX2 - aX1, DY = aY2 - aY1, dd = DX*DX + DY*DY;


#ifdef G002
        if( Sqrt(dd) < aPrecision )
#else
	if( dd < aPrecision )
#endif
	  return (Abs (aX - aX1) + Abs (aY - aY1)) < aPrecision;

Standard_ShortReal lambda = (DX*(aX-aX1) + DY*(aY-aY1)) / dd;

	if ( lambda >= 0. && lambda <= 1. ) {

		//  On prend comme norme la somme des valeurs absolues:
		Standard_ShortReal Xproj = aX1 + lambda * DX;
		Standard_ShortReal Yproj = aY1 + lambda * DY;
		return (Abs (aX - Xproj) + Abs (aY - Yproj)) < aPrecision;

	}
	else
		return Standard_False;
}

void Graphic2d_Line::Save(Aspect_FStream& aFStream) const
{
	*aFStream << myColorIndex << ' ' << myWidthIndex << endl;
	*aFStream << myTypeIndex << ' ' << myPatternIndex << endl;
	*aFStream << myInteriorColorIndex << ' ' << myTypeOfPolygonFilling << ' ' << myDrawEdge << endl;
}

void Graphic2d_Line::Retrieve(Aspect_IFStream& anIFStream)
{
	int topf;
	*anIFStream >> myColorIndex >> myWidthIndex;
	*anIFStream >> myTypeIndex >> myPatternIndex;
	*anIFStream >> myInteriorColorIndex;
	*anIFStream >> topf;
	myTypeOfPolygonFilling=Graphic2d_TypeOfPolygonFilling(topf);
	*anIFStream >> myDrawEdge;
}

