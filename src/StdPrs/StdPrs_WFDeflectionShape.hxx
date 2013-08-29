// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
