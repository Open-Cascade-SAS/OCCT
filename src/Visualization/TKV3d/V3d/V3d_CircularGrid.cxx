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

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Message.hxx>
#include <NCollection_LinearVector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <V3d_Viewer.hxx>
#include <gp_Pnt.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_CircularGrid, Aspect_CircularGrid)

namespace
{
constexpr double THE_DEFAULT_GRID_STEP = 10.0;
constexpr int    THE_DEFAULT_DIVISION  = 8;
constexpr double THE_MYFACTOR          = 50.0;
} // namespace

class V3d_CircularGrid::CircularGridStructure : public Graphic3d_Structure
{
public:
  CircularGridStructure(const occ::handle<Graphic3d_StructureManager>& theManager,
                        V3d_CircularGrid*                              theGrid)
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
  V3d_CircularGrid* myGrid;
};

//=================================================================================================

V3d_CircularGrid::V3d_CircularGrid(const V3d_ViewerPointer& aViewer,
                                   const Quantity_Color&    aColor,
                                   const Quantity_Color&    aTenthColor)
    : Aspect_CircularGrid(1., THE_DEFAULT_DIVISION),
      myViewer(aViewer),
      myCurAreDefined(false),
      myToComputePrs(false),
      myCurDrawMode(Aspect_GDM_Lines),
      myCurXo(0.0),
      myCurYo(0.0),
      myCurAngle(0.0),
      myCurStep(0.0),
      myCurDivi(0),
      myCurRadius(0.0),
      myCurOffSet(0.0),
      myArcWarned(false)
{
  myColor      = aColor;
  myTenthColor = aTenthColor;

  myStructure = new CircularGridStructure(aViewer->StructureManager(), this);
  myGroup     = myStructure->NewGroup();
  myStructure->SetInfiniteState(true);

  SetRadiusStep(THE_DEFAULT_GRID_STEP);
  Aspect_CircularGrid::SetRadius(0.5 * aViewer->DefaultViewSize());
  Aspect_CircularGrid::SetZOffset(THE_DEFAULT_GRID_STEP / THE_MYFACTOR);
}

//=================================================================================================

V3d_CircularGrid::~V3d_CircularGrid()
{
  myGroup.Nullify();
  if (!myStructure.IsNull())
  {
    myStructure->Erase();
  }
}

//=================================================================================================

void V3d_CircularGrid::SetColors(const Quantity_Color& aColor, const Quantity_Color& aTenthColor)
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

void V3d_CircularGrid::Display()
{
  myStructure->SetDisplayPriority(Graphic3d_DisplayPriority_AlmostBottom);
  myStructure->Display();
  UpdateDisplay();
}

//=================================================================================================

void V3d_CircularGrid::Erase() const
{
  myStructure->Erase();
}

//=================================================================================================

bool V3d_CircularGrid::IsDisplayed() const
{
  return myStructure->IsDisplayed();
}

//=================================================================================================

