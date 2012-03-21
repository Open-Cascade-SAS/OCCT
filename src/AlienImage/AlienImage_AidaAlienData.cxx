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

#define TEST    //GG_140699
//              Check file extension, must be ".aida".

#include <TCollection_AsciiString.hxx>
#include <Image_Convertor.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_ColorCubeColorMap.hxx>
#include <AlienImage_AidaAlienData.ixx>
#include <Aspect_ColorMapEntry.hxx>

# include <stdio.h>

#ifdef TRACE
static int Verbose = 0 ;
#endif


AlienImage_AidaAlienData::AlienImage_AidaAlienData()

{
  myDataIsDef		= Standard_False ;
  myColorsIsDef		= Standard_False ;
  myDitheringColorMap	= new Aspect_ColorCubeColorMap( 40, 5,1, 8,6, 3,54 ) ;
  myDitheringMethod	= Image_DM_NearestColor ;
}

void AlienImage_AidaAlienData::Clear()

{
  myDataIsDef	= Standard_False ;
  myColorsIsDef	= Standard_False ;
}

Standard_Boolean AlienImage_AidaAlienData::Write( OSD_File& file ) const

{ Standard_Integer  r,c ;
  TCollection_AsciiString out ;
  char hexa[3] ;
  unsigned char p ;
  unsigned long int rc,gc,bc ;
  TCollection_AsciiString Space = " " ;

  if ( ! myDataIsDef )   	return( Standard_False ) ;
  if ( ! myColorsIsDef ) 	return( Standard_False ) ;

  out = TCollection_AsciiString( "#BC(" ) +
	TCollection_AsciiString( myData->RowLength() ) +
	Space +
	TCollection_AsciiString( myData->ColLength() ) +
	TCollection_AsciiString( " #[\n" ) ;

  file.Write( out, out.Length() ) ;

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  for ( r = myData->LowerRow() ; r <= myData->UpperRow() ; r++ ) {
    out = TCollection_AsciiString( "#*" ) ;
    for ( c = myData->LowerCol() ; c <= myData->UpperCol() ; c++ ) {
	p = ( unsigned char ) myData->Value( r, c ) ;
	sprintf( hexa , "%.2x", p ) ;
	out += TCollection_AsciiString( hexa ) ;
    }
    out += TCollection_AsciiString( "\n" ) ;

    file.Write( out, out.Length() ) ;
  
    if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
    }
  }

  out = TCollection_AsciiString( "](\n" ) ;

  file.Write( out, out.Length() ) ;

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  // write out the color map buffer
  for ( c = 1 ; c <= myColors->Size() ; c++ ) {
	rc = (long unsigned int )( myColors->Entry(c).Color().Red()   * 32767 + 0.5 );
	gc = (long unsigned int )( myColors->Entry(c).Color().Green() * 32767 + 0.5 );
	bc = (long unsigned int )( myColors->Entry(c).Color().Blue()  * 32767 + 0.5 );

	out = TCollection_AsciiString( "#[" ) + 
	      TCollection_AsciiString( myColors->Entry(c).Index() ) + Space +
	      TCollection_AsciiString( Standard_Integer(rc) ) + Space +
	      TCollection_AsciiString( Standard_Integer(gc) ) + Space +
	      TCollection_AsciiString( Standard_Integer(bc) ) + Space +
	      TCollection_AsciiString( "()]\n" ) ;

	file.Write( out, out.Length() ) ;

	if ( file.Failed() ) {
		// ERROR
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ;
        }

  }

  out = TCollection_AsciiString( "))" ) ;

  file.Write( out, out.Length() ) ;

  if ( file.Failed() ) {
	// ERROR
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ;
  }

  return( Standard_True ) ;
  
}


Standard_Boolean AlienImage_AidaAlienData::Read( OSD_File& file )

