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

#include <Aspect_GenericColorMap.hxx>
#include <Image_PseudoColorImage.hxx>

#include <AlienImage_MemoryOperations.hxx>
#include <AlienImage_SUNRFFormat.hxx>
#include <Image_Convertor.hxx>
#include <AlienImage_SunRFAlienData.ixx>
#include <Aspect_ColorMapEntry.hxx>

#include <Standard_Byte.hxx>
#include <Standard.hxx>

#ifdef TRACE
static int Verbose = 1 ;
#endif

#define RUN_FLAG 0x80

// Each line of the image is rounded out to a multiple of 16 bits
#define ROWBYTES() (((myHeader.ras_width*myHeader.ras_depth + 7 )/8 + 1 ) & ~1 )



AlienImage_SunRFAlienData::AlienImage_SunRFAlienData()

{ Clear() ; }

void AlienImage_SunRFAlienData::SetFormat( 
	const AlienImage_SUNRFFormat aFormat )

{ switch ( aFormat ) {
	case AlienImage_SUNRF_Old         : 
		myHeader.ras_type = RT_OLD ; break ;
	case AlienImage_SUNRF_Standard    : 
		myHeader.ras_type = RT_STANDARD ; break ;
	case AlienImage_SUNRF_ByteEncoded : 
		myHeader.ras_type = RT_BYTE_ENCODED ; break ;
	case AlienImage_SUNRF_RGB : 
		myHeader.ras_type = RT_FORMAT_RGB ; break ;
	default :
		cout << "SunRFAlienData : Unknown or Unsuported Format\n" ;
		break ;
  }
}

AlienImage_SUNRFFormat AlienImage_SunRFAlienData::Format() const

{ AlienImage_SUNRFFormat ret = AlienImage_SUNRF_Unknown ;

  switch ( myHeader.ras_type ) {
	case   RT_OLD       : 
		ret = AlienImage_SUNRF_Old  ; break ;
	case  RT_STANDARD   : 
		ret = AlienImage_SUNRF_Standard ; break ;
	case RT_BYTE_ENCODED : 
		ret = AlienImage_SUNRF_ByteEncoded ; break ;
	case RT_FORMAT_RGB : 
		ret = AlienImage_SUNRF_RGB ; break ;
  }

  return ret ;

}

void AlienImage_SunRFAlienData::FreeData()

{
  if ( myData && myDataSize ) {
	//Free all allocated memory
	Standard::Free(myData) ;
	myData     = NULL ;
	myDataSize = 0 ;
  }

  if ( myRedData && myHeader.ras_maplength ) {
	//Free all allocated memory
	Standard::Free( myRedData) ;
	myRedData   = NULL ;
  }

  if ( myGreenData && myHeader.ras_maplength ) {
	//Free all allocated memory
	Standard::Free(myGreenData) ;
	myRedData   = NULL ;
  }

  if ( myBlueData && myHeader.ras_maplength ) {
	//Free all allocated memory
	Standard::Free(myBlueData) ;
	myRedData   = NULL ;
  }

}

void AlienImage_SunRFAlienData::Clear()

{ FreeData() ;


  myHeader.ras_magic 	 = RAS_MAGIC ;
  myHeader.ras_width 	 = 0 ;
  myHeader.ras_height 	 = 0 ;
  myHeader.ras_length 	 = 0 ;
  myHeader.ras_type 	 = RT_STANDARD ;
  myHeader.ras_maptype   = RMT_NONE ;
  myHeader.ras_maplength = 0 ;

}

Standard_Boolean AlienImage_SunRFAlienData::Write( OSD_File& file ) const

