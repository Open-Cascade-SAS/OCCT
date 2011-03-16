#define PRO9517	//GG_010997
//		1) Le swap ne fonctionne pas correctement sur DEC
//		car X11XColor.pixel est int et non long (::Read(),::Write()).
//		2) Initialiser la table des couleurs avant d'ecrire
//		le fichier temporaire (::Write())

#define K4	//GG_110398
//		Ne pas initialiser una AsciiString avec '\0' dans le
//		constructeur de AlienImage_X11XWDAlienData sinon RAISE !

#define TEST	//GG_140699
//		Check file extension, must be ".xwd".

#ifndef WNT
# include <X11/Xlib.h>
#endif  // WNT
#include <Aspect_GenericColorMap.hxx>
#include <Image_PseudoColorImage.hxx>
#include <AlienImage_MemoryOperations.hxx>
#include <AlienImage_X11XColor.hxx>
#include <AlienImage_X11XWDAlienData.ixx>
#include <Aspect_ColorMapEntry.hxx>
#include <Standard.hxx>

#ifdef TRACE
static int Verbose = 0 ;
#endif



AlienImage_X11XWDAlienData::AlienImage_X11XWDAlienData()

{ myData   = NULL ;
  myColors = NULL ;
}

void AlienImage_X11XWDAlienData::SetName( const TCollection_AsciiString& aName)

{ myName = aName + TCollection_AsciiString( "\0" ) ;
  myHeader.header_size = sizeof( myHeader ) + myName.Length() ;
}

const TCollection_AsciiString&  AlienImage_X11XWDAlienData::Name() const

{ return ( myName ) ; }

void AlienImage_X11XWDAlienData::Clear()

{ Standard_Integer size ;

  myHeader.header_size	= sizeof( myHeader ) ; 
				/* Size of the entire file header (bytes).*/
  myHeader.file_version = 0 ;	/* XWD_FILE_VERSION */
  myHeader.pixmap_format= 0;	/* Pixmap format */
  myHeader.pixmap_depth = 0 ;	/* Pixmap depth */
  myHeader.pixmap_width = 0 ;	/* Pixmap width */
  myHeader.pixmap_height= 0 ;	/* Pixmap height */
  myHeader.xoffset 	= 0 ;	/* Bitmap x offset */
  myHeader.byte_order	= 0;	/* MSBFirst, LSBFirst */
  myHeader.bitmap_unit 	= 0 ;	/* Bitmap unit */
  myHeader.bitmap_bit_order = 0;	/* MSBFirst, LSBFirst */
  myHeader.bitmap_pad 	= 0 ;	/* Bitmap scanline pad */
  myHeader.bits_per_pixel = 0 ;	/* Bits per pixel */
  myHeader.bytes_per_line =0 ;	/* Bytes per scanline */
  myHeader.visual_class = 0 ;	/* Class of colormap */
  myHeader.red_mask 	= 0 ;	/* Z red mask */
  myHeader.green_mask	= 0 ;	/* Z green mask */
  myHeader.blue_mask 	= 0 ;	/* Z blue mask */
  myHeader.bits_per_rgb = 0 ;	/* Log2 of distinct color values */
  myHeader.colormap_entries = 0 ;	/* Number of entries in colormap */
  myHeader.ncolors 	= 0 ;	/* Number of Color structures */
  myHeader.window_width = 0 ;	/* Window width */
  myHeader.window_height= 0 ;	/* Window height */
  myHeader.window_x 	= 0 ;	/* Window upper left X coordinate */
  myHeader.window_y 	= 0 ;	/* Window upper left Y coordinate */
  myHeader.window_bdrwidth =0 ;	/* Window border width */
  
  myName.Clear() ;

  if ( myData ) {
	//Free all allocated memory
	Standard::Free(myData) ;
	myData   = NULL ;
  }

  if ( myColors ) {
	size = ( Standard_Integer ) 
			myHeader.ncolors * sizeof( AlienImage_X11XColor ) ;
	//Free all allocated memory
	Standard::Free(myColors);
	myColors = NULL ;
  }

}

