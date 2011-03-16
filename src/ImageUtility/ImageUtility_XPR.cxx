// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
//

#include <Image_Image.hxx>
#include <AlienImage_XAlienImage.hxx>
#include <OSD_Process.hxx>
#include <OSD_File.hxx>
#include <ImageUtility_XPR.ixx>

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

void ImageUtility_XPR::XPR( 
		const Handle(AlienImage_AlienUserImage)& aAlienImage,
		const Standard_CString aName,
		const Standard_CString xprOptions )
{ if ( aAlienImage->IsKind(STANDARD_TYPE(AlienImage_XAlienImage) ) ) {
   	Handle(AlienImage_XAlienImage) aXImage =
		Handle(AlienImage_XAlienImage)::DownCast(aAlienImage) ;

	aXImage->SetName( TCollection_AsciiString( aName ) ) ;

	XPR( aXImage, xprOptions ) ;
  }
  else {
    XPR( aAlienImage->ToImage(), aName, xprOptions ) ;
  }
}

void ImageUtility_XPR::XPR( const Handle(Image_Image)& aImage,
	     			    const Standard_CString aName,
				    const Standard_CString xprOptions )

{ XAlienImage->Clear() ;
  XAlienImage->FromImage( aImage ) ;

  XAlienImage->SetName( TCollection_AsciiString( aName ) ) ;

  XPR( XAlienImage, xprOptions ) ;
}

void ImageUtility_XPR::XPR( 
	const Handle(AlienImage_XAlienImage)& aXImage,
	const Standard_CString xprOptions )

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

  XPR( File, xprOptions ) ;
}


void ImageUtility_XPR::XPR( const OSD_File& File,
				    const Standard_CString xprOptions )

{ 
  TCollection_AsciiString s, Name ;
  OSD_Path Path ;
  OSD_Process Process ;

  File.Path( Path ) ;

  Path.SystemName( Name, OSD_Default ) ;

  XPR( Name.ToCString(), xprOptions ) ;
}

void ImageUtility_XPR::XPR( const Standard_CString aName,
				    const Standard_CString xprOptions )

{ 
  TCollection_AsciiString s;
  OSD_Process Process ;

  s = TCollection_AsciiString("xpr ") 
	+ TCollection_AsciiString(xprOptions) 
	+ TCollection_AsciiString(" ") 
	+ aName
	+ TCollection_AsciiString( " | lpr &" );

  Process.Spawn( s ) ;

}

