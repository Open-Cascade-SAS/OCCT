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

/***********************************************************************

     FONCTION :
     ----------
        Fichier Image_BilinearPixelInterpolation.cxx :

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      --------  : BBL ; Creation
      31-12-97  : CAL ; Retrait de la dependance avec math. Calcul developpe.

************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Constantes
 */

#ifdef TRACE
static int Verbose = 0 ;
#endif

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <Aspect.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <Image_BilinearPixelInterpolation.ixx>

Image_BilinearPixelInterpolation::Image_BilinearPixelInterpolation() {}


Standard_Boolean Image_BilinearPixelInterpolation::Interpolate( 
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

static Standard_Real DoInterpolation(	const TColStd_Array1OfReal&    NXF,
					const TColStd_Array1OfReal&    NYF,
					const TColStd_Array1OfReal&    NZF,
					const Standard_Real    FX,
					const Standard_Real    FY    )
	
{ Standard_Real Result ;

  if ( NZF(1) == NZF(2) && NZF(2) == NZF(3) ) {
	Result = NZF(1) ;
  }
  else {
  	Standard_Integer i;
  	Standard_Boolean BResult;
  	TColStd_Array1OfReal V(1,4), R(1,4) ;
  	TColStd_Array2OfReal M( 1, 4, 1, 4 ) ;
  	TColStd_Array2OfReal MINV( 1, 4, 1, 4 ) ;

  	for ( i = 1 ; i <= 4 ; i++ ) {
		M(i,1) = NXF(i) ; M( i,2 ) = NYF(i) ; M( i,3 ) = NXF(i)*NYF(i) ;
		M(i,4) = 1. ;
		V(i)   = NZF(i) ;
  	}

  	BResult = Aspect::Inverse (M, MINV);

  	// R = M * V ;
	R (1)	= M (1, 1) * V (1) + M (1, 2) * V (2)
		+ M (1, 3) * V (3) + M (1, 4) * V (4);
	R (2)	= M (2, 1) * V (1) + M (2, 2) * V (2)
		+ M (2, 3) * V (3) + M (2, 4) * V (4);
	R (3)	= M (3, 1) * V (1) + M (3, 2) * V (2)
		+ M (3, 3) * V (3) + M (3, 4) * V (4);
	R (4)	= M (4, 1) * V (1) + M (4, 2) * V (2)
		+ M (4, 3) * V (3) + M (4, 4) * V (4);

  	Result = R(1)*FX + R(2)*FY + R(3)*FX*FY + R(4) ;

  }

  return Result ;
}

Standard_Boolean Image_BilinearPixelInterpolation::Interpolate( 
	const Handle(Image_DColorImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_ColorPixel& aPixel) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X,Y ;
  TColStd_Array1OfReal   NXF(1,4), NYF(1,4), NZFR(1,4), NZFG(1,4), NZFB(1,4);

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	if ( FX < 0. ) NX-- ;
	if ( FY < 0. ) NY-- ;

	// (0,0)
	X = NX ; Y = NY ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(1)  = Standard_Real( X ) ;
		NYF(1)  = Standard_Real( Y ) ;
		NZFR(1) = aImage->Pixel( X, Y ).Value().Red() ;
		NZFG(1) = aImage->Pixel( X, Y ).Value().Green() ;
		NZFB(1) = aImage->Pixel( X, Y ).Value().Blue() ;
	}
	else {
		return Standard_False ;
	}

	// (1,0)
	X = NX+1 ; Y = NY ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(2)  = Standard_Real( X ) ;
		NYF(2)  = Standard_Real( Y ) ;
		NZFR(2) = aImage->Pixel( X, Y ).Value().Red() ;
		NZFG(2) = aImage->Pixel( X, Y ).Value().Green() ;
		NZFB(2) = aImage->Pixel( X, Y ).Value().Blue() ;
	}
	else {
		return Standard_False ;
	}

	// (0,1)
	X = NX ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(3)  = Standard_Real( X ) ;
		NYF(3)  = Standard_Real( Y ) ;
		NZFR(3) = aImage->Pixel( X, Y ).Value().Red() ;
		NZFG(3) = aImage->Pixel( X, Y ).Value().Green() ;
		NZFB(3) = aImage->Pixel( X, Y ).Value().Blue() ;
	}
	else {
		return Standard_False ;
	}

	// (1,1)
	X = NX+1 ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(4) = Standard_Real( X ) ;
		NYF(4) = Standard_Real( Y ) ;
		NZFR(4) = aImage->Pixel( X, Y ).Value().Red() ;
		NZFG(4) = aImage->Pixel( X, Y ).Value().Green() ;
		NZFB(4) = aImage->Pixel( X, Y ).Value().Blue() ;
	}
	else {
		return Standard_False ;
	}

	// Result
	Quantity_Color Res;
	Standard_Real R,G,B ;

	R = DoInterpolation(NXF,NYF,NZFR,FX,FY );
	G = DoInterpolation(NXF,NYF,NZFG,FX,FY );
	B = DoInterpolation(NXF,NYF,NZFB,FX,FY );

	// Some times we get number like -0.3412341234e-14
#ifdef TRACE
  if ( Verbose ) 
	if ( R < 0. || R > 1. || G < 0. || G > 1. || B < 0. || B > 1. ) {
	  cout << "\tError in BilinearPixelInterpolation " <<
	    R << " " << G << " " << B << endl << flush ;
	}
#endif
	if ( R < 0. && R > -0.000001 ) R = 0. ;
	if ( G < 0. && G > -0.000001 ) G = 0. ;
	if ( B < 0. && B > -0.000001 ) B = 0. ;

	Res.SetValues( R,G,B, Quantity_TOC_RGB ) ;

	aPixel.SetValue( Res );

	return Standard_True ;
  }
}

Standard_Boolean Image_BilinearPixelInterpolation::Interpolate( 
	const Handle(Image_DIndexedImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_IndexPixel& aPixel) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X,Y ;
  TColStd_Array1OfReal   NXF(1,4), NYF(1,4), NZF(1,4);

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	if ( FX < 0. ) NX-- ;
	if ( FY < 0. ) NY-- ;

	// (0,0)
	X = NX ; Y = NY ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(1) = Standard_Real( X ) ;
		NYF(1) = Standard_Real( Y ) ;
		NZF(1) = Standard_Real( aImage->Pixel( X, Y ).Value() ) ;
	}
	else {
		return Standard_False ;
	}

	// (1,0)
	X = NX+1 ; Y = NY ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(2) = Standard_Real( X ) ;
		NYF(2) = Standard_Real( Y ) ;
		NZF(2) = Standard_Real( aImage->Pixel( X, Y ).Value() ) ;
	}
	else {
		return Standard_False ;
	}

	// (0,1)
	X = NX ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(3) = Standard_Real( X ) ;
		NYF(3) = Standard_Real( Y ) ;
		NZF(3) = Standard_Real( aImage->Pixel( X, Y ).Value() ) ;
	}
	else {
		return Standard_False ;
	}

	// (1,1)
	X = NX+1 ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
		Y < LowY || Y > UpY ) ) {
		NXF(4) = Standard_Real( X ) ;
		NYF(4) = Standard_Real( Y ) ;
		NZF(4) = Standard_Real( aImage->Pixel( X, Y ).Value() ) ;
	}
	else {
		return Standard_False ;
	}

	// Result

	aPixel.SetValue( Standard_Integer(DoInterpolation(NXF,NYF,NZF,FX,FY )));

	return Standard_True ;
  }
}

