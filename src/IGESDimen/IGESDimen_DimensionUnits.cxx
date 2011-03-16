//--------------------------------------------------------------------
//
//  File Name : IGESDimen_DimensionUnits.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDimen_DimensionUnits.ixx>


    IGESDimen_DimensionUnits::IGESDimen_DimensionUnits ()    {  }

    void  IGESDimen_DimensionUnits::Init
  (const Standard_Integer nbPropVal,
   const Standard_Integer aSecondPos, const Standard_Integer aUnitsInd,
   const Standard_Integer aCharSet,
   const Handle(TCollection_HAsciiString)& aFormat,
   const Standard_Integer aFracFlag,  const Standard_Integer aPrecision)
{
  theNbPropertyValues = nbPropVal;
  theSecondaryDimenPosition = aSecondPos;
  theUnitsIndicator         = aUnitsInd;
  theCharacterSet           = aCharSet;
  theFormatString           = aFormat;
  theFractionFlag           = aFracFlag;
  thePrecision              = aPrecision;
  InitTypeAndForm(406,28);
}


    Standard_Integer  IGESDimen_DimensionUnits::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer  IGESDimen_DimensionUnits::SecondaryDimenPosition () const
{
  return theSecondaryDimenPosition;
}

    Standard_Integer  IGESDimen_DimensionUnits::UnitsIndicator () const
{
  return theUnitsIndicator;
}

    Standard_Integer  IGESDimen_DimensionUnits::CharacterSet () const
{
  return theCharacterSet;
}

    Handle(TCollection_HAsciiString)  IGESDimen_DimensionUnits::FormatString () const
{
  return theFormatString;
}

    Standard_Integer  IGESDimen_DimensionUnits::FractionFlag () const
{
  return theFractionFlag;
}

    Standard_Integer  IGESDimen_DimensionUnits::PrecisionOrDenominator () const
{
  return thePrecision;
}
