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

#include <Image_ColorImage.ixx>

#include <Image_ColorPixelDataMap.hxx>
#include <Image_DataMapIteratorOfColorPixelDataMap.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Quantity_Array1OfColor.hxx>
#include <Aspect_ColorMapEntry.hxx>

#ifdef TRACE
static int Verbose = 0 ;
#endif

Image_ColorImage::Image_ColorImage (
			const Standard_Integer x,
			const Standard_Integer y,
			const Standard_Integer dx,
			const Standard_Integer dy) 
	: Image_DColorImage( x, y, dx, dy, 
		Aspect_ColorPixel( Quantity_Color(0.,0.,0.,Quantity_TOC_RGB)) ) 

{
}

Image_ColorImage::Image_ColorImage (
			const Standard_Integer x,
			const Standard_Integer y,
			const Standard_Integer dx,
			const Standard_Integer dy,
			const Aspect_ColorPixel&  BackPixel ) 
			: Image_DColorImage( x, y, dx, dy, BackPixel ) 

{ 
}

Handle(Image_Image) Image_ColorImage::Dup() const {

  Handle(Image_ColorImage) aImage = 
		new Image_ColorImage( LowerX(), LowerY(), Width(), Height(),
					BackgroundPixel() ) ;

  aImage->InternalDup( this ) ;

  return aImage ;
}

Image_TypeOfImage Image_ColorImage::Type () const {

  return Image_TOI_ColorImage ;

}

const Quantity_Color& Image_ColorImage::PixelColor( 
			const Standard_Integer x,
			const Standard_Integer y ) const 

{ return( Pixel( x, y ).Value() ) ;
}

Handle(Aspect_GenericColorMap) Image_ColorImage::ChooseColorMap( 
			const Standard_Integer aSize ) const

{ Handle(Aspect_GenericColorMap) TheColorMap = new Aspect_GenericColorMap() ;
  Standard_Integer x,y,i,bblcount, UpX, UpY ;
  Image_ColorPixelDataMap DataMap( 4000 ) ; // a Big DataMap !!!
  Aspect_ColorPixel pix ;
  
  UpX = UpperX() ;
  UpY = UpperY() ;

  for ( y = LowerY() ; y <= UpY ; y++ ) {
	for ( x = LowerX() ; x <= UpX ; x++ ) {
		pix = Pixel(x,y) ;
		if ( !DataMap.IsBound( pix ) ) DataMap.Bind( pix, 1 ) ;
		else 			       DataMap( pix )++ ;
	}
  }

#ifdef TRACE
  Standard_Integer CheckSum = 0 ;

  if ( Verbose ) 
    cout << "ChooseColorMap : Extent " << DataMap.Extent() << endl << flush ;
#endif

  Image_DataMapIteratorOfColorPixelDataMap It( DataMap ) ;
  TColStd_Array1OfInteger Tab( 1, aSize ) ; Tab.Init(0) ;
  Quantity_Array1OfColor  Col( 1, aSize) ; Col.Init( 
				Quantity_Color( 0.,0.,0., Quantity_TOC_RGB ) ) ;
  Standard_Integer MaxOfTab = 0 ;
  Standard_Integer MinOfTab = 0 ;

  while ( It.More() ) {
	bblcount = It.Value() ;

#ifdef TRACE
  if ( Verbose ) {
	cout << CheckSum << "," << It.Key() << "," << bblcount << endl ;

	CheckSum += bblcount ;
  }
#endif

	if ( bblcount > MinOfTab ) {
		for ( i = 1 ; i <= aSize ; i++ ) {
			if ( Tab( i ) == MinOfTab ) {
				Tab( i ) = bblcount ;
				Col( i ) = It.Key().Value() ;
				break ;
			}
		}

		for ( i = 1 ; i <= aSize ; i++ ) {
			MaxOfTab = Max( MaxOfTab, Tab( i ) ) ;
			MinOfTab = Min( MinOfTab, Tab( i ) ) ;
		}

	}

	It.Next() ;
  }

  Aspect_ColorMapEntry aEntry ;

  for ( i = 1, bblcount = 0 ; i <= aSize ; i++ ) {
	if ( Tab( i ) != 0 ) {
		aEntry.SetValue( bblcount++, Col( i ) ) ;
		TheColorMap->AddEntry( aEntry ) ;
	}
  }

  cout << flush ;

  return TheColorMap ;
}

#ifdef OLD
Handle(Standard_Transient) Image_ColorImage::ShallowCopy() const {
	return DeepCopy() ;
}

Handle(Standard_Transient) Image_ColorImage::DeepCopy() const {

  return Dup() ;
}
#endif
