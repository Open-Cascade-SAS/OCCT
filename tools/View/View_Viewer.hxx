// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef View_Viewer_H
#define View_Viewer_H

#include <Aspect_Window.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Macro.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

//! \class View_Viewer
//! \brief It is responsible for context/viewer/view creation and accepting access to:
class View_Viewer
{
public:

  //! Constructor
  View_Viewer (const Quantity_Color& theColor) : myDefaultColor(theColor) {}

  //! Destructor
  virtual ~View_Viewer() {}

  //! Returns the view default colors
  static Quantity_Color DefaultColor() { return Quantity_Color(Quantity_NOC_BLACK); }

  //! Returns the view default colors
  static Quantity_Color DisabledColor() { return Quantity_Color(195 / 255., 195 / 255., 195 / 255., Quantity_TOC_RGB); }

  //! Creates V3d view
  Standard_EXPORT void CreateView();

  //! Fills V3d view by the given window
  //! \param depending on platform it is either WNT_Window or Xw_Window
  Standard_EXPORT void SetWindow (const Handle(Aspect_Window)& theWindow);

  //! Creates OCC components on the window
  //! \param theWindowHandle an id of the application window
  Standard_EXPORT void InitStandardViewer();

  //! Returns an OCC viewer
  const Handle(V3d_Viewer)& GetViewer() { return myViewer; }

  //! Returns active view
  const Handle(V3d_View)& GetView() { return myView; }

  //! Returns OCCT context to provide display and selection mechanism
  const Handle(AIS_InteractiveContext)& GetContext() const { return myContext; }

private:

  Handle(V3d_Viewer) myViewer; //!< the OCCT viewer
  Handle(V3d_View) myView; //!< the OCCT view window
  Handle(AIS_InteractiveContext) myContext; //!< OCCT context to provide display and selection mechanism
  Quantity_Color myDefaultColor; //!< the default color of the viewer
};

#endif // View_Viewer_H
