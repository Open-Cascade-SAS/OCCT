#define TEST    //GG_140699
//              Check file extension, must be ".PIX".

#include <Aspect_GenericColorMap.hxx>
#include <Image_Convertor.hxx>
#include <Image_PseudoColorImage.hxx>
#include <AlienImage_EuclidAlienData.ixx>
#include <Aspect_ColorMapEntry.hxx>
#include <Standard.hxx>

#define xTRACE

#ifdef TRACE
static int Verbose = 1 ;
#endif

static const unsigned int EndOfPixelLine  = 65535 ;
static const unsigned int EndOfPixelField = 65534 ;

#ifdef PAGESIZE		// HPUX COMPATIBILLITY
#undef PAGESIZE
#endif
#define MAXCOUL  896
#define PAGESIZE 512

#define FIRST_EUCLID_IMAGE_COLOR 37
#define LAST_EUCLID_IMAGE_COLOR  180

#define ColorsSize() ( MAXCOUL*sizeof(int) ) 
#define COLOR(i) ( (( int * )myColors)[i-1] ) // Fortran Array Binding 

//------------------------------------------------------------------------------
//		Public Method
//------------------------------------------------------------------------------

AlienImage_EuclidAlienData::AlienImage_EuclidAlienData()

{
  myX1 = myY1 = myX2 = myY2 = myNumberOfColor = 0 ;
  myColors = NULL ;
  myPixelsIsDef = Standard_False ;
}

void AlienImage_EuclidAlienData::Clear()

{ 
  if ( myColors ) {
	// Free memory
	Standard::Free(myColors) ;
	myColors = NULL ;
  }

  myPixelsIsDef = Standard_False ;

  myX1 = myY1 = myX2 = myY2 = myNumberOfColor = 0 ;
}


Standard_Boolean AlienImage_EuclidAlienData::Read( OSD_File& file )

