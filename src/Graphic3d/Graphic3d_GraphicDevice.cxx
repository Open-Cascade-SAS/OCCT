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
#define RIC120302	//GG Add new constructor to pass Display structure
//			directly instead string connexion.

#define XDESTROY

#ifndef WNT

#include <stdio.h>
#include <sys/utsname.h>

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <Graphic3d_GraphicDevice.ixx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OSD_Function.hxx>
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

//RIC120302
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
//RIC120302

// Destructor

void Graphic3d_GraphicDevice::Destroy () {

#ifdef DESTROY
	cout << "Graphic3d_GraphicDevice::Destroy ()\n";
#endif

	MyGraphicDriver->End ();

}

// Methods in order

Handle(Aspect_GraphicDriver) Graphic3d_GraphicDevice::GraphicDriver () const {

	return MyGraphicDriver;

}

void Graphic3d_GraphicDevice::SetGraphicDriver () {

Standard_CString TheShr;

	if (! ShrIsDefined (TheShr)) {
	    Aspect_GraphicDeviceDefinitionError::Raise
		("Bad environment, Graphic Library not defined");
	}

OSD_SharedLibrary TheSharedLibrary (TheShr);

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
		cout << "Information : " << TheShr << " loaded\n" << flush;

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

#define BAD(x) (((x) == NULL) || (strlen((x)) <= 0))

Standard_Boolean Graphic3d_GraphicDevice::ShrIsDefined (Standard_CString& aShr) const {

  char *glso, *glul, *pkno;
  char *glshr, *casroot;

  casroot = getenv("CASROOT");
  glso = getenv("CSF_GraphicShr");
  glul = getenv("GRAPHICHOME");
  pkno = getenv("CSF_Graphic3dLib");

  if (! BAD(glso)) {
    glshr = getenv("CSF_GraphicShr");
  } else if (! BAD(casroot)) {
    TCollection_AsciiString buffString(casroot);
    struct utsname info; 
    uname (&info);
    buffString = buffString + "/";
    buffString = buffString + info.sysname;
#if defined(__hpux) || defined(HPUX)
    buffString = buffString + "/lib/libTKOpenGl.sl";
#elif defined(WNT)
    buffString = buffString + "/TKOpenGl.dll";
#elif defined(__APPLE__)
    buffString = buffString + "/lib/libTKOpenGl.dylib";
#else
    buffString = buffString + "/lib/libTKOpenGl.so";
#endif
    glshr = (char *) malloc (buffString.Length() + 1);
    memcpy(glshr, buffString.ToCString(), buffString.Length() + 1);
  } else {
    aShr = NULL;
    printf("You have not defined CSF_GraphicShr or CASROOT, aborting...");
    return Standard_False;
  }
 
 aShr = glshr;
 
 return Standard_True;
 
}
#endif  // WNT
