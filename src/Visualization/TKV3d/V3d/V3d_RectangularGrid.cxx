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
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <V3d_Viewer.hxx>

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
  //! Main constructor.
  RectangularGridStructure(const occ::handle<Graphic3d_StructureManager>& theManager,
                           V3d_RectangularGrid*                           theGrid)
      : Graphic3d_Structure(theManager),
        myGrid(theGrid)
  {
  }

  //! Override method initiating recomputing in V3d_RectangularGrid.
  virtual void Compute() override
  {
    GraphicClear(false);
    myGrid->myGroup         = NewGroup();
    myGrid->myCurAreDefined = false;
    myGrid->UpdateDisplay();
  }

private:
  V3d_RectangularGrid* myGrid;
};

/*----------------------------------------------------------------------*/

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
      myXSize(0.5 * aViewer->DefaultViewSize()),
      myYSize(0.5 * aViewer->DefaultViewSize()),
      myOffSet(THE_DEFAULT_GRID_STEP / THE_MYFACTOR)
{
  myColor      = aColor;
  myTenthColor = aTenthColor;

  myStructure = new RectangularGridStructure(aViewer->StructureManager(), this);
  myGroup     = myStructure->NewGroup();
  myStructure->SetInfiniteState(true);

  SetXStep(THE_DEFAULT_GRID_STEP);
  SetYStep(THE_DEFAULT_GRID_STEP);
}

V3d_RectangularGrid::~V3d_RectangularGrid()
{
  myGroup.Nullify();
  if (!myStructure.IsNull())
  {
    myStructure->Erase();
  }
}

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

void V3d_RectangularGrid::Display()
{
  myStructure->SetDisplayPriority(Graphic3d_DisplayPriority_AlmostBottom);
  myStructure->Display();
  UpdateDisplay();
}

void V3d_RectangularGrid::Erase() const
{
  myStructure->Erase();
}

bool V3d_RectangularGrid::IsDisplayed() const
{
  return myStructure->IsDisplayed();
}

