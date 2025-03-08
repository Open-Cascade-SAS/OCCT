// Created on: 2014-09-10
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

#ifndef _ViewerTest_AutoUpdater_HeaderFile
#define _ViewerTest_AutoUpdater_HeaderFile

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>

class TCollection_AsciiString;

//! Auxiliary tool to control view updates.
class ViewerTest_AutoUpdater
{
public:
  //! Enumeration to control auto-update
  enum RedrawMode
  {
    RedrawMode_Auto = -1,
    RedrawMode_Forced,
    RedrawMode_Suppressed
  };

public:
  //! Constructor
  Standard_EXPORT ViewerTest_AutoUpdater(const Handle(AIS_InteractiveContext)& theContext,
                                         const Handle(V3d_View)&               theView);

  //! Destructor to automatically update view
  Standard_EXPORT ~ViewerTest_AutoUpdater();

  //! Parse redraw mode argument
  Standard_EXPORT Standard_Boolean parseRedrawMode(const TCollection_AsciiString& theArg);

  //! Disable autoupdate
  Standard_EXPORT void Invalidate();

  //! Finally update view
  Standard_EXPORT void Update();

private:
  Handle(AIS_InteractiveContext)     myContext;
  Handle(V3d_View)                   myView;
  ViewerTest_AutoUpdater::RedrawMode myToUpdate;
  Standard_Boolean                   myWasAutoUpdate;
};

#endif // _ViewerTest_AutoUpdater_HeaderFile
