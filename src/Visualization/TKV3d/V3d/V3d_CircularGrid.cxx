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
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <V3d_Viewer.hxx>

IMPLEMENT_STANDARD_RTTIEXT(V3d_CircularGrid, Aspect_CircularGrid)

namespace
{
static const Standard_Real THE_DEFAULT_GRID_STEP = 10.0;
#define DIVISION 8
#define MYFACTOR 50.
} // namespace

//! Dummy implementation of Graphic3d_Structure overriding ::Compute() method for handling Device
//! Lost.
class V3d_CircularGrid::CircularGridStructure : public Graphic3d_Structure
{
public:
  //! Main constructor.
  CircularGridStructure(const Handle(Graphic3d_StructureManager)& theManager,
                        V3d_CircularGrid*                         theGrid)
      : Graphic3d_Structure(theManager),
        myGrid(theGrid)
  {
  }

  //! Override method initiating recomputing in V3d_CircularGrid.
  virtual void Compute() Standard_OVERRIDE
  {
    GraphicClear(Standard_False);
    myGrid->myGroup         = NewGroup();
    myGrid->myCurAreDefined = Standard_False;
    myGrid->UpdateDisplay();
  }

private:
  V3d_CircularGrid* myGrid;
};

/*----------------------------------------------------------------------*/

V3d_CircularGrid::V3d_CircularGrid(const V3d_ViewerPointer& aViewer,
                                   const Quantity_Color&    aColor,
                                   const Quantity_Color&    aTenthColor)
    : Aspect_CircularGrid(1., 8),
      myViewer(aViewer),
      myCurAreDefined(Standard_False),
      myToComputePrs(Standard_False),
      myCurDrawMode(Aspect_GDM_Lines),
      myCurXo(0.0),
      myCurYo(0.0),
      myCurAngle(0.0),
      myCurStep(0.0),
      myCurDivi(0),
      myRadius(0.5 * aViewer->DefaultViewSize()),
      myOffSet(THE_DEFAULT_GRID_STEP / MYFACTOR)
{
  myColor      = aColor;
  myTenthColor = aTenthColor;

  myStructure = new CircularGridStructure(aViewer->StructureManager(), this);
  myGroup     = myStructure->NewGroup();
  myStructure->SetInfiniteState(Standard_True);

  SetRadiusStep(THE_DEFAULT_GRID_STEP);
}

V3d_CircularGrid::~V3d_CircularGrid()
{
  myGroup.Nullify();
  if (!myStructure.IsNull())
  {
    myStructure->Erase();
  }
}

void V3d_CircularGrid::SetColors(const Quantity_Color& aColor, const Quantity_Color& aTenthColor)
{
  if (myColor != aColor || myTenthColor != aTenthColor)
  {
    myColor         = aColor;
    myTenthColor    = aTenthColor;
    myCurAreDefined = Standard_False;
    UpdateDisplay();
  }
}

void V3d_CircularGrid::Display()
{
  myStructure->SetDisplayPriority(Graphic3d_DisplayPriority_AlmostBottom);
  myStructure->Display();
  UpdateDisplay();
}

void V3d_CircularGrid::Erase() const
{
  myStructure->Erase();
}

Standard_Boolean V3d_CircularGrid::IsDisplayed() const
{
  return myStructure->IsDisplayed();
}

