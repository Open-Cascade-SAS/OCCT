// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
//

#include <Aspect_ColorRampColorMap.hxx>
#include <Standard_TypeMismatch.hxx>
#include <ImageUtility.ixx>


#define LOPTIM
#ifndef LOPTIM
static Handle(Aspect_ColorRampColorMap) TheBWColorMap = 
    new Aspect_ColorRampColorMap(0,2,Quantity_Color(1.,1.,1.,Quantity_TOC_RGB));
#else 
static Handle(Aspect_ColorRampColorMap)& _TheBWColorMap() {
    static Handle(Aspect_ColorRampColorMap) TheBWColorMap = 
	new Aspect_ColorRampColorMap(0,2,Quantity_Color(1.,1.,1.,Quantity_TOC_RGB));
return TheBWColorMap;
}
#define TheBWColorMap _TheBWColorMap()
#endif // LOPTIM


static void Rescale( const Handle(Image_PseudoColorImage) aPImage )

{ Aspect_IndexPixel PMin, PMax ;
  Standard_Integer dim, base ;
  Quantity_Color aColor ;
  Standard_Real scale, offset ; ;

  Handle(Aspect_ColorRampColorMap)::
	DownCast(aPImage->ColorMap())->ColorRampDefinition( base, dim, aColor );

  aPImage->Extrema( PMin, PMax ) ;

  if ( PMax.Value() == PMin.Value() ) return ;

  scale = Standard_Real( dim-1 ) / Standard_Real( PMax.Value() - PMin.Value()) ;
  offset = Standard_Real( base ) - Standard_Real( PMin.Value() ) * scale ;

  aPImage->Rescale( scale, offset ) ; 

}

Handle(Image_PseudoColorImage) ImageUtility::PixelColorDiff( 
		     const Handle(Image_Image)& Image1,
		     const Handle(Image_Image)& Image2 )
{ 
  return PixelColorDiff( Image1, Image2, TheBWColorMap ) ;
}

Handle(Image_PseudoColorImage) ImageUtility::PixelColorDiff( 
		const Handle(Image_Image)& Image1,
		const Handle(Image_Image)& Image2,
		const Handle(Aspect_ColorRampColorMap)& TheColorMap )

{ Handle(Image_PseudoColorImage) RetImage = NULL ;
  Standard_Integer x,y, Error, BasePixel, RampLength ;
  Standard_Integer LX, LY, UX, UY ;
  Standard_Integer LX1, LY1, UX1, UY1 ;
  Standard_Integer LX2, LY2, UX2, UY2 ;
  Aspect_IndexPixel aPixel;
  Standard_Real r1,g1,b1, r2,g2,b2 ;
  Quantity_Color aColor ;

  TheColorMap->ColorRampDefinition( BasePixel, RampLength, aColor ) ;

  Aspect_IndexPixel Pixel0(BasePixel);
  Aspect_IndexPixel Pixel1(BasePixel+RampLength-1);

  LX1 = Image1->LowerX() ; 
  LY1 = Image1->LowerY() ; 

  UX1 = Image1->UpperX() ; 
  UY1 = Image1->UpperY() ; 

  LX2 = Image2->LowerX() ; 
  LY2 = Image2->LowerY() ; 

  UX2 = Image2->UpperX() ; 
  UY2 = Image2->UpperY() ; 

  LX = Image1->LowerX() ; LX = Min( LX, Image2->LowerX() ) ;
  LY = Image1->LowerY() ; LY = Min( LY, Image2->LowerY() ) ;

  UX = Image1->UpperX() ; UX = Max( UX, Image2->UpperX() ) ;
  UY = Image1->UpperY() ; UY = Max( UY, Image2->UpperY() ) ;

  RetImage = new Image_PseudoColorImage( LX, LY, 
					 (UX-LX)+1, (UY-LY)+1, 
					TheColorMap, 
					Pixel1 ) ;

  if ( RampLength == 2 ) { //B&W ColorMap
	for ( y = LY ; y <= UY ; y++ ) {
	  for ( x = LX ; x <= UX ; x++ ) {
	    if ( ( x >= LX1 && x >= LX2 ) && ( x <= UX1 && x <= UX2 ) &&
	         ( y >= LY1 && y >= LY2 ) && ( y <= UY1 && y <= UY2 ) ) {
		if ( Image1->PixelColor(x,y) == Image2->PixelColor(x,y) ) {
		    RetImage->SetPixel( x,y, Pixel0 ) ;
		}
	    }
	  }
 	 }
  }
  else {
  	for ( y = LY ; y <= UY ; y++ ) {
	  for ( x = LX ; x <= UX ; x++ ) {
	    if ( ( x >= LX1 && x >= LX2 ) && ( x <= UX1 && x <= UX2 ) &&
		 ( y >= LY1 && y >= LY2 ) && ( y <= UY1 && y <= UY2 ) ) {
	      Image1->PixelColor(x,y).Values( r1,g1,b1, Quantity_TOC_RGB ) ;
	      Image2->PixelColor(x,y).Values( r2,g2,b2, Quantity_TOC_RGB ) ;

	      Error = BasePixel ;

	      Error += 
		Standard_Integer( (Abs(r1-r2)+Abs(g1-g2)+Abs(b1-b2))/3. 
					*(RampLength-1)
			        );

	      aPixel.SetValue( Error ) ;

	      RetImage->SetPixel( x,y, aPixel ) ;
	    }
	  }
  	}
	Rescale( RetImage ) ;
  }

  return RetImage ;
}

