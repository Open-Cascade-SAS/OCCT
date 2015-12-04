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

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_DimensionTolerance,IGESData_IGESEntity)

IGESDimen_DimensionTolerance::IGESDimen_DimensionTolerance ()    {  }


    void  IGESDimen_DimensionTolerance::Init
  (const Standard_Integer nbPropVal,
   const Standard_Integer aSecTolFlag,   const Standard_Integer aTolType,
   const Standard_Integer aTolPlaceFlag, const Standard_Real    anUpperTol,
   const Standard_Real    aLowerTol,     const Standard_Boolean aSignFlag,
   const Standard_Integer aFracFlag,     const Standard_Integer aPrecision)
{
  theNbPropertyValues = nbPropVal;
  theSecondaryToleranceFlag = aSecTolFlag;
  theToleranceType          = aTolType;
  theTolerancePlacementFlag = aTolPlaceFlag;
  theUpperTolerance         = anUpperTol;
  theLowerTolerance         = aLowerTol;
  theSignSuppressionFlag    = aSignFlag;
  theFractionFlag           = aFracFlag;
  thePrecision              = aPrecision;
  InitTypeAndForm(406,29);
}


    Standard_Integer  IGESDimen_DimensionTolerance::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer  IGESDimen_DimensionTolerance::SecondaryToleranceFlag () const
{
  return theSecondaryToleranceFlag;
}

    Standard_Integer  IGESDimen_DimensionTolerance::ToleranceType () const
{
  return theToleranceType;
}

    Standard_Integer  IGESDimen_DimensionTolerance::TolerancePlacementFlag () const
{
  return theTolerancePlacementFlag;
}

    Standard_Real  IGESDimen_DimensionTolerance::UpperTolerance () const
{
  return theUpperTolerance;
}

    Standard_Real  IGESDimen_DimensionTolerance::LowerTolerance () const
{
  return theLowerTolerance;
}

    Standard_Boolean  IGESDimen_DimensionTolerance::SignSuppressionFlag () const
{
  return theSignSuppressionFlag;
}

    Standard_Integer  IGESDimen_DimensionTolerance::FractionFlag () const
{
  return theFractionFlag;
}

    Standard_Integer  IGESDimen_DimensionTolerance::Precision () const
{
  return thePrecision;
}
