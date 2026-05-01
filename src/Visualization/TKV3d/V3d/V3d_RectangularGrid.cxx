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

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <NCollection_LinearVector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <V3d_Viewer.hxx>
#include <gp_Pnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_RectangularGrid, Aspect_RectangularGrid)

namespace
{
constexpr double THE_DEFAULT_GRID_STEP = 10.0;
constexpr double THE_MYFACTOR          = 50.0;
} // namespace

//! Dummy implementation of Graphic3d_Structure overriding ::Compute() method for handling Device
//! Lost.
class V3d_RectangularGrid::RectangularGridStructure : public Graphic3d_Structure
{
public:
  RectangularGridStructure(const occ::handle<Graphic3d_StructureManager>& theManager,
                           V3d_RectangularGrid*                           theGrid)
      : Graphic3d_Structure(theManager),
        myGrid(theGrid)
  {
  }

  void Compute() override
  {
    GraphicClear(false);
    myGrid->myGroup         = NewGroup();
    myGrid->myCurAreDefined = false;
    myGrid->UpdateDisplay();
  }

private:
  V3d_RectangularGrid* myGrid;
};

//=================================================================================================

V3d_RectangularGrid::V3d_RectangularGrid(const V3d_ViewerPointer& aViewer,
                                         const Quantity_Color&    aColor,
                                         const Quantity_Color&    aTenthColor)
    : Aspect_RectangularGrid(1., 1.),
      myViewer(aViewer),
      myCurAreDefined(false),
      myToComputePrs(true),
      myCurDrawMode(Aspect_GDM_Lines),
      myCurXo(0.0),
      myCurYo(0.0),
      myCurAngle(0.0),
      myCurXStep(0.0),
      myCurYStep(0.0),
      myCurXSize(0.0),
      myCurYSize(0.0),
      myCurOffSet(0.0)
{
  myColor      = aColor;
  myTenthColor = aTenthColor;

  myStructure = new RectangularGridStructure(aViewer->StructureManager(), this);
  myGroup     = myStructure->NewGroup();
  myStructure->SetInfiniteState(true);

  SetXStep(THE_DEFAULT_GRID_STEP);
  SetYStep(THE_DEFAULT_GRID_STEP);
  // Aspect_RectangularGrid default-constructs SizeX/SizeY to 0; restore the historical
  // CPU bound of half the default view size so existing ActivateGrid() users still see
  // a bounded grid without needing an explicit SetGraphicValues call.
  Aspect_RectangularGrid::SetSizeX(0.5 * aViewer->DefaultViewSize());
  Aspect_RectangularGrid::SetSizeY(0.5 * aViewer->DefaultViewSize());
  Aspect_RectangularGrid::SetZOffset(THE_DEFAULT_GRID_STEP / THE_MYFACTOR);
}

//=================================================================================================

V3d_RectangularGrid::~V3d_RectangularGrid()
{
  myGroup.Nullify();
  if (!myStructure.IsNull())
  {
    myStructure->Erase();
  }
}

//=================================================================================================

void V3d_RectangularGrid::SetColors(const Quantity_Color& aColor, const Quantity_Color& aTenthColor)
{
  if (myColor != aColor || myTenthColor != aTenthColor)
  {
    myColor         = aColor;
    myTenthColor    = aTenthColor;
    myCurAreDefined = false;
    UpdateDisplay();
  }
}

//=================================================================================================

void V3d_RectangularGrid::Display()
{
  myStructure->SetDisplayPriority(Graphic3d_DisplayPriority_AlmostBottom);
  myStructure->Display();
  UpdateDisplay();
}

//=================================================================================================

void V3d_RectangularGrid::Erase() const
{
  myStructure->Erase();
}

//=================================================================================================

bool V3d_RectangularGrid::IsDisplayed() const
{
  return myStructure->IsDisplayed();
}

//=================================================================================================

