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

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_AttributeTable.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_AttributeTable,IGESData_IGESEntity)

//  ATTENTION  ATTENTION : L Appellation "ROW" n est pas reconduite en l etat
//  Le Numero d Attribut est donne en 1er (donc, en colonne du HArray2 et non
//  en ligne), le numero de Colonne en 2e (donc, comme un numero de Ligne)
IGESDefs_AttributeTable::IGESDefs_AttributeTable ()    {  }


    void  IGESDefs_AttributeTable::Init
  (const Handle(TColStd_HArray2OfTransient)& attributes)
{
  if (attributes->LowerCol() != 1 || attributes->LowerRow() != 1)
    throw Standard_DimensionMismatch("IGESDefs_AttributeTable : Init");
  theAttributes = attributes;

  Standard_Integer fn = FormNumber();
  if (attributes->UpperCol() > 1) fn = 1;
  else if (fn < 0 || fn > 1) fn = 0;
  InitTypeAndForm(422,fn);
//  FormNumber : 0 SingleRow, 1 MultipleRows (can be reduced to one ...)
}

    void   IGESDefs_AttributeTable::SetDefinition
  (const Handle(IGESDefs_AttributeDef)& def)
{
  InitMisc (def,LabelDisplay(),LineWeightNumber());
}

    Handle(IGESDefs_AttributeDef)  IGESDefs_AttributeTable::Definition () const
{
  return GetCasted(IGESDefs_AttributeDef,Structure());
}


    Standard_Integer  IGESDefs_AttributeTable::NbRows () const 
{
  return theAttributes->UpperCol();
}

    Standard_Integer  IGESDefs_AttributeTable::NbAttributes () const 
{
  return theAttributes->UpperRow();
}

    Standard_Integer  IGESDefs_AttributeTable::DataType
  (const Standard_Integer Atnum) const 
{
  return Definition()->AttributeType(Atnum);
}

    Standard_Integer  IGESDefs_AttributeTable::ValueCount
  (const Standard_Integer Atnum) const 
{
  return Definition()->AttributeValueCount(Atnum);
}

    Handle(Standard_Transient)  IGESDefs_AttributeTable::AttributeList
  (const Standard_Integer Atnum, const Standard_Integer Rownum) const
{
  return theAttributes->Value(Atnum,Rownum);
}

    Standard_Integer  IGESDefs_AttributeTable::AttributeAsInteger
  (const Standard_Integer Atnum, const Standard_Integer Rownum,
   const Standard_Integer Valuenum) const
{
  return GetCasted(TColStd_HArray1OfInteger,theAttributes->Value(Atnum,Rownum))
    ->Value(Valuenum);
}

    Standard_Real  IGESDefs_AttributeTable::AttributeAsReal
  (const Standard_Integer Atnum, const Standard_Integer Rownum,
   const Standard_Integer Valuenum) const
{
  return GetCasted(TColStd_HArray1OfReal,theAttributes->Value(Atnum,Rownum))
    ->Value(Valuenum);
}

    Handle(TCollection_HAsciiString)  IGESDefs_AttributeTable::AttributeAsString
  (const Standard_Integer Atnum, const Standard_Integer Rownum,
   const Standard_Integer Valuenum) const
{
  return GetCasted(Interface_HArray1OfHAsciiString,theAttributes->Value(Atnum,Rownum))
    ->Value(Valuenum);
}

    Handle(IGESData_IGESEntity)  IGESDefs_AttributeTable::AttributeAsEntity
  (const Standard_Integer Atnum, const Standard_Integer Rownum,
   const Standard_Integer Valuenum) const
{
  return GetCasted(IGESData_HArray1OfIGESEntity,theAttributes->Value(Atnum,Rownum))
    ->Value(Valuenum);
}

    Standard_Boolean  IGESDefs_AttributeTable::AttributeAsLogical
  (const Standard_Integer Atnum, const Standard_Integer Rownum,
   const Standard_Integer Valuenum) const
{
  return (AttributeAsInteger(Atnum,Rownum,Valuenum) != 0);    // raccourci
}