{ TCollection_AsciiString tmp, in ;
  Standard_Integer loc1, loc2, width, height, i, bblcount, l ,value ;
  TCollection_AsciiString HexaSet = "0123456789ABCDEFabcdef" ;
  int r,g,b,pixel,status ;
  Handle(Aspect_GenericColorMap) GenCmap = NULL ;

#ifdef TEST
  OSD_Path path; file.Path(path);
  TCollection_AsciiString ext = path.Extension(); ext.LowerCase();
  if( ext != ".aida" ) {
    TCollection_AsciiString sysname; path.SystemName(sysname);
#ifdef TRACE
    cout << " *** AlienImage_AidaAlienData::Read('" << sysname << "'). must have an '.aida' extension" << endl;
#endif
    return Standard_False;
  }
#endif

  Clear() ;

  // Extract AidaBYTEMAPS type "#BC"

  // Get "#"
  do { 
	  file.Read( tmp, 1) ;
  } while( !file.Failed() && tmp != TCollection_AsciiString( "#" ) ) ;

  if ( file.Failed() ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }

  // Get "BC"
  file.Read( tmp, 2 ) ;

  if ( file.Failed() ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }


  if ( tmp != TCollection_AsciiString( "BC" ) ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }

  // Get "(" Start of AidaBYTEMAPS size definition
  do { 
	  file.Read( tmp, 1) ;
  } while( !file.Failed() && tmp != TCollection_AsciiString( "(" ) ) ;

  if ( file.Failed() ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }


  // Get "#" Start of AidaBYTEMAPS Pixel definition
  in.Clear() ;

  do { 
	file.Read( tmp, 1) ;

	if ( tmp.IsAscii() ) {
		in += tmp ;
	}
  } while( !file.Failed() && tmp != TCollection_AsciiString( "#" ) ) ;

  if ( file.Failed() ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }


  // Extract Image size in "Width Height #"

  loc1 = 1 ;

  if ( ( loc2 = in.Location( 1, ' ', loc1, in.Length() ) ) == 0 ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }

  if ( ( loc1 ) > ( loc2-1 ) ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }


  tmp = in.SubString( loc1, loc2-1 ) ;

  if ( !tmp.IsIntegerValue() ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }

  if ( ( width = tmp.IntegerValue() ) <= 0 ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }

  tmp = in.SubString( loc2, in.Length()-1 ) ;
  
  if ( !tmp.IsIntegerValue() ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }


  if ( ( height = tmp.IntegerValue() ) <= 0 ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }


#ifdef TRACE
  if ( Verbose )
    cout << "Width,Height :" << width << "," << height << endl << flush ;
#endif

  AllocData( width, height ) ;

  // Get "[" start of AidaBYTEMAPS Pixel definition

  do {
	file.Read( tmp, 1) ;
  } while( !file.Failed() && tmp != TCollection_AsciiString( "[" ) ) ;

  if ( file.Failed() ) {
	file.Seek( 0, OSD_FromBeginning ) ;
	return( Standard_False ) ; // ERROR
  }


  // Extract Pixel lines "#*................."
  in.Clear() ;

  for ( i = 0 ; i < height ; i++ ) {

 	// Get "#"
 	do { 
	  file.Read( tmp, 1) ;
  	} while( !file.Failed() && tmp != TCollection_AsciiString( "#" ) ) ;

  	if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
  	}

	// Get "*"
	file.Read( tmp, 1) ;

  	if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
  	}

	if ( tmp != TCollection_AsciiString( "*" ) ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
  	}

        // Get Pixel line
	bblcount = 0 ;
        in.Clear() ;

	while ( bblcount < (2*width) ) {// Two byte per PIXEL
	  file.Read( tmp, 1) ; 

  	  if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
  	  }

	  if ( tmp.IsAscii() ) {
		if ( tmp.FirstLocationInSet( HexaSet, 1, 1 ) ) {
			in += tmp ; bblcount++ ;
		}
		else {
			// Get Next line character before end of current line
			file.Seek( 0, OSD_FromBeginning ) ;
			return( Standard_False ) ; // ERROR
		}
	  }
	}

#ifdef TRACE
  if ( Verbose )
	cout << "Get one line :\"" << in << "\"\n" << flush ;
#endif

	for ( l = 0 ; l < in.Length() ; l += 2 ) {
		tmp = in.SubString( l+1, l+2 ) ;

		value = ( Standard_Integer )strtol( tmp.ToCString(), NULL, 16 );

		SetPixel( l/2 , i, value ) ;
#ifdef TRACE
  if ( Verbose > 1 )
	cout << "SetPixel(" << l/2 << "," << i << "," << value << ")\n" <<flush;
#endif
	}
  }

  // Get "]" end of Aida image data
  do { 
	  file.Read( tmp, 1) ;
  } while( !file.Failed() && tmp != TCollection_AsciiString( "]" ) ) ;

  if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
  }

  // Get "(" start of Aida colors definition or ")" end of Aida ByteMaps
  do { 
	  file.Read( tmp, 1) ;
  } while( !file.Failed() && 
	   tmp != TCollection_AsciiString( "(" ) &&
	   tmp != TCollection_AsciiString( ")" ) ) ;

  if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
  }

  if ( tmp == TCollection_AsciiString( ")" ) ) {
	return( Standard_True ) ; // No Color definition ERROR ??  
  }

  // Find Color definition "#[ Red Green Blue ... ]" color is from 0->32767

  myColors = GenCmap = new Aspect_GenericColorMap() ;
  myColorsIsDef	= Standard_True ;

  do {
    // Get "#" start of a Aida color definition or 
    // Get ")" end of Aida colors definition
    do { 
	  file.Read( tmp, 1) ;
    } while( !file.Failed() && 
	     tmp != TCollection_AsciiString( "#" ) &&
	     tmp != TCollection_AsciiString( ")" ) ) ;

    if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
    }

    if ( tmp == TCollection_AsciiString( "#" ) ) {

      // Get "[" start of a Aida color definition
      do { 
	  file.Read( tmp, 1) ;
      } while( !file.Failed() && tmp != TCollection_AsciiString( "[" ) ) ;

      if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
      }

      // Get "]" end of a Aida color definition
      in.Clear() ;

      do { 
	  file.Read( tmp, 1) ;
	  if ( tmp.IsAscii() && tmp != TCollection_AsciiString( "]" ) ) {
		in += tmp ;
	  }
      } while( !file.Failed() && tmp != TCollection_AsciiString( "]" ) ) ;

      if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
      }

      status = sscanf( in.ToCString(), "%d %d %d %d", &pixel,&r,&g,&b ) ;

      if ( status == 4 ) {
	GenCmap->AddEntry( 
		Aspect_ColorMapEntry( pixel,
			  Quantity_Color( r/32767.,g/32767.,b/32767.,
					   Quantity_TOC_RGB ) ) ) ;			

      }	

