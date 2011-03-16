/*
10-09-00  : GG  ; NEW OpenGl driver loading specification
when nothing is defined the driver TKOpenGl.dll
is loaded from the current PATH.
*/

#define XDESTROY

#include <stdlib.h>


#ifdef WNT
# include <Graphic3d_WNTGraphicDevice.ixx>
# include <InterfaceGraphic_wntio.hxx>
#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>


typedef Handle_Graphic3d_GraphicDriver ( *GET_DRIVER_PROC ) ( const char* );

Graphic3d_WNTGraphicDevice::Graphic3d_WNTGraphicDevice (): WNT_GraphicDevice (Standard_True) {

  SetGraphicDriver ();

  if (! MyGraphicDriver->Begin (""))
    Aspect_GraphicDeviceDefinitionError::Raise
    ("Cannot connect to graphic library");

}

void Graphic3d_WNTGraphicDevice::Destroy () {

#ifdef DESTROY
  cout << "Graphic3d_WNTGraphicDevice::Destroy ()\n";
#endif

  MyGraphicDriver->End ();

}

Handle(Aspect_GraphicDriver) Graphic3d_WNTGraphicDevice::GraphicDriver () const {

  return MyGraphicDriver;

}

void Graphic3d_WNTGraphicDevice::SetGraphicDriver () 
{

  Standard_Boolean Result;
  OSD_Function new_GLGraphicDriver;
  Standard_CString TheShr = getenv("CSF_GraphicShr");
  if ( ! TheShr || ( strlen( TheShr ) == 0 ) )
    TheShr = "TKOpenGl.dll";

  MySharedLibrary.SetName ( TheShr );
  Result = MySharedLibrary.DlOpen (OSD_RTLD_LAZY);

  if (! Result) {
    Aspect_GraphicDeviceDefinitionError::Raise
      (MySharedLibrary.DlError ());
  }
  else {
    // Gestion des traces
    OSD_Environment beurk("CSF_GraphicTrace");
    TCollection_AsciiString val = beurk.Value();
    if (val.Length() > 0 )
      cout << "Information : " << TheShr << " loaded\n" << flush;

    new_GLGraphicDriver =
      MySharedLibrary.DlSymb ("MetaGraphicDriverFactory");
    if (! new_GLGraphicDriver) {
      Aspect_GraphicDeviceDefinitionError::Raise
        (MySharedLibrary.DlError ());
    }
    else {
      // Sequence artistique :
      // new_GLGraphicDriver est une OSD_Function :
      // typedef int (* OSD_Function)(...);
      // d'ou le magnifique cast en GraphicDriver.
      //Handle( Graphic3d_GraphicDriver ) ADriver = new Graphic3d_GraphicDriver ( TheShr );

      GET_DRIVER_PROC fp = ( GET_DRIVER_PROC )new_GLGraphicDriver;
      //ADriver = ( *fp ) ( TheShr );
      if (!fp)
        Aspect_GraphicDeviceDefinitionError::Raise
        (MySharedLibrary.DlError ());
      MyGraphicDriver = ( *fp ) ( TheShr );

      // Gestion des traces
      if ( val.Length() > 0 && val.IsIntegerValue() )
        MyGraphicDriver->SetTrace(val.IntegerValue());
    }
  }

}

#endif  // WNT
