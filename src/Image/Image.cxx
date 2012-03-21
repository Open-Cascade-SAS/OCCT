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

// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)

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
