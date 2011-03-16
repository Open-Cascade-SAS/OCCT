#include <Image_PseudoColorImage.ixx>

#include <Image_LookupTable.hxx>
#include <TColStd_SetIteratorOfSetOfInteger.hxx>
#include <TColStd_SetOfInteger.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_ColorMapEntry.hxx>

Image_PseudoColorImage::Image_PseudoColorImage (
			const Standard_Integer x,
			const Standard_Integer y,
			const Standard_Integer dx,
			const Standard_Integer dy,
			const Handle(Aspect_ColorMap)& aColorMap) 
     : Image_DIndexedImage( x, y, dx, dy, Aspect_IndexPixel( 0 ) ) , 
	myColorMap (aColorMap) 

{ 
}

Image_PseudoColorImage::Image_PseudoColorImage (
			const Standard_Integer x,
			const Standard_Integer y,
			const Standard_Integer dx,
			const Standard_Integer dy,
			const Handle(Aspect_ColorMap)& aColorMap,
			const Aspect_IndexPixel& Back ) 
     : Image_DIndexedImage( x, y, dx, dy, Back ) , myColorMap (aColorMap) 

{
}

Handle(Image_Image) Image_PseudoColorImage::Dup() const {

  Handle(Image_PseudoColorImage) aImage = 
		new Image_PseudoColorImage(  LowerX(), LowerY(), 
				             Width(), Height(),
				             myColorMap,
					     BackgroundPixel() ) ;

	aImage->InternalDup( this ) ;

  return aImage ;
}

Image_TypeOfImage Image_PseudoColorImage::Type () const {

  return Image_TOI_PseudoColorImage ;

}

Handle(Aspect_ColorMap) Image_PseudoColorImage::ColorMap () const {

  return myColorMap;

}

const Quantity_Color& Image_PseudoColorImage::PixelColor( 
			const Standard_Integer x,
			const Standard_Integer y ) const 

{ return( myColorMap->FindEntry	( Pixel( x, y ).Value() 
				).Color() ) ;
}

void Image_PseudoColorImage::RowColor( const Standard_Integer Y,
			    Quantity_Array1OfColor&  PR) const {

  Standard_Integer TheLength = Min (PR.Length(), Width() );
  Standard_Integer L         = PR.Lower() ;
  Standard_Integer X         = LowerX() ;
  Standard_Integer NIndex    ;
  Standard_Integer PIndex    = Pixel( X, Y ).Value() ;
  Quantity_Color   PColor    = PixelColor( X, Y ) ;

  for (Standard_Integer i=0; i< TheLength; i++) {
	NIndex = Pixel(X+i,Y).Value() ;
	if ( NIndex != PIndex ) {
		PIndex  = NIndex ;
		PColor  = myColorMap->FindEntry( PIndex ).Color() ;
	}
	PR(L+i) = PColor;
  }

}

Handle(Quantity_HArray1OfColor) Image_PseudoColorImage::RowColor( 
		const Standard_Integer Y ) const {

  Standard_Integer TheLength = Width() ;
  Standard_Integer X         = LowerX() ;
  Standard_Integer NIndex ;
  Standard_Integer PIndex    = Pixel( X, Y ).Value() ;
  Quantity_Color   PColor    = PixelColor( X, Y ) ;

  Handle(Quantity_HArray1OfColor) PR = 
		new Quantity_HArray1OfColor( 0, TheLength-1) ;

  for (Standard_Integer i=0; i< TheLength; i++) {
	NIndex = Pixel(X+i,Y).Value() ;
	if ( NIndex != PIndex ) {
		PIndex  = NIndex ;
		PColor  = myColorMap->FindEntry( PIndex ).Color() ;
	}
	PR->SetValue(i,PColor);
  }

  return PR ;
}

void Image_PseudoColorImage::SqueezedLookupTable(
	const Aspect_IndexPixel& BasePixel,
	Image_LookupTable& aLookup ) const

{ TColStd_SetOfInteger 			PixelSet ;
  TColStd_SetIteratorOfSetOfInteger 	It;
  Standard_Integer 			x, y, i, UpX, UpY ;
//  Aspect_IndexPixel			aPixel ;
  //Image_LookupTable			aLookup( 101 ) ;
//  Aspect_ColorMapEntry			aEntry;

  UpX = UpperX() ;
  UpY = UpperY() ;

  for ( y = LowerY() ; y <= UpY ; y++ ) {
	for ( x = LowerX() ; x <= UpX ; x++ ) {
		PixelSet.Add( Pixel( x, y ).Value() ) ;
	}
  }

#ifdef TRACE
	cout << "Squeeze() PixelSet Extent :" << PixelSet.Extent() << endl ;
#endif

  for (It.Initialize(PixelSet), i = BasePixel.Value() ;
	 It.More(); It.Next(), i++ ) {
	aLookup.Bind( It.Value(), i ) ;
  }

  // Modif CAL 10/02/95: passage par argument car copie inaccessible.
  // return( aLookup ) ;

}

Handle(Image_PseudoColorImage) Image_PseudoColorImage::Squeeze( 
	const Aspect_IndexPixel& BasePixel ) const

