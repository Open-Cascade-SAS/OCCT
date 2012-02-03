// File:      OpenGl_GraphicDriver.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)

namespace
{
  // Global maps - shared by whole TKOpenGl module. To be removed.
  static NCollection_DataMap<Standard_Integer, Handle(OpenGl_View)>      TheMapOfView (1, NCollection_BaseAllocator::CommonBaseAllocator());
  static NCollection_DataMap<Standard_Integer, Handle(OpenGl_Workspace)> TheMapOfWS   (1, NCollection_BaseAllocator::CommonBaseAllocator());
  static NCollection_DataMap<Standard_Integer, OpenGl_Structure*>        TheMapOfStructure (1, NCollection_BaseAllocator::CommonBaseAllocator());
  static Standard_Boolean TheToUseVbo = Standard_True;
};

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
  Standard_EXPORT Handle(Graphic3d_GraphicDriver) MetaGraphicDriverFactory (const Standard_CString AShrName)
  {
    Handle(OpenGl_GraphicDriver) aOpenDriver = new OpenGl_GraphicDriver (AShrName);
    return aOpenDriver;
  }
#ifdef WNT
  #pragma warning(pop)
#endif
}

OpenGl_GraphicDriver::OpenGl_GraphicDriver (const Standard_CString theShrName)
: Graphic3d_GraphicDriver (theShrName)
{
  //
}

Standard_ShortReal OpenGl_GraphicDriver::DefaultTextHeight() const
{
  return 16.;
}

NCollection_DataMap<Standard_Integer, Handle(OpenGl_View)>& OpenGl_GraphicDriver::GetMapOfViews()
{
  return TheMapOfView;
}

NCollection_DataMap<Standard_Integer, Handle(OpenGl_Workspace)>& OpenGl_GraphicDriver::GetMapOfWorkspaces()
{
  return TheMapOfWS;
}

NCollection_DataMap<Standard_Integer, OpenGl_Structure*>& OpenGl_GraphicDriver::GetMapOfStructures()
{
  return TheMapOfStructure;
}

//TsmInitUpdateState
void OpenGl_GraphicDriver::InvalidateAllWorkspaces()
{
  for (NCollection_DataMap<Standard_Integer, Handle(OpenGl_Workspace)>::Iterator anIt (OpenGl_GraphicDriver::GetMapOfWorkspaces());
       anIt.More(); anIt.Next())
  {
    anIt.ChangeValue()->Invalidate();
    anIt.ChangeValue()->EraseAnimation();
  }
}

Standard_Boolean OpenGl_GraphicDriver::ToUseVBO()
{
  return TheToUseVbo;
}

void OpenGl_GraphicDriver::EnableVBO (const Standard_Boolean theToTurnOn)
{
  TheToUseVbo = theToTurnOn;
}
