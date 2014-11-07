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

#include <OcctJni_Window.hxx>

#include <AIS_InteractiveContext.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

//! Main C++ back-end for activity.
class OcctJni_Viewer
{

public:

  //! Empty constructor
  OcctJni_Viewer();

  //! Initialize the viewer
  bool init();

  //! Release the viewer
  void release();

  //! Resize the viewer
  void resize (int theWidth,
               int theHeight);

  //! Open CAD file
  bool open (const TCollection_AsciiString& thePath);

  //! Take snapshot
  bool saveSnapshot (const TCollection_AsciiString& thePath,
                     int theWidth  = 0,
                     int theHeight = 0);

  //! Viewer update.
  void redraw();

  //! Move camera
  void setProj (V3d_TypeOfOrientation theProj) { if (!myView.IsNull()) myView->SetProj (theProj); }

  //! Fit All.
  void fitAll();

  //! Start rotation (remember first point position)
  void startRotation (int theStartX,
                      int theStartY);

  //! Perform rotation (relative to first point)
  void onRotation (int theX,
                   int theY);

  //! Perform panning
  void onPanning (int theDX,
                  int theDY);

  //! Perform selection
  void onClick (int theX,
                int theY);

  //! Stop previously started action
  void stopAction();

protected:

  //! Reset viewer content.
  void initContent();

protected:

  Handle(V3d_Viewer)             myViewer;
  Handle(V3d_View)               myView;
  Handle(AIS_InteractiveContext) myContext;
  TopoDS_Shape                   myShape;

};