void V3d_RectangularGrid::UpdateDisplay()
{
  const gp_Ax3 aPlane = myViewer->PrivilegedPlane();

  bool   MakeTransform = false;
  double xl, yl, zl;
  double xdx, xdy, xdz;
  double ydx, ydy, ydz;
  double dx, dy, dz;
  aPlane.Location().Coord(xl, yl, zl);
  aPlane.XDirection().Coord(xdx, xdy, xdz);
  aPlane.YDirection().Coord(ydx, ydy, ydz);
  aPlane.Direction().Coord(dx, dy, dz);
  if (!myCurAreDefined)
  {
    MakeTransform = true;
  }
  else
  {
    if (RotationAngle() != myCurAngle || XOrigin() != myCurXo || YOrigin() != myCurYo)
    {
      MakeTransform = true;
    }
    if (!MakeTransform)
    {
      double curxl, curyl, curzl;
      double curxdx, curxdy, curxdz;
      double curydx, curydy, curydz;
      double curdx, curdy, curdz;
      myCurViewPlane.Location().Coord(curxl, curyl, curzl);
      myCurViewPlane.XDirection().Coord(curxdx, curxdy, curxdz);
      myCurViewPlane.YDirection().Coord(curydx, curydy, curydz);
      myCurViewPlane.Direction().Coord(curdx, curdy, curdz);
      if (xl != curxl || yl != curyl || zl != curzl || xdx != curxdx || xdy != curxdy
          || xdz != curxdz || ydx != curydx || ydy != curydy || ydz != curydz || dx != curdx
          || dy != curdy || dz != curdz)
      {
        MakeTransform = true;
      }
    }
  }

  if (MakeTransform)
  {
    const double CosAlpha = std::cos(RotationAngle());
    const double SinAlpha = std::sin(RotationAngle());

    gp_Trsf aTrsf;
    aTrsf.SetValues(xdx, ydx, dx, xl, xdy, ydy, dy, yl, xdz, ydz, dz, zl);

    gp_Trsf aTrsf2;
    aTrsf2.SetValues(CosAlpha,
                     SinAlpha,
                     0.0,
                     -XOrigin(),
                     -SinAlpha,
                     CosAlpha,
                     0.0,
                     -YOrigin(),
                     0.0,
                     0.0,
                     1.0,
                     0.0);
    aTrsf.Multiply(aTrsf2);
    myStructure->SetTransformation(new TopLoc_Datum3D(aTrsf));

    myCurAngle     = RotationAngle();
    myCurXo        = XOrigin();
    myCurYo        = YOrigin();
    myCurViewPlane = aPlane;
  }

  switch (myDrawMode)
  {
    case Aspect_GDM_Points:
      DefinePoints();
      myCurDrawMode = Aspect_GDM_Points;
      break;
    case Aspect_GDM_Lines:
      DefineLines();
      myCurDrawMode = Aspect_GDM_Lines;
      break;
    case Aspect_GDM_None:
      myCurDrawMode = Aspect_GDM_None;
      break;
  }
  myCurAreDefined = true;
}

//=================================================================================================

void V3d_RectangularGrid::DefineLines()
{
  const double aXStep   = XStep();
  const double aYStep   = YStep();
  const double aXSize   = SizeX();
  const double aYSize   = SizeY();
  const double aOffSet  = ZOffset();
  const bool   toUpdate = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Lines
                        || aXStep != myCurXStep || aYStep != myCurYStep || aXSize != myCurXSize
                        || aYSize != myCurYSize || aOffSet != myCurOffSet;
  if (!toUpdate && !myToComputePrs)
  {
    return;
  }
  else if (!myStructure->IsDisplayed())
  {
    myToComputePrs = true;
    return;
  }

  myToComputePrs = false;
  myGroup->Clear();

  int          nblines;
  double       xl, yl;
  const double zl    = aOffSet;
  const size_t aNbXc = aXStep > 0.0 ? static_cast<size_t>(aXSize / aXStep) + 1 : 1;
  const size_t aNbYc = aYStep > 0.0 ? static_cast<size_t>(aYSize / aYStep) + 1 : 1;
  const size_t aResv = 4 * (aNbXc + aNbYc) + 4;

  NCollection_LinearVector<gp_Pnt> aSeqLines(aResv), aSeqTenth(aResv);

  aSeqTenth.EmplaceAppend(0., -aYSize, -zl);
  aSeqTenth.EmplaceAppend(0., aYSize, -zl);
  for (nblines = 1, xl = aXStep; xl < aXSize; xl += aXStep, nblines++)
  {
    NCollection_LinearVector<gp_Pnt>& aSeq = (Modulus(nblines, 10) != 0) ? aSeqLines : aSeqTenth;
    aSeq.EmplaceAppend(xl, -aYSize, -zl);
    aSeq.EmplaceAppend(xl, aYSize, -zl);
    aSeq.EmplaceAppend(-xl, -aYSize, -zl);
    aSeq.EmplaceAppend(-xl, aYSize, -zl);
  }

  aSeqTenth.EmplaceAppend(-aXSize, 0., -zl);
  aSeqTenth.EmplaceAppend(aXSize, 0., -zl);
  for (nblines = 1, yl = aYStep; yl < aYSize; yl += aYStep, nblines++)
  {
    NCollection_LinearVector<gp_Pnt>& aSeq = (Modulus(nblines, 10) != 0) ? aSeqLines : aSeqTenth;
    aSeq.EmplaceAppend(-aXSize, yl, -zl);
    aSeq.EmplaceAppend(aXSize, yl, -zl);
    aSeq.EmplaceAppend(-aXSize, -yl, -zl);
    aSeq.EmplaceAppend(aXSize, -yl, -zl);
  }

  if (!aSeqLines.IsEmpty())
  {
    occ::handle<Graphic3d_AspectLine3d> aLineAspect =
      new Graphic3d_AspectLine3d(myColor, Aspect_TOL_SOLID, 1.0);
    myGroup->SetPrimitivesAspect(aLineAspect);
    const int                              nbv    = static_cast<int>(aSeqLines.Size());
    occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(nbv);
    for (const gp_Pnt& aPnt : aSeqLines)
    {
      aPrims->AddVertex(aPnt);
    }
    myGroup->AddPrimitiveArray(aPrims, false);
  }
  if (!aSeqTenth.IsEmpty())
  {
    occ::handle<Graphic3d_AspectLine3d> aLineAspect =
      new Graphic3d_AspectLine3d(myTenthColor, Aspect_TOL_SOLID, 1.0);
    myGroup->SetPrimitivesAspect(aLineAspect);
    const int                              nbv    = static_cast<int>(aSeqTenth.Size());
    occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(nbv);
    for (const gp_Pnt& aPnt : aSeqTenth)
    {
      aPrims->AddVertex(aPnt);
    }
    myGroup->AddPrimitiveArray(aPrims, false);
  }

  myGroup->SetMinMaxValues(-aXSize, -aYSize, -aOffSet, aXSize, aYSize, -aOffSet);
  myCurXStep  = aXStep;
  myCurYStep  = aYStep;
  myCurXSize  = aXSize;
  myCurYSize  = aYSize;
  myCurOffSet = aOffSet;

  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
}

