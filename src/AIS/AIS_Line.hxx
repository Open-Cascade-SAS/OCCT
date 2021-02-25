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
  Standard_EXPORT AIS_Line(const Handle(Geom_Line)& aLine);

  //! Initializes a starting point aStartPoint
  //! and a finishing point aEndPoint for the line.
  Standard_EXPORT AIS_Line(const Handle(Geom_Point)& aStartPoint, const Handle(Geom_Point)& aEndPoint);

  //! Returns the signature 5.
  virtual Standard_Integer Signature() const Standard_OVERRIDE { return 5; }

  //! Returns the type Datum.
  virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE { return AIS_KindOfInteractive_Datum; }

  //! Constructs an infinite line.
  const Handle(Geom_Line)& Line() const { return myComponent; }

  //! Returns the starting point thePStart and the end point thePEnd of the line set by SetPoints.
  void Points (Handle(Geom_Point)& thePStart, Handle(Geom_Point)& thePEnd) const
  {
    thePStart = myStartPoint;
    thePEnd   = myEndPoint;
  }

  //! instantiates an infinite line.
  void SetLine (const Handle(Geom_Line)& theLine)
  {
    myComponent = theLine;
    myLineIsSegment = Standard_False;
  }

  //! Sets the starting point thePStart and ending point thePEnd of the
  //! infinite line to create a finite line segment.
  void SetPoints (const Handle(Geom_Point)& thePStart, const Handle(Geom_Point)& thePEnd)
  {
    myStartPoint    = thePStart;
    myEndPoint      = thePEnd;
    myLineIsSegment = Standard_True;
  }

  //! Provides a new color setting aColor for the line in the drawing tool, or "Drawer".
  Standard_EXPORT void SetColor (const Quantity_Color& aColor) Standard_OVERRIDE;

  //! Provides the new width setting aValue for the line in
  //! the drawing tool, or "Drawer".
  Standard_EXPORT void SetWidth (const Standard_Real aValue) Standard_OVERRIDE;

  //! Removes the color setting and returns the original color.
  Standard_EXPORT void UnsetColor() Standard_OVERRIDE;

  //! Removes the width setting and returns the original width.
  Standard_EXPORT void UnsetWidth() Standard_OVERRIDE;

private:

  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSel,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;

  Standard_EXPORT void ComputeInfiniteLine (const Handle(Prs3d_Presentation)& aPresentation);

  Standard_EXPORT void ComputeSegmentLine (const Handle(Prs3d_Presentation)& aPresentation);

  Standard_EXPORT void ComputeInfiniteLineSelection (const Handle(SelectMgr_Selection)& aSelection);

  Standard_EXPORT void ComputeSegmentLineSelection (const Handle(SelectMgr_Selection)& aSelection);
  //! Replace aspects of already computed groups with the new value.
  void replaceWithNewLineAspect (const Handle(Prs3d_LineAspect)& theAspect);

private:

  Handle(Geom_Line) myComponent;
  Handle(Geom_Point) myStartPoint;
  Handle(Geom_Point) myEndPoint;
  Standard_Boolean myLineIsSegment;

};

DEFINE_STANDARD_HANDLE(AIS_Line, AIS_InteractiveObject)

#endif // _AIS_Line_HeaderFile
