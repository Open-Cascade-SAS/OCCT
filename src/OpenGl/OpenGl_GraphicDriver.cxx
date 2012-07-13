// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_GraphicDriver.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)

namespace
{
  // Global switch - shared by whole TKOpenGl module. To be removed.
  static Standard_Boolean TheToUseVbo = Standard_True;

  static const Handle(OpenGl_Context) TheNullGlCtx;
};

// Pour eviter de "mangler" MetaGraphicDriverFactory, le nom de la
// fonction qui cree un Graphic3d_GraphicDriver.
// En effet, ce nom est recherche par la methode DlSymb de la
// classe OSD_SharedLibrary dans la methode SetGraphicDriver de la
// classe Graphic3d_GraphicDevice
extern "C" {
#if defined(_MSC_VER) // disable MS VC++ warning on C-style function returning C++ object
  #pragma warning(push)
  #pragma warning(disable:4190)
#endif
  Standard_EXPORT Handle(Graphic3d_GraphicDriver) MetaGraphicDriverFactory (const Standard_CString theShrName)
  {
    Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver (theShrName);
    return aDriver;
  }
#if defined(_MSC_VER)
  #pragma warning(pop)
#endif
}

// =======================================================================
// function : OpenGl_GraphicDriver
// purpose  :
// =======================================================================
OpenGl_GraphicDriver::OpenGl_GraphicDriver (const Standard_CString theShrName)
: Graphic3d_GraphicDriver (theShrName),
  myMapOfView      (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myMapOfWS        (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myMapOfStructure (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myUserDrawCallback (NULL)
{
  //
}

// =======================================================================
// function : UserDrawCallback
// purpose  :
// =======================================================================
OpenGl_GraphicDriver::OpenGl_UserDrawCallback_t& OpenGl_GraphicDriver::UserDrawCallback()
{
  return myUserDrawCallback;
}

// =======================================================================
// function : DefaultTextHeight
// purpose  :
// =======================================================================
Standard_ShortReal OpenGl_GraphicDriver::DefaultTextHeight() const
{
  return 16.;
}

// =======================================================================
// function : InvalidateAllWorkspaces
// purpose  : ex-TsmInitUpdateState, deprecated, need to decide what to do with EraseAnimation() call
// =======================================================================
void OpenGl_GraphicDriver::InvalidateAllWorkspaces()
{
  for (NCollection_DataMap<Standard_Integer, Handle(OpenGl_Workspace)>::Iterator anIt (myMapOfWS);
       anIt.More(); anIt.Next())
  {
    anIt.ChangeValue()->EraseAnimation();
  }
}

// =======================================================================
// function : ToUseVBO
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::ToUseVBO()
{
  return TheToUseVbo;
}

// =======================================================================
// function : EnableVBO
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::EnableVBO (const Standard_Boolean theToTurnOn)
{
  TheToUseVbo = theToTurnOn;
}

// =======================================================================
// function : GetSharedContext
// purpose  :
// =======================================================================
const Handle(OpenGl_Context)& OpenGl_GraphicDriver::GetSharedContext() const
{
  if (myMapOfWS.IsEmpty())
  {
    return TheNullGlCtx;
  }

  NCollection_DataMap<Standard_Integer, Handle(OpenGl_Workspace)>::Iterator anIter (myMapOfWS);
  return anIter.Value()->GetGlContext();
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::MemoryInfo (Standard_Size&           theFreeBytes,
                                                   TCollection_AsciiString& theInfo) const
{
  // this is extra work (for OpenGl_Context initialization)...
  OpenGl_Context aGlCtx;
  if (!aGlCtx.Init())
  {
    return Standard_False;
  }
  theFreeBytes = aGlCtx.AvailableMemory();
  theInfo      = aGlCtx.MemoryInfo();
  return !theInfo.IsEmpty();
}
