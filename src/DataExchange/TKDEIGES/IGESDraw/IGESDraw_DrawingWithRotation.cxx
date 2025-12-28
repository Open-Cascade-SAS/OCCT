// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_View.hxx>
#include <IGESGraph_DrawingSize.hxx>
#include <IGESGraph_DrawingUnits.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_DrawingWithRotation, IGESData_IGESEntity)

IGESDraw_DrawingWithRotation::IGESDraw_DrawingWithRotation() {}

void IGESDraw_DrawingWithRotation::Init(const occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>& allViews,
                                        const occ::handle<NCollection_HArray1<gp_XY>>&    allViewOrigins,
                                        const occ::handle<NCollection_HArray1<double>>& allOrientationAngles,
                                        const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allAnnotations)
{
  int Len = allViews->Length();
  if (allViews->Lower() != 1 || (allViewOrigins->Lower() != 1 || allViewOrigins->Length() != Len)
      || (allOrientationAngles->Lower() != 1 || allOrientationAngles->Length() != Len))
    throw Standard_DimensionMismatch("IGESDraw_DrawingWithRotation : Init");
  if (!allAnnotations.IsNull())
    if (allAnnotations->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDraw_DrawingWithRotation : Init");

  theViews             = allViews;
  theViewOrigins       = allViewOrigins;
  theOrientationAngles = allOrientationAngles;
  theAnnotations       = allAnnotations;
  InitTypeAndForm(404, 1);
}

int IGESDraw_DrawingWithRotation::NbViews() const
{
  return (theViews->Length());
}

occ::handle<IGESData_ViewKindEntity> IGESDraw_DrawingWithRotation::ViewItem(
  const int Index) const
{
  return (theViews->Value(Index));
}

gp_Pnt2d IGESDraw_DrawingWithRotation::ViewOrigin(const int Index) const
{
  return (gp_Pnt2d(theViewOrigins->Value(Index)));
}

double IGESDraw_DrawingWithRotation::OrientationAngle(const int Index) const
{
  return (theOrientationAngles->Value(Index));
}

int IGESDraw_DrawingWithRotation::NbAnnotations() const
{
  return (theAnnotations.IsNull() ? 0 : theAnnotations->Length());
}

occ::handle<IGESData_IGESEntity> IGESDraw_DrawingWithRotation::Annotation(
  const int Index) const
{
  return (theAnnotations->Value(Index));
}

gp_XY IGESDraw_DrawingWithRotation::ViewToDrawing(const int NumView,
                                                  const gp_XYZ&          ViewCoords) const
{
  gp_XY         thisOrigin     = theViewOrigins->Value(NumView);
  double XOrigin        = thisOrigin.X();
  double YOrigin        = thisOrigin.Y();
  double theScaleFactor = 0.;

  occ::handle<IGESData_ViewKindEntity> tempView = theViews->Value(NumView);
  if (tempView->IsKind(STANDARD_TYPE(IGESDraw_View)))
  {
    DeclareAndCast(IGESDraw_View, thisView, tempView);
    theScaleFactor = thisView->ScaleFactor();
  }
  else if (tempView->IsKind(STANDARD_TYPE(IGESDraw_PerspectiveView)))
  {
    DeclareAndCast(IGESDraw_PerspectiveView, thisView, tempView);
    theScaleFactor = thisView->ScaleFactor();
  }

  double XV = ViewCoords.X();
  double YV = ViewCoords.Y();

  double theta = theOrientationAngles->Value(NumView);

  double XD = XOrigin + theScaleFactor * (XV * std::cos(theta) - YV * std::sin(theta));
  double YD = YOrigin + theScaleFactor * (XV * std::sin(theta) + YV * std::cos(theta));

  return (gp_XY(XD, YD));
}

bool IGESDraw_DrawingWithRotation::DrawingUnit(double& val) const
{
  val                           = 0.;
  occ::handle<Standard_Type> typunit = STANDARD_TYPE(IGESGraph_DrawingUnits);
  if (NbTypedProperties(typunit) != 1)
    return false;
  DeclareAndCast(IGESGraph_DrawingUnits, units, TypedProperty(typunit));
  if (units.IsNull())
    return false;
  val = units->UnitValue();
  return true;
}

bool IGESDraw_DrawingWithRotation::DrawingSize(double& X, double& Y) const
{
  X = Y                         = 0.;
  occ::handle<Standard_Type> typsize = STANDARD_TYPE(IGESGraph_DrawingSize);
  if (NbTypedProperties(typsize) != 1)
    return false;
  DeclareAndCast(IGESGraph_DrawingSize, size, TypedProperty(typsize));
  if (size.IsNull())
    return false;
  X = size->XSize();
  Y = size->YSize();
  return true;
}
