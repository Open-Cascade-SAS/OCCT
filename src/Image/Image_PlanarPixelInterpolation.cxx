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

#include <Image_PlanarPixelInterpolation.ixx>


Image_PlanarPixelInterpolation::Image_PlanarPixelInterpolation() {}

Standard_Boolean Image_PlanarPixelInterpolation::Interpolate( 
	const Handle(Image_Image)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_Pixel& aPixel ) const

{ 

  if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {

	return Interpolate( Handle(Image_DIndexedImage)::DownCast( aImage ),
			FX,FY,LowX,LowY,UpX,UpY,(Aspect_IndexPixel &)aPixel ) ;

  }
  else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {

	return Interpolate( Handle(Image_DColorImage)::DownCast( aImage ),
			FX,FY,LowX,LowY,UpX,UpY,(Aspect_ColorPixel &)aPixel ) ;
  }
  else {
	return Image_PixelInterpolation::Interpolate( aImage,
			FX,FY,LowX,LowY,UpX,UpY,aPixel ) ;
  }
}


static Standard_Real DoInterpolation(	const Standard_Integer NX[3],
					const Standard_Integer NY[3],
					const Standard_Real    NZ[3],
					const Standard_Real    FX,
					const Standard_Real    FY    )
	
{ Standard_Real VX[3], VY[3], VZ[3] ;
  Standard_Real Result ;

  if ( NZ[0] == NZ[1] && NZ[0] == NZ[2] ) {
	Result = NZ[0] ;
  }
  else {

		VX[1] = NX[1] - NX[0] ; 
		VY[1] = NY[1] - NY[0] ; 
		VZ[1] = NZ[1] - NZ[0] ;

		VX[2] = NX[2] - NX[0] ; 
		VY[2] = NY[2] - NY[0] ; 
		VZ[2] = NZ[2] - NZ[0] ;

		if ( VZ[1] == 0. && VZ[2] == 0. ) {
		  Result = NZ[0] ;
		}
		else {
		  VX[0] = VY[1]*VZ[2] - VY[2]*VZ[1] ;
		  VY[0] = VZ[1]*VX[2] - VZ[2]*VX[1] ;
		  VZ[0] = VX[1]*VY[2] - VX[2]*VY[1] ;

		  if ( VZ[0] != 0. ) {
		  	Result = NZ[0] - 
				( (FX-NX[0])*VX[0] + (FY-NY[0])*VY[0] ) / VZ[0];
		  }
		  else {
		  	Result = NZ[0] ;
		  }
		}
  }

  return Result ;

}

