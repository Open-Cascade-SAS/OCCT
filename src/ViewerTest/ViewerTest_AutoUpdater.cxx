// Created on: 2014-04-24
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <ViewerTest_AutoUpdater.hxx>

//=======================================================================
//function : ViewerTest_AutoUpdater
//purpose  :
//=======================================================================
ViewerTest_AutoUpdater::ViewerTest_AutoUpdater (const Handle(AIS_InteractiveContext)& theContext,
                                                const Handle(V3d_View)&               theView)
: myContext       (theContext),
  myView          (theView),
  myToUpdate      (RedrawMode_Auto),
  myWasAutoUpdate (Standard_False)
{
  if (!theView.IsNull())
  {
    myWasAutoUpdate = theView->SetImmediateUpdate (Standard_False);
  }
}

//=======================================================================
//function : ~ViewerTest_AutoUpdater
//purpose  :
//=======================================================================
ViewerTest_AutoUpdater::~ViewerTest_AutoUpdater()
{
  Update();
}

//=======================================================================
//function : parseRedrawMode
//purpose  :
//=======================================================================
Standard_Boolean ViewerTest_AutoUpdater::parseRedrawMode (const TCollection_AsciiString& theArg)
{
  TCollection_AsciiString anArgCase (theArg);
  anArgCase.LowerCase();
  if (anArgCase == "-update"
   || anArgCase == "-redraw")
  {
    myToUpdate = RedrawMode_Forced;
    return Standard_True;
  }
  else if (anArgCase == "-noupdate"
        || anArgCase == "-noredraw")
  {
    myToUpdate = RedrawMode_Suppressed;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : Invalidate
//purpose  :
//=======================================================================
void ViewerTest_AutoUpdater::Invalidate()
{
  myToUpdate = ViewerTest_AutoUpdater::RedrawMode_Suppressed;
  if (myWasAutoUpdate
  && !myView.IsNull())
  {
    myView->SetImmediateUpdate (myWasAutoUpdate);
  }
}

//=======================================================================
//function : Update
//purpose  :
//=======================================================================
void ViewerTest_AutoUpdater::Update()
{
  if (!myView.IsNull())
  {
    myView->SetImmediateUpdate (myWasAutoUpdate);
  }

  switch (myToUpdate)
  {
    case ViewerTest_AutoUpdater::RedrawMode_Suppressed:
    {
      return;
    }
    case ViewerTest_AutoUpdater::RedrawMode_Auto:
    {
      if (!myWasAutoUpdate)
      {
        return;
      }
    }
    case ViewerTest_AutoUpdater::RedrawMode_Forced:
    {
      if (!myContext.IsNull())
      {
        myContext->UpdateCurrentViewer();
      }
      else if (!myView.IsNull())
      {
        myView->Redraw();
      }
    }
  }
}
