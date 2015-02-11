// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Prs3d_WFShape_H__
#define _Prs3d_WFShape_H__

#include <Prs3d_Root.hxx>
#include <Prs3d_NListOfSequenceOfPnt.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Quantity_Length.hxx>

class Adaptor3d_Curve;
class TopoDS_Edge;
class TopoDS_Shape;
class TColgp_SequenceOfPnt;
class Handle(BRepAdaptor_HSurface);
class Handle(Prs3d_Drawer);

class Prs3d_WFShape : Prs3d_Root
{

public:

  DEFINE_STANDARD_ALLOC

public:

  class Face
  {
  public:
    virtual void Add (const Handle(Prs3d_Presentation)&   thePrs,
                      const Handle(BRepAdaptor_HSurface)& theFace,
                      const Standard_Boolean              theToDrawUIso,
                      const Standard_Boolean              theToDrawVIso,
                      const Quantity_Length               theDeflection,
                      const Standard_Integer              theNBUiso,
                      const Standard_Integer              theNBViso,
                      const Handle(Prs3d_Drawer)&         theDrawer,
                      Prs3d_NListOfSequenceOfPnt&         theCurves) const = 0;

    virtual Standard_Boolean Match (const Quantity_Length               theX,
                                    const Quantity_Length               theY,
                                    const Quantity_Length               theZ,
                                    const Quantity_Length               theDistance,
                                    const Handle(BRepAdaptor_HSurface)& theFace,
                                    const Handle(Prs3d_Drawer)&         theDrawer) const = 0;
  };

  class Curve
  {
  public:
    virtual void Add (const Handle(Prs3d_Presentation)& thePrs,
                      Adaptor3d_Curve&                  theCurve,
                      const Quantity_Length             theDeflection,
                      const Handle(Prs3d_Drawer)&       theDrawer,
                      TColgp_SequenceOfPnt&             thePoints,
                      const Standard_Boolean            theToDrawCurve) const = 0;

    virtual Standard_Boolean Match (const Quantity_Length       theX,
                                    const Quantity_Length       theY,
                                    const Quantity_Length       theZ,
                                    const Quantity_Length       theDistance,
                                    const Adaptor3d_Curve&      theCurve,
                                    const Handle(Prs3d_Drawer)& theDrawer) const = 0;
  };

  inline Prs3d_WFShape (const Face&  theFace,
                        const Curve& theCurve)
  : myFaceAlgo (theFace),
    myCurveAlgo (theCurve) {}

public:

  //! Add shape to presentation
  Standard_EXPORT void Add (const Handle (Prs3d_Presentation)& thePresentation,
                            const TopoDS_Shape&                theShape,
                            const Handle (Prs3d_Drawer)&       theDrawer);

  Standard_EXPORT Handle(TopTools_HSequenceOfShape) PickCurve
      (const Quantity_Length        theX,
       const Quantity_Length        theY,
       const Quantity_Length        theZ,
       const Quantity_Length        theDistance,
       const TopoDS_Shape&          theShape,
       const Handle (Prs3d_Drawer)& theDrawer);

  Standard_EXPORT Handle(TopTools_HSequenceOfShape) PickPatch
      (const Quantity_Length       theX,
       const Quantity_Length       theY,
       const Quantity_Length       theZ,
       const Quantity_Length       theDistance,
       const TopoDS_Shape&         theShape,
       const Handle(Prs3d_Drawer)& theDrawer);

private:

  static Standard_Boolean AddPolygon (const TopoDS_Edge&    theEdge,
                                      TColgp_SequenceOfPnt& thePoints);

private:

  const Face&  myFaceAlgo;
  const Curve& myCurveAlgo;

private:

  Prs3d_WFShape            (const Prs3d_WFShape& );
  Prs3d_WFShape& operator= (const Prs3d_WFShape& );

};

#endif // _Prs3d_WFShape_H__
