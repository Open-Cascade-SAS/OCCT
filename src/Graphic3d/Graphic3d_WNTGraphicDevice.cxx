// File:      Graphic3d_WNTGraphicDevice.cxx

#if (defined(_WIN32) || defined(__WIN32__))

#include <stdlib.h>

#include <Graphic3d_WNTGraphicDevice.ixx>
#include <InterfaceGraphic_wntio.hxx>
#include <OSD_Environment.hxx>
#include <TCollection_AsciiString.hxx>

// =======================================================================
// function : Graphic3d_WNTGraphicDevice
// purpose  :
// =======================================================================
Graphic3d_WNTGraphicDevice::Graphic3d_WNTGraphicDevice()
: WNT_GraphicDevice (Standard_True)
{
  SetGraphicDriver();
  if (!MyGraphicDriver->Begin (""))
    Aspect_GraphicDeviceDefinitionError::Raise ("Cannot connect to graphic library");

}

// =======================================================================
// function : Graphic3d_WNTGraphicDevice
// purpose  :
// =======================================================================
Graphic3d_WNTGraphicDevice::Graphic3d_WNTGraphicDevice (const Standard_CString theGraphicLib)
: WNT_GraphicDevice (Standard_True)
{
  SetGraphicDriver (theGraphicLib);
  if (!MyGraphicDriver->Begin (""))
    Aspect_GraphicDeviceDefinitionError::Raise ("Cannot connect to graphic library");
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void Graphic3d_WNTGraphicDevice::Destroy()
{
  MyGraphicDriver->End();
}

// =======================================================================
// function : GraphicDriver
// purpose  :
// =======================================================================
Handle(Aspect_GraphicDriver) Graphic3d_WNTGraphicDevice::GraphicDriver() const
{
  return MyGraphicDriver;
}

// =======================================================================
// function : SetGraphicDriver
// purpose  :
// =======================================================================
void Graphic3d_WNTGraphicDevice::SetGraphicDriver() 
{
  Standard_CString aLibPath = getenv ("CSF_GraphicShr");
  if (aLibPath == NULL || strlen (aLibPath) == 0)
    aLibPath = "TKOpenGl.dll";

  SetGraphicDriver (aLibPath);
}

// =======================================================================
// function : SetGraphicDriver
// purpose  :
// =======================================================================
void Graphic3d_WNTGraphicDevice::SetGraphicDriver (const Standard_CString theGraphicLib)
{
  MyGraphicDriver.Nullify();

  // load the library
  MySharedLibrary.SetName (theGraphicLib);
  if (!MySharedLibrary.DlOpen (OSD_RTLD_LAZY))
  {
    Aspect_GraphicDeviceDefinitionError::Raise (MySharedLibrary.DlError());
  }

  // management of traces
  OSD_Environment aTraceEnv ("CSF_GraphicTrace");
  TCollection_AsciiString aTrace = aTraceEnv.Value();
  if (aTrace.Length() > 0)
    cout << "Information : " << theGraphicLib << " loaded\n" << flush;

  // retrieve factory function pointer
  typedef Handle(Graphic3d_GraphicDriver) (*GET_DRIVER_PROC) (const char* );
  GET_DRIVER_PROC aGraphicDriverConstructor = (GET_DRIVER_PROC )MySharedLibrary.DlSymb ("MetaGraphicDriverFactory");
  if (aGraphicDriverConstructor == NULL)
  {
    Aspect_GraphicDeviceDefinitionError::Raise (MySharedLibrary.DlError());
    return;
  }

  // create driver instance
  MyGraphicDriver = aGraphicDriverConstructor (theGraphicLib);

  // management of traces
  if (aTrace.Length() > 0 && aTrace.IsIntegerValue())
    MyGraphicDriver->SetTrace (aTrace.IntegerValue());
}

#endif  // WNT
