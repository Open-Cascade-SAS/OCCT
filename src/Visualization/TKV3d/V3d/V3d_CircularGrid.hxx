// Created on: 1998-07-16
// Created by: CAL
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _V3d_CircularGrid_HeaderFile
#define _V3d_CircularGrid_HeaderFile

#include <Standard.hxx>

#include <Aspect_CircularGrid.hxx>
#include <V3d_ViewerPointer.hxx>
#include <gp_Ax3.hxx>
class Graphic3d_Structure;
class Graphic3d_Group;

//! @deprecated Kept for backward compatibility. CPU-generated circular grid bound to a
//! V3d_Viewer. New code should drive grids through V3d_View::GridDisplay(Aspect_GridParams,
//! gp_Ax3), which renders an AA, shader-based grid and supports unbounded extents, background mode,
//! arc ranges and per-axis scales. This class consumes the same Aspect_CircularGrid
//! parameters where the CPU path can render them; arc ranges (AngleStart/AngleEnd) are not
//! supported by the CPU path and are reported via Message::SendWarning() and ignored.
class V3d_CircularGrid : public Aspect_CircularGrid
{
  DEFINE_STANDARD_RTTIEXT(V3d_CircularGrid, Aspect_CircularGrid)
public:
  //! Constructor. Builds a CPU-rendered circular grid bound to @p aViewer.
  //! Default radius is 0.5 * Viewer->DefaultViewSize(); ZOffset defaults to step / 50.
  //! @deprecated Prefer V3d_View::GridDisplay with Aspect_GridParams for shader-based grids.
  //! @param[in] aViewer     viewer that owns the grid (and provides DefaultViewSize)
  //! @param[in] aColor      color of the regular rings / spokes
  //! @param[in] aTenthColor color of every 10-th ring (and the diameter spokes)
  Standard_EXPORT V3d_CircularGrid(const V3d_ViewerPointer& aViewer,
                                   const Quantity_Color&    aColor,
                                   const Quantity_Color&    aTenthColor);

  Standard_EXPORT ~V3d_CircularGrid() override;

  //! Updates the grid colors and triggers a re-display when they actually change.
  //! @param[in] aColor      color of the regular rings / spokes
  //! @param[in] aTenthColor color of every 10-th ring
  Standard_EXPORT void SetColors(const Quantity_Color& aColor,
                                 const Quantity_Color& aTenthColor) override;

  //! Display the CPU grid in the owning viewer's structure manager.
  Standard_EXPORT void Display() override;

  //! Erase the CPU grid (the underlying Graphic3d_Structure is hidden, not destroyed).
  Standard_EXPORT void Erase() const override;

  //! Returns true if the grid structure is currently displayed.
  Standard_EXPORT bool IsDisplayed() const override;

  //! Returns the grid extent and Z offset (alias for Radius/ZOffset).
  //! @param[out] Radius outermost ring radius
  //! @param[out] OffSet plane-normal displacement of the rendered grid
  Standard_EXPORT void GraphicValues(double& Radius, double& OffSet) const;

  //! Sets the grid extent and Z offset (alias for SetRadius/SetZOffset).
  //! @param[in] Radius outermost ring radius
  //! @param[in] OffSet plane-normal displacement
  Standard_EXPORT void SetGraphicValues(const double Radius, const double OffSet);

  //! Dumps the content of me into the stream.
  //! @param[in,out] theOStream destination stream
  //! @param[in]     theDepth   recursion depth (-1 for full)
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

protected:
  //! Recomputes the structure transformation and the ring / spoke primitive arrays
  //! whenever a parameter or the privileged plane changes. Emits a one-shot
  //! Message::SendWarning() when an arc range is set on the base class - the CPU
  //! renderer cannot honor it and falls back to a full circle.
  Standard_EXPORT void UpdateDisplay() override;

private:
  Standard_EXPORT void DefineLines();

  Standard_EXPORT void DefinePoints();

private:
  //! Custom Graphic3d_Structure implementation.
  class CircularGridStructure;

private:
  occ::handle<Graphic3d_Structure> myStructure;
  occ::handle<Graphic3d_Group>     myGroup;
  gp_Ax3                           myCurViewPlane;
  V3d_ViewerPointer                myViewer;
  bool                             myCurAreDefined;
  bool                             myToComputePrs;
  Aspect_GridDrawMode              myCurDrawMode;
  double                           myCurXo;
  double                           myCurYo;
  double                           myCurAngle;
  double                           myCurStep;
  int                              myCurDivi;
  double                           myCurRadius;
  double                           myCurOffSet;
  mutable bool                     myArcWarned;
};

#endif // _V3d_CircularGrid_HeaderFile
