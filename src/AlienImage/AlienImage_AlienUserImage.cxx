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
 
