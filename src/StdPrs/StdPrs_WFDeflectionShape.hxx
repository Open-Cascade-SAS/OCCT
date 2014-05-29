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

#ifndef _StdPrs_WFDeflectionShape_H__
#define _StdPrs_WFDeflectionShape_H__

#include <Prs3d_Presentation.hxx>
#include <Prs3d_WFShape.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Root.hxx>
#include <StdPrs_WFDeflectionRestrictedFace.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

// Computes the wireframe presentation of surfaces
// by displaying planar sections.

class StdPrs_WFDeflectionShape : public Prs3d_Root
{

public:

  //! Add shape to presentation
  static inline void Add (const Handle (Prs3d_Presentation)& thePrs,
                          const TopoDS_Shape&                theShape,
                          const Handle (Prs3d_Drawer)&       theDrawer)
  {
    Face  aFaceAlgo;
    Curve aCurveAlgo;
    Prs3d_WFShape anAlgo (aFaceAlgo, aCurveAlgo);
    anAlgo.Add (thePrs, theShape, theDrawer);
  }

  static inline Handle(TopTools_HSequenceOfShape) PickCurve
      (const Quantity_Length        theX,
       const Quantity_Length        theY,
       const Quantity_Length        theZ,
       const Quantity_Length        theDistance,
       const TopoDS_Shape&          theShape,
       const Handle (Prs3d_Drawer)& theDrawer)
  {
    Face  aFaceAlgo;
    Curve aCurveAlgo;
    Prs3d_WFShape anAlgo (aFaceAlgo, aCurveAlgo);
    return anAlgo.PickCurve (theX, theY, theZ, theDistance, theShape, theDrawer);
  }

  static inline Handle(TopTools_HSequenceOfShape) PickPatch
      (const Quantity_Length       theX,
       const Quantity_Length       theY,
       const Quantity_Length       theZ,
       const Quantity_Length       theDistance,
       const TopoDS_Shape&         theShape,
       const Handle(Prs3d_Drawer)& theDrawer)
  {
    Face  aFaceAlgo;
    Curve aCurveAlgo;
    Prs3d_WFShape anAlgo (aFaceAlgo, aCurveAlgo);
    return anAlgo.PickPatch (theX, theY, theZ, theDistance, theShape, theDrawer);
  }

private:

  class Face : public Prs3d_WFShape::Face
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
                      Prs3d_NListOfSequenceOfPnt&         theCurves) const
    {
      StdPrs_WFDeflectionRestrictedFace::Add (thePrs, theFace, theToDrawUIso, theToDrawVIso, theDeflection,
                                              theNBUiso, theNBViso, theDrawer, theCurves);
    }

    virtual Standard_Boolean Match (const Quantity_Length               theX,
                                    const Quantity_Length               theY,
                                    const Quantity_Length               theZ,
                                    const Quantity_Length               theDistance,
                                    const Handle(BRepAdaptor_HSurface)& theFace,
                                    const Handle(Prs3d_Drawer)&         theDrawer) const
    {
      return StdPrs_WFDeflectionRestrictedFace::Match (theX, theY, theZ, theDistance, theFace, theDrawer);
    }
  };

  class Curve : public Prs3d_WFShape::Curve
  {
  public:
    virtual void Add (const Handle(Prs3d_Presentation)& thePrs,
                      Adaptor3d_Curve&                  theCurve,
                      const Quantity_Length             theDeflection,
                      const Handle(Prs3d_Drawer)&       theDrawer,
                      TColgp_SequenceOfPnt&             thePoints,
                      const Standard_Boolean            theToDrawCurve) const
    {
      StdPrs_DeflectionCurve::Add (thePrs, theCurve, theDeflection, theDrawer, thePoints, theToDrawCurve);
    }

    virtual Standard_Boolean Match (const Quantity_Length       theX,
                                    const Quantity_Length       theY,
                                    const Quantity_Length       theZ,
                                    const Quantity_Length       theDistance,
                                    const Adaptor3d_Curve&      theCurve,
                                    const Handle(Prs3d_Drawer)& theDrawer) const
    {
      return StdPrs_DeflectionCurve::Match (theX, theY, theZ, theDistance, theCurve, theDrawer);
    }
  };

};

#endif // _StdPrs_WFDeflectionShape_H__
