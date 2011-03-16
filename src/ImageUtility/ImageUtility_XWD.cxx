#include <Image_Image.hxx>
#include <AlienImage_XAlienImage.hxx>
#include <OSD_Process.hxx>
#include <OSD_File.hxx>
#include <ImageUtility_XWD.ixx>

ImageUtility_XWD::ImageUtility_XWD()

{ myXAlienImage = NULL ;
  myImage	= NULL ;
}

Handle(Image_Image) ImageUtility_XWD::Image() const 
{ return myImage ; }

Handle(AlienImage_XAlienImage) ImageUtility_XWD::XAlienImage() const
{ return myXAlienImage; }
 
Standard_Boolean ImageUtility_XWD::XWD( const Standard_CString xwdOptions )

{ Standard_Boolean RetStatus ;

  OSD_File File = OSD_File::BuildTemporary() ;

  TCollection_AsciiString s, Name ;
  OSD_Path Path ;
  OSD_Process Process ;

  File.Path( Path ) ;

  Path.SystemName( Name, OSD_Default ) ;

#ifdef TRACE
  cout << "BuildTemporaryFile :" << Name << endl << flush ;
#endif

  s = TCollection_AsciiString("xwd ") 
	+ TCollection_AsciiString(xwdOptions) 
	+ TCollection_AsciiString(" -out ") 
	+ Name ;

  Process.Spawn( s ) ;

  myXAlienImage = new AlienImage_XAlienImage() ;

  if (( RetStatus = myXAlienImage->Read( File ) )) {
	myImage = myXAlienImage->ToImage() ;
  }
  else {
	myImage 	= NULL ;
	myXAlienImage 	= NULL ;
  }

  return RetStatus ;
}

