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

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESDimen_SectionedArea.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_SectionedArea, IGESData_IGESEntity)

IGESDimen_SectionedArea::IGESDimen_SectionedArea() = default;

void IGESDimen_SectionedArea::Init(
  const occ::handle<IGESData_IGESEntity>&                                   aCurve,
  const int                                                                 aPattern,
  const gp_XYZ&                                                             aPoint,
  const double                                                              aDistance,
  const double                                                              anAngle,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& someIslands)
{
  if (!someIslands.IsNull())
    if (someIslands->Lower() != 1)
      throw Standard_DimensionMismatch("IGESDimen_SectionedArea : Init");
  theExteriorCurve = aCurve;
  thePattern       = aPattern;
  thePassingPoint  = aPoint;
  theDistance      = aDistance;
  theAngle         = anAngle;
  theIslandCurves  = someIslands;
  InitTypeAndForm(230, FormNumber());
}

void IGESDimen_SectionedArea::SetInverted(const bool mode)
{
  InitTypeAndForm(230, (mode ? 1 : 0));
}

bool IGESDimen_SectionedArea::IsInverted() const
{
  return (FormNumber() != 0);
}

occ::handle<IGESData_IGESEntity> IGESDimen_SectionedArea::ExteriorCurve() const
{
  return theExteriorCurve;
}

int IGESDimen_SectionedArea::Pattern() const
{
  return thePattern;
}

gp_Pnt IGESDimen_SectionedArea::PassingPoint() const
{
  return gp_Pnt(thePassingPoint);
}

gp_Pnt IGESDimen_SectionedArea::TransformedPassingPoint() const
{
  gp_XYZ tmpXYZ(thePassingPoint);
  if (HasTransf())
    Location().Transforms(tmpXYZ);
  return gp_Pnt(tmpXYZ);
}

double IGESDimen_SectionedArea::ZDepth() const
{
  return thePassingPoint.Z();
}

double IGESDimen_SectionedArea::Distance() const
{
  return theDistance;
}

double IGESDimen_SectionedArea::Angle() const
{
  return theAngle;
}

int IGESDimen_SectionedArea::NbIslands() const
{
  return (theIslandCurves.IsNull() ? 0 : theIslandCurves->Length());
}

occ::handle<IGESData_IGESEntity> IGESDimen_SectionedArea::IslandCurve(const int num) const
{
  return theIslandCurves->Value(num);
}
