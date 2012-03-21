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

#include <Image_PixelInterpolation.ixx>

Image_PixelInterpolation::Image_PixelInterpolation() {}

Standard_Boolean Image_PixelInterpolation::DoInterpolate( 
	const Handle(Image_Image)& aImage,
	const Standard_Real X, const Standard_Real Y,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_Pixel& aPixel ) const

{ Standard_Integer NX ; 
  Standard_Integer NY ; 

  if ( X < 0. ) NX = Standard_Integer(X-0.5) ;
  else		NX = Standard_Integer(X+0.5) ;

  if ( Y < 0. ) NY = Standard_Integer(Y-0.5) ;
  else          NY = Standard_Integer(Y+0.5) ;

  if ( NX < LowX || NX > UpX ||
       NY < LowY || NY > UpY ) {
	return Standard_False ;
  }
  else {
	aImage->Pixel( NX, NY, aPixel );
	return Standard_True ;
  }
}

Standard_Boolean Image_PixelInterpolation::Interpolate( 
	const Handle(Image_Image)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_Pixel& aPixel ) const

{ return DoInterpolate( aImage, FX, FY, LowX, LowY, UpX, UpY, aPixel ) ; }

Standard_Boolean Image_PixelInterpolation::Interpolate( 
	const Handle(Image_DColorImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_ColorPixel& aPixel ) const

{ return DoInterpolate( aImage, FX, FY, LowX, LowY, UpX, UpY, aPixel ) ; }

Standard_Boolean Image_PixelInterpolation::Interpolate( 
	const Handle(Image_DIndexedImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_IndexPixel& aPixel ) const

{ return DoInterpolate( aImage, FX, FY, LowX, LowY, UpX, UpY, aPixel ) ; }

