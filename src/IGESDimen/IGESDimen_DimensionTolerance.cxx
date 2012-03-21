// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDimen_DimensionTolerance.ixx>

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
