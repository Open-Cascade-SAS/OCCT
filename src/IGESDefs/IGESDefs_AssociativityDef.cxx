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

#include <IGESDefs_AssociativityDef.ixx>


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
    Standard_DimensionMismatch::Raise("IGESDefs_AssociativityDef : Init");

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
