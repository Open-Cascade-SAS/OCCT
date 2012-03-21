// Created by: GG
// Copyright (c) 1991-1999 Matra Datavision
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

#define PURIFY	//GG_180297
//		Liberation memoire allouee par les icones.



//-Version

//-Design	Creation d'une Icon Box 

//-Warning

//-References

//-Language	C++ 2.0

//-Declarations

// for the class
#include <Xw_IconBox.ixx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Static data definitions

static XW_STATUS status ;

//-Aliases

//-Global data definitions

//-Constructors

Xw_IconBox::Xw_IconBox (const Handle(Xw_GraphicDevice)& Device, const Standard_CString Name, const Standard_Real Xc, const Standard_Real Yc, const Quantity_NameOfColor BackColor,const Xw_WindowQuality aQuality) : 
	Xw_Window(Device,Name,Xc,Yc,0.1,0.1,aQuality,BackColor,0) {

	MyNumberOfIcons = Xw_load_icons(MyExtendedWindow,(Standard_PCharacter)Name) ;

	if( !MyNumberOfIcons ) {
	    PrintError() ;
    	}
}

void Xw_IconBox::Destroy () {

	MyNumberOfIcons = 0 ;
#ifdef PURIFY
   	if( MyExtendedWindow ) Xw_clear_icons(MyExtendedWindow,NULL);
#endif
}

Standard_Integer Xw_IconBox::UnloadIcons ( const Standard_CString Name) {
Standard_Integer nicons = 0;

        nicons = Xw_clear_icons (MyExtendedWindow,(Standard_PCharacter)Name);

        if( !nicons ) Xw_print_error() ; 

	MyNumberOfIcons -= nicons ;

	return nicons;
}

void Xw_IconBox::Show () const {

        status = Xw_show_icons (MyExtendedWindow);

        if( !status ) Xw_print_error() ; 
}

Standard_Integer Xw_IconBox::LoadIcons ( const Standard_CString Name ) {
int nicons ;

	nicons = Xw_load_icons(MyExtendedWindow,(Standard_PCharacter)Name) ;

        if( nicons > 0 ) MyNumberOfIcons += nicons ;
	else Xw_print_error() ; 

	return nicons;
}

void Xw_IconBox::AddIcon ( const Handle(Xw_Window)& W,const Standard_CString Name,
		const Standard_Integer aWidth,const Standard_Integer aHeight ) {
	status = Xw_put_window_icon(MyExtendedWindow,W->ExtendedWindow(),
				    (Standard_PCharacter)Name,aWidth,aHeight) ;

        if( status ) MyNumberOfIcons++ ;
	else	Xw_print_error() ; 
}

Standard_Integer Xw_IconBox::SaveIcons () const {
Standard_Integer nicons = 0;

	nicons = Xw_save_icons(MyExtendedWindow) ;

	return nicons;
}

Standard_Integer Xw_IconBox::IconNumber () const {

	return (MyNumberOfIcons);
}

Standard_CString Xw_IconBox::IconName ( const Standard_Integer Index) const {
char *Name = NULL ;

	if( Index < 0 || Index > MyNumberOfIcons ) {
	    PrintError() ;
	}

	Name = Xw_get_icon_name(MyExtendedWindow,(int)Index) ;

	if( !Name ) Xw_print_error() ;

	return (Name);
}

Standard_Boolean Xw_IconBox::IconSize ( const Standard_CString Name, Standard_Integer& Width, Standard_Integer& Height ) const {
int width,height ;

	status = Xw_get_icon_size( MyExtendedWindow,(Standard_PCharacter)Name,&width,&height) ;

	if( status ) {
	    Width = width ;
	    Height = height ;
	} else {
	    Xw_print_error() ;
	}

	return Standard_Boolean(status);
}

Aspect_Handle Xw_IconBox::IconPixmap ( const Standard_CString Name ) const {
Aspect_Handle pixmap = 0 ;

	pixmap = Xw_get_icon_pixmap( MyExtendedWindow,0,0,(Standard_PCharacter)Name) ;

	if( !pixmap ) Xw_print_error() ;

	return (pixmap);
}

Aspect_Handle Xw_IconBox::IconPixmap ( const Standard_CString Name, const Standard_Integer Width,const Standard_Integer Height ) const {
Aspect_Handle pixmap = 0 ;

	pixmap = Xw_get_icon_pixmap( MyExtendedWindow,
				    (int)Width,(int)Height,
				    (Standard_PCharacter)Name) ;

	if( !pixmap ) Xw_print_error() ;

	return (pixmap);
}
