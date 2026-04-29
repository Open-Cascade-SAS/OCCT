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

//! Rectangular grid bound to a V3d_Viewer. Snap math (Compute/Hit) is inherited
//! from Aspect_RectangularGrid; rendering goes through the shader-based infinite
//! grid (OpenGl_View::renderGrid) instead of CPU-generated line segments.
class V3d_RectangularGrid : public Aspect_RectangularGrid
{
  DEFINE_STANDARD_RTTIEXT(V3d_RectangularGrid, Aspect_RectangularGrid)
public:
  Standard_EXPORT V3d_RectangularGrid(const V3d_ViewerPointer& aViewer,
                                      const Quantity_Color&    aColor,
                                      const Quantity_Color&    aTenthColor);

  Standard_EXPORT ~V3d_RectangularGrid() override;

  Standard_EXPORT void SetColors(const Quantity_Color& aColor,
                                 const Quantity_Color& aTenthColor) override;

  Standard_EXPORT void Display() override;

  Standard_EXPORT void Erase() const override;

  Standard_EXPORT bool IsDisplayed() const override;

  Standard_EXPORT void GraphicValues(double& XSize, double& YSize, double& OffSet) const;

  Standard_EXPORT void SetGraphicValues(const double XSize,
                                        const double YSize,
                                        const double OffSet);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

protected:
  Standard_EXPORT void UpdateDisplay() override;

private:
  //! Broadcast current parameters to every view owned by the viewer.
  //! When theDoDisplay is false, erases the shader grid from each view instead.
  void syncViews(const bool theDoDisplay) const;

private:
  V3d_ViewerPointer myViewer;
  mutable bool      myIsDisplayed;
};

#endif // _V3d_RectangularGrid_HeaderFile
