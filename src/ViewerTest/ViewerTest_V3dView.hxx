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

#ifndef _ViewerTest_V3dView_HeaderFile
#define _ViewerTest_V3dView_HeaderFile

#include <V3d_View.hxx>

//! Setting additional flag to store 2D mode of the View to avoid scene rotation by mouse/key events
class ViewerTest_V3dView : public V3d_View
{
  DEFINE_STANDARD_RTTIEXT(ViewerTest_V3dView, V3d_View)
public:
  //! Initializes the view.
  Standard_EXPORT ViewerTest_V3dView (const Handle(V3d_Viewer)& theViewer,
                                      const V3d_TypeOfView theType = V3d_ORTHOGRAPHIC,
                                      bool theIs2dMode = false);

  //! Initializes the view by copying.
  Standard_EXPORT ViewerTest_V3dView (const Handle(V3d_Viewer)& theViewer,
                                      const Handle(V3d_View)& theView);

  //! Returns true if 2D mode is set for the view
  bool IsViewIn2DMode() const { return myIs2dMode; }

  //! Sets 2D mode for the view
  void SetView2DMode (bool the2dMode) { myIs2dMode = the2dMode; }

public:

  //! Returns true if active view in 2D mode.
  Standard_EXPORT  static bool IsCurrentViewIn2DMode();

  //! Set if active view in 2D mode.
  Standard_EXPORT static void SetCurrentView2DMode (bool theIs2d);

private:

  Standard_Boolean myIs2dMode; //!< 2D mode flag

};

#endif // _ViewerTest_V3dView_HeaderFile
