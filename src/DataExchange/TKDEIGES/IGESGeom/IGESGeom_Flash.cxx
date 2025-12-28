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
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESGeom_Flash.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_Flash, IGESData_IGESEntity)

IGESGeom_Flash::IGESGeom_Flash() {}

void IGESGeom_Flash::Init(const gp_XY&                       aPoint,
                          const double                aDim1,
                          const double                aDim2,
                          const double                aRotation,
                          const occ::handle<IGESData_IGESEntity>& aReference)
{
  thePoint     = aPoint;
  theDim1      = aDim1;
  theDim2      = aDim2;
  theRotation  = aRotation;
  theReference = aReference;
  InitTypeAndForm(125, FormNumber());
  // FormNumber : 0-4, Shape of the Flash
}

void IGESGeom_Flash::SetFormNumber(const int form)
{
  if (form < 0 || form > 4)
    throw Standard_OutOfRange("IGESGeom_Flash : SetFormNumber");
  InitTypeAndForm(125, form);
}

gp_Pnt2d IGESGeom_Flash::ReferencePoint() const
{
  return (gp_Pnt2d(thePoint));
}

gp_Pnt IGESGeom_Flash::TransformedReferencePoint() const
{
  gp_XYZ Point(thePoint.X(), thePoint.Y(), 0.0);
  if (HasTransf())
    Location().Transforms(Point);
  return gp_Pnt(Point);
}

double IGESGeom_Flash::Dimension1() const
{
  return theDim1;
}

double IGESGeom_Flash::Dimension2() const
{
  return theDim2;
}

double IGESGeom_Flash::Rotation() const
{
  return theRotation;
}

bool IGESGeom_Flash::HasReferenceEntity() const
{
  return (!theReference.IsNull());
}

occ::handle<IGESData_IGESEntity> IGESGeom_Flash::ReferenceEntity() const
{
  return theReference;
}
