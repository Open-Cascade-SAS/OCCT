// Created on: 1993-10-14
// Created by: GG
// Copyright (c) 1993-1999 Matra Datavision
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

#define GG001	//GG_300197
//		Preferer utiliser le visual de la root window en mode
//		2D plutot que de prendre un visual PseudoColor alors que la root
//		est TrueColor.
//		Ceci corrige notamment les pb de colormap non intallees sur DEC
//		lorsque la station est bootee en TrueColor.



//-Version

//-Design	Creation d'une unite graphique

//-Warning

//-References

//-Language	C++ 2.0

//-Declarations

// for the class
#include <stdio.h>
#include <Xw.hxx>
#include <Xw_GraphicDevice.ixx>

// Routines C a declarer en extern
//extern "C" {
#include <Xw_Cextern.hxx>
//}

//-Static data definitions

//static char *ErrorMessag,LocalMessag[80] ;
static char LocalMessag[80] ;
//static int ErrorNumber ;
//static int ErrorGravity ;
static XW_STATUS status ;

//-Aliases

//-Global data definitions

//-Constructors

Xw_GraphicDevice::Xw_GraphicDevice () : Aspect_GraphicDevice() {
  MyExtendedDisplay = NULL;
}

Xw_GraphicDevice::Xw_GraphicDevice (const Standard_CString Connexion, const Xw_TypeOfMapping Mapping, const Standard_Integer Ncolors, const Standard_Boolean UseDefault)  : Aspect_GraphicDevice() {

	InitMaps(Connexion,Mapping,Ncolors,UseDefault) ;
}

void Xw_GraphicDevice::Destroy () {

	if( MyColorMap2D != MyColorMap3D ) {
	  MyColorMap3D.Nullify();
	  MyColorMap2D.Nullify();
	} else {
	  MyColorMap2D.Nullify();
	}
	MyTypeMap.Nullify();
	MyWidthMap.Nullify();
	MyFontMap.Nullify();
	MyMarkMap.Nullify();

	if( MyExtendedDisplay ) {
	  status = Xw_close_display(MyExtendedDisplay) ;

	  MyExtendedDisplay = NULL;
        }
}

void Xw_GraphicDevice::InitMaps (const Standard_CString Connexion,const Xw_TypeOfMapping Mapping,const Standard_Integer Ncolors,const Standard_Boolean UseDefault) {
#ifdef GG001
Xw_TypeOfVisual v2d = Xw_TOV_DEFAULT ;
#else
Xw_TypeOfVisual v2d = Xw_TOV_PREFERRED_PSEUDOCOLOR ;
#endif
Xw_TypeOfVisual v3d = Xw_TOV_PREFERRED_TRUECOLOR ;

	Xw_set_trace(Xw::TraceLevel(),Xw::ErrorLevel()) ;

	if( Connexion ) {
	    MyDisplay = TCollection_AsciiString(Connexion) ;
	} else {
	    MyDisplay = TCollection_AsciiString("") ;
	}

	MyExtendedDisplay = Xw_open_display((Standard_PCharacter)MyDisplay.ToCString()) ;

	if( !MyExtendedDisplay ) {
	   sprintf(LocalMessag,"Cannot connect to server '%s'",
						MyDisplay.ToCString());
           Aspect_GraphicDeviceDefinitionError::Raise (LocalMessag);
        }

	v2d = Xw_get_display_visual(MyExtendedDisplay,v2d) ;
	MyColorMap2D = new Xw_ColorMap(MyDisplay.ToCString(),
					v2d,Mapping,Ncolors,UseDefault) ;

	v3d = Xw_get_display_visual(MyExtendedDisplay,v3d) ;
	if( v2d != v3d ) {
	    MyColorMap3D = new Xw_ColorMap(MyDisplay.ToCString(),
					v3d,Mapping,Ncolors,UseDefault) ;
	} else {
	    MyColorMap3D = MyColorMap2D ;
 	}

	MyTypeMap = new Xw_TypeMap(MyDisplay.ToCString()) ;
	MyWidthMap = new Xw_WidthMap(MyDisplay.ToCString()) ;
	MyFontMap = new Xw_FontMap(MyDisplay.ToCString()) ;
	MyMarkMap = new Xw_MarkMap(MyDisplay.ToCString()) ;

Standard_Address Bof;
	Bof	= ExtendedColorMap2D();
	Bof	= ExtendedColorMap3D();
	Bof	= ExtendedTypeMap();
	Bof	= ExtendedWidthMap();
	Bof	= ExtendedFontMap();
	Bof	= ExtendedMarkMap();

	if( !ExtendedColorMap2D() ||
	    !ExtendedColorMap3D() ||
	    !ExtendedTypeMap() ||
	    !ExtendedWidthMap() ||
	    !ExtendedFontMap() ||
	    !ExtendedMarkMap() ) {
	   sprintf(LocalMessag,"Bad Graphic Device Attributs on '%s'",
								Connexion);
           Aspect_GraphicDeviceDefinitionError::Raise (LocalMessag);
	}
}

