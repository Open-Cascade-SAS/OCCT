// Created on: 1998-08-27
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <AIS_InteractiveContext.hxx>
#include <Aspect_Grid.hxx>
#include <Standard_Type.hxx>
#include <V3d_View.hxx>
#include <ViewerTest_EventManager.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ViewerTest_EventManager,Standard_Transient)

//=======================================================================
//function : ViewerTest_EventManager
//purpose  :
//=======================================================================
ViewerTest_EventManager::ViewerTest_EventManager (const Handle(V3d_View)&               theView,
                                                  const Handle(AIS_InteractiveContext)& theCtx)
: myCtx  (theCtx),
  myView (theView),
  myX    (-1),
  myY    (-1)
{}

//=======================================================================
//function : MoveTo
//purpose  :
//=======================================================================

void ViewerTest_EventManager::MoveTo (const Standard_Integer theXPix,
                                      const Standard_Integer theYPix)
{
  Standard_Real aPnt3d[3] = {0.0, 0.0, 0.0};
  if (!myCtx.IsNull()
   && !myView.IsNull())
  {
    const Standard_Boolean toEchoGrid = myView->Viewer()->Grid()->IsActive()
                                     && myView->Viewer()->GridEcho();
    switch (myCtx->MoveTo (theXPix, theYPix, myView, !toEchoGrid))
    {
      case AIS_SOD_Nothing:
      {
        if (toEchoGrid)
        {
          myView->ConvertToGrid (theXPix, theYPix, aPnt3d[0], aPnt3d[1], aPnt3d[2]);
          myView->Viewer()->ShowGridEcho (myView, Graphic3d_Vertex (aPnt3d[0], aPnt3d[1], aPnt3d[2]));
          myView->RedrawImmediate();
        }
        break;
      }
      default:
      {
        if (toEchoGrid)
        {
          myView->Viewer()->HideGridEcho (myView);
          myView->RedrawImmediate();
        }
        break;
      }
    }
  }

  myX = theXPix;
  myY = theYPix;
}

//=======================================================================
//function : Select
//purpose  :
//=======================================================================

void ViewerTest_EventManager::Select (const Standard_Integer theXPressed,
                                      const Standard_Integer theYPressed,
                                      const Standard_Integer theXMotion,
                                      const Standard_Integer theYMotion,
                                      const Standard_Boolean theIsAutoAllowOverlap)
{
  #define IS_FULL_INCLUSION Standard_True
  if (myView.IsNull()
   || Abs (theXPressed - theXMotion) < 2
   || Abs (theYPressed - theYMotion) < 2)
  {
    return;
  }
  else if (!myCtx.IsNull())
  {
    if (theIsAutoAllowOverlap)
    {
      if (theYPressed == Min (theYPressed, theYMotion))
      {
        myCtx->MainSelector()->AllowOverlapDetection (Standard_False);
      }
      else
      {
        myCtx->MainSelector()->AllowOverlapDetection (Standard_True);
      }
    }
    myCtx->Select (Min (theXPressed, theXMotion),
                   Min (theYPressed, theYMotion),
                   Max (theXPressed, theXMotion),
                   Max (theYPressed, theYMotion),
                   myView,
                   Standard_False);

    // to restore default state of viewer selector
    if (theIsAutoAllowOverlap)
    {
      myCtx->MainSelector()->AllowOverlapDetection (Standard_False);
    }
  }

  myView->Redraw();
}

//=======================================================================
//function : ShiftSelect
//purpose  :
//=======================================================================

void ViewerTest_EventManager::ShiftSelect (const Standard_Integer theXPressed,
                                           const Standard_Integer theYPressed,
                                           const Standard_Integer theXMotion,
                                           const Standard_Integer theYMotion,
                                           const Standard_Boolean theIsAutoAllowOverlap)
{
  if (myView.IsNull()
   || Abs (theXPressed - theXMotion) < 2
   || Abs (theYPressed - theYMotion) < 2)
  {
    return;
  }
  else if (!myCtx.IsNull())
  {
    if (theIsAutoAllowOverlap)
    {
      if (theYPressed == Min (theYPressed, theYMotion))
      {
        myCtx->MainSelector()->AllowOverlapDetection (Standard_False);
      }
      else
      {
        myCtx->MainSelector()->AllowOverlapDetection (Standard_True);
      }
    }
    myCtx->ShiftSelect (Min (theXPressed, theXMotion),
                        Min (theYPressed, theYMotion),
                        Max (theXPressed, theXMotion),
                        Max (theYPressed, theYMotion),
                        myView,
                        Standard_False);

    // to restore default state of viewer selector
    if (theIsAutoAllowOverlap)
    {
      myCtx->MainSelector()->AllowOverlapDetection (Standard_False);
    }
  }
  myView->Redraw();
}

//=======================================================================
//function : Select
//purpose  :
//=======================================================================

void ViewerTest_EventManager::Select()
{
  if (myView.IsNull())
  {
    return;
  }
  else if (!myCtx.IsNull())
  {
    myCtx->Select (Standard_False);
  }

  myView->Redraw();
}

//=======================================================================
//function : ShiftSelect
//purpose  :
//=======================================================================

void ViewerTest_EventManager::ShiftSelect()
{
  if (myView.IsNull())
  {
    return;
  }
  else if (!myCtx.IsNull())
  {
    myCtx->ShiftSelect (Standard_False);
  }

  myView->Redraw();
}

//=======================================================================
//function : Select
//purpose  : Selection with polyline
//=======================================================================

void ViewerTest_EventManager::Select (const TColgp_Array1OfPnt2d& thePolyline)
{
  if (myView.IsNull())
  {
    return;
  }
  else if (!myCtx.IsNull())
  {
    myCtx->Select (thePolyline, myView, Standard_False);
  }

  myView->Redraw();
}

//=======================================================================
//function : ShiftSelect
//purpose  : Selection with polyline without erasing of current selection
//=======================================================================

void ViewerTest_EventManager::ShiftSelect (const TColgp_Array1OfPnt2d& thePolyline)
{
  if (myView.IsNull())
  {
    return;
  }
  else if (!myCtx.IsNull())
  {
    myCtx->ShiftSelect (thePolyline, myView, Standard_False);
  }

  myView->Redraw();
}

void ViewerTest_EventManager::GetCurrentPosition (Standard_Integer& theXPix, Standard_Integer& theYPix) const
{
  theXPix = myX;
  theYPix = myY;
}
