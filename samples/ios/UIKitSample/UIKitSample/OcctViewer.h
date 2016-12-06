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

#ifndef OcctViewer_H
#define OcctViewer_H

#include "OcctDocument.h"
#include "CafShapePrs.h"

#include <AIS_ColoredDrawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <OSD_Timer.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <V3d_DirectionalLight.hxx>
#include <XSControl_WorkSession.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFPrs_Style.hxx>

#import <UIKit/UIKit.h>

//! OCCT 3D Viewer holder.
class OcctViewer
{
public:
  
  //! Empty constructor.
  Standard_EXPORT OcctViewer();
  
  //! Destructor.
  Standard_EXPORT virtual ~OcctViewer();
  
  //! Release the viewer.
  Standard_EXPORT void release();
  
public:
  
  //! Return viewer instance.
  const Handle(V3d_Viewer)& V3dViewer() const { return myViewer; }
  
  //! Return active view.
  const Handle(V3d_View)& ActiveView() const { return myView; }
  
  //! Interactive context.
  const Handle(AIS_InteractiveContext)& AisContex() const { return myContext; }
  
  //! Invalidate active viewer.
  void Invalidate()
  {
    myView->Invalidate();
  }
  
public:
  
  //! Perform OCCT Viewer (re)initialization.
  Standard_EXPORT bool InitViewer (UIView* theWin);
  
  Standard_EXPORT void FitAll();
  
  Standard_EXPORT void StartRotation(int theX, int theY);
  Standard_EXPORT void Rotation(int theX, int theY);
  Standard_EXPORT void Pan(int theX, int theY);
  Standard_EXPORT void Zoom(int theX, int theY, double theDelta);
  Standard_EXPORT void Select(int theX, int theY);
  
  Standard_EXPORT bool ImportSTEP(std::string theFilename);
  
private:
  void clearSession(const Handle(XSControl_WorkSession)& theSession);
  
  void displayWithChildren (XCAFDoc_ShapeTool&             theShapeTool,
                            XCAFDoc_ColorTool&             theColorTool,
                            const TDF_Label&               theLabel,
                            const TopLoc_Location&         theParentTrsf,
                            const XCAFPrs_Style&           theParentStyle,
                            const TCollection_AsciiString& theParentId,
                            MapOfPrsForShapes&             theMapOfShapes);
  void clearContext();
  
protected:
  
  Handle(V3d_Viewer)              myViewer;            //!< main viewer
  Handle(V3d_View)                myView;              //!< main view
  Handle(AIS_InteractiveContext)  myContext;           //!< interactive context containing displayed objects
  Handle(OcctDocument)            myDoc;
};

#endif // OcctViewer_H
