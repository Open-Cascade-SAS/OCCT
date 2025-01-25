// Copyright (c) 2019 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <ViewerTest_V3dView.hxx>

#include <ViewerTest.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ViewerTest_V3dView, V3d_View)

// =======================================================================
// function : ViewerTest_V3dView
// purpose  :
// =======================================================================
ViewerTest_V3dView::ViewerTest_V3dView(const Handle(V3d_Viewer)& theViewer,
                                       const V3d_TypeOfView      theType,
                                       bool                      theIs2dMode)
    : V3d_View(theViewer, theType),
      myIs2dMode(theIs2dMode)
{
  //
}

// =======================================================================
// function : ViewerTest_V3dView
// purpose  :
// =======================================================================
ViewerTest_V3dView::ViewerTest_V3dView(const Handle(V3d_Viewer)& theViewer,
                                       const Handle(V3d_View)&   theView)
    : V3d_View(theViewer, theView),
      myIs2dMode(false)
{
  if (Handle(ViewerTest_V3dView) aV3dView = Handle(ViewerTest_V3dView)::DownCast(theView))
  {
    myIs2dMode = aV3dView->IsViewIn2DMode();
  }
}

// =======================================================================
// function : IsCurrentViewIn2DMode
// purpose  :
// =======================================================================
bool ViewerTest_V3dView::IsCurrentViewIn2DMode()
{
  if (Handle(ViewerTest_V3dView) aV3dView =
        Handle(ViewerTest_V3dView)::DownCast(ViewerTest::CurrentView()))
  {
    return aV3dView->IsViewIn2DMode();
  }
  return false;
}

// =======================================================================
// function : SetCurrentView2DMode
// purpose  :
// =======================================================================
void ViewerTest_V3dView::SetCurrentView2DMode(bool theIs2d)
{
  if (Handle(ViewerTest_V3dView) aV3dView =
        Handle(ViewerTest_V3dView)::DownCast(ViewerTest::CurrentView()))
  {
    aV3dView->SetView2DMode(theIs2d);
  }
}