Standard_Boolean AlienImage_X11XWDAlienData::Write( OSD_File& file ) const

{ Standard_Integer  size, i ;
  unsigned long swaptest = 1;
  AlienImage_X11XWDFileHeader TheHeader = myHeader ;

  if ( myData == NULL )   	return( Standard_False ) ;
  if ( TheHeader.ncolors && 
       myColors == NULL ) 	return( Standard_False ) ;

  
  // Write out TheHeader information

  size = ( Standard_Integer ) TheHeader.header_size - sizeof( TheHeader ) ;

  if ( size ) {
	// Add '\0' at the end of name
	TheHeader.header_size++ ;
  }

  if (*(char *) &swaptest) {
    AlienImage_X11XWDFileHeader SwapHeader = TheHeader ;

    AlienImage_MemoryOperations::SwapLong( ( Standard_Address ) &SwapHeader, 
					   sizeof(SwapHeader));
    file.Write( &SwapHeader ,  sizeof( SwapHeader ) ) ;
  }
  else {
    const Standard_Address pHeader = ( Standard_Address ) &TheHeader ;

    file.Write( pHeader,  sizeof( TheHeader ) ) ;
  }

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  size = ( Standard_Integer ) TheHeader.header_size - sizeof( TheHeader ) ;

  if ( size ) {
	char end = '\0' ;
	Standard_Address pend = Standard_Address( &end ) ;

	file.Write( myName, myName.Length() ) ;

	file.Write( pend, 1 ) ;
  }

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  // write out the color map buffer

  if ( TheHeader.ncolors ) {
	size = ( Standard_Integer ) TheHeader.ncolors 
					* sizeof( AlienImage_X11XColor ) ;

	if (*(char *) &swaptest) {
	  Standard_Address palloc =  
			Standard::Allocate( size ) ;
#ifdef PRO9517
	  const AlienImage_X11XColor *p = ( AlienImage_X11XColor * ) myColors ;
	  AlienImage_X11XColor *pp = ( AlienImage_X11XColor * ) palloc ;

	  for (i = 0 ; (unsigned int ) i < TheHeader.ncolors; i++,p++,pp++) {
	    pp->pixel = p->pixel;
	    pp->red = p->red;
	    pp->green = p->green;
	    pp->blue = p->blue;
	    pp->flags = p->flags;
	    AlienImage_MemoryOperations::SwapLong (
			(Standard_Address) &(pp->pixel), sizeof(int));
	    AlienImage_MemoryOperations::SwapShort(
			(Standard_Address) &(pp->red)  , 3 * sizeof(short));
	  }
#else
          const AlienImage_X11XColor *p = ( AlienImage_X11XColor * ) palloc ;

          for (i = 0 ; i < TheHeader.ncolors; i++,p++) {
                  AlienImage_MemoryOperations::SwapLong (
                        (Standard_Address) &(p->pixel), sizeof(long));
                  AlienImage_MemoryOperations::SwapShort(
                        (Standard_Address) &(p->red)  , 3 * sizeof(short));
          }
#endif
		
	  file.Write( palloc, size ) ;

	  //Free all allocated memory
	  Standard::Free(palloc) ;
	}
	else {
		file.Write( myColors, size ) ;
	}

    	if ( file.Failed() ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}

  }

  if ( DataSize() ) {
	file.Write( myData, DataSize() ) ;

    	if ( file.Failed() ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}

  }

  return( Standard_True ) ;
  
}


Standard_Boolean AlienImage_X11XWDAlienData::Read( OSD_File& file )

