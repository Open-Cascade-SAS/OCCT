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

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

class AIS_ViewCube;

//! Main C++ back-end for activity.
class OcctJni_Viewer : public AIS_ViewController
{

public:

  //! Empty constructor
  OcctJni_Viewer (float theDispDensity);

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
  //! Returns TRUE if more frames should be requested.
  bool redraw();

  //! Move camera
  void setProj (V3d_TypeOfOrientation theProj)
  {
    if (myView.IsNull())
    {
      return;
    }

    myView->SetProj (theProj);
    myView->Invalidate();
  }

  //! Fit All.
  void fitAll();

protected:

  //! Reset viewer content.
  void initContent();

  //! Print information about OpenGL ES context.
  void dumpGlInfo (bool theIsBasic);

  //! Handle redraw.
  virtual void handleViewRedraw (const Handle(AIS_InteractiveContext)& theCtx,
                                 const Handle(V3d_View)& theView) override;

protected:

  Handle(V3d_Viewer)             myViewer;
  Handle(V3d_View)               myView;
  Handle(AIS_InteractiveContext) myContext;
  Handle(Prs3d_TextAspect)       myTextStyle; //!< text style for OSD elements
  Handle(AIS_ViewCube)           myViewCube;  //!< view cube object
  TopoDS_Shape                   myShape;
  float                          myDevicePixelRatio; //!< device pixel ratio for handling high DPI displays
  bool                           myIsJniMoreFrames; //!< need more frame flag

};