{ Standard_Integer  size;
  AlienImage_SUNRFFileHeader TheHeader = myHeader ;

  // Write out TheHeader information

  if ( myData && myDataSize &&
	myHeader.ras_type == RT_FORMAT_RGB &&
	myHeader.ras_depth == 8 ) {
	// Convert PseudoColorImage to TrueColor

	Handle(Image_Image) 	 aImage = ToImage() ;

	if ( aImage->IsKind( STANDARD_TYPE(Image_PseudoColorImage) ) ) {
  	  Image_Convertor Convertor;

	  Handle(Image_ColorImage) aCImage = 
	    Convertor.Convert(Handle(Image_PseudoColorImage)::DownCast(aImage));

	  Handle(AlienImage_SunRFAlienData) newThis = 
	    new AlienImage_SunRFAlienData() ;

	  newThis->FromImage( aCImage ) ;
	  newThis->SetFormat( AlienImage_SUNRF_RGB ) ;
	  return newThis->Write( file ) ;
	}
  }

  size = ( Standard_Integer ) sizeof( TheHeader ) ;

  const Standard_Address pHeader = ( Standard_Address ) &TheHeader ;

  file.Write( pHeader,  sizeof( TheHeader ) ) ;

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  // write out the color map buffer

  if ( TheHeader.ras_maplength ) {
	file.Write( myRedData, myHeader.ras_maplength/3 ) ;
	file.Write( myGreenData, myHeader.ras_maplength/3 ) ;
	file.Write( myBlueData, myHeader.ras_maplength/3 ) ;

    	if ( file.Failed() ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}
  }

  Standard_Integer rwbytes ;

  rwbytes = ROWBYTES() ;

  if ( myData && myDataSize ) {
    if ( myHeader.ras_type == RT_OLD || 
	 myHeader.ras_type == RT_STANDARD ||
	 myHeader.ras_type == RT_FORMAT_RGB ) {
	if ( myHeader.ras_type == RT_FORMAT_RGB ) {
	  // Swap Sun Default BGR Format to RGB
	  Standard_Byte *p = ( Standard_Byte * )myData ;
	  Standard_Byte tmp, *pix ;
	  Standard_Integer j, i ;

	  if ( myHeader.ras_depth == 24 || myHeader.ras_depth == 32 ) {
		for ( i = 0 ;i < myHeader.ras_height ; i++, p += rwbytes ) {
		  for ( j = 0, pix=p; j < myHeader.ras_width ; j++,pix+=3) {
			if ( myHeader.ras_depth == 32 ) pix++ ;
			tmp 	 = *pix ;
			*pix 	 = *(pix+2) ;
			*(pix+2) = tmp ;
		  }
		}
	  }
	  else if ( myHeader.ras_depth == 8 ) {
			// ERROR
			file.Seek( 0, OSD_FromBeginning ) ;
			return( Standard_False ) ;
	  }
	}

	file.Write( myData, myDataSize ) ;

    	if ( file.Failed() ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}

	if ( myHeader.ras_type == RT_FORMAT_RGB &&
	     ( myHeader.ras_depth == 24 || myHeader.ras_depth == 32 ) ) {
	  // Swap RGB Format to Sun Default 
	  Standard_Byte *p = ( Standard_Byte * )myData ;
	  Standard_Byte tmp, *pix ;
	  Standard_Integer j, i ;

		for ( i = 0 ;i < myHeader.ras_height ; i++, p += rwbytes ) {
		  for ( j = 0, pix=p; j < myHeader.ras_width ; j++,pix+=3) {
			if ( myHeader.ras_depth == 32 ) pix++ ;
			tmp 	 = *pix ;
			*pix 	 = *(pix+2) ;
			*(pix+2) = tmp ;
		  }
		}
	  }
      }
      else if ( myHeader.ras_type == RT_BYTE_ENCODED ) {
		Standard_Byte *p = ( Standard_Byte * )myData ;
		Standard_Integer i ;

		for ( i = 0 ; i < myHeader.ras_height ; i++, p += rwbytes ) {
		  if ( WritePixelRow( file, ( Standard_Address) p, rwbytes ) ==
				Standard_False ) {
			file.Seek( 0, OSD_FromBeginning ) ;
			return( Standard_False ) ;
		  }

		}
      }

  }

  return( Standard_True ) ;
  
}


Standard_Boolean AlienImage_SunRFAlienData::Read( OSD_File& file )

