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

#include <IGESAppli_LevelToPWBLayerMap.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_LevelToPWBLayerMap, IGESData_IGESEntity)

IGESAppli_LevelToPWBLayerMap::IGESAppli_LevelToPWBLayerMap() = default;

void IGESAppli_LevelToPWBLayerMap::Init(
  const int                                                                      nbPropVal,
  const occ::handle<NCollection_HArray1<int>>&                                   allExchLevels,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allNativeLevels,
  const occ::handle<NCollection_HArray1<int>>&                                   allPhysLevels,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allExchIdents)
{
  int num = allExchLevels->Length();
  if (allExchLevels->Lower() != 1
      || (allNativeLevels->Lower() != 1 || allNativeLevels->Length() != num)
      || (allPhysLevels->Lower() != 1 || allPhysLevels->Length() != num)
      || (allExchIdents->Lower() != 1 || allExchIdents->Length() != num))
    throw Standard_DimensionMismatch("IGESAppli_LevelToPWBLayerMap: Init");
  theNbPropertyValues        = nbPropVal;
  theExchangeFileLevelNumber = allExchLevels;
  theNativeLevel             = allNativeLevels;
  thePhysicalLayerNumber     = allPhysLevels;
  theExchangeFileLevelIdent  = allExchIdents;
  InitTypeAndForm(406, 24);
}

int IGESAppli_LevelToPWBLayerMap::NbPropertyValues() const
{
  return theNbPropertyValues;
}

int IGESAppli_LevelToPWBLayerMap::NbLevelToLayerDefs() const
{
  return theExchangeFileLevelNumber->Length();
}

int IGESAppli_LevelToPWBLayerMap::ExchangeFileLevelNumber(const int Index) const
{
  return theExchangeFileLevelNumber->Value(Index);
}

occ::handle<TCollection_HAsciiString> IGESAppli_LevelToPWBLayerMap::NativeLevel(
  const int Index) const
{
  return theNativeLevel->Value(Index);
}

int IGESAppli_LevelToPWBLayerMap::PhysicalLayerNumber(const int Index) const
{
  return thePhysicalLayerNumber->Value(Index);
}

occ::handle<TCollection_HAsciiString> IGESAppli_LevelToPWBLayerMap::ExchangeFileLevelIdent(
  const int Index) const
{
  return theExchangeFileLevelIdent->Value(Index);
}