void ImageUtility::PixelColorDiff( 
		const Handle(Image_Image)& Image1,
		const Handle(Image_Image)& Image2,
		const Handle(Aspect_ColorRampColorMap)& TheColorMap,
		Handle(Image_PseudoColorImage)& RedDiff,
		Handle(Image_PseudoColorImage)& GreenDiff,
		Handle(Image_PseudoColorImage)& BlueDiff )

{ Standard_Integer x,y, Error, BasePixel, RampLength ;
  Standard_Integer LX, LY, UX, UY ;
  Standard_Integer LX1, LY1, UX1, UY1 ;
  Standard_Integer LX2, LY2, UX2, UY2 ;
  Aspect_IndexPixel aPixel;
  Standard_Real r1,g1,b1, r2,g2,b2 ;
  Quantity_Color aColor ;

  TheColorMap->ColorRampDefinition( BasePixel, RampLength, aColor ) ;

  Aspect_IndexPixel Pixel0(BasePixel);
  Aspect_IndexPixel Pixel1(BasePixel+RampLength-1);

  LX1 = Image1->LowerX() ; 
  LY1 = Image1->LowerY() ; 

  UX1 = Image1->UpperX() ; 
  UY1 = Image1->UpperY() ; 

  LX2 = Image2->LowerX() ; 
  LY2 = Image2->LowerY() ; 

  UX2 = Image2->UpperX() ; 
  UY2 = Image2->UpperY() ; 

  LX = Image1->LowerX() ; LX = Min( LX, Image2->LowerX() ) ;
  LY = Image1->LowerY() ; LY = Min( LY, Image2->LowerY() ) ;

  UX = Image1->UpperX() ; UX = Max( UX, Image2->UpperX() ) ;
  UY = Image1->UpperY() ; UY = Max( UY, Image2->UpperY() ) ;

  RedDiff = new Image_PseudoColorImage( LX, LY, 
					 (UX-LX)+1, (UY-LY)+1, 
					TheColorMap, 
					Pixel1 ) ;

  GreenDiff = new Image_PseudoColorImage( LX, LY, 
					 (UX-LX)+1, (UY-LY)+1, 
					TheColorMap, 
					Pixel1 ) ;

  BlueDiff = new Image_PseudoColorImage( LX, LY, 
					 (UX-LX)+1, (UY-LY)+1, 
					TheColorMap, 
					Pixel1 ) ;

  if ( RampLength == 2 ) { //B&W ColorMap
	for ( y = LY ; y <= UY ; y++ ) {
	  for ( x = LX ; x <= UX ; x++ ) {
	    if ( ( x >= LX1 && x >= LX2 ) && ( x <= UX1 && x <= UX2 ) &&
	         ( y >= LY1 && y >= LY2 ) && ( y <= UY1 && y <= UY2 ) ) {
	      Image1->PixelColor(x,y).Values( r1,g1,b1, Quantity_TOC_RGB ) ;
	      Image2->PixelColor(x,y).Values( r2,g2,b2, Quantity_TOC_RGB ) ;

	      if ( r1 == r2 ) RedDiff->SetPixel  ( x,y, Pixel0 ) ;
	      if ( g1 == g2 ) GreenDiff->SetPixel( x,y, Pixel0 ) ;
	      if ( b1 == b2 ) BlueDiff->SetPixel ( x,y, Pixel0 ) ;
	    }
	  }
 	 }
  }
  else {
  	for ( y = LY ; y <= UY ; y++ ) {
	  for ( x = LX ; x <= UX ; x++ ) {
	    if ( ( x >= LX1 && x >= LX2 ) && ( x <= UX1 && x <= UX2 ) &&
		  ( y >= LY1 && y >= LY2 ) && ( y <= UY1 && y <= UY2 ) ) {
	      Image1->PixelColor(x,y).Values( r1,g1,b1, Quantity_TOC_RGB ) ;
	      Image2->PixelColor(x,y).Values( r2,g2,b2, Quantity_TOC_RGB ) ;

	      Error = BasePixel + Standard_Integer( Abs(r1-r2)*(RampLength-1) );

	      aPixel.SetValue( Error ) ; RedDiff->SetPixel( x,y, aPixel ) ;

	      Error = BasePixel + Standard_Integer( Abs(g1-g2)*(RampLength-1) );

	      aPixel.SetValue( Error ) ; GreenDiff->SetPixel( x,y, aPixel ) ;

	      Error = BasePixel + Standard_Integer( Abs(b1-b2)*(RampLength-1) );

	      aPixel.SetValue( Error ) ; BlueDiff->SetPixel( x,y, aPixel ) ;
	    }
	  }
	}
	Rescale( RedDiff ) ;
	Rescale( GreenDiff ) ;
	Rescale( BlueDiff ) ;
  }
}
