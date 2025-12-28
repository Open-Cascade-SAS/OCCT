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
#include <NCollection_Vec2.hxx>
#include <Standard_TypeDef.hxx>
#include <NCollection_Sequence.hxx>

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

  //! Constructs the rubber band with empty filling and defined line style.
  //! @param[in] theLineColor  color of rubber band lines
  //! @param[in] theType  type of rubber band lines
  //! @param[in] theLineWidth  width of rubber band line. By default it is 1.
  //! @warning It binds this object with Graphic3d_ZLayerId_TopOSD layer.
  Standard_EXPORT AIS_RubberBand(const Quantity_Color&   theLineColor,
                                 const Aspect_TypeOfLine theType,
                                 const double            theLineWidth       = 1.0,
                                 const bool              theIsPolygonClosed = true);

  //! Constructs the rubber band with defined filling and line parameters.
  //! @param[in] theLineColor  color of rubber band lines
  //! @param[in] theType  type of rubber band lines
  //! @param[in] theFillColor  color of rubber band filling
  //! @param[in] theTransparency  transparency of the filling. 0 is for opaque filling. By default
  //! it is transparent.
  //! @param[in] theLineWidth  width of rubber band line. By default it is 1.
  //! @warning It binds this object with Graphic3d_ZLayerId_TopOSD layer.
  Standard_EXPORT AIS_RubberBand(const Quantity_Color&   theLineColor,
                                 const Aspect_TypeOfLine theType,
                                 const Quantity_Color    theFillColor,
                                 const double            theTransparency    = 1.0,
                                 const double            theLineWidth       = 1.0,
                                 const bool              theIsPolygonClosed = true);

  Standard_EXPORT virtual ~AIS_RubberBand();

  //! Sets rectangle bounds.
  Standard_EXPORT void SetRectangle(const int theMinX,
                                    const int theMinY,
                                    const int theMaxX,
                                    const int theMaxY);

  //! Adds last point to the list of points. They are used to build polygon for rubber band.
  //! @sa RemoveLastPoint(), GetPoints()
  Standard_EXPORT void AddPoint(const NCollection_Vec2<int>& thePoint);

  //! Remove last point from the list of points for the rubber band polygon.
  //! @sa AddPoint(), GetPoints()
  Standard_EXPORT void RemoveLastPoint();

  //! @return points for the rubber band polygon.
  Standard_EXPORT const NCollection_Sequence<NCollection_Vec2<int>>& Points() const;

  //! Remove all points for the rubber band polygon.
  void ClearPoints() { myPoints.Clear(); }

  //! @return the Color attributes.
  Standard_EXPORT Quantity_Color LineColor() const;

  //! Sets color of lines for rubber band presentation.
  Standard_EXPORT void SetLineColor(const Quantity_Color& theColor);

  //! @return the color of rubber band filling.
  Standard_EXPORT Quantity_Color FillColor() const;

  //! Sets color of rubber band filling.
  Standard_EXPORT void SetFillColor(const Quantity_Color& theColor);

  //! Sets width of line for rubber band presentation.
  Standard_EXPORT void SetLineWidth(const double theWidth) const;

  //! @return width of lines.
  Standard_EXPORT double LineWidth() const;

  //! Sets type of line for rubber band presentation.
  Standard_EXPORT void SetLineType(const Aspect_TypeOfLine theType);

  //! @return type of lines.
  Standard_EXPORT Aspect_TypeOfLine LineType() const;

  //! Sets fill transparency.
  //! @param[in] theValue  the transparency value. 1.0 is for transparent background
  Standard_EXPORT void SetFillTransparency(const double theValue) const;

  //! @return fill transparency.
  Standard_EXPORT double FillTransparency() const;

  //! Enable or disable filling of rubber band.
  Standard_EXPORT void SetFilling(const bool theIsFilling);

  //! Enable filling of rubber band with defined parameters.
  //! @param[in] theColor  color of filling
  //! @param[in] theTransparency  transparency of the filling. 0 is for opaque filling.
  Standard_EXPORT void SetFilling(const Quantity_Color theColor, const double theTransparency);

  //! @return true if filling of rubber band is enabled.
  Standard_EXPORT bool IsFilling() const;

  //! @return true if automatic closing of rubber band is enabled.
  Standard_EXPORT bool IsPolygonClosed() const;

  //! Automatically create an additional line connecting the first and
  //! the last screen points to close the boundary polyline
  Standard_EXPORT void SetPolygonClosed(bool theIsPolygonClosed);

protected:
  //! Returns true if the interactive object accepts the display mode.
  bool AcceptDisplayMode(const int theMode) const override { return theMode == 0; }

  //! Computes presentation of rubber band.
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  //! Does not fill selection primitives for rubber band.
  virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& /*aSelection*/,
                                const int /*aMode*/) override {};

  //! Fills triangles primitive array for rubber band filling.
  //! It uses Delaunay triangulation.
  //! @return true if array of triangles is successfully filled.
  Standard_EXPORT bool fillTriangles();

protected:
  NCollection_Sequence<NCollection_Vec2<int>> myPoints; //!< Array of screen points

  occ::handle<Graphic3d_ArrayOfTriangles> myTriangles; //!< Triangles for rubber band filling
  occ::handle<Graphic3d_ArrayOfPolylines> myBorders;   //!< Polylines for rubber band borders

  bool myIsPolygonClosed; //!< automatic closing of rubber-band flag
};
#endif
