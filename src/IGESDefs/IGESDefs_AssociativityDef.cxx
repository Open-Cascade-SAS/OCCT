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

#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>
#include <IGESDefs_AssociativityDef.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_AssociativityDef,IGESData_IGESEntity)

IGESDefs_AssociativityDef::IGESDefs_AssociativityDef ()    {  }


    void  IGESDefs_AssociativityDef::Init
  (const Handle(TColStd_HArray1OfInteger)& requirements,
   const Handle(TColStd_HArray1OfInteger)& orders,
   const Handle(TColStd_HArray1OfInteger)& numItems,
   const Handle(IGESBasic_HArray1OfHArray1OfInteger)& items)
{
  Standard_Integer len = requirements->Length();
  if ( requirements->Lower() != 1 ||
      (orders->Lower()       != 1 || orders->Length()   != len) ||
      (numItems->Lower()     != 1 || numItems->Length() != len) ||
      (items->Lower()        != 1 || items->Length()    != len) )
    throw Standard_DimensionMismatch("IGESDefs_AssociativityDef : Init");

  theBackPointerReqs = requirements;
  theClassOrders     = orders;
  theNbItemsPerClass = numItems;
  theItems           = items;
  InitTypeAndForm(302,FormNumber());
//  FormNumber is free over 5000
}

    void IGESDefs_AssociativityDef::SetFormNumber (const Standard_Integer form)
{
  InitTypeAndForm(302,form);
}

    Standard_Integer  IGESDefs_AssociativityDef::NbClassDefs () const 
{
  return theBackPointerReqs->Length();
}

    Standard_Boolean  IGESDefs_AssociativityDef::IsBackPointerReq
  (const Standard_Integer ClassNum) const 
{
  return (theBackPointerReqs->Value(ClassNum) == 1);
//  1 True  2 False
}

    Standard_Integer  IGESDefs_AssociativityDef::BackPointerReq
  (const Standard_Integer ClassNum) const 
{
  return theBackPointerReqs->Value(ClassNum);
}

    Standard_Boolean  IGESDefs_AssociativityDef::IsOrdered
  (const Standard_Integer ClassNum) const 
{
  return (theClassOrders->Value(ClassNum) == 1);
//  1 True  2 False
}

    Standard_Integer  IGESDefs_AssociativityDef::ClassOrder
  (const Standard_Integer ClassNum) const 
{
  return theClassOrders->Value(ClassNum);
}

    Standard_Integer  IGESDefs_AssociativityDef::NbItemsPerClass
  (const Standard_Integer ClassNum) const 
{
  return theNbItemsPerClass->Value(ClassNum);
}

    Standard_Integer  IGESDefs_AssociativityDef::Item
  (const Standard_Integer ClassNum, const Standard_Integer ItemNum) const 
{
  return theItems->Value(ClassNum)->Value(ItemNum);
}