{ Standard_Integer bblcount, i ;

#ifdef TEST
  OSD_Path path; file.Path(path);
  TCollection_AsciiString ext = path.Extension(); ext.LowerCase();
  if( ext != ".pix" ) {
    TCollection_AsciiString sysname; path.SystemName(sysname);
#ifdef TRACE
    cout << " *** AlienImage_EuclidAlienData::Read('" << sysname << "'). must have an '.PIX' extension" << endl;
#endif
    return Standard_False;
  }
#endif

  if ( myColors == NULL ) 
	myColors = Standard::Allocate( ColorsSize() ) ;

  // Read in Header information

  file.Read( myColors, ColorsSize(), bblcount ) ;

  if ( file.Failed() || ( bblcount != ColorsSize() ) ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  myNumberOfColor = COLOR(1)+1 ;
  myX1		  = COLOR(2) ;
  myY1		  = COLOR(3) ;
  myX2		  = COLOR(4) ;
  myY2		  = COLOR(5) ;

#ifdef TRACE
  if ( Verbose ) {
	cout << "EuclidAlienData::Read(" << myNumberOfColor-1 << "," <<
					    myX1 << "," <<
					    myY1 << "," <<
					    myX2 << "," <<
					    myY2 << ")\n" << flush ;
  }
#endif

  myPixels = new TColStd_HArray2OfInteger( myX1, myX2, myY1, myY2, 0 ) ;
  myPixelsIsDef = Standard_True ;

  Standard_Boolean K_FLAG, K_LIGNE;
  Standard_Integer INCRE, JNCRE, ILIGNE, NUMBLK, NMOT, NPIX, IPIXCR;
  int LINPIX[(PAGESIZE*128)/sizeof(int)] ;
  Standard_Address pLINPIX = ( Standard_Address ) LINPIX ;

  K_FLAG  = Standard_True ;
  K_LIGNE = Standard_False ;

  INCRE   = myX1 ;
  JNCRE   = 1 ;
  ILIGNE  = myY1 ;

  NUMBLK = 8 ;
  NMOT   = (PAGESIZE*128) ;

  file.Seek( PAGESIZE*(NUMBLK-1), OSD_FromBeginning ) ;
  file.Read( pLINPIX, NMOT*sizeof(int), bblcount ) ;

  NUMBLK += 128 ;

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  while( K_FLAG ) {
	while( !K_LIGNE ) {
		NPIX   = LINPIX[JNCRE-1] & 0xffff ;
		IPIXCR = ( LINPIX[JNCRE-1] >> 16 ) & 0xffff ;
		if ( IPIXCR == (Standard_Integer ) EndOfPixelLine ) {
			K_LIGNE = Standard_True ;
		}
		else if ( IPIXCR < 512 ) {
			for( i = 0 ; i < NPIX ; i++ ) {
				myPixels->SetValue(INCRE,ILIGNE,IPIXCR & 0xff);
				INCRE++ ;
			}

			JNCRE++ ;

			if ( JNCRE >= NMOT ) {
  			  file.Seek( PAGESIZE*(NUMBLK-1), OSD_FromBeginning ) ;
  			  file.Read( pLINPIX, NMOT*sizeof(int),bblcount );

			  NUMBLK += 128 ;
			  JNCRE   = 1 ;

 			  if ( file.Failed() ){		// ERROR
				file.Seek( 0, OSD_FromBeginning ) ;
				return( Standard_False ) ;
  			  }
			}
		}
		else {
			K_LIGNE = Standard_True ;
		}
	}
	// Next Line
	K_LIGNE = Standard_False ;
	ILIGNE++ ;
	INCRE   = myX1 ;

	JNCRE++ ;
	IPIXCR = ( LINPIX[JNCRE-1] >> 16 ) & 0xffff ;
	if ( IPIXCR == (Standard_Integer ) EndOfPixelField )
          K_FLAG = Standard_False ; // End of Image
  }

  return Standard_True ;
  
}

Standard_Boolean AlienImage_EuclidAlienData::Write( OSD_File& file ) const

{ Standard_Integer x, y ;
  Standard_Integer NUMBLK, NMOT, NPIX, MAXMOT ;
  Standard_Integer ITSTPIX ;
  int LINPIX[(PAGESIZE*128)/sizeof(int)] ;

  if ( myNumberOfColor == 0 || myColors == NULL || ! myPixelsIsDef ) 
	return Standard_False ;

  MAXMOT = (PAGESIZE*128) / sizeof(int) ;

  // Write in Header information

  file.Write( myColors, ColorsSize() ) ;

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  NUMBLK = 8 ;
  NMOT   = 0 ;

  for ( y = myY1 ; y <= myY2 ; y++ ) {
	x = myX1 ;
	ITSTPIX = myPixels->Value( x, y ) ;
	x++ ;

	while( x <= myX2 ) {
		NPIX = 1 ;

		while ( myPixels->Value( x, y ) == ITSTPIX ) {
			NPIX++ ;
			x++ ;
			if ( x > myX2 ) break ;// End of Pixel Line
		}

		LINPIX[NMOT] = ( ( ( ITSTPIX&0xff ) << 16 ) & 0xffff0000 ) | 
				 ( NPIX & 0xffff );
		NMOT++ ;

		if ( NMOT >= MAXMOT ) {
  			file.Seek( PAGESIZE*(NUMBLK-1), OSD_FromBeginning ) ;
  			file.Write( ( void *)LINPIX, NMOT*sizeof(int) );

			NMOT   = 0 ;
			NUMBLK += 128 ;
		}

		if ( x <= myX2 ) {
		  ITSTPIX = myPixels->Value( x, y ) ;
		  x++ ;

		  if ( x == myX2 ) {// Last Pixel 
		    LINPIX[NMOT] = ( ( (ITSTPIX&0xff)<<16 ) & 0xffff0000 ) |
				 	( 1 & 0xffff );
		    NMOT++ ;

		    if ( NMOT >= MAXMOT ) {
  			file.Seek( PAGESIZE*(NUMBLK-1), OSD_FromBeginning ) ;
  			file.Write( ( void *)LINPIX, NMOT*sizeof(int) );

			NMOT   = 0 ;
			NUMBLK += 128 ;
		    }
		  }
		}
	}

	// End of Pixel Line
	LINPIX[NMOT] = ( ( EndOfPixelLine<<16 ) & 0xffff0000 ) | 
			 ( 0 & 0xffff );
	NMOT++ ;

	if ( NMOT >= MAXMOT ) {
  		file.Seek( PAGESIZE*(NUMBLK-1), OSD_FromBeginning ) ;
  		file.Write( ( void *)LINPIX, NMOT*sizeof(int) );

		NMOT   = 0 ;
		NUMBLK += 128 ;
	}
  }

  // End of Pixel Field
  LINPIX[NMOT] = ( ( EndOfPixelField<<16 ) & 0xffff0000 ) | 
		   ( 0 & 0xffff );
  NMOT++ ;

  file.Seek( PAGESIZE*(NUMBLK-1), OSD_FromBeginning ) ;
  file.Write( ( void *)LINPIX, NMOT*sizeof(int) );

  return Standard_False ;

}

Handle_Image_Image AlienImage_EuclidAlienData::ToImage() const

{ Standard_Real r,g,b ;
  Standard_Integer x, y, i ;
  Aspect_IndexPixel IPixel ;
  Aspect_ColorMapEntry Centry ;
  Quantity_Color       color ;

  if ( myNumberOfColor == 0 || myColors == NULL || ! myPixelsIsDef ) 
	return NULL ;

  Handle(Aspect_GenericColorMap) colormap = new Aspect_GenericColorMap() ;
  Handle(Image_PseudoColorImage) PImage = 
	new Image_PseudoColorImage( myX1, myY1, 
				   (myX2-myX1)+1, (myY2-myY1)+1, colormap ) ;
  Handle(Image_Image) RetImage = PImage ;

  // Get Colors

  for ( i = 0 ; i < myNumberOfColor ; i++ ) {
	r = ( Standard_Real ) COLOR(129+3*i) / 255. ;
	g = ( Standard_Real ) COLOR(130+3*i) / 255. ;
	b = ( Standard_Real ) COLOR(131+3*i) / 255. ;

	color.SetValues( r,g,b, Quantity_TOC_RGB );

	Centry.SetValue( i, color ) ;

	colormap->AddEntry( Centry ) ;
  }

  for ( y = myY1 ; y <= myY2 ; y++ ) {
	for ( x = myX1 ; x <= myX2 ; x++ ) {
		IPixel.SetValue( myPixels->Value( x, y ) ) ;
		PImage->SetPixel( x,myY2+myY1-y, IPixel ) ;
	}
  }


  return RetImage ;

}


void AlienImage_EuclidAlienData::FromImage( const Handle_Image_Image& anImage )

{ 
  if ( anImage->Type() == Image_TOI_PseudoColorImage ) {
	Handle(Image_PseudoColorImage) aPImage =
		Handle(Image_PseudoColorImage)::DownCast(anImage) ;

	FromPseudoColorImage( aPImage ) ;
  }
  else if ( anImage->Type() == Image_TOI_ColorImage )  {
	Handle(Image_ColorImage) aCImage =
		Handle(Image_ColorImage)::DownCast(anImage) ;

	FromColorImage( aCImage ) ;
  }
  else {
 	 Standard_TypeMismatch_Raise_if( Standard_True,
	 "Attempt to convert a unknown Image_Image type to a EuclidAlienImage");
  }
}

//------------------------------------------------------------------------------
//		Private Method
//------------------------------------------------------------------------------

void AlienImage_EuclidAlienData::FromColorImage(
			const Handle_Image_ColorImage& anImage)

{ Image_Convertor Convertor ;
  Handle(Aspect_ColorMap) aCmap = 
    anImage->ChooseColorMap(LAST_EUCLID_IMAGE_COLOR-FIRST_EUCLID_IMAGE_COLOR+1);
  Handle(Image_PseudoColorImage) aPImage = Convertor.Convert( anImage, aCmap ) ;

  FromPseudoColorImage( aPImage ) ;
}

void AlienImage_EuclidAlienData::FromPseudoColorImage(
			const Handle_Image_PseudoColorImage& PImage)


{
  if ( myColors == NULL ) 
	myColors = Standard::Allocate( ColorsSize() ) ;

  //
  // Euclid ColorMap goes from 0 to 255
  // We must define all colors form 0 to 255 in a EUCLID .PIX file
  //

  Standard_Real r,g,b ;
  Standard_Integer x, y, i, index ;
  Aspect_IndexPixel IPixel ;
  Aspect_ColorMapEntry Centry ;

  Handle(Aspect_ColorMap) colormap = PImage->ColorMap() ;

  for ( i = 1 ; i <= MAXCOUL ; i++ ) {
	COLOR(i) = 0 ;
  }

  myX1 = PImage->LowerX() ; myX2 = PImage->UpperX() ;
  myY1 = PImage->LowerY() ; myY2 = PImage->UpperY() ;

  myPixels = new TColStd_HArray2OfInteger( myX1, myX2, myY1, myY2, 0 ) ;
  myPixelsIsDef = Standard_True ;

  myNumberOfColor = 0 ;

  // Get Colors

  for ( i = 1 ; i <= colormap->Size() ; i++ ) {
	Centry = colormap->Entry( i ) ;
	index = Centry.Index() ;
	if ( index >= 0 && index <= 255 ) {
		myNumberOfColor = Max( myNumberOfColor, index ) ;
		Centry.Color().Values( r,g,b, Quantity_TOC_RGB ) ;

		COLOR(129+3*index) = ( int ) ( r * 255. + 0.5 ) ;
		COLOR(130+3*index) = ( int ) ( g * 255. + 0.5 ) ;
		COLOR(131+3*index) = ( int ) ( b * 255. + 0.5 ) ;
	}
  }

  COLOR(1) = myNumberOfColor ;
  COLOR(2) = myX1;
  COLOR(3) = myY1 ; 
  COLOR(4) = myX2 ;
  COLOR(5) = myY2 ; 

  for ( y = myY1 ; y <= myY2 ; y++ ) {
	for ( x = myX1 ; x <= myX2 ; x++ ) {
		PImage->Pixel( x,myY2+myY1-y, IPixel ) ;
		myPixels->SetValue( x, y, IPixel.Value() ) ;
	}
  }
}
 
