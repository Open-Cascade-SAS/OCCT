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

#include <Image_Convertor.ixx>

#include <Image_PseudoColorImage.hxx>
#include <Image_ColorImage.hxx>
#include <Image_LookupTable.hxx>
#include <Aspect_ColorMapEntry.hxx>

#ifdef TRACE
static int Verbose = 0 ;
#endif

Image_Convertor::Image_Convertor() { myDitheringMethod = Image_DM_NearestColor;}

void Image_Convertor::SetDitheringMethod( const Image_DitheringMethod Method )

{ myDitheringMethod = Method ; }

Handle(Image_PseudoColorImage) Image_Convertor::Convert( 
	const Handle(Image_ColorImage)& aImage,
	const Handle(Aspect_ColorMap)& aColorMap ) const

{ Handle(Image_PseudoColorImage) ret_image = NULL ;

  if ( myDitheringMethod == Image_DM_NearestColor ) {
	ret_image = NearestDithering( aImage, aColorMap ) ;
  }
  else if ( myDitheringMethod == Image_DM_ErrorDiffusion ) {
	ret_image = ErrorDiffusionDithering( aImage, aColorMap ) ;
  }

  return ret_image ;
}


Handle(Image_PseudoColorImage) Image_Convertor::Convert( 
	const Handle(Image_PseudoColorImage)& aImage,
	const Handle(Aspect_ColorMap)& aColorMap ) const

{ Handle(Image_PseudoColorImage) ret_image = NULL ;

  if ( myDitheringMethod == Image_DM_NearestColor ) {
	ret_image = NearestDithering( aImage, aColorMap ) ;
  }
  else if ( myDitheringMethod == Image_DM_ErrorDiffusion ) {
	ret_image = ErrorDiffusionDithering( aImage, aColorMap ) ;
  }

  return ret_image ;
}


Handle(Image_ColorImage) Image_Convertor::Convert( 
	const Handle(Image_PseudoColorImage)& aImage ) const

{ Handle(Image_ColorImage) ret_image = NULL ;
  Standard_Integer x, y, val, lastval ;
  Quantity_Color aColor ;
  Standard_Integer UpX = aImage->UpperX() ;
  Standard_Integer UpY = aImage->UpperY() ;

  ret_image = new Image_ColorImage( aImage->LowerX(), aImage->LowerY(),
				    aImage->Width() , aImage->Height() ) ;

  lastval = aImage->Pixel(aImage->LowerX(), aImage->LowerY() ).Value() ;

  aColor  = aImage->ColorMap()->FindEntry(lastval).Color() ;

  for ( y = aImage->LowerY() ; y <= UpY ; y++ ) {
    for ( x = aImage->LowerX() ; x <= UpX ; x++ ) {

	val = aImage->Pixel(x,y).Value() ;

	if ( lastval != val ) {
	  lastval = val ;
	  aColor  = aImage->ColorMap()->FindEntry(lastval).Color() ;		
	}

        ret_image->SetPixel( x, y, aColor );
    }
  }

  return( ret_image ) ;

}

Handle(Image_PseudoColorImage) Image_Convertor::NearestDithering( 
	const Handle(Image_ColorImage)& aImage,
	const Handle(Aspect_ColorMap)&  aColorMap ) const

{ Handle(Image_PseudoColorImage) ret_image = NULL ;
  Standard_Integer x, y, index ;
  Quantity_Color lastval, val ;
  Standard_Integer UpX = aImage->UpperX() ;
  Standard_Integer UpY = aImage->UpperY() ;

  ret_image = new Image_PseudoColorImage( aImage->LowerX(), aImage->LowerY(),
				          aImage->Width() , aImage->Height(),
					  aColorMap );

  lastval = aImage->PixelColor(aImage->LowerX(),aImage->LowerY()) ;
  index   = aColorMap->NearestEntry( lastval ).Index() ;

  for ( y = aImage->LowerY() ; y <= UpY ; y++ ) {
    for ( x = aImage->LowerX() ; x <= UpX ; x++ ) {
       val = aImage->PixelColor(x,y) ;
       if ( !( val == lastval ) ) {
		lastval = val ;
		index   = aColorMap->NearestEntry( lastval ).Index() ;
       }
       ret_image->SetPixel( x, y, index ) ;
    }
  }

  return( ret_image ) ;

}


Handle(Image_PseudoColorImage) Image_Convertor::NearestDithering( 
	const Handle(Image_PseudoColorImage)& aImage,
	const Handle(Aspect_ColorMap)&  aColorMap ) const

