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

#include <IGESGeom_Line.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_SurfaceOfRevolution, IGESData_IGESEntity)

IGESGeom_SurfaceOfRevolution::IGESGeom_SurfaceOfRevolution() = default;

void IGESGeom_SurfaceOfRevolution::Init(const occ::handle<IGESGeom_Line>&       anAxis,
                                        const occ::handle<IGESData_IGESEntity>& aGeneratrix,
                                        const double                            aStartAngle,
                                        const double                            anEndAngle)
{
  theLine       = anAxis;
  theGeneratrix = aGeneratrix;
  theStartAngle = aStartAngle;
  theEndAngle   = anEndAngle;
  InitTypeAndForm(120, 0);
}

occ::handle<IGESGeom_Line> IGESGeom_SurfaceOfRevolution::AxisOfRevolution() const
{
  return theLine;
}

occ::handle<IGESData_IGESEntity> IGESGeom_SurfaceOfRevolution::Generatrix() const
{
  return theGeneratrix;
}

double IGESGeom_SurfaceOfRevolution::StartAngle() const
{
  return theStartAngle;
}

double IGESGeom_SurfaceOfRevolution::EndAngle() const
{
  return theEndAngle;
}