Handle(Xw_ColorMap) Xw_GraphicDevice::ColorMap2D () const {

	return (MyColorMap2D) ;

}

Xw_TypeOfVisual Xw_GraphicDevice::VisualClass2D () const {

	return (MyColorMap2D->VisualClass()) ;
}

Xw_TypeOfVisual Xw_GraphicDevice::OverlayVisualClass2D () const {

	return (MyColorMap2D->OverlayVisualClass()) ;
}

Standard_Address Xw_GraphicDevice::ExtendedColorMap2D () const {

        return (MyColorMap2D->ExtendedColorMap());
}

Standard_Address Xw_GraphicDevice::ExtendedOverlayColorMap2D () const {

        return (MyColorMap2D->ExtendedOverlayColorMap());
}

Handle(Xw_ColorMap) Xw_GraphicDevice::ColorMap3D () const {

	return (MyColorMap3D) ;
}

Xw_TypeOfVisual Xw_GraphicDevice::VisualClass3D () const {

	return (MyColorMap3D->VisualClass()) ;
}

Xw_TypeOfVisual Xw_GraphicDevice::OverlayVisualClass3D () const {

	return (MyColorMap3D->OverlayVisualClass()) ;
}

Standard_Address Xw_GraphicDevice::ExtendedColorMap3D () const {

        return (MyColorMap3D->ExtendedColorMap());
}

Standard_Address Xw_GraphicDevice::ExtendedOverlayColorMap3D () const {

        return (MyColorMap3D->ExtendedOverlayColorMap());
}

Handle(Xw_TypeMap) Xw_GraphicDevice::TypeMap () const {

	return (MyTypeMap) ;
}

Standard_Address Xw_GraphicDevice::ExtendedTypeMap () const {

        return (MyTypeMap->ExtendedTypeMap());
}

Handle(Xw_WidthMap) Xw_GraphicDevice::WidthMap () const {

	return (MyWidthMap) ;
}

Standard_Address Xw_GraphicDevice::ExtendedWidthMap () const {

        return (MyWidthMap->ExtendedWidthMap());
}

Handle(Xw_FontMap) Xw_GraphicDevice::FontMap () const {

	return (MyFontMap) ;
}

Standard_Address Xw_GraphicDevice::ExtendedFontMap () const {

        return (MyFontMap->ExtendedFontMap());
}

Handle(Xw_MarkMap) Xw_GraphicDevice::MarkMap () const {

	return (MyMarkMap) ;
}

Standard_Address Xw_GraphicDevice::ExtendedMarkMap () const {

        return (MyMarkMap->ExtendedMarkMap());
}

Standard_CString Xw_GraphicDevice::Display () const {

        return (MyDisplay.ToCString());
}

Standard_Address Xw_GraphicDevice::XDisplay () const {
Aspect_Handle *display ;
Aspect_Handle root,colormap ;
Xw_TypeOfVisual pclass ;
int depth ;

	status = Xw_get_display_info(MyExtendedDisplay,&display,
				&root,&colormap,&pclass,&depth) ;

        return (display);
}

Standard_Address Xw_GraphicDevice::ExtendedDisplay () const {

        return (MyExtendedDisplay);
}

void Xw_GraphicDevice::DisplaySize (Standard_Integer &Width,Standard_Integer &Height) const {
int width,height ;

	status = Xw_get_screen_size(MyExtendedDisplay,&width,&height) ;

	Width = width ;
	Height = height ;
}

void Xw_GraphicDevice::DisplaySize (Quantity_Length &Width,Quantity_Length &Height) const {
float width,height ;

	status = Xw_get_mmscreen_size(MyExtendedDisplay,&width,&height) ;

	Width = FROMMILLIMETER(width) ;
	Height = FROMMILLIMETER(height) ;
}

Standard_Integer Xw_GraphicDevice::PlaneLayer (const Standard_Integer aVisualID) const {

	return Xw_get_plane_layer(MyExtendedDisplay,aVisualID);
}

Handle(Aspect_GraphicDriver) Xw_GraphicDevice::GraphicDriver () const {

static Handle(Aspect_GraphicDriver) foo;

	cout << "Xw_GraphicDevice::GraphicDriver returns foo\n" << flush;

	return foo;

}
