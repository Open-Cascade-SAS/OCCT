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
                                      const Standard_Real   theDeflection,
                                      TColgp_SequenceOfPnt& thePoints);

private:

  const Face&  myFaceAlgo;
  const Curve& myCurveAlgo;

private:

  Prs3d_WFShape            (const Prs3d_WFShape& );
  Prs3d_WFShape& operator= (const Prs3d_WFShape& );

};

#endif // _Prs3d_WFShape_H__
