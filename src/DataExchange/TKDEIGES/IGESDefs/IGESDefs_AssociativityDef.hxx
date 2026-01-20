// Created on: 1993-01-13
// Created by: CKY / Contract Toubro-Larsen ( Deepak PRABHU )
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

#ifndef _IGESDefs_AssociativityDef_HeaderFile
#define _IGESDefs_AssociativityDef_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
class IGESBasic_HArray1OfHArray1OfInteger;

//! defines IGES Associativity Definition Entity, Type <302>
//! Form <5001 - 9999> in package IGESDefs.
//! This class permits the preprocessor to define an
//! associativity schema. i.e., by using it preprocessor
//! defines the type of relationship.
class IGESDefs_AssociativityDef : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESDefs_AssociativityDef();

  //! This method is used to set the fields of the class
  //! AssociativityDef
  //! - requirements : Back Pointers requirements
  //! - orders       : Class Orders
  //! - numItems     : Number of Items per Class
  //! - items        : Items in each class
  //! raises exception if lengths of the arrays are not the same.
  Standard_EXPORT void Init(const occ::handle<NCollection_HArray1<int>>&            requirements,
                            const occ::handle<NCollection_HArray1<int>>&            orders,
                            const occ::handle<NCollection_HArray1<int>>&            numItems,
                            const occ::handle<IGESBasic_HArray1OfHArray1OfInteger>& items);

  Standard_EXPORT void SetFormNumber(const int form);

  //! returns the Number of class definitions
  Standard_EXPORT int NbClassDefs() const;

  //! returns 1 if the theBackPointerReqs(ClassNum) = 1
  //! returns 0 if the theBackPointerReqs(ClassNum) = 2
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT bool IsBackPointerReq(const int ClassNum) const;

  //! returns 1 or 2
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT int BackPointerReq(const int ClassNum) const;

  //! returns 1 if theClassOrders(ClassNum) = 1 (ordered class)
  //! returns 0 if theClassOrders(ClassNum) = 2 (unordered class)
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT bool IsOrdered(const int ClassNum) const;

  //! returns 1 or 2
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT int ClassOrder(const int ClassNum) const;

  //! returns no. of items per class entry
  //! raises exception if ClassNum <= 0 or ClassNum > NbClassDefs()
  Standard_EXPORT int NbItemsPerClass(const int ClassNum) const;

  //! returns ItemNum'th Item of ClassNum'th Class
  //! raises exception if
  //! ClassNum <= 0 or ClassNum > NbClassDefs()
  //! ItemNum <= 0 or ItemNum > NbItemsPerClass(ClassNum)
  Standard_EXPORT int Item(const int ClassNum,
                                        const int ItemNum) const;

  DEFINE_STANDARD_RTTIEXT(IGESDefs_AssociativityDef, IGESData_IGESEntity)

private:
  occ::handle<NCollection_HArray1<int>>            theBackPointerReqs;
  occ::handle<NCollection_HArray1<int>>            theClassOrders;
  occ::handle<NCollection_HArray1<int>>            theNbItemsPerClass;
  occ::handle<IGESBasic_HArray1OfHArray1OfInteger> theItems;
};

#endif // _IGESDefs_AssociativityDef_HeaderFile
