// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
//

#include <Image_Image.hxx>
#include <AlienImage_XAlienImage.hxx>
#include <OSD_Process.hxx>
#include <OSD_File.hxx>
#include <ImageUtility_XWUD.ixx>

#define LOPTIM
#ifndef LOPTIM
static Handle(AlienImage_XAlienImage) XAlienImage = new AlienImage_XAlienImage();
#else 
static Handle(AlienImage_XAlienImage)& _XAlienImage() {
    static Handle(AlienImage_XAlienImage) XAlienImage = new AlienImage_XAlienImage();
return XAlienImage;
}
#define XAlienImage _XAlienImage()
#endif // LOPTIM

void ImageUtility_XWUD::XWUD( 
		const Handle(AlienImage_AlienUserImage)& aAlienImage,
		const Standard_CString aName,
		const Standard_CString xwudOptions )

{ 
 if ( aAlienImage->IsKind(STANDARD_TYPE(AlienImage_XAlienImage) ) ) {
   	Handle(AlienImage_XAlienImage) aXImage =
		Handle(AlienImage_XAlienImage)::DownCast(aAlienImage) ;

	aXImage->SetName( TCollection_AsciiString( aName ) ) ;

	XWUD( aXImage, xwudOptions ) ;
  }
  else {
    XWUD( aAlienImage->ToImage(), aName, xwudOptions ) ;
  }
}

void ImageUtility_XWUD::XWUD( const Handle(Image_Image)& aImage,
	     			    const Standard_CString aName,
				    const Standard_CString xwudOptions )

{ XAlienImage->Clear() ;
  XAlienImage->FromImage( aImage ) ;

  XAlienImage->SetName( TCollection_AsciiString( aName ) ) ;

  XWUD( XAlienImage, xwudOptions ) ;
}

void ImageUtility_XWUD::XWUD( 
	const Handle(AlienImage_XAlienImage)& aXImage,
	const Standard_CString xwudOptions )

{ 
  OSD_File File = OSD_File::BuildTemporary() ;

#ifdef TRACE
  OSD_Path Path ;
  TCollection_AsciiString Name ;

  File.Path(Path) ;

  Path.SystemName( Name ) ;

  cout << "BuildTemporaryFile :" << Name << endl << flush ;
#endif

  aXImage->Write( File ) ;

  File.Close() ;

  XWUD( File, xwudOptions ) ;
}


void ImageUtility_XWUD::XWUD( const OSD_File& File,
				    const Standard_CString xwudOptions )

{ 
  TCollection_AsciiString s, Name ;
  OSD_Path Path ;
  OSD_Process Process ;

  File.Path( Path ) ;

  Path.SystemName( Name, OSD_Default ) ;

  XWUD( Name.ToCString(), xwudOptions ) ;
}

void ImageUtility_XWUD::XWUD( const Standard_CString aName,
				    const Standard_CString xwudOptions )

{ 
  TCollection_AsciiString s;
  OSD_Process Process ;

  s = TCollection_AsciiString("xwud ") 
	+ TCollection_AsciiString(xwudOptions) 
	+ TCollection_AsciiString(" -in ") 
	+ aName
	+ TCollection_AsciiString( "&" );

  Process.Spawn( s ) ;

}

