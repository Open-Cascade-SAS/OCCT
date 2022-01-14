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

#ifndef _Sample2D_Face_HeaderFile
#define _Sample2D_Face_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <TopoDS_Face.hxx>
#include <TColGeom_SequenceOfCurve.hxx>

//! AIS interactive Object for sample 2D face
class Sample2D_Face : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTI_INLINE(Sample2D_Face, AIS_InteractiveObject)
public:

  Standard_EXPORT Sample2D_Face (const TopoDS_Shape& theFace);

public:

  TopoDS_Shape& Shape() { return myshape; }
  void SetFace (const TopoDS_Shape& theFace) { myshape = theFace; }

public:

  Quantity_Color myFORWARDColor;
  Quantity_Color myREVERSEDColor;
  Quantity_Color myINTERNALColor;
  Quantity_Color myEXTERNALColor;
  Standard_Integer myWidthIndex;
  Standard_Integer myTypeIndex;

private:

  //! Return TRUE for supported display modes (only mode 0 is supported).
  virtual Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE { return theMode == 0; }

  //! Compute presentation.
  virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                        const Handle(Prs3d_Presentation)& thePrs,
                        const Standard_Integer theMode) Standard_OVERRIDE;

  //! Compute selection.
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                 const Standard_Integer theMode) Standard_OVERRIDE;

  virtual void ClearSelected() Standard_OVERRIDE;

  //! Method for advanced customizable selection of picked object
  virtual void HilightSelected (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                const SelectMgr_SequenceOfOwner& theOwners) Standard_OVERRIDE;

  //! Method for advanced customizable highlighting of picked object.
  virtual void HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                      const Handle(Prs3d_Drawer)& theStyle,
                                      const Handle(SelectMgr_EntityOwner)& theOwner) Standard_OVERRIDE;

  void DrawMarker (const Handle(Geom2d_TrimmedCurve)& theCurve,
                   const Handle(Prs3d_Presentation)& thePresentation);

  void FillData(Standard_Boolean isSizesRecomputed = Standard_False);

private:

  TopoDS_Shape myshape;
  TColGeom_SequenceOfCurve mySeq_FORWARD;
  TColGeom_SequenceOfCurve mySeq_REVERSED;
  TColGeom_SequenceOfCurve mySeq_INTERNAL;
  TColGeom_SequenceOfCurve mySeq_EXTERNAL;

  Handle(Graphic3d_ArrayOfPolylines) myForwardArray;
  Handle(Graphic3d_ArrayOfPolylines) myReversedArray;
  Handle(Graphic3d_ArrayOfPolylines) myInternalArray;
  Handle(Graphic3d_ArrayOfPolylines) myExternalArray;

  Standard_Integer myForwardNum;
  Standard_Integer myReversedNum;
  Standard_Integer myInternalNum;
  Standard_Integer myExternalNum;
  Standard_Integer myForwardBounds;
  Standard_Integer myReversedBounds;
  Standard_Integer myInternalBounds;
  Standard_Integer myExternalBounds;

};

#endif
