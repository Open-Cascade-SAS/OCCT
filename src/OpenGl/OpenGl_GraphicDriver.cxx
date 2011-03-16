
// File         OpenGl_GraphicDriver.cxx
// Created      Mardi 28 janvier 1997
// Author       CAL

//-Copyright    MatraDatavision 1997

//-Version      

//-Design       Declaration des variables specifiques aux Drivers

//-Warning      Un driver encapsule les Pex, Phigs et OpenGl drivers

//-References   

//-Language     C++ 2.0

//-Declarations

// for the class
#include <OpenGl_GraphicDriver.ixx>
#include <OpenGl_Extension.hxx>

//-Aliases

//-Global data definitions

// Pour eviter de "mangler" MetaGraphicDriverFactory, le nom de la
// fonction qui cree un Graphic3d_GraphicDriver.
// En effet, ce nom est recherche par la methode DlSymb de la
// classe OSD_SharedLibrary dans la methode SetGraphicDriver de la
// classe Graphic3d_GraphicDevice
extern "C" {
#ifdef WNT /* disable MS VC++ warning on C-style function returning C++ object */
#pragma warning(push)
#pragma warning(disable:4190)
#endif
  Standard_EXPORT Handle(Graphic3d_GraphicDriver) MetaGraphicDriverFactory
    (const Standard_CString AShrName);
  Standard_EXPORT Handle(Graphic3d_GraphicDriver) MetaGraphicDriverFactory
    (const Standard_CString AShrName) {
      Handle(OpenGl_GraphicDriver)  aOpenDriver = new OpenGl_GraphicDriver (AShrName);
      return aOpenDriver;
      //              return new OpenGl_GraphicDriver (AShrName);
    }
#ifdef WNT
#pragma warning(pop)
#endif
}

//-Constructors

OpenGl_GraphicDriver::OpenGl_GraphicDriver (const Standard_CString AShrName):Graphic3d_GraphicDriver (AShrName) 
{
}

//-Methods, in order

Standard_ShortReal OpenGl_GraphicDriver::DefaultTextHeight() const
{
  return 16.;
}

GLboolean OpenGl_QueryExtensionGLX (const char *extName)
{
  return QueryExtensionGLX(const_cast<char *>(extName));
}

GLboolean OpenGl_QueryExtension    (const char *extName)
{
  return QueryExtension(const_cast<char *>(extName));
}
