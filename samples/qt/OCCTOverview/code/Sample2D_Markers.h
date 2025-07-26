// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _Sample2D_Markers_HeaderFile
#define _Sample2D_Markers_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_AspectMarker3d.hxx>

//! AIS AIS interactive object for sample 2D marker
class Sample2D_Markers : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTI_INLINE(Sample2D_Markers, AIS_InteractiveObject)
private:
  enum Sample2D_CurrentTypeOfMarker
  {
    Sample2D_CTOM_Generic,
    Sample2D_CTOM_Polyline,
    Sample2D_CTOM_Circle,
    Sample2D_CTOM_Ellips
  };

public:

 //! Generic marker
 Standard_EXPORT Sample2D_Markers (const Standard_Real theXPosition,
                   const Standard_Real theYPosition,
                   const Aspect_TypeOfMarker theMarkerType,
                   const Quantity_Color theColor,
                   const Standard_Real theScaleOrId=5.0);

  //! Polyline marker
  Standard_EXPORT Sample2D_Markers (const Standard_Real theXPosition,
                   const Standard_Real theYPosition,
                   const Handle(Graphic3d_ArrayOfPoints)& theArrayOfPoints,
                   const Aspect_TypeOfMarker theMarkerType,
                   const Quantity_Color theColor,
                   const Standard_Real theScaleOrId=2.0);

private:

  //! Return TRUE for supported display modes (only mode 0 is supported).
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Compute presentation.
  virtual void Compute (const Handle(PrsMgr_PresentationManager)& /*aPresentationManager*/,
                        const Handle(Prs3d_Presentation)& /*aPresentation*/,
                        const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute selection (not implemented).
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& ,
                                 const Standard_Integer ) Standard_OVERRIDE {}

  virtual void SetContext(const Handle(AIS_InteractiveContext)& ) Standard_OVERRIDE {}

private:

  Sample2D_CurrentTypeOfMarker myCurrentTypeOfMarker;
  Standard_Real  myXPosition;
  Standard_Real  myYPosition;
  Aspect_TypeOfMarker myMarkerType;
  Quantity_Color myColor;
  Standard_Real  myWidth;
  Standard_Real  myHeight;
  Standard_Real  myIndex;
  //! specific polyline marker
  Handle(Graphic3d_ArrayOfPoints) myArrayOfPoints;
};

#endif