{ Standard_Integer bblcount, size ;
  Standard_Address pheader = ( Standard_Address ) &myHeader ;

  // Read in myHeader information

  file.Read( pheader, sizeof( myHeader ), bblcount ) ;

  if ( file.Failed() || ( bblcount != sizeof( myHeader ) ) ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  // check to see if the dump file is in the proper format */
  if (myHeader.ras_magic != RAS_MAGIC) {
	// ERROR "XWD file format version mismatch."

	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

#ifdef TRACE
  if ( Verbose ) cout << myHeader << endl << flush ;
#endif

  // read in the color map buffer

  if ( myHeader.ras_maplength ) {
	size = myHeader.ras_maplength / 3  ;

	myRedData   = Standard::Allocate( size ) ;
	myGreenData = Standard::Allocate( size ) ;
	myBlueData  = Standard::Allocate( size ) ;

	file.Read( myRedData, size, bblcount ) ;
	file.Read( myGreenData, size, bblcount ) ;
	file.Read( myBlueData, size, bblcount ) ;

    	if ( file.Failed() || ( bblcount != size ) ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}

#ifdef TRACE
	if ( Verbose ) 	{ 
	  Standard_Byte *r = ( Standard_Byte * )myRedData ;
	  Standard_Byte *g = ( Standard_Byte * )myGreenData ;
	  Standard_Byte *b = ( Standard_Byte * )myBlueData ;

	  for (i = 0 ; i < myHeader.ncolors; i++,p++) {
		cout << "(" << r << "," << g << "," << b << ")\n" << flush ;
	  }

	}
#endif
  }

  if ( myHeader.ras_width && myHeader.ras_height && myHeader.ras_depth ) {
	Standard_Integer rwbytes ;

	rwbytes = ROWBYTES() ;

	myDataSize = rwbytes * myHeader.ras_height ;

	myData = Standard::Allocate( myDataSize ) ;

	if ( myHeader.ras_type == RT_OLD || 
	     myHeader.ras_type == RT_STANDARD ||
	     myHeader.ras_type == RT_FORMAT_RGB ) {
		file.Read( myData, myDataSize, bblcount ) ;

    		if ( file.Failed() || ( bblcount != myDataSize ) ) {
			// ERROR
			file.Seek( 0, OSD_FromBeginning ) ;
			return( Standard_False ) ;
    		}

		if ( myHeader.ras_type == RT_FORMAT_RGB && 
		     ( myHeader.ras_depth == 24 || myHeader.ras_depth == 32 )) {
		  // Swap RGB to Sun Default BGR Format
		  Standard_Byte *p = ( Standard_Byte * )myData ;
		  Standard_Byte tmp, *pix ;
		  Standard_Integer i, j ;

		  for ( i = 0 ; i < myHeader.ras_height ; i++, p += rwbytes ) {
		    for ( j = 0, pix = p; j < myHeader.ras_width ; j++,pix+=3) {
			if ( myHeader.ras_depth == 32 ) pix++ ;
			tmp 	 = *pix ;
			*pix 	 = *(pix+2) ;
			*(pix+2) = tmp ;
		    }
		  }
		}
	}
	else if ( myHeader.ras_type == RT_BYTE_ENCODED ) {
		Standard_Byte *p = ( Standard_Byte * )myData ;
		Standard_Integer i ;

		for ( i = 0 ; i < myHeader.ras_height ; i++, p += rwbytes ) {
		  if ( ReadPixelRow( file, ( Standard_Address) p, rwbytes ) ==
				Standard_False ) {
			file.Seek( 0, OSD_FromBeginning ) ;
			return( Standard_False ) ;
		  }
		}
	}

  }

  return( Standard_True ) ;
  
}

Handle_Image_Image AlienImage_SunRFAlienData::ToImage() const

{   if ( myHeader.ras_depth <= 8 && 
       myHeader.ras_maplength ) {
	return( ToPseudoColorImage() ) ;
    }
    else if ( myHeader.ras_depth == 24 || myHeader.ras_depth == 32 ) {
	return( ToColorImage() ) ;
    }
    else {
  	 Standard_TypeMismatch_Raise_if( Standard_True,
	 "Attempt to convert a SunRFAlienData to a unknown Image_Image type");

	return( NULL ) ;
    }
}

void AlienImage_SunRFAlienData::FromImage( const Handle_Image_Image& anImage )

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
	 "Attempt to convert a unknown Image_Image type to a SunRFAlienData");
  }
}

