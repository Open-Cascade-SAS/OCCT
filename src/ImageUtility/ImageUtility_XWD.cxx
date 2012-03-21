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

