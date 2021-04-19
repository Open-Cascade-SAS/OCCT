// Created on: 2015-11-23
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _AIS_RubberBand_HeaderFile
#define _AIS_RubberBand_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_Vec2.hxx>
#include <NCollection_Sequence.hxx>

DEFINE_STANDARD_HANDLE(AIS_RubberBand, AIS_InteractiveObject)

//! Presentation for drawing rubber band selection.
//! It supports rectangle and polygonal selection.
//! It is constructed in 2d overlay.
//! Default configuration is built without filling.
//! For rectangle selection use SetRectangle() method.
//! For polygonal selection use AddPoint() and GetPoints() methods.
class AIS_RubberBand : public AIS_InteractiveObject
{
public:

  DEFINE_STANDARD_RTTIEXT(AIS_RubberBand, AIS_InteractiveObject)

  //! Constructs rubber band with default configuration: empty filling and white solid lines.
  //! @warning It binds this object with Graphic3d_ZLayerId_TopOSD layer.
  Standard_EXPORT AIS_RubberBand();

  //! Consructs the rubber band with empty filling and defined line style.
  //! @param theLineColor [in] color of rubber band lines
  //! @param theType [in] type of rubber band lines
  //! @param theLineWidth [in] width of rubber band line. By default it is 1.
  //! @warning It binds this object with Graphic3d_ZLayerId_TopOSD layer.
  Standard_EXPORT AIS_RubberBand (const Quantity_Color& theLineColor,
                                  const Aspect_TypeOfLine theType,
                                  const Standard_Real theLineWidth = 1.0,
                                  const Standard_Boolean theIsPolygonClosed = Standard_True);

  //! Constructs the rubber band with defined filling and line parameters.
  //! @param theLineColor [in] color of rubber band lines
  //! @param theType [in] type of rubber band lines
  //! @param theFillColor [in] color of rubber band filling
  //! @param theTransparency [in] transparency of the filling. 0 is for opaque filling. By default it is transparent.
  //! @param theLineWidth [in] width of rubber band line. By default it is 1.
  //! @warning It binds this object with Graphic3d_ZLayerId_TopOSD layer.
  Standard_EXPORT AIS_RubberBand (const Quantity_Color& theLineColor,
                                  const Aspect_TypeOfLine theType,
                                  const Quantity_Color theFillColor,
                                  const Standard_Real theTransparency = 1.0,
                                  const Standard_Real theLineWidth = 1.0,
                                  const Standard_Boolean theIsPolygonClosed = Standard_True);

  Standard_EXPORT virtual ~AIS_RubberBand();

  //! Sets rectangle bounds.
  Standard_EXPORT void SetRectangle (const Standard_Integer theMinX, const Standard_Integer theMinY,
                                     const Standard_Integer theMaxX, const Standard_Integer theMaxY);

  //! Adds last point to the list of points. They are used to build polygon for rubber band.
  //! @sa RemoveLastPoint(), GetPoints()
  Standard_EXPORT void AddPoint (const Graphic3d_Vec2i& thePoint);

  //! Remove last point from the list of points for the rubber band polygon.
  //! @sa AddPoint(), GetPoints()
  Standard_EXPORT void RemoveLastPoint();

  //! @return points for the rubber band polygon.
  Standard_EXPORT const NCollection_Sequence<Graphic3d_Vec2i>& Points() const;

  //! Remove all points for the rubber band polygon.
  void ClearPoints() { myPoints.Clear(); }

  //! @return the Color attributes.
  Standard_EXPORT Quantity_Color LineColor() const;

  //! Sets color of lines for rubber band presentation.
  Standard_EXPORT void SetLineColor (const Quantity_Color& theColor);

  //! @return the color of rubber band filling.
  Standard_EXPORT Quantity_Color FillColor() const;

  //! Sets color of rubber band filling.
  Standard_EXPORT void SetFillColor (const Quantity_Color& theColor);

  //! Sets width of line for rubber band presentation.
  Standard_EXPORT void SetLineWidth (const Standard_Real theWidth) const;

  //! @return width of lines.
  Standard_EXPORT Standard_Real LineWidth() const;

  //! Sets type of line for rubber band presentation.
  Standard_EXPORT void SetLineType (const Aspect_TypeOfLine theType);

  //! @return type of lines.
  Standard_EXPORT Aspect_TypeOfLine LineType() const;

  //! Sets fill transparency.
  //! @param theValue [in] the transparency value. 1.0 is for transparent background
  Standard_EXPORT void SetFillTransparency (const Standard_Real theValue) const;

  //! @return fill transparency.
  Standard_EXPORT Standard_Real FillTransparency() const;

  //! Enable or disable filling of rubber band.
  Standard_EXPORT void SetFilling (const Standard_Boolean theIsFilling);

  //! Enable filling of rubber band with defined parameters.
  //! @param theColor [in] color of filling
  //! @param theTransparency [in] transparency of the filling. 0 is for opaque filling.
  Standard_EXPORT void SetFilling (const Quantity_Color theColor, const Standard_Real theTransparency);

  //! @return true if filling of rubber band is enabled.
  Standard_EXPORT Standard_Boolean IsFilling() const;

  //! @return true if automatic closing of rubber band is enabled.
  Standard_EXPORT Standard_Boolean IsPolygonClosed() const;

  //! Automatically create an additional line connecting the first and 
  //! the last screen points to close the boundary polyline
  Standard_EXPORT void SetPolygonClosed(Standard_Boolean theIsPolygonClosed);

protected:

  //! Returns true if the interactive object accepts the display mode.
  Standard_Boolean AcceptDisplayMode (const Standard_Integer theMode) const Standard_OVERRIDE
  {
    return theMode == 0;
  }

  //! Computes presentation of rubber band.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                                        const Handle(Prs3d_Presentation)& thePrs,
                                        const Standard_Integer theMode) Standard_OVERRIDE;

  //! Does not fill selection primitives for rubber band.
  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/,
                                 const Standard_Integer /*aMode*/) Standard_OVERRIDE { };

  //! Fills triangles primitive array for rubber band filling.
  //! It uses Delaunay triangulation.
  //! @return true if array of triangles is successfully filled.
  Standard_EXPORT Standard_Boolean fillTriangles();

protected:

  NCollection_Sequence<Graphic3d_Vec2i> myPoints; //!< Array of screen points

  Handle(Graphic3d_ArrayOfTriangles) myTriangles; //!< Triangles for rubber band filling
  Handle(Graphic3d_ArrayOfPolylines) myBorders; //!< Polylines for rubber band borders

  Standard_Boolean                   myIsPolygonClosed; //!< automatic closing of rubber-band flag
};
#endif