void V3d_CircularGrid::UpdateDisplay()
{
  gp_Ax3 ThePlane = myViewer->PrivilegedPlane();

  Standard_Real xl, yl, zl;
  Standard_Real xdx, xdy, xdz;
  Standard_Real ydx, ydy, ydz;
  Standard_Real dx, dy, dz;
  ThePlane.Location().Coord(xl, yl, zl);
  ThePlane.XDirection().Coord(xdx, xdy, xdz);
  ThePlane.YDirection().Coord(ydx, ydy, ydz);
  ThePlane.Direction().Coord(dx, dy, dz);

  Standard_Boolean MakeTransform = !myCurAreDefined;
  if (!MakeTransform)
  {
    MakeTransform = (RotationAngle() != myCurAngle || XOrigin() != myCurXo || YOrigin() != myCurYo);
    if (!MakeTransform)
    {
      Standard_Real curxl, curyl, curzl;
      Standard_Real curxdx, curxdy, curxdz;
      Standard_Real curydx, curydy, curydz;
      Standard_Real curdx, curdy, curdz;
      myCurViewPlane.Location().Coord(curxl, curyl, curzl);
      myCurViewPlane.XDirection().Coord(curxdx, curxdy, curxdz);
      myCurViewPlane.YDirection().Coord(curydx, curydy, curydz);
      myCurViewPlane.Direction().Coord(curdx, curdy, curdz);
      if (xl != curxl || yl != curyl || zl != curzl || xdx != curxdx || xdy != curxdy
          || xdz != curxdz || ydx != curydx || ydy != curydy || ydz != curydz || dx != curdx
          || dy != curdy || dz != curdz)
        MakeTransform = Standard_True;
    }
  }

  if (MakeTransform)
  {
    const Standard_Real CosAlpha = Cos(RotationAngle());
    const Standard_Real SinAlpha = Sin(RotationAngle());

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
  myCurAreDefined = Standard_True;
}

void V3d_CircularGrid::DefineLines()
{
  const Standard_Real    aStep     = RadiusStep();
  const Standard_Real    aDivision = DivisionNumber();
  const Standard_Boolean toUpdate  = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Lines
                                    || aDivision != myCurDivi || aStep != myCurStep;
  if (!toUpdate && !myToComputePrs)
  {
    return;
  }
  else if (!myStructure->IsDisplayed())
  {
    myToComputePrs = Standard_True;
    return;
  }

  myToComputePrs = Standard_False;
  myGroup->Clear();

  const Standard_Integer Division =
    (Standard_Integer)((aDivision >= DIVISION ? aDivision : DIVISION));

  Standard_Integer nbpnts = 2 * Division;
  // diametres
  Standard_Real alpha = M_PI / aDivision;

  myGroup->SetGroupPrimitivesAspect(
    new Graphic3d_AspectLine3d(myTenthColor, Aspect_TOL_SOLID, 1.0));
  Handle(Graphic3d_ArrayOfSegments) aPrims1 = new Graphic3d_ArrayOfSegments(2 * nbpnts);
  const gp_Pnt                      p0(0., 0., -myOffSet);
  for (Standard_Integer i = 1; i <= nbpnts; i++)
  {
    aPrims1->AddVertex(p0);
    aPrims1->AddVertex(Cos(alpha * i) * myRadius, Sin(alpha * i) * myRadius, -myOffSet);
  }
  myGroup->AddPrimitiveArray(aPrims1, Standard_False);

  // circles
  nbpnts                       = 2 * Division + 1;
  alpha                        = M_PI / Division;
  Standard_Integer     nblines = 0;
  TColgp_SequenceOfPnt aSeqLines, aSeqTenth;
  for (Standard_Real r = aStep; r <= myRadius; r += aStep, nblines++)
  {
    const Standard_Boolean isTenth = (Modulus(nblines, 10) == 0);
    for (Standard_Integer i = 0; i < nbpnts; i++)
    {
      const gp_Pnt pt(Cos(alpha * i) * r, Sin(alpha * i) * r, -myOffSet);
      (isTenth ? aSeqTenth : aSeqLines).Append(pt);
    }
  }
  if (aSeqTenth.Length())
  {
    myGroup->SetGroupPrimitivesAspect(
      new Graphic3d_AspectLine3d(myTenthColor, Aspect_TOL_SOLID, 1.0));
    Standard_Integer                   n, np;
    const Standard_Integer             nbl = aSeqTenth.Length() / nbpnts;
    Handle(Graphic3d_ArrayOfPolylines) aPrims2 =
      new Graphic3d_ArrayOfPolylines(aSeqTenth.Length(), nbl);
    for (np = 1, n = 0; n < nbl; n++)
    {
      aPrims2->AddBound(nbpnts);
      for (Standard_Integer i = 0; i < nbpnts; i++, np++)
        aPrims2->AddVertex(aSeqTenth(np));
    }
    myGroup->AddPrimitiveArray(aPrims2, Standard_False);
  }
  if (aSeqLines.Length())
  {
    myGroup->SetPrimitivesAspect(new Graphic3d_AspectLine3d(myColor, Aspect_TOL_SOLID, 1.0));
    Standard_Integer                   n, np;
    const Standard_Integer             nbl = aSeqLines.Length() / nbpnts;
    Handle(Graphic3d_ArrayOfPolylines) aPrims3 =
      new Graphic3d_ArrayOfPolylines(aSeqLines.Length(), nbl);
    for (np = 1, n = 0; n < nbl; n++)
    {
      aPrims3->AddBound(nbpnts);
      for (Standard_Integer i = 0; i < nbpnts; i++, np++)
        aPrims3->AddVertex(aSeqLines(np));
    }
    myGroup->AddPrimitiveArray(aPrims3, Standard_False);
  }

  myGroup->SetMinMaxValues(-myRadius, -myRadius, -myOffSet, myRadius, myRadius, -myOffSet);
  myCurStep = aStep, myCurDivi = (Standard_Integer)aDivision;

  // update bounding box
  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
}

void V3d_CircularGrid::DefinePoints()
{
  const Standard_Real    aStep     = RadiusStep();
  const Standard_Real    aDivision = DivisionNumber();
  const Standard_Boolean toUpdate  = !myCurAreDefined || myCurDrawMode != Aspect_GDM_Points
                                    || aDivision != myCurDivi || aStep != myCurStep;
  if (!toUpdate && !myToComputePrs)
  {
    return;
  }
  else if (!myStructure->IsDisplayed())
  {
    myToComputePrs = Standard_True;
    return;
  }

  myToComputePrs = Standard_False;
  myGroup->Clear();

  Handle(Graphic3d_AspectMarker3d) MarkerAttrib = new Graphic3d_AspectMarker3d();
  MarkerAttrib->SetColor(myColor);
  MarkerAttrib->SetType(Aspect_TOM_POINT);
  MarkerAttrib->SetScale(3.);

  const Standard_Integer nbpnts = Standard_Integer(2 * aDivision);
  Standard_Real          r, alpha = M_PI / aDivision;

  // diameters
  TColgp_SequenceOfPnt aSeqPnts;
  aSeqPnts.Append(gp_Pnt(0.0, 0.0, -myOffSet));
  for (r = aStep; r <= myRadius; r += aStep)
  {
    for (Standard_Integer i = 0; i < nbpnts; i++)
      aSeqPnts.Append(gp_Pnt(Cos(alpha * i) * r, Sin(alpha * i) * r, -myOffSet));
  }
  myGroup->SetGroupPrimitivesAspect(MarkerAttrib);
  if (aSeqPnts.Length())
  {
    Standard_Real                   X, Y, Z;
    const Standard_Integer          nbv    = aSeqPnts.Length();
    Handle(Graphic3d_ArrayOfPoints) Cercle = new Graphic3d_ArrayOfPoints(nbv);
    for (Standard_Integer i = 1; i <= nbv; i++)
    {
      aSeqPnts(i).Coord(X, Y, Z);
      Cercle->AddVertex(X, Y, Z);
    }
    myGroup->AddPrimitiveArray(Cercle, Standard_False);
  }
  myGroup->SetMinMaxValues(-myRadius, -myRadius, -myOffSet, myRadius, myRadius, -myOffSet);

  myCurStep = aStep, myCurDivi = (Standard_Integer)aDivision;

  // update bounding box
  myStructure->CalculateBoundBox();
  myViewer->StructureManager()->Update(myStructure->GetZLayer());
}

void V3d_CircularGrid::GraphicValues(Standard_Real& theRadius, Standard_Real& theOffSet) const
{
  theRadius = myRadius;
  theOffSet = myOffSet;
}

void V3d_CircularGrid::SetGraphicValues(const Standard_Real theRadius,
                                        const Standard_Real theOffSet)
{
  if (!myCurAreDefined)
  {
    myRadius = theRadius;
    myOffSet = theOffSet;
  }
  if (myRadius != theRadius)
  {
    myRadius        = theRadius;
    myCurAreDefined = Standard_False;
  }
  if (myOffSet != theOffSet)
  {
    myOffSet        = theOffSet;
    myCurAreDefined = Standard_False;
  }
  if (!myCurAreDefined)
    UpdateDisplay();
}

//=================================================================================================

void V3d_CircularGrid::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
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
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRadius)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myOffSet)
}