void V3d_CircularGrid::UpdateDisplay()
{
  if (IsArc() && !myArcWarned)
  {
    Message::SendWarning() << "V3d_CircularGrid: arc range (AngleStart/AngleEnd) is not "
                              "supported by the CPU rendering path and will be ignored. "
                              "Use V3d_View::GridDisplay with Aspect_GridParams::SetArcRange "
                              "for arc rendering.";
    myArcWarned = true;
  }

  const gp_Ax3 aPlane = myViewer->PrivilegedPlane();

  double xl, yl, zl;
  double xdx, xdy, xdz;
  double ydx, ydy, ydz;
  double dx, dy, dz;
  aPlane.Location().Coord(xl, yl, zl);
  aPlane.XDirection().Coord(xdx, xdy, xdz);
  aPlane.YDirection().Coord(ydx, ydy, ydz);
  aPlane.Direction().Coord(dx, dy, dz);

  bool MakeTransform = !myCurAreDefined;
  if (!MakeTransform)
  {
    MakeTransform = (RotationAngle() != myCurAngle || XOrigin() != myCurXo || YOrigin() != myCurYo);
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

void V3d_CircularGrid::DefineLines()
{
  const double aStep     = RadiusStep();
  const double aDivision = DivisionNumber();
  const double aRadius   = Radius();
  const double aOffSet   = ZOffset();
  const bool   toUpdate  = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Lines
                        || aDivision != myCurDivi || aStep != myCurStep || aRadius != myCurRadius
                        || aOffSet != myCurOffSet;
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

  const int Division =
    (int)((aDivision >= THE_DEFAULT_DIVISION ? aDivision : THE_DEFAULT_DIVISION));

  int    nbpnts = 2 * Division;
  double alpha  = M_PI / aDivision;

  myGroup->SetGroupPrimitivesAspect(
    new Graphic3d_AspectLine3d(myTenthColor, Aspect_TOL_SOLID, 1.0));
  occ::handle<Graphic3d_ArrayOfSegments> aPrims1 = new Graphic3d_ArrayOfSegments(2 * nbpnts);
  const gp_Pnt                           p0(0., 0., -aOffSet);
  for (int i = 1; i <= nbpnts; i++)
  {
    aPrims1->AddVertex(p0);
    aPrims1->AddVertex(std::cos(alpha * i) * aRadius, std::sin(alpha * i) * aRadius, -aOffSet);
  }
  myGroup->AddPrimitiveArray(aPrims1, false);

  nbpnts               = 2 * Division + 1;
  alpha                = M_PI / Division;
  int          nblines = 0;
  const size_t aNbR    = aStep > 0.0 ? static_cast<size_t>(aRadius / aStep) + 1 : 1;
  const size_t aResv   = aNbR * static_cast<size_t>(nbpnts);
  NCollection_LinearVector<gp_Pnt> aSeqLines(aResv), aSeqTenth(aResv);
  for (double r = aStep; r <= aRadius; r += aStep, nblines++)
  {
    const bool isTenth = (Modulus(nblines, 10) == 0);
    for (int i = 0; i < nbpnts; i++)
    {
      (isTenth ? aSeqTenth : aSeqLines)
        .EmplaceAppend(std::cos(alpha * i) * r, std::sin(alpha * i) * r, -aOffSet);
    }
  }
  if (!aSeqTenth.IsEmpty())
  {
    myGroup->SetGroupPrimitivesAspect(
      new Graphic3d_AspectLine3d(myTenthColor, Aspect_TOL_SOLID, 1.0));
    const int                               aSize   = static_cast<int>(aSeqTenth.Size());
    const int                               nbl     = aSize / nbpnts;
    occ::handle<Graphic3d_ArrayOfPolylines> aPrims2 = new Graphic3d_ArrayOfPolylines(aSize, nbl);
    int                                     np      = 0;
    for (int n = 0; n < nbl; n++)
    {
      aPrims2->AddBound(nbpnts);
      for (int i = 0; i < nbpnts; i++, np++)
      {
        aPrims2->AddVertex(aSeqTenth[np]);
      }
    }
    myGroup->AddPrimitiveArray(aPrims2, false);
  }
  if (!aSeqLines.IsEmpty())
  {
    myGroup->SetPrimitivesAspect(new Graphic3d_AspectLine3d(myColor, Aspect_TOL_SOLID, 1.0));
    const int                               aSize   = static_cast<int>(aSeqLines.Size());
    const int                               nbl     = aSize / nbpnts;
    occ::handle<Graphic3d_ArrayOfPolylines> aPrims3 = new Graphic3d_ArrayOfPolylines(aSize, nbl);
    int                                     np      = 0;
    for (int n = 0; n < nbl; n++)
    {
      aPrims3->AddBound(nbpnts);
      for (int i = 0; i < nbpnts; i++, np++)
      {
        aPrims3->AddVertex(aSeqLines[np]);
      }
    }
    myGroup->AddPrimitiveArray(aPrims3, false);
  }

  myGroup->SetMinMaxValues(-aRadius, -aRadius, -aOffSet, aRadius, aRadius, -aOffSet);
  myCurStep   = aStep;
  myCurDivi   = (int)aDivision;
  myCurRadius = aRadius;
  myCurOffSet = aOffSet;

  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
}

//=================================================================================================

void V3d_CircularGrid::DefinePoints()
{
  const double aStep     = RadiusStep();
  const double aDivision = DivisionNumber();
  const double aRadius   = Radius();
  const double aOffSet   = ZOffset();
  const bool   toUpdate  = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Points
                        || aDivision != myCurDivi || aStep != myCurStep || aRadius != myCurRadius
                        || aOffSet != myCurOffSet;
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

  occ::handle<Graphic3d_AspectMarker3d> aMarkerAttrib = new Graphic3d_AspectMarker3d();
  aMarkerAttrib->SetColor(myColor);
  aMarkerAttrib->SetType(Aspect_TOM_POINT);
  aMarkerAttrib->SetScale(3.);

  const int nbpnts = int(2 * aDivision);
  double    r, alpha = M_PI / aDivision;

  const size_t aNbR = aStep > 0.0 ? static_cast<size_t>(aRadius / aStep) + 1 : 1;
  NCollection_LinearVector<gp_Pnt> aSeqPnts(aNbR * static_cast<size_t>(nbpnts) + 1);
  aSeqPnts.EmplaceAppend(0.0, 0.0, -aOffSet);
  for (r = aStep; r <= aRadius; r += aStep)
  {
    for (int i = 0; i < nbpnts; i++)
    {
      aSeqPnts.EmplaceAppend(std::cos(alpha * i) * r, std::sin(alpha * i) * r, -aOffSet);
    }
  }
  myGroup->SetGroupPrimitivesAspect(aMarkerAttrib);
  if (!aSeqPnts.IsEmpty())
  {
    const int                            nbv     = static_cast<int>(aSeqPnts.Size());
    occ::handle<Graphic3d_ArrayOfPoints> aPoints = new Graphic3d_ArrayOfPoints(nbv);
    for (const gp_Pnt& aPnt : aSeqPnts)
    {
      aPoints->AddVertex(aPnt);
    }
    myGroup->AddPrimitiveArray(aPoints, false);
  }
  myGroup->SetMinMaxValues(-aRadius, -aRadius, -aOffSet, aRadius, aRadius, -aOffSet);

  myCurStep   = aStep;
  myCurDivi   = (int)aDivision;
  myCurRadius = aRadius;
  myCurOffSet = aOffSet;

  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
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
  SetRadius(theRadius);
  SetZOffset(theOffSet);
}

//=================================================================================================

void V3d_CircularGrid::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Aspect_CircularGrid)

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
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurStep)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurDivi)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCurOffSet)
}
