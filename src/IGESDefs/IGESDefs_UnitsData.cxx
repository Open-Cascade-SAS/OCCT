//--------------------------------------------------------------------
//
//  File Name : IGESDefs_UnitsData.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESDefs_UnitsData.ixx>


    IGESDefs_UnitsData::IGESDefs_UnitsData ()    {  }


    void  IGESDefs_UnitsData::Init
  (const Handle(Interface_HArray1OfHAsciiString)& unitTypes,
   const Handle(Interface_HArray1OfHAsciiString)& unitValues,
   const Handle(TColStd_HArray1OfReal)& unitScales)
{
  Standard_Integer length = unitTypes->Length();
  if ( unitTypes->Lower()  != 1  ||
      (unitValues->Lower() != 1 || unitValues->Length() != length) ||
      (unitScales->Lower() != 1 || unitScales->Length() != length) )
    Standard_DimensionMismatch::Raise("IGESDefs_UnitsData : Init");
  theUnitTypes   = unitTypes;
  theUnitValues  = unitValues;
  theUnitScales  = unitScales;
  InitTypeAndForm(316,0);
}

    Standard_Integer  IGESDefs_UnitsData::NbUnits () const 
{
  return theUnitTypes->Length();
}

    Handle(TCollection_HAsciiString)  IGESDefs_UnitsData::UnitType
  (const Standard_Integer UnitNum) const 
{
  return theUnitTypes->Value(UnitNum);
}

    Handle(TCollection_HAsciiString)  IGESDefs_UnitsData::UnitValue
  (const Standard_Integer UnitNum) const 
{
  return theUnitValues->Value(UnitNum);
}

    Standard_Real  IGESDefs_UnitsData::ScaleFactor
  (const Standard_Integer UnitNum) const 
{
  return theUnitScales->Value(UnitNum);
}
