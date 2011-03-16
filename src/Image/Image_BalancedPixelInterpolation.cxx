#include <Image_BalancedPixelInterpolation.ixx>


Image_BalancedPixelInterpolation::Image_BalancedPixelInterpolation() {}

Standard_Boolean Image_BalancedPixelInterpolation::Interpolate( 
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

Standard_Boolean Image_BalancedPixelInterpolation::Interpolate( 
	const Handle(Image_DColorImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_ColorPixel& aPixel ) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X[4],Y[4], i ;
  Standard_Real R[4], G[4], B[4];
  Standard_Boolean DoV[4], SamePixels = 1 ;
  Standard_Real SR, SG, SB;

  if ( FX < 0. ) NX-- ;
  if ( FY < 0. ) NY-- ;

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	for ( i = 0 ; i < 4 ; i++ ) DoV[i] = 0 ;

	// (0,0)
	i = 0; X[i] = NX ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	aImage->Pixel(X[i],Y[i]).Value().
				Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  	DoV[i] = 1 ;
	}
	else {
		return Standard_False ;
	}

	// (1,0)
	i++ ; X[i] = NX+1 ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	aImage->Pixel(X[i],Y[i]).Value().
				Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  	DoV[i] = 1 ;
	}
	else {
		R[i] = R[0] ; G[i] = G[0] ; B[i] = B[0] ;
	}

	// (0,1)
	i++ ; X[i] = NX ; Y[i] = NY+1 ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	aImage->Pixel(X[i],Y[i]).Value().
				Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  	DoV[i] = 1 ;
	}
	else {
		R[i] = R[0] ; G[i] = G[0] ; B[i] = B[0] ;
	}


	// (1,1)
	i++ ; X[i] = NX+1 ; Y[i] = NY+1 ; 

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	aImage->Pixel(X[i],Y[i]).Value().
				Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  	DoV[i] = 1 ;
	}
	else {
		R[i] = R[0] ; G[i] = G[0] ; B[i] = B[0] ;
	}

