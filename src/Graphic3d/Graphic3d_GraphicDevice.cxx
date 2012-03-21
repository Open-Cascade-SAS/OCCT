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

/***********************************************************************

     FONCTION :
     ----------
        Classe Graphic3d_GraphicDevice.cxx :


     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      Janvier 1994 : CAL ; Creation.
      04-02-97  : FMN ; Suppression appel call_tox_set_colormap_mapping()
      10-09-00  : GG  ; NEW OpenGl driver loading specification
			when nothing is defined the driver libTKOpenGl[.so|.sl]
			is loading from the current PATH

************************************************************************/
#if (!defined(_WIN32) && !defined(__WIN32__))

#include <stdio.h>
#include <sys/utsname.h>

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <Graphic3d_GraphicDevice.ixx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OSD_Function.hxx>
#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>

#include <Xw_Cextern.hxx>

//-Static data definitions

static char LocalMessag[80] ;

//-Aliases

//-Global data definitions

//-Constructor

Graphic3d_GraphicDevice::Graphic3d_GraphicDevice (const Standard_CString Connexion, const Xw_TypeOfMapping /*Mapping*/, const Standard_Integer Ncolors, const Standard_Boolean UseDefault ):
Xw_GraphicDevice () {
Standard_CString connexion = (Connexion) ? Connexion : Standard_CString("") ;
Standard_Boolean status ;

        MyExtendedDisplay = Xw_open_display((Standard_PCharacter)connexion) ;

        if( !MyExtendedDisplay ) {
           sprintf(LocalMessag,"Cannot connect to server '%s'",connexion) ;
           Aspect_GraphicDeviceDefinitionError::Raise (LocalMessag);
        }

	SetGraphicDriver ();

	status = MyGraphicDriver->Begin (connexion);

	if( !status ) {
           sprintf(LocalMessag,"Cannot connect to graphic library from '%s'",
		connexion) ;
           Aspect_GraphicDeviceDefinitionError::Raise (LocalMessag);
	}

	this->InitMaps (connexion,Xw_TOM_READONLY,Ncolors,UseDefault);


}

Graphic3d_GraphicDevice::Graphic3d_GraphicDevice (const Aspect_Display pdisplay)
  : Xw_GraphicDevice ()
 {
  Standard_Boolean status ;

  if( !pdisplay )
    Aspect_GraphicDeviceDefinitionError::Raise ("Bad display pointer");
  MyExtendedDisplay = Xw_set_display(pdisplay) ;
  Standard_CString connexion = Xw_get_display_name(MyExtendedDisplay);

  if( !MyExtendedDisplay ) {
    if( connexion )
      sprintf(LocalMessag,"Cannot connect to server '%s'",connexion) ;
    else
      sprintf(LocalMessag,"Cannot connect to an UNKNOWN server") ;
    Aspect_GraphicDeviceDefinitionError::Raise (LocalMessag);
  }

  SetGraphicDriver ();

  status = MyGraphicDriver->Begin (pdisplay);

  if( !status ) {
    sprintf(LocalMessag,"Cannot connect to graphic library from '%s'",
            connexion) ;
    Aspect_GraphicDeviceDefinitionError::Raise (LocalMessag);
  }

  this->InitMaps (connexion,Xw_TOM_READONLY,0,Standard_True);
}

// Destructor
void Graphic3d_GraphicDevice::Destroy()
{
  MyGraphicDriver->End();
}

// Methods in order

Handle(Aspect_GraphicDriver) Graphic3d_GraphicDevice::GraphicDriver () const {

	return MyGraphicDriver;

}

void Graphic3d_GraphicDevice::SetGraphicDriver () {

  TCollection_AsciiString aShr = ShrEnvString ();

  OSD_SharedLibrary TheSharedLibrary (aShr.ToCString());

  Standard_Boolean Result = TheSharedLibrary.DlOpen (OSD_RTLD_LAZY);

	if (! Result) {
	    Aspect_GraphicDeviceDefinitionError::Raise
		(TheSharedLibrary.DlError ());
	}
	else {
	    // Management of traces
char *tracevalue = NULL;
	    tracevalue = (char *)(getenv ("CSF_GraphicTrace"));
	    if (tracevalue)
		cout << "Information : " << aShr << " loaded\n" << flush;

OSD_Function new_GLGraphicDriver =
	    TheSharedLibrary.DlSymb ("MetaGraphicDriverFactory");
	    if (tracevalue)
		cout << "Information : MetaGraphicDriverFactory "
		<< (new_GLGraphicDriver ? "found\n" : "not found\n") << flush;
	    if (! new_GLGraphicDriver) {
		Aspect_GraphicDeviceDefinitionError::Raise
			(TheSharedLibrary.DlError ());
	    }
	    else {
		// Sequence :
		// new_GLGraphicDriver is OSD_Function :
		// typedef int (* OSD_Function)(...);
		// that is why good cast in GraphicDriver.
		Handle(Graphic3d_GraphicDriver)
			(*fp) (Standard_CString) = NULL;
		fp = (Handle(Graphic3d_GraphicDriver)
			(*) (Standard_CString)) new_GLGraphicDriver;
		if (tracevalue) cout << "FP == "<<(void *) fp<<endl;
		Standard_CString aSharedName = TheSharedLibrary.Name ();
		MyGraphicDriver = (*fp) (aSharedName);

//		MyGraphicDriver = (*fp) (TheSharedLibrary.Name ());

		// Management of traces
		if (tracevalue)
		    MyGraphicDriver->SetTrace
			((Standard_Integer) atoi (tracevalue));
	    }
	}

}

TCollection_AsciiString Graphic3d_GraphicDevice::ShrEnvString() const
{
  OSD_Environment aEnvShr ("CSF_GraphicShr");
  if (!aEnvShr.Value().IsEmpty())
  {
    return aEnvShr.Value();
  }

  // load TKOpenGl using default searching mechanisms in system
#if defined(__hpux) || defined(HPUX)
  return TCollection_AsciiString ("libTKOpenGl.sl");
#elif defined(__APPLE__)
  return TCollection_AsciiString ("libTKOpenGl.dylib");
#else
  return TCollection_AsciiString ("libTKOpenGl.so");
#endif
}

#endif // !WNT
