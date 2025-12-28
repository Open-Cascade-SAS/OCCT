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

#include <IGESDimen_DimensionTolerance.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_DimensionTolerance, IGESData_IGESEntity)

IGESDimen_DimensionTolerance::IGESDimen_DimensionTolerance() {}

void IGESDimen_DimensionTolerance::Init(const int    nbPropVal,
                                        const int    aSecTolFlag,
                                        const int    aTolType,
                                        const int    aTolPlaceFlag,
                                        const double anUpperTol,
                                        const double aLowerTol,
                                        const bool   aSignFlag,
                                        const int    aFracFlag,
                                        const int    aPrecision)
{
  theNbPropertyValues       = nbPropVal;
  theSecondaryToleranceFlag = aSecTolFlag;
  theToleranceType          = aTolType;
  theTolerancePlacementFlag = aTolPlaceFlag;
  theUpperTolerance         = anUpperTol;
  theLowerTolerance         = aLowerTol;
  theSignSuppressionFlag    = aSignFlag;
  theFractionFlag           = aFracFlag;
  thePrecision              = aPrecision;
  InitTypeAndForm(406, 29);
}

int IGESDimen_DimensionTolerance::NbPropertyValues() const
{
  return theNbPropertyValues;
}

int IGESDimen_DimensionTolerance::SecondaryToleranceFlag() const
{
  return theSecondaryToleranceFlag;
}

int IGESDimen_DimensionTolerance::ToleranceType() const
{
  return theToleranceType;
}

int IGESDimen_DimensionTolerance::TolerancePlacementFlag() const
{
  return theTolerancePlacementFlag;
}

double IGESDimen_DimensionTolerance::UpperTolerance() const
{
  return theUpperTolerance;
}

double IGESDimen_DimensionTolerance::LowerTolerance() const
{
  return theLowerTolerance;
}

bool IGESDimen_DimensionTolerance::SignSuppressionFlag() const
{
  return theSignSuppressionFlag;
}

int IGESDimen_DimensionTolerance::FractionFlag() const
{
  return theFractionFlag;
}

int IGESDimen_DimensionTolerance::Precision() const
{
  return thePrecision;
}