{ Handle(Image_PseudoColorImage) ret_image = NULL ;
  Standard_Integer x,y, val, lastval, index  ;
  Standard_Integer UpX = aImage->UpperX() ;
  Standard_Integer UpY = aImage->UpperY() ;
  Image_LookupTable ColorMapLookup ;
  Aspect_ColorMapEntry aEntry ;
  Standard_Integer i, size ;

  for ( i = 1 , size = aImage->ColorMap()->Size() ; i <= size ; i++ ) {
	aEntry = aImage->ColorMap()->Entry(i) ;
	index  = aColorMap->NearestEntry( aEntry.Color() ).Index() ;

	ColorMapLookup.Bind( aEntry.Index(), index ) ;
  }

  ret_image = new Image_PseudoColorImage( aImage->LowerX(), aImage->LowerY(),
				          aImage->Width() , aImage->Height(),
					  aColorMap );

  lastval = aImage->Pixel( aImage->LowerX(), aImage->LowerY() ).Value() ;
  index   = ColorMapLookup.Find( lastval ).Value() ;


  for ( y = aImage->LowerY() ; y <= UpY ; y++ ) {
    for ( x = aImage->LowerX() ; x <= UpX ; x++ ) {

       val = aImage->Pixel(x,y).Value() ;

       if ( val != lastval ) {
		lastval = val ;
		index   = ColorMapLookup.Find( lastval ).Value() ;
       }

       ret_image->SetPixel( x, y, index ) ;
    }
  }

  return( ret_image ) ;

}

static Standard_Boolean ManageLastKernelError = 1 ;

static struct {
	Standard_Real v ;
	Standard_Integer dx, dy ;
} Kernel[4] = {		{ 7./16.,  1, 0 },
			{ 3./16., -1, 1 },
			{ 5./16.,  0, 1 },
			{ 1./16.,  1, 1 } } ;

Handle(Image_PseudoColorImage) Image_Convertor::ErrorDiffusionDithering( 
	const Handle(Image_PseudoColorImage)& RefImage,
	const Handle(Aspect_ColorMap)&  aColorMap ) const

{ Handle(Image_PseudoColorImage) ret_image = NULL ;
  Standard_Integer UpX  =  RefImage->UpperX() ;
  Standard_Integer UpY  =  RefImage->UpperY() ;
  Standard_Integer LowX =  RefImage->LowerX() ;
  Standard_Integer LowY =  RefImage->LowerY() ;
  Standard_Integer x,y, index, i, nx,ny  ;
  Standard_Integer val, lastval ;
  Standard_Integer Error, ErrorDiff, pix, npix, MinIndex, MaxIndex ;
  Aspect_IndexPixel aPixel ;
  Image_LookupTable ColorMapLookup ;
  Aspect_ColorMapEntry aEntry ;
  Standard_Integer size ;

  MaxIndex = MinIndex = RefImage->ColorMap()->Entry(1).Index() ;

  for ( i = 1 , size = RefImage->ColorMap()->Size() ; i <= size ; i++ ) {
	aEntry   = RefImage->ColorMap()->Entry(i) ;
	MinIndex = Min( aEntry.Index(), MinIndex ) ;
	MaxIndex = Max( aEntry.Index(), MaxIndex ) ;
	index    = aColorMap->NearestEntry( aEntry.Color() ).Index() ;

	ColorMapLookup.Bind( aEntry.Index(), index ) ;
  }


  Handle(Image_PseudoColorImage) aImage = 
	Handle(Image_PseudoColorImage)::DownCast( RefImage->Dup() ) ;

  ret_image = new Image_PseudoColorImage( aImage->LowerX(), aImage->LowerY(),
				          aImage->Width() , aImage->Height(),
					  aColorMap );

  lastval = aImage->Pixel( aImage->LowerX(), aImage->LowerY() ).Value() ;
  index   = ColorMapLookup.Find( lastval ).Value() ;

  for ( y = aImage->LowerY() ; y <= UpY ; y++ ) {
    for ( x = aImage->LowerX() ; x <= UpX ; x++ ) {

       	val = aImage->Pixel(x,y).Value() ;

       	if ( ! ( val == lastval ) ) {
		lastval = val ;
  		index   = ColorMapLookup.Find( lastval ).Value() ;
       	}

	ErrorDiff = Error = val - index ;

#ifdef TRACE
	if ( Verbose > 3 ) 
	  cout << "ErrorDiffusion : " << Error << endl << flush ;
#endif

	if ( Error != 0 ) {

	  for ( i = 0 ; i < 4 ; i++ ) {
		nx = x+Kernel[i].dx ;
		ny = y+Kernel[i].dy ;

		if ( nx >= LowX && nx <= UpX &&
		     ny >= LowY && ny <= UpY ) {
			aImage->Pixel( nx, ny, aPixel ) ;
			pix = aPixel.Value() ;

			if ( ManageLastKernelError && i == 3 ) {
			  // Last one
			  npix = pix + ErrorDiff ;
			}
			else {
			  npix = pix + Standard_Integer( Kernel[i].v * Error ) ;
			}

			// ErrorDiffusion may produce Pixel out of ColorMap
			// Clamp value to min max of ColorMap
			npix = Max( npix, MinIndex ) ;
			npix = Min( npix, MaxIndex ) ;

			ErrorDiff -= ( npix - pix ) ; 

#ifdef TRACE
			if ( Verbose > 2 && i == 3 ) {
	 		   cout << "ErrorDiffusion Last Error: " << 
				(ErrorDiff-( Kernel[i].v*Error)) <<endl<<flush ;
			}
#endif

			aPixel.SetValue( npix ) ;

			aImage->SetPixel( nx, ny, aPixel ) ;
		}

	  }
	}

       	ret_image->SetPixel( x, y, index ) ;
    }
  }

  return( ret_image ) ;

}

