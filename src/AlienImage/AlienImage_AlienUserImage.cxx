#include <OSD_Process.hxx>
#include <OSD_Protection.hxx>
#include <AlienImage_XAlienImage.hxx>
#include <AlienImage_AlienUserImage.ixx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

AlienImage_AlienUserImage::AlienImage_AlienUserImage()
{
#ifdef TRACE
	cout << "AlienImage_AlienUserImage constructor \n" ;
#endif
}

#ifdef IMPLEMENTED
AlienImage_AlienUserImage::AlienImage_AlienUserImage( 
		const Standard_CString afile,
		Standard_Boolean& Success )
{
#ifdef TRACE
	cout << "AlienImage_AlienUserImage constructor \n" ;
#endif

  Success = Read( afile ) ;
}

AlienImage_AlienUserImage::AlienImage_AlienUserImage( 
		const Handle(Image_Image)& anImage)
{
#ifdef TRACE
	cout << "AlienImage_AlienUserImage constructor \n" ;
#endif

  FromImage( anImage ) ;
}
#endif

Standard_Boolean AlienImage_AlienUserImage::Read( const Standard_CString file )
{ OSD_File 		File ;
  OSD_Protection 	Protection ;
  OSD_Path 		Path;
  Standard_Boolean	Ret ;

  Protection.SetValues( OSD_R, OSD_R, OSD_R, OSD_R ) ;   
  Path = OSD_Path( TCollection_AsciiString( file ),OSD_Default);
  File.SetPath   ( Path );                                  // Instanciate
  File.Open      ( OSD_ReadOnly, Protection ) ;

  Ret = Read( File ) ;

  File.Close() ;

  return( Ret ) ; 
}

Standard_Boolean AlienImage_AlienUserImage::Write(
					const Standard_CString file)const

{ OSD_File 		File ;
  OSD_Protection 	Protection ;
  OSD_Path 		Path;
  Standard_Boolean	Ret ;

  Protection.SetValues( OSD_RW, OSD_RW, OSD_RW, OSD_RW ) ;   
  Path = OSD_Path( TCollection_AsciiString( file ),OSD_Default);
  File.SetPath   ( Path );                                  // Instanciate
  File.Build     ( OSD_WriteOnly, Protection ) ;

  Ret = Write( File ) ;

  File.Close() ;

  return( Ret ) ; 
}
 
