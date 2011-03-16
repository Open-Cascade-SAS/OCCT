#include <Image_AveragePixelInterpolation.ixx>


Image_AveragePixelInterpolation::Image_AveragePixelInterpolation() {}

Standard_Boolean Image_AveragePixelInterpolation::Interpolate( 
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

Standard_Boolean Image_AveragePixelInterpolation::Interpolate( 
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
  Standard_Boolean SamePixels = 1 ;

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

		if ( NZ[0] == NZ[1] && NZ[0] == NZ[2] ) {
			R = NZ[0] ;
		}
		else {
			R = ( NZ[0] + NZ[1] + NZ[2] ) / 3. ;
			SamePixels = 0 ;
		}

		NZ[0] = aImage->Pixel( NX[0],NY[0] ).Value().Green() ;
		NZ[1] = aImage->Pixel( NX[1],NY[1] ).Value().Green() ;
		NZ[2] = aImage->Pixel( NX[2],NY[2] ).Value().Green() ;

		if ( NZ[0] == NZ[1] && NZ[0] == NZ[2] ) {
			G = NZ[0] ;
		}
		else {
			G = ( NZ[0] + NZ[1] + NZ[2] ) / 3. ;
			SamePixels = 0 ;
		}

		NZ[0] = aImage->Pixel( NX[0],NY[0] ).Value().Blue() ;
		NZ[1] = aImage->Pixel( NX[1],NY[1] ).Value().Blue() ;
		NZ[2] = aImage->Pixel( NX[2],NY[2] ).Value().Blue() ;

		if ( NZ[0] == NZ[1] && NZ[0] == NZ[2] ) {
			B = NZ[0] ;
		}
		else {
			B = ( NZ[0] + NZ[1] + NZ[2] ) / 3. ;
			SamePixels = 0 ;
		}

		if ( SamePixels ) {
			aPixel.SetValue( aImage->Pixel( NX[0],NY[0] ).Value() );
		}
		else {
		  	Col.SetValues(  R, G, B, Quantity_TOC_RGB ) ;

		  	aPixel.SetValue( Col ) ;
		}
	}

	return Standard_True ;
  }
}

Standard_Boolean Image_AveragePixelInterpolation::Interpolate( 
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

		if ( NZ[0] == NZ[1] && NZ[0] == NZ[2] ) {
		  aPixel.SetValue( Standard_Integer( NZ[0] ) ) ;
		}
		else {
		  aPixel.SetValue( Standard_Integer((NZ[0]+NZ[1]+NZ[2])/3.) ) ;
		}

	}

	return Standard_True ;
  }
}

//##############################################################################

#ifdef OLD
Standard_Boolean Image_AveragePixelInterpolation::Interpolate( 
	const Handle(Image_DColorImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_ColorPixel& aPixel ) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X,Y ;
  Standard_Real SD, SR, SG, SB, R, G, B ;

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	if ( FX < 0. ) NX-- ;
	if ( FY < 0. ) NY-- ;

	SR = SG = SB = SD = 0. ;

	// (0,0)
	X = NX ; Y = NY ;

	if ( !( X < LowX || X > UpX || 
	        Y < LowY || Y > UpY ) ) {
	  aImage->Pixel( X,Y ).Value().Values( R, G, B, Quantity_TOC_RGB ) ;

	  SR += R ; SG += G ; SB += B ; SD += 1. ;
	}

	// (1,0)
	X = NX+1 ; Y = NY ;

	if ( !( X < LowX || X > UpX || Y < LowY || Y > UpY ) ) {
	  aImage->Pixel( X,Y ).Value().Values( R, G, B, Quantity_TOC_RGB ) ;

	  SR += R ; SG += G ; SB += B ; SD += 1. ;
	}

	// (0,1)
	X = NX ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
	        Y < LowY || Y > UpY ) ) {
	  aImage->Pixel( X,Y ).Value().Values( R, G, B, Quantity_TOC_RGB ) ;

	  SR += R ; SG += G ; SB += B ; SD += 1. ;
	}

	// (1,1)
	X = NX+1 ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
	        Y < LowY || Y > UpY ) ) {
	  aImage->Pixel( X,Y ).Value().Values( R, G, B, Quantity_TOC_RGB ) ;

	  SR += R ; SG += G ; SB += B ; SD += 1. ;
	}

	// Result
	if ( SD != 0. ) {
	  SR /= SD ; SG /= SD ; SB /= SD ;

	  aPixel.SetValue( Quantity_Color( SR, SG, SB, Quantity_TOC_RGB ) ) ;

	  return Standard_True ;
	}
	else {
	  return Standard_False ;
	}
  }
}

Standard_Boolean Image_AveragePixelInterpolation::Interpolate( 
	const Handle(Image_DIndexedImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_IndexPixel& aPixel ) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X,Y ;
  Standard_Real SD, SP ;

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	if ( FX < 0. ) NX-- ;
	if ( FY < 0. ) NY-- ;

	SP = SD = 0. ;

	// (0,0)
	X = NX ; Y = NY ;

	if ( !( X < LowX || X > UpX || 
	        Y < LowY || Y > UpY ) ) {
	  SP += aImage->Pixel( X,Y ).Value() ; SD += 1. ;
	}

	// (1,0)
	X = NX+1 ; Y = NY ;

	if ( !( X < LowX || X > UpX || 
	        Y < LowY || Y > UpY ) ) {
	  SP += aImage->Pixel( X,Y ).Value() ; SD += 1. ;
	}

	// (0,1)
	X = NX ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
	        Y < LowY || Y > UpY ) ) {
	  SP += aImage->Pixel( X,Y ).Value() ; SD += 1. ;
	}

	// (1,1)
	X = NX+1 ; Y = NY+1 ;

	if ( !( X < LowX || X > UpX || 
	        Y < LowY || Y > UpY ) ) {
	  SP += aImage->Pixel( X,Y ).Value() ; SD += 1. ;
	}

	// Result

	if ( SD != 0. ) {
	  SP /= SD ;

	  aPixel.SetValue( Standard_Integer(SP+0.5) ) ;

	  return Standard_True ;
	}
	else {
	  return Standard_False ;
	}
  }
}
#endif
