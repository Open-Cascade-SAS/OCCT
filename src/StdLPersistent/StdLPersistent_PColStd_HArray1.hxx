// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _StdLPersistent_PColStd_HArray1_HeaderFile
#define _StdLPersistent_PColStd_HArray1_HeaderFile

#include <StdLPersistent_PColStd_HArray1Base.hxx>

#include <StdObjMgt_ReadData.hxx>

#include <NCollection_Handle.hxx>
#include <NCollection_Array1.hxx>

#include <Standard_TypeDef.hxx>

class StdObjMgt_Persistent;


template <class Type>
class StdLPersistent_PColStd_HArray1 : public StdLPersistent_PColStd_HArray1Base
{
public:
  //! Get the value.
  const NCollection_Array1<Type>& Array() const { return *myArray; }

protected:
  virtual void CreateArray (const Standard_Integer theLowerBound,
                            const Standard_Integer theUpperBound)
  {
    myArray = new NCollection_Array1<Type> (theLowerBound, theUpperBound);
  }

  virtual void ReadValue (StdObjMgt_ReadData& theReadData,
                          const Standard_Integer theIndex)
  {
    Type aValue;
    theReadData >> aValue;
    myArray->SetValue (theIndex, aValue);
  }

private:
  NCollection_Handle<NCollection_Array1<Type>> myArray;
};

typedef StdLPersistent_PColStd_HArray1<Standard_Integer>
        StdLPersistent_PColStd_HArray1OfInteger;

typedef StdLPersistent_PColStd_HArray1<Handle(StdObjMgt_Persistent)>
        StdLPersistent_PDF_HAttributeArray1;

#endif
