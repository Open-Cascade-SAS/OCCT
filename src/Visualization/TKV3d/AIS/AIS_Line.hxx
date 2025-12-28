// Created on: 1997-01-21
// Created by: Prestataire Christiane ARMAND
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _AIS_Line_HeaderFile
#define _AIS_Line_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_KindOfInteractive.hxx>

class Geom_Line;
class Geom_Point;

//! Constructs line datums to be used in construction of
//! composite shapes.
class AIS_Line : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_Line, AIS_InteractiveObject)
public:
  //! Initializes the line aLine.
  Standard_EXPORT AIS_Line(const occ::handle<Geom_Line>& aLine);

  //! Initializes a starting point aStartPoint
  //! and a finishing point aEndPoint for the line.
  Standard_EXPORT AIS_Line(const occ::handle<Geom_Point>& aStartPoint,
                           const occ::handle<Geom_Point>& aEndPoint);

  //! Returns the signature 5.
  virtual int Signature() const override { return 5; }

  //! Returns the type Datum.
  virtual AIS_KindOfInteractive Type() const override { return AIS_KindOfInteractive_Datum; }

  //! Constructs an infinite line.
  const occ::handle<Geom_Line>& Line() const { return myComponent; }

  //! Returns the starting point thePStart and the end point thePEnd of the line set by SetPoints.
  void Points(occ::handle<Geom_Point>& thePStart, occ::handle<Geom_Point>& thePEnd) const
  {
    thePStart = myStartPoint;
    thePEnd   = myEndPoint;
  }

  //! instantiates an infinite line.
  void SetLine(const occ::handle<Geom_Line>& theLine)
  {
    myComponent     = theLine;
    myLineIsSegment = false;
  }

  //! Sets the starting point thePStart and ending point thePEnd of the
  //! infinite line to create a finite line segment.
  void SetPoints(const occ::handle<Geom_Point>& thePStart, const occ::handle<Geom_Point>& thePEnd)
  {
    myStartPoint    = thePStart;
    myEndPoint      = thePEnd;
    myLineIsSegment = true;
  }

  //! Provides a new color setting aColor for the line in the drawing tool, or "Drawer".
  Standard_EXPORT void SetColor(const Quantity_Color& aColor) override;

  //! Provides the new width setting aValue for the line in
  //! the drawing tool, or "Drawer".
  Standard_EXPORT void SetWidth(const double aValue) override;

  //! Removes the color setting and returns the original color.
  Standard_EXPORT void UnsetColor() override;

  //! Removes the width setting and returns the original width.
  Standard_EXPORT void UnsetWidth() override;

private:
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  Standard_EXPORT virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSel,
                                                const int theMode) override;

  Standard_EXPORT void ComputeInfiniteLine(const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeSegmentLine(const occ::handle<Prs3d_Presentation>& aPresentation);

  Standard_EXPORT void ComputeInfiniteLineSelection(
    const occ::handle<SelectMgr_Selection>& aSelection);

  Standard_EXPORT void ComputeSegmentLineSelection(
    const occ::handle<SelectMgr_Selection>& aSelection);
  //! Replace aspects of already computed groups with the new value.
  void replaceWithNewLineAspect(const occ::handle<Prs3d_LineAspect>& theAspect);

private:
  occ::handle<Geom_Line>  myComponent;
  occ::handle<Geom_Point> myStartPoint;
  occ::handle<Geom_Point> myEndPoint;
  bool                    myLineIsSegment;
};

#endif // _AIS_Line_HeaderFile