//=================================================================================================

void V3d_RectangularGrid::DefinePoints()
{
  const double aXStep   = XStep();
  const double aYStep   = YStep();
  const double aXSize   = SizeX();
  const double aYSize   = SizeY();
  const double aOffSet  = ZOffset();
  const bool   toUpdate = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Points
                        || aXStep != myCurXStep || aYStep != myCurYStep || aXSize != myCurXSize
                        || aYSize != myCurYSize || aOffSet != myCurOffSet;
  if (!toUpdate && !myToComputePrs)
  {
    return;
  }
  else if (!myStructure->IsDisplayed())
  {
    myToComputePrs = true;
    return;
  }

  myToComputePrs = false;
  myGroup->Clear();

  double       xl, yl;
  const size_t aNbXc = aXStep > 0.0 ? static_cast<size_t>(aXSize / aXStep) + 1 : 1;
  const size_t aNbYc = aYStep > 0.0 ? static_cast<size_t>(aYSize / aYStep) + 1 : 1;

  NCollection_LinearVector<gp_Pnt> aSeqPnts((2 * aNbXc + 1) * (2 * aNbYc + 1));
  for (xl = 0.0; xl <= aXSize; xl += aXStep)
  {
    aSeqPnts.EmplaceAppend(xl, 0.0, -aOffSet);
    aSeqPnts.EmplaceAppend(-xl, 0.0, -aOffSet);
    for (yl = aYStep; yl <= aYSize; yl += aYStep)
    {
      aSeqPnts.EmplaceAppend(xl, yl, -aOffSet);
      aSeqPnts.EmplaceAppend(xl, -yl, -aOffSet);
      aSeqPnts.EmplaceAppend(-xl, yl, -aOffSet);
      aSeqPnts.EmplaceAppend(-xl, -yl, -aOffSet);
    }
  }
  if (!aSeqPnts.IsEmpty())
  {
    const int                            nbv     = static_cast<int>(aSeqPnts.Size());
    occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(nbv);
    for (const gp_Pnt& aPnt : aSeqPnts)
    {
      aPoints->AddVertex(aPnt);
    }

    occ::handle<Graphic3d_AspectMarker3d> aMarkerAspect =
      new Graphic3d_AspectMarker3d(Aspect_TOM_POINT, myColor, 3.0);
    myGroup->SetGroupPrimitivesAspect(aMarkerAspect);
    myGroup->AddPrimitiveArray(aPoints, false);
  }

  myGroup->SetMinMaxValues(-aXSize, -aYSize, -aOffSet, aXSize, aYSize, -aOffSet);
  myCurXStep  = aXStep;
  myCurYStep  = aYStep;
  myCurXSize  = aXSize;
  myCurYSize  = aYSize;
  myCurOffSet = aOffSet;

  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
}

//=================================================================================================

void V3d_RectangularGrid::GraphicValues(double& theXSize, double& theYSize, double& theOffSet) const
{
  theXSize  = SizeX();
  theYSize  = SizeY();
  theOffSet = ZOffset();
}

//=================================================================================================

void V3d_RectangularGrid::SetGraphicValues(const double theXSize,
                                           const double theYSize,
                                           const double theOffSet)
{
  // Aspect_RectangularGrid setters trigger UpdateDisplay() on real change only.
  SetSizeX(theXSize);
  SetSizeY(theYSize);
  SetZOffset(theOffSet);
}

//=================================================================================================

void V3d_RectangularGrid::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Aspect_RectangularGrid)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myStructure.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myGroup.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCurViewPlane)
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myViewer)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurAreDefined)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToComputePrs)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurDrawMode)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurXo)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurYo)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurAngle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurXStep)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurYStep)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurXSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurYSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurOffSet)
}
