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
