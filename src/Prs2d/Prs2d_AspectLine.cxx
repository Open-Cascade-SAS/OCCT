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

#include <Prs2d_AspectLine.ixx>
#include <Aspect_LineStyle.hxx>
#include <Aspect_GenericColorMap.hxx> 
#include <Aspect_TypeMap.hxx>
#include <Aspect_WidthMap.hxx>

Prs2d_AspectLine::Prs2d_AspectLine()
  :
   Prs2d_AspectRoot( Prs2d_AN_LINE ),

   myColor( Quantity_Color(Quantity_NOC_YELLOW) ),
   myType( Aspect_TOL_SOLID ),
   myWidth( Aspect_WOL_THIN ),
   myInterColor( Quantity_Color(Quantity_NOC_YELLOW) ),
   myFillType( Graphic2d_TOPF_EMPTY ),
   myTile( 0 ),
   myDrawEdge( Standard_True ),
   myColorIndex( 0 ),
   myTypeIndex( 0 ),
   myWidthIndex( 0 ),
   myIntColorInd( 0 )

{
}

Prs2d_AspectLine::Prs2d_AspectLine( 
				const Quantity_NameOfColor aColor, 
     			const Aspect_TypeOfLine aType,
				const Aspect_WidthOfLine aWidth,
                const Quantity_NameOfColor aIntColor,
                const Graphic2d_TypeOfPolygonFilling aTypeFill,
         	    const Standard_Integer aTile,
                const Standard_Boolean aDrawEdge ) 
 :
   Prs2d_AspectRoot( Prs2d_AN_LINE ),

   myColor( Quantity_Color(aColor) ),
   myType( aType ),
   myWidth( aWidth ),
   myInterColor( Quantity_Color(aIntColor) ),
   myFillType( aTypeFill ),
   myTile( aTile ),
   myDrawEdge( aDrawEdge ),
   myColorIndex( 0 ),
   myTypeIndex( 0 ),
   myWidthIndex( 0 ),
   myIntColorInd( 0 )
{
}

Prs2d_AspectLine::Prs2d_AspectLine( 
				const Quantity_Color& aColor, 
     			const Aspect_TypeOfLine aType,
				const Aspect_WidthOfLine aWidth,
                const Quantity_Color& aIntColor,
                const Graphic2d_TypeOfPolygonFilling aTypeFill,
         	    const Standard_Integer aTile,
                const Standard_Boolean aDrawEdge )
 :
   Prs2d_AspectRoot( Prs2d_AN_LINE ),

   myColor( aColor ),
   myType( aType ),
   myWidth( aWidth ),
   myInterColor( aIntColor ),
   myFillType( aTypeFill ),
   myTile( aTile ),
   myDrawEdge( aDrawEdge ),
   myColorIndex(0),
   myTypeIndex(0),
   myWidthIndex(0),
   myIntColorInd( 0 )
{
}

void Prs2d_AspectLine::SetColor(const Quantity_NameOfColor aColor) {

	 SetColor( Quantity_Color(aColor) );
}

void Prs2d_AspectLine::SetColor(const Quantity_Color& aColor) {

	if ( myColor != aColor ) {
       myColor = aColor;
       myColorIndex = 0;
    }
}

void Prs2d_AspectLine::SetType(const Aspect_TypeOfLine aType) {

	if ( myType != aType ) {
       myType = aType;
       myTypeIndex = 0;
    }
}

void Prs2d_AspectLine::SetWidth(const Aspect_WidthOfLine aWidth) {

	if ( myWidth != aWidth ) {
       myWidth = aWidth;
       myWidthIndex = 0;
    }
}

void Prs2d_AspectLine::SetInterColor(const Quantity_NameOfColor aColor) {

	 SetInterColor( Quantity_Color(aColor) );
}

void Prs2d_AspectLine::SetInterColor(const Quantity_Color& aColor) {

	if ( myInterColor != aColor ) {
       myInterColor = aColor;
       myIntColorInd = 0;
    }
}

void Prs2d_AspectLine::SetTypeOfFill( const Graphic2d_TypeOfPolygonFilling aType ) {
    myFillType = aType;
}

void Prs2d_AspectLine::SetTile( const Standard_Integer aTile ) {
	 myTile = aTile;
}

void Prs2d_AspectLine::SetDrawEdge( const Standard_Boolean aDrawEdge ) {
	  myDrawEdge = aDrawEdge;
}

void Prs2d_AspectLine::ValuesOfLine( Quantity_Color& aColor,
	                                 Aspect_TypeOfLine& aType,
	                                 Aspect_WidthOfLine& aWidth) const{
	aColor = myColor;
	aType  = myType;
	aWidth = myWidth;

}

void Prs2d_AspectLine::ValuesOfPoly( Quantity_Color& aColor,
                                     Graphic2d_TypeOfPolygonFilling& aType,
	                                 Standard_Integer& aTile,
	                                 Standard_Boolean& aDrawEdge ) const {
    aColor    = myInterColor;
    aType     = myFillType;
	aTile     = myTile;
	aDrawEdge = myDrawEdge;
}

Standard_Integer Prs2d_AspectLine::ColorIndex() const {

  return myColorIndex;
}

Standard_Integer Prs2d_AspectLine::TypeIndex() const {

  return myTypeIndex;
}

Standard_Integer Prs2d_AspectLine::WidthIndex() const {

   return myWidthIndex;
}

Standard_Integer Prs2d_AspectLine::InterColorIndex() const {

  return myIntColorInd;
}

void Prs2d_AspectLine::SetColorIndex( const Standard_Integer anInd )  {
  
	myColorIndex = anInd;
}

void Prs2d_AspectLine::SetTypeIndex( const Standard_Integer anInd )  {

    myTypeIndex = anInd;
}

void Prs2d_AspectLine::SetWidthIndex( const Standard_Integer anInd )  {
 
	myWidthIndex = anInd;
}

void Prs2d_AspectLine::SetIntColorInd( const Standard_Integer anInd )  {
  
	myIntColorInd = anInd;
}
