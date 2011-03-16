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

