//--------------------------------------------------------------------
//
//  File Name : IGESAppli_LevelToPWBLayerMap.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_LevelToPWBLayerMap.ixx>


    IGESAppli_LevelToPWBLayerMap::IGESAppli_LevelToPWBLayerMap ()    {  }


    void  IGESAppli_LevelToPWBLayerMap::Init
  (const Standard_Integer nbPropVal,
   const Handle(TColStd_HArray1OfInteger)& allExchLevels,
   const Handle(Interface_HArray1OfHAsciiString)& allNativeLevels,
   const Handle(TColStd_HArray1OfInteger)& allPhysLevels,
   const Handle(Interface_HArray1OfHAsciiString)& allExchIdents)
{
  Standard_Integer num = allExchLevels->Length();
  if ( allExchLevels->Lower()   != 1 ||
      (allNativeLevels->Lower() != 1 || allNativeLevels->Length() != num) ||
      (allPhysLevels->Lower()   != 1 || allPhysLevels->Length()   != num) ||
      (allExchIdents->Lower()   != 1 || allExchIdents->Length()   != num) )
    Standard_DimensionMismatch::Raise("IGESAppli_LevelToPWBLayerMap: Init");
  theNbPropertyValues        = nbPropVal;
  theExchangeFileLevelNumber = allExchLevels;
  theNativeLevel             = allNativeLevels;
  thePhysicalLayerNumber     = allPhysLevels;
  theExchangeFileLevelIdent  = allExchIdents;
  InitTypeAndForm(406,24);
}

    Standard_Integer  IGESAppli_LevelToPWBLayerMap::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer  IGESAppli_LevelToPWBLayerMap::NbLevelToLayerDefs () const
{
  return theExchangeFileLevelNumber->Length();
}

    Standard_Integer  IGESAppli_LevelToPWBLayerMap::ExchangeFileLevelNumber
  (const Standard_Integer Index) const
{
  return theExchangeFileLevelNumber->Value(Index);
}

    Handle(TCollection_HAsciiString)  IGESAppli_LevelToPWBLayerMap::NativeLevel
  (const Standard_Integer Index) const
{
  return theNativeLevel->Value(Index);
}

    Standard_Integer  IGESAppli_LevelToPWBLayerMap::PhysicalLayerNumber
  (const Standard_Integer Index) const
{
  return thePhysicalLayerNumber->Value(Index);
}

    Handle(TCollection_HAsciiString)  IGESAppli_LevelToPWBLayerMap::ExchangeFileLevelIdent
  (const Standard_Integer Index) const
{
  return theExchangeFileLevelIdent->Value(Index);
}
