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

#include <V3d_RectangularGrid.hxx>

#include <Aspect_GridParams.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_RectangularGrid, Aspect_RectangularGrid)

namespace
{
constexpr double THE_DEFAULT_GRID_STEP = 10.0;
constexpr double THE_MYFACTOR          = 50.0;
} // namespace

//=================================================================================================

V3d_RectangularGrid::V3d_RectangularGrid(const V3d_ViewerPointer& aViewer,
                                         const Quantity_Color&    aColor,
                                         const Quantity_Color&    aTenthColor)
    : Aspect_RectangularGrid(1., 1.),
      myViewer(aViewer),
      myIsDisplayed(false),
      myXSize(0.5 * aViewer->DefaultViewSize()),
      myYSize(0.5 * aViewer->DefaultViewSize()),
      myOffSet(THE_DEFAULT_GRID_STEP / THE_MYFACTOR)
{
  myColor      = aColor;
  myTenthColor = aTenthColor;

  SetXStep(THE_DEFAULT_GRID_STEP);
  SetYStep(THE_DEFAULT_GRID_STEP);
}

//=================================================================================================

V3d_RectangularGrid::~V3d_RectangularGrid()
{
  if (myIsDisplayed)
  {
    syncViews(false);
  }
}

//=================================================================================================

void V3d_RectangularGrid::SetColors(const Quantity_Color& aColor, const Quantity_Color& aTenthColor)
{
  if (myColor != aColor || myTenthColor != aTenthColor)
  {
    myColor      = aColor;
    myTenthColor = aTenthColor;
    UpdateDisplay();
  }
}

//=================================================================================================

void V3d_RectangularGrid::Display()
{
  myIsDisplayed = true;
  syncViews(true);
}

//=================================================================================================

void V3d_RectangularGrid::Erase() const
{
  myIsDisplayed = false;
  syncViews(false);
}

//=================================================================================================

bool V3d_RectangularGrid::IsDisplayed() const
{
  return myIsDisplayed;
}

//=================================================================================================

void V3d_RectangularGrid::UpdateDisplay()
{
  if (myIsDisplayed)
  {
    syncViews(true);
  }
}

//=================================================================================================

void V3d_RectangularGrid::GraphicValues(double& theXSize, double& theYSize, double& theOffSet) const
{
  theXSize  = myXSize;
  theYSize  = myYSize;
  theOffSet = myOffSet;
}

//=================================================================================================

void V3d_RectangularGrid::SetGraphicValues(const double theXSize,
                                           const double theYSize,
                                           const double theOffSet)
{
  bool aChanged = false;
  if (myXSize != theXSize)
  {
    myXSize  = theXSize;
    aChanged = true;
  }
  if (myYSize != theYSize)
  {
    myYSize  = theYSize;
    aChanged = true;
  }
  if (myOffSet != theOffSet)
  {
    myOffSet = theOffSet;
    aChanged = true;
  }
  if (aChanged)
  {
    UpdateDisplay();
  }
}

//=================================================================================================

void V3d_RectangularGrid::syncViews(const bool theDoDisplay) const
{
  if (myViewer == nullptr)
  {
    return;
  }

  // XStep/YStep carry world-unit spacing; the shader consumes cells-per-unit (1/step).
  // Guard zero/negative step that could come from Aspect_RectangularGrid defaults.
  const double aXStep = XStep() > 0.0 ? XStep() : THE_DEFAULT_GRID_STEP;
  const double aYStep = YStep() > 0.0 ? YStep() : THE_DEFAULT_GRID_STEP;

  Aspect_GridParams aParams;
  aParams.SetColor(myColor);
  aParams.SetOrigin(gp_Pnt(XOrigin(), YOrigin(), -myOffSet));
  aParams.SetScale(1.0 / aXStep);
  aParams.SetScaleY(1.0 / aYStep);
  aParams.SetRotationAngle(RotationAngle());
  aParams.SetDrawMode(DrawMode());
  aParams.SetIsBackground(false);
  aParams.SetIsDrawAxis(false);
  aParams.SetIsInfinity(false);

  const gp_Ax3 aPlane = myViewer->PrivilegedPlane();

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

void V3d_RectangularGrid::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Aspect_RectangularGrid)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myViewer)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsDisplayed)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myXSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myYSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOffSet)
}