{ Standard_Integer bblcount, i, size ;
  unsigned long swaptest = 1;
  Standard_Address pheader = ( Standard_Address ) &myHeader ;

#ifdef TEST
  OSD_Path path; file.Path(path); 
  TCollection_AsciiString ext = path.Extension(); ext.LowerCase();
  if( ext != ".xwd" ) {
    TCollection_AsciiString sysname; path.SystemName(sysname);
#ifdef TRACE
    cout << " *** AlienImage_X11XWDAlienData::Read('" << sysname << "'). must have an '.xwd' extension" << endl;
#endif
    return Standard_False;
  }
#endif

  // Read in myHeader information

  file.Read( pheader, sizeof( myHeader ), bblcount ) ;

  if ( file.Failed() || ( bblcount != sizeof( myHeader ) ) ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  if (*(char *) &swaptest) 
    AlienImage_MemoryOperations::SwapLong( ( Standard_Address ) &myHeader, 
					   sizeof(myHeader));

  // check to see if the dump file is in the proper format */
  if (myHeader.file_version != XWD_FILE_VERSION) {
	// ERROR "XWD file format version mismatch."

  	if (*(char *) &swaptest) 
	  AlienImage_MemoryOperations::SwapLong( ( Standard_Address ) &myHeader,
					          sizeof(myHeader));

	if (myHeader.file_version != XWD_FILE_VERSION) {
		// ERROR "XWD file format version mismatch."
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
	}
	else {
		// Data come from a swaped computer ??
		swaptest = 0 ;
	}
  }

  if (myHeader.header_size < sizeof(myHeader)) {
	// ERROR "XWD header size is too small."
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

#ifdef TRACE
  if ( Verbose ) cout << myHeader << endl << flush ;
#endif

  // read in window name

  size = ( Standard_Integer ) myHeader.header_size - sizeof( myHeader ) ;

  if ( size > 0 ) {
#ifdef K4
    TCollection_AsciiString name( bblcount ) ;
#else
    TCollection_AsciiString name( bblcount , '\0') ;
#endif

    file.Read( name, size ) ; bblcount = name.Length() ;
#ifdef WNT
    --size;
#endif  // WNT

    if ( file.Failed() || ( bblcount != size ) ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
    }

    myName = name ;

#ifdef TRACE
    if ( Verbose ) cout << myName << endl << flush ;
#endif

  }

  // read in the color map buffer

  if ( myHeader.ncolors ) {
	size = ( Standard_Integer ) myHeader.ncolors 
			* sizeof( AlienImage_X11XColor ) ;

	myColors = Standard::Allocate( size ) ;

	file.Read( myColors, size, bblcount ) ;

    	if ( file.Failed() || ( bblcount != size ) ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}

	if ( *(char *) &swaptest ) {
		AlienImage_X11XColor *p  
				= ( AlienImage_X11XColor * )myColors ;
		for (i = 0 ; (unsigned int ) i < myHeader.ncolors; i++,p++) {
#ifdef PRO9517
		  AlienImage_MemoryOperations::SwapLong (
			(Standard_Address) &(p->pixel), sizeof(int));
#else
		  AlienImage_MemoryOperations::SwapLong (
			(Standard_Address) &(p->pixel), sizeof(long));
#endif
		  AlienImage_MemoryOperations::SwapShort(
			(Standard_Address) &(p->red)  , 3 * sizeof(short));
	    	}
	}

#ifdef DEB
	AlienImage_X11XColor *p  = ( AlienImage_X11XColor * )myColors ;
#endif

#ifdef TRACE
	if ( Verbose ) 	{ 
	  AlienImage_X11XColor *p = ( AlienImage_X11XColor * )myColors;

	  for (i = 0 ; i < myHeader.ncolors; i++,p++) {
		cout << *p << endl << flush ;
	  }

	}
#endif
  }

  if ( DataSize() ) {
	myData = Standard::Allocate( DataSize() ) ;

	file.Read( myData, DataSize(), bblcount ) ;

    	if ( file.Failed() || ( bblcount != DataSize() ) ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}
  }

  return( Standard_True ) ;
  
}

Handle_Image_Image AlienImage_X11XWDAlienData::ToImage() const

{   if ( myHeader.pixmap_depth <= 8 && 
       myHeader.ncolors &&
       myHeader.pixmap_format == ZPixmap) {
	return( ToPseudoColorImage() ) ;
    }
    else if ( myHeader.visual_class  == TrueColor && 
       	      myHeader.pixmap_format == ZPixmap) {
	return( ToColorImage() ) ;
    }
    else {
  	 Standard_TypeMismatch_Raise_if( Standard_True,
	 "Attempt to convert a X11XWDAlienData to a unknown Image_Image type");

	return( NULL ) ;
    }
}

void AlienImage_X11XWDAlienData::FromImage( const Handle_Image_Image& anImage )

{ if ( anImage->Type() == Image_TOI_PseudoColorImage ) {
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
	 "Attempt to convert a unknown Image_Image type to a X11XWDAlienData");
  }
}

//------------------------------------------------------------------------------
//		Private Method
//------------------------------------------------------------------------------


void AlienImage_X11XWDAlienData::FromPseudoColorImage(
			const Handle_Image_PseudoColorImage& anImage)

{ Standard_Integer size, i ;

  /* Size of the entire file header (bytes).*/
  myHeader.header_size	    = sizeof(myHeader) + myName.Length() ;
  myHeader.file_version     = XWD_FILE_VERSION ;	/* XWD_FILE_VERSION */
  myHeader.pixmap_format    = ZPixmap ;		/* Pixmap format */
  myHeader.pixmap_depth     = 8 ;		/* Pixmap depth */
  myHeader.pixmap_width     = anImage->Width() ;/* Pixmap width */
  myHeader.pixmap_height    = anImage->Height() ;/* Pixmap height */
  myHeader.xoffset 	    = 0 ;		/* Bitmap x offset */
  myHeader.byte_order	    = MSBFirst;		/* MSBFirst, LSBFirst */
  myHeader.bitmap_unit 	    = 32 ;		/* Bitmap unit */
  myHeader.bitmap_bit_order = MSBFirst;		/* MSBFirst, LSBFirst */
  myHeader.bitmap_pad 	    = 32 ;		/* Bitmap scanline pad */
  myHeader.bits_per_pixel   = 8 ;		/* Bits per pixel */
  /* Bytes per scanline */
  size = ( Standard_Integer ) ( anImage->Width() * myHeader.bits_per_pixel );

  myHeader.bytes_per_line = size / myHeader.bitmap_unit ;
  if ( size %  myHeader.bitmap_pad ) myHeader.bytes_per_line += 1 ;

  myHeader.bytes_per_line *= ( myHeader.bitmap_unit / 8 ) ;

  myHeader.visual_class     = PseudoColor ;	/* Class of colormap */
  myHeader.red_mask 	    = 0 ;		/* Z red mask */
  myHeader.green_mask	    = 0 ;		/* Z green mask */
  myHeader.blue_mask 	    = 0 ;		/* Z blue mask */
  myHeader.bits_per_rgb     = 8 ;	/* Log2 of distinct color values */
  myHeader.colormap_entries = 256 ;	/* Number of entries in colormap */
  myHeader.ncolors 	    = (anImage->ColorMap())->Size() ;
  					/* Number of Color structures */
  myHeader.window_width     = anImage->Width() ;	/* Window width */
  myHeader.window_height    = anImage->Height() ;	/* Window height */
  myHeader.window_x 	    = 0 ;	/* Window upper left X coordinate */
  myHeader.window_y 	    = 0 ;	/* Window upper left Y coordinate */
  myHeader.window_bdrwidth  = 0 ;	/* Window border width */

  size = ( Standard_Integer ) myHeader.ncolors 
				* sizeof( AlienImage_X11XColor ) ;

  myColors = Standard::Allocate( size ) ;

  AlienImage_X11XColor *p = ( AlienImage_X11XColor * )myColors ;
  Aspect_ColorMapEntry  aCentry ;

  for ( i = 1 ; (unsigned int ) i <= myHeader.ncolors ; i++, p++ ) {
	p->pixel = 0 ;
	p->red   = p->green = p->blue = 0 ;
	p->flags = 0 ;

	aCentry = (anImage->ColorMap())->Entry( i ) ;

	if ( aCentry.IsAllocated() == Standard_True ) {
		p->flags = DoRed | DoGreen | DoBlue ;
		p->pixel = aCentry.Index() ;
		p->red   = ( unsigned short )
				( (aCentry.Color()).Red()   * 0xffff + 0.5 ) ;
		p->green = ( unsigned short )
				( (aCentry.Color()).Green() * 0xffff + 0.5 ) ;
		p->blue  = ( unsigned short )
				( (aCentry.Color()).Blue()  * 0xffff + 0.5 ) ;
	}
  }

  if ( anImage->Size() ) {
	Standard_Integer x, y ;
	
	myData = Standard::Allocate( DataSize() ) ;

    	for ( y = 0 ; (unsigned int ) y < myHeader.pixmap_height ; y++ ) {
      	  for ( x = 0 ; (unsigned int ) x < myHeader.pixmap_width ; x++ ) {
		SetPixel( x, y, 
			  ( anImage->Pixel( anImage->LowerX()+x , 
			                    anImage->LowerY()+y )).Value() ) ;
      	  }
    	}

  }
}

void AlienImage_X11XWDAlienData::FromColorImage(
			const Handle_Image_ColorImage& anImage)

{ long int size ;;

  /* Size of the entire file header (bytes).*/
  myHeader.header_size	    = sizeof(myHeader) + myName.Length() ;
  myHeader.file_version     = XWD_FILE_VERSION ;	/* XWD_FILE_VERSION */
  myHeader.pixmap_format    = ZPixmap ;		/* Pixmap format */
  myHeader.pixmap_depth     = 24 ;		/* Pixmap depth */
  myHeader.pixmap_width     = anImage->Width() ;/* Pixmap width */
  myHeader.pixmap_height    = anImage->Height() ;/* Pixmap height */
  myHeader.xoffset 	    = 0 ;		/* Bitmap x offset */
  myHeader.byte_order	    = MSBFirst;		/* MSBFirst, LSBFirst */
  myHeader.bitmap_unit 	    = 32 ;		/* Bitmap unit */
  myHeader.bitmap_bit_order = MSBFirst;		/* MSBFirst, LSBFirst */
  myHeader.bitmap_pad 	    = 32 ;		/* Bitmap scanline pad */
  myHeader.bits_per_pixel   = 32 ;		/* Bits per pixel */
  /* Bytes per scanline */
  size = anImage->Width() * myHeader.bits_per_pixel ;

  myHeader.bytes_per_line = size / myHeader.bitmap_unit ;
  if ( size %  myHeader.bitmap_pad ) myHeader.bytes_per_line += 1 ;

  myHeader.bytes_per_line *= ( myHeader.bitmap_unit / 8 ) ;

  myHeader.visual_class     = TrueColor ;	/* Class of colormap */
  myHeader.red_mask 	    = 0xff ;		/* Z red mask */
  myHeader.green_mask	    = 0xff00 ;		/* Z green mask */
  myHeader.blue_mask 	    = 0xff0000 ;	/* Z blue mask */
  myHeader.bits_per_rgb     = 8 ;	/* Log2 of distinct color values */
  myHeader.colormap_entries = 256 ;	/* Number of entries in colormap */
  myHeader.ncolors 	    = 0 ;	/* Number of Color structures */
  myHeader.window_width     = anImage->Width() ;	/* Window width */
  myHeader.window_height    = anImage->Height() ;	/* Window height */
  myHeader.window_x 	    = 0 ;	/* Window upper left X coordinate */
  myHeader.window_y 	    = 0 ;	/* Window upper left Y coordinate */
  myHeader.window_bdrwidth  = 0 ;	/* Window border width */


  myColors = NULL ;

  if ( anImage->Size() ) {
	Standard_Integer x, y, pix, c ;
        const Standard_Integer rs = RedShift() ;
        const Standard_Integer gs = GreenShift() ;
        const Standard_Integer bs = BlueShift() ;
        const Standard_Integer ColorRange = 
			Standard_Integer( ( 1 << myHeader.bits_per_rgb ) - 1 );
	Quantity_Color col ;
	
	myData = Standard::Allocate( DataSize() ) ;

    	for ( y = 0 ; (unsigned int ) y < myHeader.pixmap_height ; y++ ) {
      	  for ( x = 0 ; (unsigned int ) x < myHeader.pixmap_width ; x++ ) {
		col = anImage->Pixel( anImage->LowerX()+x , 
			                anImage->LowerY()+y ).Value() ;

		pix = 0 ;

		c = ( Standard_Integer ) ( col.Red() * ColorRange + 0.5 );
		c = ( Standard_Integer ) (( c << rs ) & myHeader.red_mask) ;
		pix |= c ;
		
		c = ( Standard_Integer ) ( col.Green() * ColorRange + 0.5 );
		c = ( Standard_Integer ) (( c << gs ) & myHeader.green_mask) ;
		pix |= c ;
		
		c = ( Standard_Integer ) ( col.Blue() * ColorRange + 0.5 );
		c = ( Standard_Integer ) (( c << bs ) & myHeader.blue_mask) ;
		pix |= c ;
		
		SetPixel( x, y, pix ) ;
      	  }
    	}

  }
}

Handle_Image_ColorImage AlienImage_X11XWDAlienData::ToColorImage() const

{ Aspect_ColorPixel CPixel ;
  Quantity_Color    acolor ;
  Handle(Image_ColorImage) ret_image = NULL ;
  Standard_Integer pix,x,y ;
  Standard_Real r,g,b, maxcol ;

  if ( myHeader.visual_class  == TrueColor && 
       myHeader.pixmap_format == ZPixmap) {
    ret_image = new Image_ColorImage( 0,0,
			(Standard_Integer)myHeader.pixmap_width,
			(Standard_Integer)myHeader.pixmap_height ) ;

    maxcol = ( 1 << myHeader.bits_per_rgb ) - 1 ;

    for ( y = 0 ; (unsigned int ) y < myHeader.pixmap_height ; y++ ) {
      for ( x = 0 ; (unsigned int ) x < myHeader.pixmap_width ; x++ ) {
	pix = Pixel( x, y ) ;

	r = ( ( pix & myHeader.red_mask )   >> RedShift() )    / maxcol ;
	g = ( ( pix & myHeader.green_mask ) >> GreenShift() )  / maxcol ;
	b = ( ( pix & myHeader.blue_mask )  >> BlueShift() )   / maxcol ;
	
	acolor.SetValues( r,g,b, Quantity_TOC_RGB ) ;
	CPixel.SetValue ( acolor ) ;

        ret_image->SetPixel( ret_image->LowerX()+x , 
			     ret_image->LowerY()+y, CPixel ) ;
      }
    }
	
  }

  return( ret_image ) ;
}

Handle_Image_PseudoColorImage AlienImage_X11XWDAlienData::ToPseudoColorImage() 
								const

{ Standard_Real r,g,b ;
  Standard_Integer x, y ;
  const Standard_Real XRange = Standard_Real( Standard_Integer(0xffff) );
  const Standard_Integer newColorSize = 
       Standard_Integer(myHeader.colormap_entries*sizeof(AlienImage_X11XColor));
  Handle(Image_PseudoColorImage) ret_image = NULL ;

  if ( myHeader.pixmap_depth <= 8 && 
       myHeader.ncolors &&
       myHeader.pixmap_format == ZPixmap) {	
//    unsigned long int i, j, ncol ;
    unsigned long int i, ncol ;
    Aspect_ColorMapEntry Centry ;
    Quantity_Color       color ;
    AlienImage_X11XColor *p ;
    AlienImage_X11XColor *newColor ;
    Standard_Address	 palloc ;
    Aspect_IndexPixel IPixel ;

    palloc = Standard::Allocate( newColorSize ) ;

    newColor = ( AlienImage_X11XColor * ) palloc ;

    p = ( AlienImage_X11XColor * )myColors;

    for ( i = 0 ; i < myHeader.ncolors ; i++, p++ )     newColor[p->pixel] = *p;
    for ( i = 0 ; i < myHeader.colormap_entries ; i++ ) newColor[i].flags  = 0 ;

    for ( y = 0 ; (unsigned int ) y < myHeader.pixmap_height ; y++ ) {
	for ( x = 0 ; (unsigned int ) x < myHeader.pixmap_width ; x++ ) {
		newColor[ Pixel( x, y ) ].flags = DoRed | DoGreen | DoBlue ;
	}
    }

    for ( i = ncol = 0 ; i < myHeader.colormap_entries ; i++ ) {
	if ( newColor[i].flags ) ncol++ ;
    }

    Handle(Aspect_GenericColorMap) colormap = 
	new Aspect_GenericColorMap();

    for ( i = 0 ; i < myHeader.colormap_entries ; i++ ) {
	if ( newColor[i].flags ) {
		r = ( Standard_Real ) newColor[i].red   / XRange ;
		g = ( Standard_Real ) newColor[i].green / XRange ;
		b = ( Standard_Real ) newColor[i].blue  / XRange ;
		color.SetValues( r,g,b, Quantity_TOC_RGB );
		Centry.SetValue( Standard_Integer(newColor[i].pixel), color ) ;
		colormap->AddEntry( Centry ) ;
	}
    }

    ret_image = new Image_PseudoColorImage( 0,0,
				   Standard_Integer(myHeader.pixmap_width),
				   Standard_Integer(myHeader.pixmap_height),
				   colormap ) ;

    for ( y = 0 ; (unsigned int ) y < myHeader.pixmap_height ; y++ ) {
      for ( x = 0 ; (unsigned int ) x < myHeader.pixmap_width ; x++ ) {
	IPixel.SetValue( Pixel( x, y ) ) ;
        ret_image->SetPixel( ret_image->LowerX()+x , 
			     ret_image->LowerY()+y, IPixel ) ;
      }
    }
	

    //Free all allocated memory
    Standard::Free(palloc);
  }

  return ret_image ;

}

Standard_Integer AlienImage_X11XWDAlienData::DataSize() const

{ 
    if ( myHeader.pixmap_format != ZPixmap)
      return( myHeader.bytes_per_line * myHeader.pixmap_height 
				      * myHeader.pixmap_depth );

    return( myHeader.bytes_per_line * myHeader.pixmap_height);
}

void AlienImage_X11XWDAlienData::SetPixel( 
	const Standard_Integer x, 
	const Standard_Integer y, 
	const Standard_Integer value )

{ unsigned char *p ;
  unsigned long int pixel_size ;
 
  Standard_OutOfRange_Raise_if(
	( x < 0 || (unsigned int ) x >= myHeader.pixmap_width ||
       	  y < 0 || (unsigned int ) y >= myHeader.pixmap_height ),
         "Index out of range in X11XWDAlienData::Pixel");

  pixel_size = ( unsigned long int ) myHeader.bytes_per_line / 
				     myHeader.pixmap_width;
 
  p  = ( unsigned char * ) myData ;
  p += y * myHeader.bytes_per_line + x * pixel_size ;
 
  if ( pixel_size == 1 ) {
        *p = ( unsigned char ) value ;
  }
  else if ( pixel_size == 2 ) {
        *( ( unsigned short int * ) p ) = ( unsigned short int ) value ;
  }
  else if ( pixel_size == 4 ) {
        *( ( unsigned long int * ) p )  = ( unsigned long int ) value ;
  }
}

Standard_Integer AlienImage_X11XWDAlienData::Pixel( 
	const Standard_Integer x, const Standard_Integer y ) const

{ unsigned char *p ;
  unsigned long int  pixel_size ;
  unsigned long int pix ;
  Standard_Integer ret ;

  Standard_OutOfRange_Raise_if(
	( x < 0 || (unsigned int ) x >= myHeader.pixmap_width ||
       	  y < 0 || (unsigned int ) y >= myHeader.pixmap_height ),
         "Index out of range in X11XWDAlienData::Pixel");

  pixel_size = ( unsigned long int ) myHeader.bytes_per_line / 
				     myHeader.pixmap_width;
 
  p  = ( unsigned char * ) myData ;
  p  += y * myHeader.bytes_per_line + x * pixel_size ;
 
  if ( pixel_size == 1 ) {
        pix = ( unsigned long int ) *p ;
  }
  else if ( pixel_size == 2 ) {
        pix = *( ( unsigned short int * ) p ) ;
  }
  else {
        pix = *( ( unsigned long int * ) p ) ;
  }

  ret = Standard_Integer ( pix );
 
  return( ret ) ;
 
}

Standard_Integer AlienImage_X11XWDAlienData::RedShift() const

{ Standard_Integer shift = 0 ;

  Standard_TypeMismatch_Raise_if( myHeader.visual_class != TrueColor ,
	  "Attempt to get RedShift from a non TrueColor X11XWDImage" ) ;

  if ( ( myHeader.red_mask >> myHeader.bits_per_rgb ) == 0 ) {
	shift = 0 ;
  }
  else if ( ( myHeader.red_mask >> ( 2 * myHeader.bits_per_rgb ) ) == 0 ) {
	shift = Standard_Integer( myHeader.bits_per_rgb ) ;
  }
  else {
	shift = Standard_Integer( 2 * myHeader.bits_per_rgb );
  }
  return shift ;
}

Standard_Integer AlienImage_X11XWDAlienData::GreenShift() const

{ Standard_Integer shift = 0 ;

  Standard_TypeMismatch_Raise_if( myHeader.visual_class != TrueColor ,
	  "Attempt to get GreenShift from a non TrueColor X11XWDImage" ) ;

  if ( ( myHeader.green_mask >> myHeader.bits_per_rgb ) == 0 ) {
	shift = 0 ;
  }
  else if ( ( myHeader.green_mask >> ( 2 * myHeader.bits_per_rgb ) ) == 0 ) {
	shift = Standard_Integer( myHeader.bits_per_rgb ) ;
  }
  else {
	shift = Standard_Integer( 2 * myHeader.bits_per_rgb ) ;
  }
  return shift ;
}

Standard_Integer AlienImage_X11XWDAlienData::BlueShift() const 

{ Standard_Integer shift = 0 ;

  Standard_TypeMismatch_Raise_if( myHeader.visual_class != TrueColor ,
	  "Attempt to get BlueShift from a non TrueColor X11XWDImage" ) ;

  if ( ( myHeader.blue_mask >> myHeader.bits_per_rgb ) == 0 ) {
	shift = 0 ;
  }
  else if ( ( myHeader.blue_mask >> ( 2 * myHeader.bits_per_rgb ) ) == 0 ) {
	shift = Standard_Integer( myHeader.bits_per_rgb ) ;
  }
  else {
	shift = Standard_Integer( 2 * myHeader.bits_per_rgb ) ;
  }

  return shift ;
}
 
