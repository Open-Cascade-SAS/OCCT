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

#include <V3d_CircularGrid.hxx>

#include <Aspect_GridParams.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_CircularGrid, Aspect_CircularGrid)

namespace
{
constexpr double THE_DEFAULT_GRID_STEP = 10.0;
constexpr int    THE_DEFAULT_DIVISION  = 8;
constexpr double THE_MYFACTOR          = 50.0;
} // namespace

//=================================================================================================

V3d_CircularGrid::V3d_CircularGrid(const V3d_ViewerPointer& aViewer,
                                   const Quantity_Color&    aColor,
                                   const Quantity_Color&    aTenthColor)
    : Aspect_CircularGrid(1., THE_DEFAULT_DIVISION),
      myViewer(aViewer),
      myIsDisplayed(false)
{
  myColor      = aColor;
  myTenthColor = aTenthColor;

  SetRadiusStep(THE_DEFAULT_GRID_STEP);
  Aspect_CircularGrid::SetRadius(0.5 * aViewer->DefaultViewSize());
  Aspect_CircularGrid::SetZOffset(THE_DEFAULT_GRID_STEP / THE_MYFACTOR);
}

//=================================================================================================

V3d_CircularGrid::~V3d_CircularGrid()
{
  if (myIsDisplayed)
  {
    syncViews(false);
  }
}

//=================================================================================================

void V3d_CircularGrid::SetColors(const Quantity_Color& aColor, const Quantity_Color& aTenthColor)
{
  if (myColor != aColor || myTenthColor != aTenthColor)
  {
    myColor      = aColor;
    myTenthColor = aTenthColor;
    UpdateDisplay();
  }
}

//=================================================================================================

void V3d_CircularGrid::Display()
{
  myIsDisplayed = true;
  syncViews(true);
}

//=================================================================================================

void V3d_CircularGrid::Erase() const
{
  myIsDisplayed = false;
  syncViews(false);
}

//=================================================================================================

bool V3d_CircularGrid::IsDisplayed() const
{
  return myIsDisplayed;
}

//=================================================================================================

void V3d_CircularGrid::UpdateDisplay()
{
  if (myIsDisplayed)
  {
    syncViews(true);
  }
}

//=================================================================================================

void V3d_CircularGrid::GraphicValues(double& theRadius, double& theOffSet) const
{
  theRadius = Radius();
  theOffSet = ZOffset();
}

//=================================================================================================

void V3d_CircularGrid::SetGraphicValues(const double theRadius, const double theOffSet)
{
  // Base-class setters each trigger UpdateDisplay only on real change.
  SetRadius(theRadius);
  SetZOffset(theOffSet);
}

//=================================================================================================

void V3d_CircularGrid::syncViews(const bool theDoDisplay) const
{
  if (myViewer == nullptr)
  {
    return;
  }

  const double aRadiusStep = RadiusStep() > 0.0 ? RadiusStep() : THE_DEFAULT_GRID_STEP;
  const int    aDivisions  = DivisionNumber() > 0 ? DivisionNumber() : THE_DEFAULT_DIVISION;

  const gp_Ax3 aPlane = myViewer->PrivilegedPlane();

  // Same convention as V3d_RectangularGrid::syncViews: origin is a world-space
  // offset aligned with the plane basis so the shader's aPlaneOrigin matches
  // V3d_View::Compute's aPnt0 used for snap selection.
  const gp_XYZ aOriginOffset =
    aPlane.XDirection().XYZ() * -XOrigin() + aPlane.YDirection().XYZ() * -YOrigin();

  Aspect_GridParams aParams;
  aParams.SetColor(myColor);
  aParams.SetOrigin(gp_Pnt(aOriginOffset));
  aParams.SetScale(1.0 / aRadiusStep);
  aParams.SetScaleY(0.0); // unused in circular mode
  aParams.SetRotationAngle(RotationAngle());
  aParams.SetAngularDivisions(aDivisions);
  aParams.SetDrawMode(DrawMode());
  aParams.SetRadius(Radius());
  aParams.SetZOffset(ZOffset());
  aParams.SetArcRange(AngleStart(), AngleEnd());
  aParams.SetIsBackground(false);
  aParams.SetIsDrawAxis(false);
  aParams.SetIsInfinity(false);

  for (const occ::handle<V3d_View>& aView : myViewer->DefinedViews())
  {
    if (aView.IsNull())
    {
      continue;
    }
    if (theDoDisplay)
    {
      aView->GridDisplay(aParams, aPlane);
    }
    else
    {
      aView->GridErase();
    }
  }
}

//=================================================================================================

void V3d_CircularGrid::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Aspect_CircularGrid)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myViewer)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsDisplayed)
}