void V3d_RectangularGrid::UpdateDisplay()
{
  gp_Ax3 ThePlane = myViewer->PrivilegedPlane();

  bool   MakeTransform = false;
  double xl, yl, zl;
  double xdx, xdy, xdz;
  double ydx, ydy, ydz;
  double dx, dy, dz;
  ThePlane.Location().Coord(xl, yl, zl);
  ThePlane.XDirection().Coord(xdx, xdy, xdz);
  ThePlane.YDirection().Coord(ydx, ydy, ydz);
  ThePlane.Direction().Coord(dx, dy, dz);
  if (!myCurAreDefined)
    MakeTransform = true;
  else
  {
    if (RotationAngle() != myCurAngle || XOrigin() != myCurXo || YOrigin() != myCurYo)
      MakeTransform = true;
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
        MakeTransform = true;
    }
  }

  if (MakeTransform)
  {
    const double CosAlpha = std::cos(RotationAngle());
    const double SinAlpha = std::sin(RotationAngle());

    gp_Trsf aTrsf;
    // Translation
    // Transformation of change of marker
    aTrsf.SetValues(xdx, ydx, dx, xl, xdy, ydy, dy, yl, xdz, ydz, dz, zl);

    // Translation of the origin
    // Rotation Alpha around axis -Z
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

    myCurAngle = RotationAngle();
    myCurXo = XOrigin(), myCurYo = YOrigin();
    myCurViewPlane = ThePlane;
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

void V3d_RectangularGrid::DefineLines()
{
  const double aXStep   = XStep();
  const double aYStep   = YStep();
  const bool   toUpdate = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Lines
                        || aXStep != myCurXStep || aYStep != myCurYStep;
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

  int    nblines;
  double xl, yl, zl = myOffSet;

  NCollection_Sequence<gp_Pnt> aSeqLines, aSeqTenth;

  // verticals
  aSeqTenth.Append(gp_Pnt(0., -myYSize, -zl));
  aSeqTenth.Append(gp_Pnt(0., myYSize, -zl));
  for (nblines = 1, xl = aXStep; xl < myXSize; xl += aXStep, nblines++)
  {
    NCollection_Sequence<gp_Pnt>& aSeq = (Modulus(nblines, 10) != 0) ? aSeqLines : aSeqTenth;
    aSeq.Append(gp_Pnt(xl, -myYSize, -zl));
    aSeq.Append(gp_Pnt(xl, myYSize, -zl));
    aSeq.Append(gp_Pnt(-xl, -myYSize, -zl));
    aSeq.Append(gp_Pnt(-xl, myYSize, -zl));
  }

  // horizontals
  aSeqTenth.Append(gp_Pnt(-myXSize, 0., -zl));
  aSeqTenth.Append(gp_Pnt(myXSize, 0., -zl));
  for (nblines = 1, yl = aYStep; yl < myYSize; yl += aYStep, nblines++)
  {
    NCollection_Sequence<gp_Pnt>& aSeq = (Modulus(nblines, 10) != 0) ? aSeqLines : aSeqTenth;
    aSeq.Append(gp_Pnt(-myXSize, yl, -zl));
    aSeq.Append(gp_Pnt(myXSize, yl, -zl));
    aSeq.Append(gp_Pnt(-myXSize, -yl, -zl));
    aSeq.Append(gp_Pnt(myXSize, -yl, -zl));
  }

  if (aSeqLines.Length())
  {
    occ::handle<Graphic3d_AspectLine3d> aLineAspect =
      new Graphic3d_AspectLine3d(myColor, Aspect_TOL_SOLID, 1.0);
    myGroup->SetPrimitivesAspect(aLineAspect);
    const int                              nbv    = aSeqLines.Length();
    occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(nbv);
    int                                    n      = 1;
    while (n <= nbv)
      aPrims->AddVertex(aSeqLines(n++));
    myGroup->AddPrimitiveArray(aPrims, false);
  }
  if (aSeqTenth.Length())
  {
    occ::handle<Graphic3d_AspectLine3d> aLineAspect =
      new Graphic3d_AspectLine3d(myTenthColor, Aspect_TOL_SOLID, 1.0);
    myGroup->SetPrimitivesAspect(aLineAspect);
    const int                              nbv    = aSeqTenth.Length();
    occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(nbv);
    int                                    n      = 1;
    while (n <= nbv)
      aPrims->AddVertex(aSeqTenth(n++));
    myGroup->AddPrimitiveArray(aPrims, false);
  }

  myGroup->SetMinMaxValues(-myXSize, -myYSize, -myOffSet, myXSize, myYSize, -myOffSet);
  myCurXStep = aXStep, myCurYStep = aYStep;

  // update bounding box
  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
}

void V3d_RectangularGrid::DefinePoints()
{
  const double aXStep   = XStep();
  const double aYStep   = YStep();
  const bool   toUpdate = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Points
                        || aXStep != myCurXStep || aYStep != myCurYStep;
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

  // horizontals
  double                       xl, yl;
  NCollection_Sequence<gp_Pnt> aSeqPnts;
  for (xl = 0.0; xl <= myXSize; xl += aXStep)
  {
    aSeqPnts.Append(gp_Pnt(xl, 0.0, -myOffSet));
    aSeqPnts.Append(gp_Pnt(-xl, 0.0, -myOffSet));
    for (yl = aYStep; yl <= myYSize; yl += aYStep)
    {
      aSeqPnts.Append(gp_Pnt(xl, yl, -myOffSet));
      aSeqPnts.Append(gp_Pnt(xl, -yl, -myOffSet));
      aSeqPnts.Append(gp_Pnt(-xl, yl, -myOffSet));
      aSeqPnts.Append(gp_Pnt(-xl, -yl, -myOffSet));
    }
  }
  if (aSeqPnts.Length())
  {
    int                                  i;
    double                               X, Y, Z;
    const int                            nbv      = aSeqPnts.Length();
    occ::handle<Graphic3d_ArrayOfPoints> Vertical = new Graphic3d_ArrayOfPoints(nbv);
    for (i = 1; i <= nbv; i++)
    {
      aSeqPnts(i).Coord(X, Y, Z);
      Vertical->AddVertex(X, Y, Z);
    }

    occ::handle<Graphic3d_AspectMarker3d> aMarkerAspect =
      new Graphic3d_AspectMarker3d(Aspect_TOM_POINT, myColor, 3.0);
    myGroup->SetGroupPrimitivesAspect(aMarkerAspect);
    myGroup->AddPrimitiveArray(Vertical, false);
  }

  myGroup->SetMinMaxValues(-myXSize, -myYSize, -myOffSet, myXSize, myYSize, -myOffSet);
  myCurXStep = aXStep, myCurYStep = aYStep;

  // update bounding box
  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
}

void V3d_RectangularGrid::GraphicValues(double& theXSize, double& theYSize, double& theOffSet) const
{
  theXSize  = myXSize;
  theYSize  = myYSize;
  theOffSet = myOffSet;
}

void V3d_RectangularGrid::SetGraphicValues(const double theXSize,
                                           const double theYSize,
                                           const double theOffSet)
{
  if (!myCurAreDefined)
  {
    myXSize  = theXSize;
    myYSize  = theYSize;
    myOffSet = theOffSet;
  }
  if (myXSize != theXSize)
  {
    myXSize         = theXSize;
    myCurAreDefined = false;
  }
  if (myYSize != theYSize)
  {
    myYSize         = theYSize;
    myCurAreDefined = false;
  }
  if (myOffSet != theOffSet)
  {
    myOffSet        = theOffSet;
    myCurAreDefined = false;
  }
  if (!myCurAreDefined)
    UpdateDisplay();
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
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myXSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myYSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOffSet)
}
