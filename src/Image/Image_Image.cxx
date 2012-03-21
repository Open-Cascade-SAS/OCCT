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

#include <Image_Image.ixx>

#ifdef TRACE
static int ImageCount = 0 ;
#endif

Image_Image::Image_Image ( const Handle(Standard_Type)& aPixelType )
{
#ifdef TRACE
  cout << "\tCreate a new Image ( Count : " << ++ImageCount << " )" << endl
       << flush ;
#endif

  myPixelType = aPixelType ;
}

Standard_Integer Image_Image::Size() const {

  return ( Width() * Height() ) ;

}

void Image_Image::InternalDup( const Handle(Image_Image)& aImage ) 
{ // myPixelType is set in Image_Image Constructor
}

Handle(Standard_Type) Image_Image::PixelType() const { return myPixelType ; }

void Image_Image::Destroy ()
{
#ifdef TRACE
  cout << "\tDelete an Image ( Count : " << --ImageCount << " )" << endl
       << flush ;
#endif
}

void Image_Image::RowColor( const Standard_Integer Y,
			    Quantity_Array1OfColor&  PR) const {

  Standard_Integer TheLength = Min (PR.Length(), Width() );
  Standard_Integer L         = PR.Lower() ;
  Standard_Integer X         = LowerX() ;

  for (Standard_Integer i=0; i< TheLength; i++) {
    PR(L+i) = PixelColor(X+i,Y);
  }

}

Handle(Quantity_HArray1OfColor) Image_Image::RowColor( 
		const Standard_Integer Y ) const {

  Standard_Integer TheLength = Width() ;
  Standard_Integer X         = LowerX() ;
  Handle(Quantity_HArray1OfColor) PR = 
		new Quantity_HArray1OfColor( 0, TheLength-1) ;

  for (Standard_Integer i=0; i< TheLength; i++) {
    PR->SetValue(i,PixelColor(X+i,Y));
  }

  return PR ;
}

