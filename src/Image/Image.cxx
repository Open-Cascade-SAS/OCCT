// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
//

#include <Image_DIndexedImage.hxx>
#include <Image_DColorImage.hxx>
#include <Image_PixelInterpolation.hxx>
#include <Image.ixx>

#define LOPTIM
#ifndef LOPTIM
static Image_PixelInterpolation DefaultPixelInterpolation ;
#else 
static Image_PixelInterpolation& _DefaultPixelInterpolation() {
    static Image_PixelInterpolation DefaultPixelInterpolation ;
return DefaultPixelInterpolation;
}
#define DefaultPixelInterpolation _DefaultPixelInterpolation()
#endif // LOPTIM

//-------------------------------  Image::Zoom  --------------------------------

void Image::Zoom(  const Handle(Image_Image)& aImage,
	          const Standard_Real CoefX, const Standard_Real CoefY )

{   if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
	(Handle(Image_DIndexedImage)::DownCast( aImage ))->Zoom(
			DefaultPixelInterpolation, CoefX, CoefY ) ;
    }
    else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
	(Handle(Image_DColorImage)::DownCast( aImage ))->Zoom(
			DefaultPixelInterpolation, CoefX, CoefY ) ;
    }
}

void Image::Zoom( const Handle(Image_Image)& aImage,
	          const Image_PixelInterpolation& aInterpolation,
	          const Standard_Real CoefX, const Standard_Real CoefY )

{   
  if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
    (Handle(Image_DIndexedImage)::DownCast( aImage ))->Zoom(
    			aInterpolation, CoefX, CoefY ) ;
  }
  else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
    (Handle(Image_DColorImage)::DownCast( aImage ))->Zoom(
    			aInterpolation, CoefX, CoefY ) ;
  }
}

//-------------------------------  Image::Translate  ---------------------------

void Image::Translate( const Handle(Image_Image)& aImage,
	          const Standard_Real DX, const Standard_Real DY )

{   if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
	(Handle(Image_DIndexedImage)::DownCast( aImage ))->Translate(
			DefaultPixelInterpolation, DX, DY ) ;
    }
    else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
	(Handle(Image_DColorImage)::DownCast( aImage ))->Translate(
			DefaultPixelInterpolation, DX, DY ) ;
    }
}

void Image::Translate( const Handle(Image_Image)& aImage,
	          const Image_PixelInterpolation& aInterpolation,
	          const Standard_Real DX, const Standard_Real DY )

{   
  if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
    (Handle(Image_DIndexedImage)::DownCast( aImage ))->Translate(
    			aInterpolation, DX, DY ) ;
  }
  else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
    (Handle(Image_DColorImage)::DownCast( aImage ))->Translate(
    			aInterpolation, DX, DY ) ;
  }
}

//-------------------------------  Image::Rotate  ------------------------------

void Image::Rotate( const Handle(Image_Image)& aImage,
	          const Quantity_PlaneAngle aAngle )

{   if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
	(Handle(Image_DIndexedImage)::DownCast( aImage ))->Rotate(
			DefaultPixelInterpolation, aAngle ) ;
    }
    else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
	(Handle(Image_DColorImage)::DownCast( aImage ))->Rotate(
			DefaultPixelInterpolation, aAngle ) ;
    }
}

void Image::Rotate( const Handle(Image_Image)& aImage,
	          const Image_PixelInterpolation& aInterpolation,
	          const Quantity_PlaneAngle aAngle )

{   
  if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
    (Handle(Image_DIndexedImage)::DownCast( aImage ))->Rotate(
    			aInterpolation,aAngle ) ;
  }
  else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
    (Handle(Image_DColorImage)::DownCast( aImage ))->Rotate(
 			aInterpolation,aAngle ) ;
  }
}

//-------------------------------  Image::Affine  ------------------------------

void Image::Affine( const Handle(Image_Image)& aImage,
	            const gp_GTrsf2d& aTrsf )

{   if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
	(Handle(Image_DIndexedImage)::DownCast( aImage ))->Affine(
			DefaultPixelInterpolation, aTrsf ) ;
    }
    else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
	(Handle(Image_DColorImage)::DownCast( aImage ))->Affine(
			DefaultPixelInterpolation, aTrsf ) ;
    }
}

void Image::Affine( const Handle(Image_Image)& aImage,
	            const Image_PixelInterpolation& aInterpolation,
	            const gp_GTrsf2d& aTrsf )

{   
  if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
    (Handle(Image_DIndexedImage)::DownCast( aImage ))->Affine(
    			aInterpolation,aTrsf ) ;
  }
  else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
    (Handle(Image_DColorImage)::DownCast( aImage ))->Affine(
 			aInterpolation,aTrsf ) ;
  }
}


void Image::Affine( const Handle(Image_Image)& aImage,
	             const gp_Trsf& aTrsf )

{   if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
	(Handle(Image_DIndexedImage)::DownCast( aImage ))->Affine(
			DefaultPixelInterpolation, aTrsf ) ;
    }
    else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
	(Handle(Image_DColorImage)::DownCast( aImage ))->Affine(
			DefaultPixelInterpolation, aTrsf ) ;
    }
}

void Image::Affine( const Handle(Image_Image)& aImage,
	             const Image_PixelInterpolation& aInterpolation,
	             const gp_Trsf& aTrsf )

{   
  if ( aImage->IsKind(STANDARD_TYPE(Image_DIndexedImage))) {
    (Handle(Image_DIndexedImage)::DownCast( aImage ))->Affine(
    			aInterpolation,aTrsf ) ;
  }
  else if ( aImage->IsKind(STANDARD_TYPE(Image_DColorImage))) {
    (Handle(Image_DColorImage)::DownCast( aImage ))->Affine(
 			aInterpolation,aTrsf ) ;
  }
}