{ Handle(Image_PseudoColorImage) 	ret_image = NULL ;
  Handle(Aspect_GenericColorMap) 	newcmap   = NULL ;
  Handle(Aspect_ColorMap) 	        cmap      = ColorMap() ;
  TColStd_SetOfInteger 			PixelSet ;
  TColStd_SetIteratorOfSetOfInteger 	It;
  Standard_Integer 			x, y, i, UpX, UpY ;
//  Aspect_IndexPixel			aPixel ;
  Image_LookupTable			aLookup( 101 ) ;
  Aspect_ColorMapEntry			aEntry;

  UpX = UpperX() ;
  UpY = UpperY() ;

  for ( y = LowerY() ; y <= UpY ; y++ ) {
	for ( x = LowerX() ; x <= UpX ; x++ ) {
		PixelSet.Add( Pixel( x, y ).Value() ) ;
	}
  }

#ifdef TRACE
	cout << "Squeeze() PixelSet Extent :" << PixelSet.Extent() << endl ;
#endif

  if ( PixelSet.Extent() != 0 ) {
	newcmap = new Aspect_GenericColorMap() ;

    	for (It.Initialize(PixelSet), i = BasePixel.Value() ; 
		It.More(); It.Next(), i++ ) {
		aLookup.Bind( It.Value(), i ) ;
		aEntry.SetValue( i, cmap->FindEntry( It.Value() ).Color() ) ;
		newcmap->AddEntry( aEntry ) ;
	}

	ret_image = new Image_PseudoColorImage( LowerX(), LowerY(),
				          	Width() , Height(),
					  	newcmap );

	ret_image->Fill( this ) ;

	ret_image->Lookup( aLookup ) ;

  }

  return( ret_image ) ;

}

void Image_PseudoColorImage::Lookup( const Image_LookupTable& aLookup )

{ Standard_Integer x,y, UpX, UpY;
  Aspect_IndexPixel val, lastval, newval;

  UpX = UpperX() ;
  UpY = UpperY() ;

  val     = Pixel( LowerX(), LowerY() ) ;
  lastval = val ;
  newval  = aLookup.Find( lastval ) ;

  for ( y = LowerY() ; y <= UpY ; y++ ) {
	for ( x = LowerX() ; x <= UpX ; x++ ) {
		val = Pixel( x, y ) ;
		if ( !(val == lastval) ) {
			lastval = val ;
			newval  = aLookup.Find( lastval ) ;
		}
		SetPixel( x, y , newval ) ;
	}
  }
  
}

void Image_PseudoColorImage::Rescale( const Standard_Real Scale,
				      const Standard_Real Offset )

{ Standard_Real S = Scale ;
  Standard_Real O = Offset ;
  Standard_Integer x,y, UpX, UpY, val;

  UpX = UpperX() ;
  UpY = UpperY() ;

  for ( y = LowerY() ; y <= UpY ; y++ ) {
	for ( x = LowerX() ; x <= UpX ; x++ ) {
		val = ( Standard_Integer ) ( Pixel(x, y).Value() * S + O ) ;
		MutPixel( x, y ).SetValue( val ) ;
	}
  }
  
}

void Image_PseudoColorImage::Extrema( Aspect_IndexPixel& PMin,
				      Aspect_IndexPixel& PMax ) const

{ Standard_Integer x,y, UpX, UpY, min, max, val, lastval;
//  Aspect_IndexPixel aPixel ;

  UpX = UpperX() ;
  UpY = UpperY() ;

  max = IntegerFirst() ;
  min = IntegerLast() ;

  lastval = Pixel( LowerX(), LowerY() ).Value() ;

  max = Max( max, lastval ) ;
  min = Min( min, lastval ) ;

  for ( y = LowerY() ; y <= UpY ; y++ ) {
	for ( x = LowerX() ; x <= UpX ; x++ ) {
		val = Pixel( x, y ).Value();
		if ( val != lastval ) {
			lastval = val ;
			max = Max( max, lastval ) ;
			min = Min( min, lastval ) ;
		}		
	}
  }

  PMin.SetValue( min ) ;
  PMax.SetValue( max ) ;
  
}

void Image_PseudoColorImage::Threshold( const Aspect_IndexPixel& PMin,
				        const Aspect_IndexPixel& PMax, 
				        const Aspect_IndexPixel& PMap ) 

{ Standard_Integer x,y, UpX, UpY, min, max, val, map ;
  Aspect_IndexPixel ThePixel ;
  UpX = UpperX() ;
  UpY = UpperY() ;

  max = PMax.Value() ;
  min = PMin.Value() ;
  map = PMap.Value() ;

  for ( y = LowerY() ; y <= UpY ; y++ ) {
	for ( x = LowerX() ; x <= UpX ; x++ ) {
		ThePixel = Pixel( x, y ) ;
		val = Pixel( x, y ).Value();
		if ( val >= min && val <= max ) {
			MutPixel( x, y ).SetValue( map ) ;
		}		
	}
  }
}
#ifdef OLD
Handle(Standard_Transient) Image_PseudoColorImage::ShallowCopy() const {
	return DeepCopy() ;
}

Handle(Standard_Transient) Image_PseudoColorImage::DeepCopy() const {

  return Dup() ;
}
#endif
