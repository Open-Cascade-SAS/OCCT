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


#ifndef _StdLPersistent_NamedData_HeaderFile
#define _StdLPersistent_NamedData_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <StdLPersistent_HArray2.hxx>

#include <TDataStd_NamedData.hxx>
#include <TCollection_HExtendedString.hxx>

class StdLPersistent_NamedData : public StdObjMgt_Attribute<TDataStd_NamedData>
{
  template <class HValuesArray>
  class element
  {
  public:
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myKeys >> myValues; }

    inline operator bool() const
      { return ! myKeys.IsNull(); }

    template <class Value>
    inline const TCollection_ExtendedString& Get
      (Standard_Integer theIndex, Value& theValue) const
    {
      if (myValues)
        theValue = myValues->Array()->Value(theIndex);
      return myKeys->Array()->Value(theIndex)->ExtString()->String();
    }

  private:
    Reference <StdLPersistent_HArray1::Persistent> myKeys;
    Reference <HValuesArray>                       myValues;
  };

public:
  //! Read persistent data from a file.
  inline void Read (StdObjMgt_ReadData& theReadData)
  {
    theReadData >> myDimensions >>
      myInts >> myReals >> myStrings >> myBytes >> myIntArrays >> myRealArrays;
  }

  //! Import transient attribuite from the persistent data.
  void Import (const Handle(TDataStd_NamedData)& theAttribute) const;

private:
  inline Standard_Integer lower (Standard_Integer theIndex) const;
  inline Standard_Integer upper (Standard_Integer theIndex) const;

private:
  Reference <StdLPersistent_HArray2::Integer>           myDimensions;
  Object <element<StdLPersistent_HArray1::Integer>    > myInts;
  Object <element<StdLPersistent_HArray1::Real>       > myReals;
  Object <element<StdLPersistent_HArray1::Persistent> > myStrings;
  Object <element<StdLPersistent_HArray1::Byte>       > myBytes;
  Object <element<StdLPersistent_HArray1::Persistent> > myIntArrays;
  Object <element<StdLPersistent_HArray1::Persistent> > myRealArrays;
};

#endif