//------------------------------------------------------------------------------
//		Private Method
//------------------------------------------------------------------------------

Standard_Boolean AlienImage_SunRFAlienData::ReadPixelRow(   
						OSD_File& file,
						const Standard_Address pdata,
						const Standard_Integer rwbytes)

{ Standard_Byte *p = ( Standard_Byte * )pdata ;
  Standard_Byte byte, val ;
  Standard_Integer RLEcnt, PixelCount, i, bblcount ;
  Standard_Address pb = ( Standard_Address ) &byte ;

  PixelCount = 0 ;

  while ( PixelCount < myHeader.ras_width ) {
	file.Read( pb, 1, bblcount ) ;

	if ( file.Failed() || ( bblcount != 1 ) ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
    	}

	if ( byte != RUN_FLAG ) { 
		// Get a single pixel byte
		RLEcnt = 1 , val = byte ; 
	}
	else { // RLE Flag
		file.Read( pb, 1, bblcount ) ;

		if ( file.Failed() || ( bblcount != 1 ) ) {
			// ERROR
			file.Seek( 0, OSD_FromBeginning ) ;
			return( Standard_False ) ;
    		}

		if ( byte == 0 ) {
			RLEcnt = 1 , val = RUN_FLAG ;
		}
		else {
			RLEcnt = byte ;

			file.Read( pb, 1, bblcount ) ;

			if ( file.Failed() || ( bblcount != 1 ) ) {
				// ERROR
				file.Seek( 0, OSD_FromBeginning ) ;
				return( Standard_False ) ;
    			}

			val = byte ;
		}

		for ( i = 0 ; i < RLEcnt ; i++, PixelCount++, p++ ) *p = val ;
	}
  }

  return( Standard_True ) ;
  
}

Standard_Boolean AlienImage_SunRFAlienData::WritePixelRow(
					OSD_File& file,
					const Standard_Address pdata,
					const Standard_Integer rwbytes ) const 
{ Standard_Integer n, n1, n2 = 0;
  Standard_Byte *scanln = ( Standard_Byte * ) pdata ;
  Standard_Byte b ;

  while ( n2 < rwbytes ) {
	n1 = n2 ;
	n2 = n1 + 1 ;

	while( ( n2 < rwbytes ) && ( scanln[n1] == scanln[n2] ) ) n2++ ;

	n = n2 - n1 ;

	if ( n == 1 ) {
		b = scanln[n1]; file.Write( ( Standard_Address ) &b, 1 ) ;

		if ( scanln[n1] == RUN_FLAG ) {
			b = 0 ;	file.Write( ( Standard_Address ) &b, 1 ) ;
		}

    		if ( file.Failed() ) {
			// ERROR
			file.Seek( 0, OSD_FromBeginning ) ;
			return( Standard_False ) ;
    		}

	}
	else {
		while ( n > 256 ) {
			b = RUN_FLAG ; file.Write( ( Standard_Address ) &b, 1) ;
			b = 255 ;      file.Write( ( Standard_Address ) &b, 1) ;
			b = scanln[n1];file.Write( ( Standard_Address ) &b, 1) ;
			n -= 256 ;

    			if ( file.Failed() ) {
				// ERROR
				file.Seek( 0, OSD_FromBeginning ) ;
				return( Standard_False ) ;
    			}
		}

		b = RUN_FLAG ; file.Write( ( Standard_Address ) &b, 1 ) ;
		b = n-1 ;      file.Write( ( Standard_Address ) &b, 1 ) ;
		b = scanln[n1];file.Write( ( Standard_Address ) &b, 1 ) ;

	}
  }

  return( Standard_True ) ;

}

