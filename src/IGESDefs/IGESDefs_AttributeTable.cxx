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

#include <IGESDefs_AttributeTable.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Macros.hxx>


//  ATTENTION  ATTENTION : L Appellation "ROW" n est pas reconduite en l etat
//  Le Numero d Attribut est donne en 1er (donc, en colonne du HArray2 et non
//  en ligne), le numero de Colonne en 2e (donc, comme un numero de Ligne)


IGESDefs_AttributeTable::IGESDefs_AttributeTable ()    {  }


    void  IGESDefs_AttributeTable::Init
  (const Handle(TColStd_HArray2OfTransient)& attributes)
{
  if (attributes->LowerCol() != 1 || attributes->LowerRow() != 1)
    Standard_DimensionMismatch::Raise("IGESDefs_AttributeTable : Init");
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