#ifdef TRACE
  if ( Verbose )
      cout << in << endl << flush ;
#endif
    }
    
  } while ( !file.Failed() && tmp != TCollection_AsciiString( ")" ) )  ;


  if ( file.Failed() ) {
		file.Seek( 0, OSD_FromBeginning ) ;
		return( Standard_False ) ; // ERROR
  }

  
  return( Standard_True ) ;
  
}

Handle_Image_Image AlienImage_AidaAlienData::ToImage() const

{ Standard_Integer x,y ;
  Handle(Image_PseudoColorImage) aPImage = 
	new Image_PseudoColorImage( 0, 0,
				    myData->RowLength(), myData->ColLength(),
				    myColors );

  for( y = 0 ; y < aPImage->Height() ; y++ ) {
    for( x = 0 ; x < aPImage->Width() ; x++ ) {
	aPImage->SetPixel( aPImage->LowerX()+x, 
			   aPImage->LowerY()+y, Pixel( x, y ) ) ;
    }
  }

  return aPImage ;
}

void AlienImage_AidaAlienData::FromImage( const Handle_Image_Image& anImage )

{ if ( anImage->IsInstance(STANDARD_TYPE(Image_PseudoColorImage)) ) {
	Handle(Image_PseudoColorImage) aPImage =
		Handle(Image_PseudoColorImage)::DownCast(anImage) ;

	FromPseudoColorImage( aPImage ) ;
  }
  else if ( anImage->IsInstance(STANDARD_TYPE(Image_ColorImage)) )  {
	Handle(Image_ColorImage) aCImage =
		Handle(Image_ColorImage)::DownCast(anImage) ;

	FromColorImage( aCImage ) ;
  }
  else {
 	 Standard_TypeMismatch_Raise_if( Standard_True,
	 "Attempt to convert a unknown Image_Image type to a AidaAlienData");
  }
}

void AlienImage_AidaAlienData::SetColorImageDitheringMethod( 
	const Image_DitheringMethod aMethod ,
	const Handle(Aspect_ColorMap)& aColorMap )

{ myDitheringMethod = aMethod ; myDitheringColorMap = aColorMap ; }

//------------------------------------------------------------------------------
//		Private Method
//------------------------------------------------------------------------------


void AlienImage_AidaAlienData::FromPseudoColorImage(
			const Handle_Image_PseudoColorImage& anImage)

{ Standard_Integer x,y ;

  AllocData( anImage->Width(), anImage->Height() ) ;

  myColors = anImage->ColorMap() ;
  myColorsIsDef	= Standard_True ;

  for ( y = 0 ; y < anImage->Height() ; y++ ) {
    for ( x = 0 ; x < anImage->Width() ; x++ ) {
	SetPixel( x, 
		  y, 
		  anImage->Pixel( anImage->LowerX()+x,
				  anImage->LowerY()+y ).Value() ) ;
    }
  }
}

void AlienImage_AidaAlienData::FromColorImage(
			const Handle_Image_ColorImage& anImage)

{ Image_Convertor Convertor ;

  Handle(Image_PseudoColorImage) aPImage = 
	new Image_PseudoColorImage( anImage->LowerX(), anImage->LowerY(),
				    anImage->Width(),  anImage->Height(),
				    myDitheringColorMap ) ;

  Convertor.SetDitheringMethod( myDitheringMethod ) ;

  aPImage = Convertor.Convert( anImage, myDitheringColorMap ) ;

  FromPseudoColorImage( aPImage ) ;
}

void AlienImage_AidaAlienData::AllocData( 
	const Standard_Integer dx, 
	const Standard_Integer dy )
{
  myData = new TColStd_HArray2OfInteger( 0, dy-1 , 0, dx-1 ) ;
  myDataIsDef = Standard_True ;
}

void AlienImage_AidaAlienData::SetPixel( 
	const Standard_Integer x, 
	const Standard_Integer y, 
	const Standard_Integer value )
{
  myData->SetValue( y, x, value ) ;
}

Standard_Integer AlienImage_AidaAlienData::Pixel( 
	const Standard_Integer x, const Standard_Integer y ) const

{ return myData->Value( y, x ) ; }
 