Standard_Boolean Image_PlanarPixelInterpolation::Interpolate( 
	const Handle(Image_DColorImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_ColorPixel& aPixel ) const

{ Standard_Integer NX[3], NY[3] ;
  Standard_Real    NZ[3] ;
  Standard_Real    R,G,B ;
  static Quantity_Color Col ;

  if ( FX < 0. ) NX[0] = Standard_Integer(FX-0.5) ;
  else           NX[0] = Standard_Integer(FX+0.5) ;

  if ( FY < 0. ) NY[0] = Standard_Integer(FY-0.5) ;
  else           NY[0] = Standard_Integer(FY+0.5) ;

  if ( NX[0] < LowX || NX[0] > UpX ||
       NY[0] < LowY || NY[0] > UpY ) {
	return Standard_False ;
  }
  else if ( ( FX-NX[0] ) == 0. && ( FY-NY[0] ) == 0. ) {
	aImage->Pixel( NX[0], NY[0], aPixel );
	return Standard_True ;
  }
  else {

	if ( ( FX-NX[0] ) >= 0. )	{ NX[1] = NX[0]+1 ; NY[1] = NY[0] ; }
	else				{ NX[1] = NX[0]-1 ; NY[1] = NY[0] ; }
	if ( ( FY-NY[0] ) >= 0. )	{ NX[2] = NX[0]   ; NY[2] = NY[0]+1 ; }
	else				{ NX[2] = NX[0]   ; NY[2] = NY[0]-1 ; }

	if ( NX[1] < LowX || NX[1] > UpX || NY[1] < LowY || NY[1] > UpY ||
	     NX[2] < LowX || NX[2] > UpX || NY[2] < LowY || NY[2] > UpY ) {
	  	aImage->Pixel( NX[0], NY[0], aPixel );
	}
	else {
		NZ[0] = aImage->Pixel( NX[0],NY[0] ).Value().Red() ;
		NZ[1] = aImage->Pixel( NX[1],NY[1] ).Value().Red() ;
		NZ[2] = aImage->Pixel( NX[2],NY[2] ).Value().Red() ;

		R = DoInterpolation( NX,NY,NZ, FX,FY ) ;

		NZ[0] = aImage->Pixel( NX[0],NY[0] ).Value().Green() ;
		NZ[1] = aImage->Pixel( NX[1],NY[1] ).Value().Green() ;
		NZ[2] = aImage->Pixel( NX[2],NY[2] ).Value().Green() ;

		G = DoInterpolation( NX,NY,NZ, FX,FY ) ;

		NZ[0] = aImage->Pixel( NX[0],NY[0] ).Value().Blue() ;
		NZ[1] = aImage->Pixel( NX[1],NY[1] ).Value().Blue() ;
		NZ[2] = aImage->Pixel( NX[2],NY[2] ).Value().Blue() ;

		B = DoInterpolation( NX,NY,NZ, FX,FY ) ;

		Col.SetValues(  R, G, B, Quantity_TOC_RGB ) ;

		aPixel.SetValue( Col ) ;
	}

	return Standard_True ;
  }
}

Standard_Boolean Image_PlanarPixelInterpolation::Interpolate( 
	const Handle(Image_DIndexedImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_IndexPixel& aPixel ) const

{ Standard_Integer NX[3], NY[3] ;
  Standard_Real    NZ[3] ;


  if ( FX < 0. ) NX[0] = Standard_Integer(FX-0.5) ;
  else           NX[0] = Standard_Integer(FX+0.5) ;

  if ( FY < 0. ) NY[0] = Standard_Integer(FY-0.5) ;
  else           NY[0] = Standard_Integer(FY+0.5) ;

  if ( NX[0] < LowX || NX[0] > UpX ||
       NY[0] < LowY || NY[0] > UpY ) {
	return Standard_False ;
  }
  else if ( ( FX-NX[0] ) == 0. && ( FY-NY[0] ) == 0. ) {
	aImage->Pixel( NX[0], NY[0], aPixel );
	return Standard_True ;
  }
  else {

	if ( ( FX-NX[0] ) >= 0. )	{ NX[1] = NX[0]+1 ; NY[1] = NY[0] ; }
	else				{ NX[1] = NX[0]-1 ; NY[1] = NY[0] ; }
	if ( ( FY-NY[0] ) >= 0. )	{ NX[2] = NX[0]   ; NY[2] = NY[0]+1 ; }
	else				{ NX[2] = NX[0]   ; NY[2] = NY[0]-1 ; }

	if ( NX[1] < LowX || NX[1] > UpX || NY[1] < LowY || NY[1] > UpY ||
	     NX[2] < LowX || NX[2] > UpX || NY[2] < LowY || NY[2] > UpY ) {
	  	aImage->Pixel( NX[0], NY[0], aPixel );
	}
	else {
		NZ[0] = aImage->Pixel( NX[0],NY[0] ).Value() ;
		NZ[1] = aImage->Pixel( NX[1],NY[1] ).Value() ;
		NZ[2] = aImage->Pixel( NX[2],NY[2] ).Value() ;

		aPixel.SetValue( 
			Standard_Integer( DoInterpolation( NX,NY,NZ, FX,FY ) )
			       ) ;

	}

	return Standard_True ;
  }
}

//##############################################################################

#ifdef OLD
{ Standard_Integer NX[3], NY[3], NZ[3] ;
  Standard_Integer Result ;
  Standard_Real VX[3], VY[3], VZ[3], PVALUE ;

  NX[0] = Standard_Integer(FX+0.5) ;
  NY[0] = Standard_Integer(FY+0.5) ;

  if ( NX[0] < LowX || NX[0] > UpX ||
       NY[0] < LowY || NY[0] > UpY ) {
	return Standard_False ;
  }
  else if ( ( FX-NX[0] ) == 0. && ( FY-NY[0] ) == 0. ) {
	aImage->Pixel( NX[0], NY[0], aPixel );
	return Standard_True ;
  }
  else {

	if ( ( FX-NX[0] ) >= 0. )	{ NX[1] = NX[0]+1 ; NY[1] = NY[0] ; }
	else				{ NX[1] = NX[0]-1 ; NY[1] = NY[0] ; }
	if ( ( FY-NY[0] ) >= 0. )	{ NX[2] = NX[0]   ; NY[2] = NY[0]+1 ; }
	else				{ NX[2] = NX[0]   ; NY[2] = NY[0]-1 ; }

	if ( NX[1] < LowX || NX[1] > UpX || NY[1] < LowY || NY[1] > UpY ||
	     NX[2] < LowX || NX[2] > UpX || NY[2] < LowY || NY[2] > UpY ) {
	  aImage->Pixel( NX[0], NY[0], aPixel );
	}
	else {
		NZ[0] = aImage->Pixel( NX[0],NY[0] ).Value() ;
		NZ[1] = aImage->Pixel( NX[1],NY[1] ).Value() ;
		NZ[2] = aImage->Pixel( NX[2],NY[2] ).Value() ;
		
		VX[1] = NX[1] - NX[0] ; 
		VY[1] = NY[1] - NY[0] ; 
		VZ[1] = NZ[1] - NZ[0] ;

		VX[2] = NX[2] - NX[0] ; 
		VY[2] = NY[2] - NY[0] ; 
		VZ[2] = NZ[2] - NZ[0] ;

		if ( VZ[1] == 0. && VZ[2] == 0. ) {
		  Result = NZ[0] ;
		}
		else {
		  VX[0] = VY[1]*VZ[2] - VY[2]*VZ[1] ;
		  VY[0] = VZ[1]*VX[2] - VZ[2]*VX[1] ;
		  VZ[0] = VX[1]*VY[2] - VX[2]*VY[1] ;

		  if ( VZ[0] != 0. ) {
		  	PVALUE = NZ[0] - 
				( (FX-NX[0])*VX[0] + (FY-NY[0])*VY[0] ) / VZ[0];
		  	Result = Standard_Integer( PVALUE ) ;
		  }
		  else {
		  	Result = NZ[0] ;
		  }
		}

		aPixel.SetValue( Result ) ;
	}

	return Standard_True ;
  }
}
#endif