void AlienImage_SunRFAlienData::FromPseudoColorImage(
			const Handle(Image_PseudoColorImage)& TheImage)

{ Standard_Integer rowbytes,i  ;
  Standard_Integer x, y, pix;
  Handle(Image_PseudoColorImage)anImage = 
	TheImage->Squeeze(Aspect_IndexPixel( 0 )) ;
  Handle(Aspect_ColorMap) Cmap = anImage->ColorMap() ;
  Aspect_ColorMapEntry aEntry ;

  FreeData() ;
  myHeader.ras_magic     = RAS_MAGIC ;
  myHeader.ras_width     = anImage->Width() ;
  myHeader.ras_height    = anImage->Height() ;
  myHeader.ras_depth     = 8 ;

  rowbytes = ROWBYTES() ;

  myDataSize = myHeader.ras_height * rowbytes ;
  myData     = Standard::Allocate( myDataSize ) ;
  myHeader.ras_length    = myDataSize ;

  myHeader.ras_maptype   = RMT_EQUAL_RGB ;
  myHeader.ras_maplength = Cmap->Size() ;

  myRedData     = Standard::Allocate( myHeader.ras_maplength ) ;
  myGreenData   = Standard::Allocate( myHeader.ras_maplength ) ;
  myBlueData    = Standard::Allocate( myHeader.ras_maplength ) ;

  Standard_Byte *pr = ( Standard_Byte * ) myRedData ;
  Standard_Byte *pg = ( Standard_Byte * ) myGreenData ;
  Standard_Byte *pb = ( Standard_Byte * ) myBlueData ;

  for ( i = 0 ; i < myHeader.ras_maplength ; i++, pr++, pg++, pb++ ) {
	aEntry = Cmap->FindEntry( i ) ;
	*pr = ( Standard_Byte ) ( aEntry.Color().Red()   * 255. + 0.5 ) ;
	*pg = ( Standard_Byte ) ( aEntry.Color().Green() * 255. + 0.5 ) ;
	*pb = ( Standard_Byte ) ( aEntry.Color().Blue()  * 255. + 0.5 ) ;
  }

  myHeader.ras_maplength *= 3 ;

  if ( myData != NULL ) {
    Standard_Byte *pr = ( Standard_Byte * ) myData ;
    Standard_Byte *p ;

    for ( y = 0 ; y < myHeader.ras_height ; y++, pr += rowbytes ) {
      for ( x = 0, p = pr ; x < myHeader.ras_width ; x++ ) {

		pix = anImage->Pixel( anImage->LowerX()+x , 
			                anImage->LowerY()+y ).Value() ;

		*p = ( Standard_Byte ) pix ; p++ ;
      }
    }		
  }
}

void AlienImage_SunRFAlienData::FromColorImage(
			const Handle_Image_ColorImage& anImage)

{ Standard_Integer rowbytes ;
  Standard_Integer x, y;
  Quantity_Color col ;
  Standard_Real r,g,b ;

  FreeData() ;
 
  myHeader.ras_magic     = RAS_MAGIC ;
  myHeader.ras_width     = anImage->Width() ;
  myHeader.ras_height    = anImage->Height() ;
  myHeader.ras_depth     = 24 ;

  rowbytes = ROWBYTES() ;

  myDataSize = myHeader.ras_height * rowbytes ;
  myData     = Standard::Allocate( myDataSize ) ;
  myHeader.ras_length    = myDataSize ;

  myHeader.ras_maptype   = RMT_NONE ;
  myHeader.ras_maplength = 0 ;

  if ( myData != NULL ) {
    Standard_Byte *pr = ( Standard_Byte * ) myData ;
    Standard_Byte *p ;

    for ( y = 0 ; y < myHeader.ras_height ; y++, pr += rowbytes ) {
      for ( x = 0, p = pr ; x < myHeader.ras_width ; x++ ) {

		col = anImage->Pixel( anImage->LowerX()+x , 
			                anImage->LowerY()+y ).Value() ;

		r = ( Standard_Integer ) ( col.Red()   * 255. + 0.5 );
		g = ( Standard_Integer ) ( col.Green() * 255. + 0.5 );
		b = ( Standard_Integer ) ( col.Blue()  * 255. + 0.5 );

		*p = ( Standard_Byte ) b ; p++ ;
		*p = ( Standard_Byte ) g ; p++ ;
		*p = ( Standard_Byte ) r ; p++ ;
      }
    }		
  }
}

