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
// rln 11.05.2000 BUC60660

#include <IGESDefs_GenericData.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_GenericData, IGESData_IGESEntity)

IGESDefs_GenericData::IGESDefs_GenericData() {}

void IGESDefs_GenericData::Init(
  const int                                                                nbPropVal,
  const occ::handle<TCollection_HAsciiString>&                             aName,
  const occ::handle<NCollection_HArray1<int>>&                             allTypes,
  const occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>>& allValues)
{
  // rln May 11, 2000 BUC60660
  // Number of TYPE/VALUE pairs is 0 and arrays are null handles,
  // this caused exception
  if (!allTypes.IsNull() && !allValues.IsNull()
      && (allValues->Lower() != 1 || allTypes->Lower() != 1
          || allTypes->Length() != allValues->Length()))
    throw Standard_DimensionMismatch("IGESDefs_GenericData: Init");
  theNbPropertyValues = nbPropVal;
  theName             = aName;
  theTypes            = allTypes;
  theValues           = allValues;
  InitTypeAndForm(406, 27);
}

int IGESDefs_GenericData::NbPropertyValues() const
{
  //  return 2 * theTypes->Length() + 2;
  return theNbPropertyValues;
}

occ::handle<TCollection_HAsciiString> IGESDefs_GenericData::Name() const
{
  return theName;
}

int IGESDefs_GenericData::NbTypeValuePairs() const
{
  return theTypes->Length();
}

int IGESDefs_GenericData::Type(const int Index) const
{
  return theTypes->Value(Index);
}

occ::handle<Standard_Transient> IGESDefs_GenericData::Value(const int Index) const
{
  return theValues->Value(Index);
}

int IGESDefs_GenericData::ValueAsInteger(const int Index) const
{
  return GetCasted(NCollection_HArray1<int>, theValues->Value(Index))->Value(1);
}

double IGESDefs_GenericData::ValueAsReal(const int Index) const
{
  return GetCasted(NCollection_HArray1<double>, theValues->Value(Index))->Value(1);
}

occ::handle<TCollection_HAsciiString> IGESDefs_GenericData::ValueAsString(const int Index) const
{
  return GetCasted(TCollection_HAsciiString, theValues->Value(Index));
}

occ::handle<IGESData_IGESEntity> IGESDefs_GenericData::ValueAsEntity(const int Index) const
{
  return GetCasted(IGESData_IGESEntity, theValues->Value(Index));
}

bool IGESDefs_GenericData::ValueAsLogical(const int Index) const
{
  return (GetCasted(NCollection_HArray1<int>, theValues->Value(Index))->Value(1) != 0);
}