//	  Pixel A [0]			Pixel B [1]
//			Pixel X
//	  Pixel C [2]			Pixel D [3]


	Standard_Boolean First ;
	Standard_Integer Ref ;
	Standard_Real ColDelta,RowDelta, ContribFromAB, ContribFromCD ;

	ColDelta = FX - X[0] ;
	RowDelta = FY - Y[0] ;

	// Red Componant

	for ( i = 0 , First = 1, Ref = -1, SamePixels = 1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( R[i] != R[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	SR = R[Ref] ;
	}
        else {
		ContribFromAB = ColDelta * ( R[1] - R[0] ) + R[0] ;
		ContribFromCD = ColDelta * ( R[3] - R[2] ) + R[2] ;

		SR = ContribFromAB + 
			( ContribFromCD - ContribFromAB ) * RowDelta ;
        }

	// Green Componant

	for ( i = 0 , First = 1, Ref = -1, SamePixels = 1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( G[i] != G[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	SG = G[Ref] ;
	}
        else {
		ContribFromAB = ColDelta * ( G[1] - G[0] ) + G[0] ;
		ContribFromCD = ColDelta * ( G[3] - G[2] ) + G[2] ;

		SG = ContribFromAB + 
			( ContribFromCD - ContribFromAB ) * RowDelta ;
        }


	// Blue Componant

	for ( i = 0 , First = 1, Ref = -1, SamePixels = 1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( B[i] != B[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	SB = B[Ref] ;
	}
        else {
		ContribFromAB = ColDelta * ( B[1] - B[0] ) + B[0] ;
		ContribFromCD = ColDelta * ( B[3] - B[2] ) + B[2] ;

		SB = ContribFromAB + 
			( ContribFromCD - ContribFromAB ) * RowDelta ;
        }

	// Result
	aPixel.SetValue( Quantity_Color( SR, SG, SB, Quantity_TOC_RGB ) ) ;

	return Standard_True ;
  }
}

Standard_Boolean Image_BalancedPixelInterpolation::Interpolate( 
	const Handle(Image_DIndexedImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_IndexPixel& aPixel ) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X[4],Y[4] ;
  Standard_Integer V[4], i ;
  Standard_Boolean DoV[4], SamePixels = 1 ;
  Standard_Real SP ;

  if ( FX < 0. ) NX-- ;
  if ( FY < 0. ) NY-- ;

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	for ( i = 0 ; i < 4 ; i++ ) DoV[i] = 0 ;

	i = 0;

	// (0,0)
	i = 0; X[i] = NX ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}
	else {
		return Standard_False ;
	}

	// (1,0)
	i++ ; X[i] = NX+1 ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}
	else {
		V[i] = V[0] ;
	}

	// (0,1)
	i++ ; X[i] = NX ; Y[i] = NY+1 ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}
	else {
		V[i] = V[0] ;
	}

	// (1,1)
	i++ ; X[i] = NX+1 ; Y[i] = NY+1 ; 

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  	V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}
	else {
		V[i] = V[0] ;
	}


//	  Pixel A [0]			Pixel B [1]
//			Pixel X
//	  Pixel C [2]			Pixel D [3]


	Standard_Boolean First ;
	Standard_Integer Ref ;
	Standard_Real ColDelta,RowDelta, ContribFromAB, ContribFromCD ;

	for ( i = 0 , First = 1, Ref = -1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( V[i] != V[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	aPixel.SetValue( V[Ref] ) ;
	  	return Standard_True ;
	}
	else {
		ColDelta = FX - X[0] ;
		RowDelta = FY - Y[0] ;

		ContribFromAB = ColDelta * ( V[1] - V[0] ) + V[0] ;
		ContribFromCD = ColDelta * ( V[3] - V[2] ) + V[2] ;

		SP = ContribFromAB + 
			( ContribFromCD - ContribFromAB ) * RowDelta ;

	  	aPixel.SetValue( Standard_Integer(SP+0.5) ) ;

	  	return Standard_True ;
	}
  }
}

#ifdef OLD

Standard_Boolean Image_BalancedPixelInterpolation::Interpolate( 
	const Handle(Image_DColorImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_ColorPixel& aPixel ) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X[4],Y[4], i ;
  Standard_Real R[4], G[4], B[4];
  Standard_Boolean DoV[4], SamePixels = 1 ;
  Standard_Real D[4], SD, SR, SG, SB;

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	if ( FX < 0. ) NX-- ;
	if ( FY < 0. ) NY-- ;

	for ( i = 0 ; i < 4 ; i++ ) DoV[i] = 0 ;

	// (0,0)
	i = 0; X[i] = NX ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  aImage->Pixel(X[i],Y[i]).Value().
		Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  DoV[i] = 1 ;
	}

	// (1,0)
	i++ ; X[i] = NX+1 ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  aImage->Pixel(X[i],Y[i]).Value().
		Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  DoV[i] = 1 ;
	}

	// (0,1)
	i++ ; X[i] = NX ; Y[i] = NY+1 ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  aImage->Pixel(X[i],Y[i]).Value().
		Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  DoV[i] = 1 ;
	}

	// (1,1)
	i++ ; X[i] = NX+1 ; Y[i] = NY+1 ; 

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  aImage->Pixel(X[i],Y[i]).Value().
		Values( R[i],G[i],B[i],Quantity_TOC_RGB);
	  DoV[i] = 1 ;
	}

	Standard_Boolean First, DIsComputed ;
	Standard_Integer Ref ;

	SR = SG = SB = SD = 0. ;
        DIsComputed = 0 ;

	// Red Componant

	for ( i = 0 , First = 1, Ref = -1, SamePixels = 1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( R[i] != R[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	SR = R[Ref] ;
	}
        else {
	  for ( i = 0 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
	  		D[i] = Sqrt( (X[i]-FX)*(X[i]-FX) + (Y[i]-FY)*(Y[i]-FY) );
			SD += D[i] ;
			SR += D[i] * R[i] ; 
		}
	  }

	  DIsComputed = 1 ;

	  if ( SD != 0. ) SR /= SD ;
	  else {
		return Standard_False ;
	  }
        }

	// Green Componant

	for ( i = 0 , First = 1, Ref = -1, SamePixels = 1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( G[i] != G[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	SG = G[Ref] ;
	}
        else {
	  for ( i = 0 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
		  if ( !DIsComputed ) {
	  		D[i] = Sqrt( (X[i]-FX)*(X[i]-FX) + (Y[i]-FY)*(Y[i]-FY) );
			SD += D[i] ;
		  }
		  SG += D[i] * G[i] ; 
		}
	  }

	  DIsComputed = 1 ;

	  if ( SD != 0. ) SG /= SD ;
	  else {
		return Standard_False ;
	  }
        }

	// Blue Componant

	for ( i = 0 , First = 1, Ref = -1, SamePixels = 1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( B[i] != B[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	SB = B[Ref] ;
	}
        else {
	  for ( i = 0 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
		  if ( !DIsComputed ) {
	  		D[i] = Sqrt( (X[i]-FX)*(X[i]-FX) + (Y[i]-FY)*(Y[i]-FY) );
			SD += D[i] ;
		  }
		  SB += D[i] * B[i] ; 
		}
	  }

	  DIsComputed = 1 ;

	  if ( SD != 0. ) SB /= SD ;
	  else {
		return Standard_False ;
	  }
        }


	// Result
	aPixel.SetValue( Quantity_Color( SR, SG, SB, Quantity_TOC_RGB ) ) ;

	return Standard_True ;
  }
}

Standard_Boolean Image_BalancedPixelInterpolation::Interpolate( 
	const Handle(Image_DIndexedImage)& aImage,
	const Standard_Real FX, const Standard_Real FY,
	const Standard_Integer LowX,
	const Standard_Integer LowY,
	const Standard_Integer UpX,
	const Standard_Integer UpY,
	Aspect_IndexPixel& aPixel ) const

{ Standard_Integer NX = Standard_Integer(FX) ;
  Standard_Integer NY = Standard_Integer(FY) ;
  Standard_Integer X[4],Y[4] ;
  Standard_Integer V[4], i ;
  Standard_Boolean DoV[4], SamePixels = 1 ;
  Standard_Real D, SD, SP ;

  if ( NX < ( LowX-1 ) || NX > UpX || 
       NY < ( LowY-1 ) || NY > UpY ) {
	return Standard_False ;
  }
  else {

	if ( FX < 0. ) NX-- ;
	if ( FY < 0. ) NY-- ;

	SP = SD = 0. ;

	for ( i = 0 ; i < 4 ; i++ ) DoV[i] = 0 ;

	i = 0;

	// (0,0)
	i = 0; X[i] = NX ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}

	// (1,0)
	i++ ; X[i] = NX+1 ; Y[i] = NY ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}

	// (0,1)
	i++ ; X[i] = NX ; Y[i] = NY+1 ;

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}

	// (1,1)
	i++ ; X[i] = NX+1 ; Y[i] = NY+1 ; 

	if ( !( X[i] < LowX || X[i] > UpX || 
		Y[i] < LowY || Y[i] > UpY ) ) {
	  V[i] = aImage->Pixel( X[i],Y[i] ).Value() ; DoV[i] = 1 ;
	}

	Standard_Boolean First ;
	Standard_Integer Ref ;

	for ( i = 0 , First = 1, Ref = -1 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
			if ( First ) 		   { Ref = i ; First = 0 ; }
			else if ( V[i] != V[Ref] ) { SamePixels = 0 ; break ;}
		}
	}

	if ( Ref == -1 ) {
	  	return Standard_False ;
	}
	else if ( SamePixels ) {
	  	aPixel.SetValue( V[Ref] ) ;
	  	return Standard_True ;
	}
	else {
	  for ( i = 0 ; i < 4 ; i++ ) {
		if ( DoV[i] ) {
	  		D = Sqrt( (X[i]-FX)*(X[i]-FX) + (Y[i]-FY)*(Y[i]-FY) );

	  		SP += D * V[i] ; SD += D ;
		}
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
}
#endif