Handle(Image_PseudoColorImage) Image_Convertor::ErrorDiffusionDithering( 
	const Handle(Image_ColorImage)& RefImage,
	const Handle(Aspect_ColorMap)&  aColorMap ) const

{ Handle(Image_PseudoColorImage) ret_image = NULL ;
  Standard_Integer UpX  =  RefImage->UpperX() ;
  Standard_Integer UpY  =  RefImage->UpperY() ;
  Standard_Integer LowX =  RefImage->LowerX() ;
  Standard_Integer LowY =  RefImage->LowerY() ;
  Standard_Integer x,y, index, i, nx,ny  ;
  Quantity_Color val, lastval, NewCol ;
  Aspect_ColorMapEntry aEntry ;
  Aspect_ColorPixel aPixel ;
  Standard_Real RedError, GreenError, BlueError, r,g,b,cr,cg,cb ;
  Standard_Real RedDiffError, GreenDiffError, BlueDiffError ;
  Handle(Image_ColorImage) aImage = 
	Handle(Image_ColorImage)::DownCast( RefImage->Dup() ) ;
  ret_image = new Image_PseudoColorImage( aImage->LowerX(), aImage->LowerY(),
				          aImage->Width() , aImage->Height(),
					  aColorMap );

  lastval = aImage->PixelColor( aImage->LowerX(), aImage->LowerY() ) ;

  aEntry = aColorMap->NearestEntry( lastval ) ;
  index  = aEntry.Index() ;

  for ( y = aImage->LowerY() ; y <= UpY ; y++ ) {
    for ( x = aImage->LowerX() ; x <= UpX ; x++ ) {

       	val = aImage->PixelColor(x,y) ;

       	if ( ! ( val == lastval ) ) {
		lastval = val ;
		aEntry  = aColorMap->NearestEntry( lastval ) ;
  		index   = aEntry.Index() ;
       	}

	RedDiffError   = RedError   = val.Red()   - aEntry.Color().Red() ;
	GreenDiffError = GreenError = val.Green() - aEntry.Color().Green() ;
	BlueDiffError  = BlueError  = val.Blue()  - aEntry.Color().Blue() ;

#ifdef TRACE
	if ( Verbose > 3 ) 
	  cout << "ErrorDiffusion : " << RedError << "," <<
					  GreenError << "," <<
					  BlueError << "," << endl << flush ;
#endif
	if ( RedError != 0. && GreenError != 0. && BlueError != 0. ) {

	  for ( i = 0 ; i < 4 ; i++ ) {
		nx = x+Kernel[i].dx ;
		ny = y+Kernel[i].dy ;

		if ( nx >= LowX && nx <= UpX &&
		     ny >= LowY && ny <= UpY ) {
			aImage->Pixel( nx, ny, aPixel ) ;
			aPixel.Value().Values( r,g,b, Quantity_TOC_RGB ) ;

			if ( ManageLastKernelError && i == 3 ) {
			  // Last one
			  cr = r + RedDiffError ;
			  cg = g + GreenDiffError ;
			  cb = b + BlueDiffError ;
				
			}
			else {
			  cr = r + ( Kernel[i].v * RedError ) ;
			  cg = g + ( Kernel[i].v * GreenError ) ;
			  cb = b + ( Kernel[i].v * BlueError ) ;

			}

			// ErrorDiffusion may produce Pixel out of ColorMap
			// Clamp Color
			cr = Max( cr, 0. ) ; cr = Min( cr, 1. ) ;
			cg = Max( cg, 0. ) ; cg = Min( cg, 1. ) ;
			cb = Max( cb, 0. ) ; cb = Min( cb, 1. ) ;

			RedDiffError   -= ( cr - r ) ;
			GreenDiffError -= ( cg - g ) ;
			BlueDiffError  -= ( cb - b ) ;

#ifdef TRACE
			if ( Verbose > 2 && i == 3 ) {

	 		   cout << "ErrorDiffusion Last Error: " << 
					RedDiffError << "," <<
					GreenDiffError << "," <<
					BlueDiffError << "," << endl << flush ;
			}
#endif
			NewCol.SetValues( cr,cg,cb, Quantity_TOC_RGB ) ;
			aPixel.SetValue( NewCol ) ;
			aImage->SetPixel( nx, ny, aPixel ) ;
		}

	  }
	}

       	ret_image->SetPixel( x, y, index ) ;
    }
  }

  return( ret_image ) ;

}
