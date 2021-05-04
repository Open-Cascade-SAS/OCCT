// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef OcctViewer_H
#define OcctViewer_H

#include "OcctDocument.h"
#include "CafShapePrs.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
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
  const Handle(AIS_InteractiveContext)& AisContext() const { return myContext; }

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
  
  Handle(V3d_Viewer)              myViewer;  //!< main viewer
  Handle(V3d_View)                myView;    //!< main view
  Handle(AIS_InteractiveContext)  myContext; //!< interactive context containing displayed objects
  Handle(OcctDocument)            myDoc;
};

#endif // OcctViewer_H
