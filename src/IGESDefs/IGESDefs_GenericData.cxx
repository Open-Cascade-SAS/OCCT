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

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDefs_GenericData.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_GenericData,IGESData_IGESEntity)

IGESDefs_GenericData::IGESDefs_GenericData ()    {  }


    void  IGESDefs_GenericData::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& aName,
   const Handle(TColStd_HArray1OfInteger)& allTypes,
   const Handle(TColStd_HArray1OfTransient)& allValues)
{
  // rln May 11, 2000 BUC60660
  // Number of TYPE/VALUE pairs is 0 and arrays are null handles,
  // this caused exception
  if ( !allTypes.IsNull() && !allValues.IsNull() &&
      (allValues->Lower() != 1 || allTypes->Lower() != 1 ||
       allTypes->Length() != allValues->Length() ) )
    throw Standard_DimensionMismatch("IGESDefs_GenericData: Init");
  theNbPropertyValues = nbPropVal;
  theName   = aName;
  theTypes  = allTypes;
  theValues = allValues;
  InitTypeAndForm(406,27);
}

    Standard_Integer  IGESDefs_GenericData::NbPropertyValues () const
{
//  return 2 * theTypes->Length() + 2;
    return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)  IGESDefs_GenericData::Name () const
{
  return theName;
}

    Standard_Integer  IGESDefs_GenericData::NbTypeValuePairs () const
{
  return theTypes->Length();
}

    Standard_Integer  IGESDefs_GenericData::Type (const Standard_Integer Index) const
{
  return theTypes->Value(Index);
}

    Handle(Standard_Transient)  IGESDefs_GenericData::Value
  (const Standard_Integer Index) const
{
  return theValues->Value(Index);
}

    Standard_Integer  IGESDefs_GenericData::ValueAsInteger
  (const Standard_Integer Index) const
{
  return GetCasted(TColStd_HArray1OfInteger,theValues->Value(Index))->Value(1);
}

    Standard_Real  IGESDefs_GenericData::ValueAsReal
  (const Standard_Integer Index) const
{
  return GetCasted(TColStd_HArray1OfReal,theValues->Value(Index))->Value(1);
}

    Handle(TCollection_HAsciiString)  IGESDefs_GenericData::ValueAsString
  (const Standard_Integer Index) const
{
  return GetCasted(TCollection_HAsciiString,theValues->Value(Index));
}

    Handle(IGESData_IGESEntity)  IGESDefs_GenericData::ValueAsEntity
  (const Standard_Integer Index) const
{
  return GetCasted(IGESData_IGESEntity,theValues->Value(Index));
}

    Standard_Boolean  IGESDefs_GenericData::ValueAsLogical
  (const Standard_Integer Index) const
{
  return (GetCasted(TColStd_HArray1OfInteger,theValues->Value(Index))->Value(1)
	  != 0);
}