Handle_Image_ColorImage AlienImage_SunRFAlienData::ToColorImage() const

{ Aspect_ColorPixel CPixel ;
  Quantity_Color    acolor ;
  Handle(Image_ColorImage) ret_image = NULL ;
  Standard_Integer x,y, rowbytes ;
  Standard_Real r,g,b ;
  Standard_Byte *pr = ( Standard_Byte * ) myData ;
  Standard_Byte *p ;

  if ( myHeader.ras_depth == 24 || myHeader.ras_depth == 32 ) {
    ret_image = new Image_ColorImage( 0,0,
			(Standard_Integer)myHeader.ras_width,
			(Standard_Integer)myHeader.ras_height ) ;

    rowbytes = ROWBYTES() ;

    for ( y = 0 ; y < myHeader.ras_height ; y++, pr += rowbytes ) {
      for ( x = 0, p = pr ; x < myHeader.ras_width ; x++ ) {
	if ( myHeader.ras_depth == 32 ) p++ ; // Skeep Alpha
	b = ( Standard_Real ) *p / 255. ; p++ ;
	g = ( Standard_Real ) *p / 255. ; p++ ;
	r = ( Standard_Real ) *p / 255. ; p++ ;

	acolor.SetValues( r,g,b, Quantity_TOC_RGB ) ;

	CPixel.SetValue ( acolor ) ;

        ret_image->SetPixel( ret_image->LowerX()+x , 
			     ret_image->LowerY()+y, CPixel ) ;
      }
    }
	
  }

  return( ret_image ) ;
}

Handle_Image_PseudoColorImage AlienImage_SunRFAlienData::ToPseudoColorImage() 
								const

{ Standard_Real r,g,b ;
  Standard_Integer x, y ;
  Handle(Image_PseudoColorImage) ret_image = NULL ;

  if ( myHeader.ras_depth <= 8 && 
       myHeader.ras_maplength ) {	
    Standard_Integer i,rowbytes ;
    Aspect_ColorMapEntry Centry ;
    Quantity_Color       color ;
    Aspect_IndexPixel 	 IPixel ;
    Standard_Byte *red   = ( Standard_Byte * ) myRedData ;
    Standard_Byte *green = ( Standard_Byte * ) myGreenData ;    
    Standard_Byte *blue  = ( Standard_Byte * ) myBlueData ;
    Standard_Byte *p ;
    Standard_Byte *pr = ( Standard_Byte * ) myData ;
    Handle(Aspect_GenericColorMap) colormap = 
	new Aspect_GenericColorMap();

    for ( i = 0 ; i < myHeader.ras_maplength/3 ; i++, red++, green++, blue++ ) {
		r = ( Standard_Real ) *red   / 255. ;
		g = ( Standard_Real ) *green / 255. ;
		b = ( Standard_Real ) *blue  / 255. ;
		color.SetValues( r,g,b, Quantity_TOC_RGB );
		Centry.SetValue( i, color ) ;
		colormap->AddEntry( Centry ) ;
    }

    ret_image = new Image_PseudoColorImage( 0,0,
				   Standard_Integer(myHeader.ras_width),
				   Standard_Integer(myHeader.ras_height),
				   colormap ) ;

    rowbytes = ROWBYTES() ;

    for ( y = 0 ; y < myHeader.ras_height ; y++, pr += rowbytes ) {
      for ( x = 0, p = pr ; x < myHeader.ras_width ; x++, p++ ) {

	IPixel.SetValue( Standard_Integer( *p ) ) ;
        ret_image->SetPixel( ret_image->LowerX()+x , 
			     ret_image->LowerY()+y, IPixel ) ;
      }
    }
  }

    return ret_image ;
}
 
