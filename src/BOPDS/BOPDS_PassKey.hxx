// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPDS_PassKey_HeaderFile
#define _BOPDS_PassKey_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPCol_BaseAllocator.hxx>
#include <Standard_Integer.hxx>
#include <BOPCol_PInteger.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <Standard_Boolean.hxx>



//! The class BOPDS_PassKey is to provide
//! possibility to map objects that
//! have a set of integer IDs as a base
class BOPDS_PassKey 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty contructor
    BOPDS_PassKey();
virtual ~BOPDS_PassKey();
  

  //! Contructor
  //! theAllocator - the allocator to manage the memory
    BOPDS_PassKey(const BOPCol_BaseAllocator& theAllocator);
  

  //! Copy Contructor
    BOPDS_PassKey(const BOPDS_PassKey& Other);
BOPDS_PassKey& operator =(const BOPDS_PassKey& Other);
  

  //! Clear the contents
    void Clear();
  

  //! Modifier
  //! Sets one Id <theI1>
    void SetIds (const Standard_Integer theI1);
  

  //! Modifier
  //! Sets two Id <theI1>,<theI2>
    void SetIds (const Standard_Integer theI1, const Standard_Integer theI2);
  

  //! Modifier
  //! Sets three Id <theI1>,<theI2>,<theI3>
    void SetIds (const Standard_Integer theI1, const Standard_Integer theI2, const Standard_Integer theI3);
  

  //! Modifier
  //! Sets four Id <theI1>,<theI2>,<theI3>,<theI4>
    void SetIds (const Standard_Integer theI1, const Standard_Integer theI2, const Standard_Integer theI3, const Standard_Integer theI4);
  

  //! Modifier
  //! Sets the list of Id <theLI>
  Standard_EXPORT void SetIds (const BOPCol_ListOfInteger& theLI);
  

  //! Selector
  //! Returns the number of Ids>
    Standard_Integer NbIds() const;
  

  //! Query
  //! Returns true if the PassKey is equal to <the theOther>
    Standard_Boolean IsEqual (const BOPDS_PassKey& theOther) const;
  

  //! Query
  //! Returns hash  code
    Standard_Integer HashCode (const Standard_Integer theUpper) const;
  

  //! Selector
  //! Returns Id of index <theIndex>
    Standard_Integer Id (const Standard_Integer theIndex) const;
  

  //! Selector
  //! Returns the first two Ids <theI1>,<theI2>
    void Ids (Standard_Integer& theI1, Standard_Integer& theI2) const;
  
  Standard_EXPORT void Dump (const Standard_Integer aHex = 0) const;




protected:

  
    BOPCol_PInteger Allocate (const Standard_Integer theSize);


  BOPCol_BaseAllocator myAllocator;
  Standard_Integer myNbIds;
  Standard_Integer mySum;
  BOPCol_PInteger myPtr;


private:





};


#include <BOPDS_PassKey.lxx>





#endif // _BOPDS_PassKey_HeaderFile
