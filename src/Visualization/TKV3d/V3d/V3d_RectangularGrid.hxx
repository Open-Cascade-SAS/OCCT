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

#ifndef _V3d_RectangularGrid_HeaderFile
#define _V3d_RectangularGrid_HeaderFile

#include <Standard.hxx>

#include <Aspect_RectangularGrid.hxx>
#include <V3d_ViewerPointer.hxx>
#include <gp_Ax3.hxx>
class Graphic3d_Structure;
class Graphic3d_Group;

//! @deprecated Kept for backward compatibility. CPU-generated grid bound to a V3d_Viewer.
//! New code should drive grids through V3d_View::GridDisplay(Aspect_GridParams, gp_Ax3),
//! which renders an infinite, AA, shader-based grid and supports background mode, arc
//! ranges and per-axis scales. This class consumes the same Aspect_RectangularGrid
//! parameters where the CPU path can render them; unsupported parameters are reported
//! via Message::SendWarning() and ignored.
class V3d_RectangularGrid : public Aspect_RectangularGrid
{
  DEFINE_STANDARD_RTTIEXT(V3d_RectangularGrid, Aspect_RectangularGrid)
public:
  //! Constructor. Builds a CPU-rendered rectangular grid bound to @p aViewer.
  //! Default size is 0.5 * Viewer->DefaultViewSize() on each axis (bounded);
  //! ZOffset defaults to step / 50.
  //! @deprecated Prefer V3d_View::GridDisplay with Aspect_GridParams for shader-based
  //!             infinite grids.
  //! @param[in] aViewer     viewer that owns the grid (and provides DefaultViewSize)
  //! @param[in] aColor      color of the regular grid lines / points
  //! @param[in] aTenthColor color of every 10-th line (axis emphasis)
  Standard_EXPORT V3d_RectangularGrid(const V3d_ViewerPointer& aViewer,
                                      const Quantity_Color&    aColor,
                                      const Quantity_Color&    aTenthColor);

  Standard_EXPORT ~V3d_RectangularGrid() override;

  //! Updates the grid colors and triggers a re-display when they actually change.
  //! @param[in] aColor      color of the regular lines / points
  //! @param[in] aTenthColor color of every 10-th line
  Standard_EXPORT void SetColors(const Quantity_Color& aColor,
                                 const Quantity_Color& aTenthColor) override;

  //! Display the CPU grid in the owning viewer's structure manager.
  Standard_EXPORT void Display() override;

  //! Erase the CPU grid (the underlying Graphic3d_Structure is hidden, not destroyed).
  Standard_EXPORT void Erase() const override;

  //! Returns true if the grid structure is currently displayed.
  Standard_EXPORT bool IsDisplayed() const override;

  //! Returns the grid bounds and Z offset (alias for SizeX/SizeY/ZOffset).
  //! @param[out] XSize  width  along grid X
  //! @param[out] YSize  height along grid Y
  //! @param[out] OffSet plane-normal displacement of the rendered grid
  Standard_EXPORT void GraphicValues(double& XSize, double& YSize, double& OffSet) const;

  //! Sets the grid bounds and Z offset (alias for SetSizeX/SetSizeY/SetZOffset).
  //! @param[in] XSize  width  along grid X
  //! @param[in] YSize  height along grid Y
  //! @param[in] OffSet plane-normal displacement
  Standard_EXPORT void SetGraphicValues(const double XSize,
                                        const double YSize,
                                        const double OffSet);

  //! Dumps the content of me into the stream.
  //! @param[in,out] theOStream destination stream
  //! @param[in]     theDepth   recursion depth (-1 for full)
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

protected:
  //! Recomputes the structure transformation and the line / point primitive arrays
  //! whenever a parameter or the privileged plane changes.
  Standard_EXPORT void UpdateDisplay() override;

private:
  Standard_EXPORT void DefineLines();

  Standard_EXPORT void DefinePoints();

private:
  //! Custom Graphic3d_Structure implementation.
  class RectangularGridStructure;

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
  double                           myCurXStep;
  double                           myCurYStep;
  double                           myCurXSize;
  double                           myCurYSize;
  double                           myCurOffSet;
};

#endif // _V3d_RectangularGrid_HeaderFile
