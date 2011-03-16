//--------------------------------------------------------------------
//
//  File Name : IGESDimen_DimensionTolerance.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
