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
  class pMapData
  {
  public:
    typedef typename HValuesArray::ValueType ValueType;

    inline void Read(StdObjMgt_ReadData& theReadData) { theReadData >> myKeys >> myValues; }

    inline void Write(StdObjMgt_WriteData& theWriteData) const
    {
      theWriteData << myKeys << myValues;
    }

    inline operator bool() const { return !myKeys.IsNull(); }

    const TCollection_ExtendedString& Key(Standard_Integer theIndex) const
    {
      return myKeys->Array()->Value(theIndex)->ExtString()->String();
    }

    ValueType Value(Standard_Integer theIndex) const
    {
      return myValues ? myValues->Array()->Value(theIndex) : 0;
    }

  private:
    Handle(StdLPersistent_HArray1::Persistent) myKeys;
    Handle(HValuesArray)                       myValues;
  };

public:
  //! Read persistent data from a file.
  inline void Read(StdObjMgt_ReadData& theReadData)
  {
    theReadData >> myDimensions;
    myInts.Read(theReadData);
    myReals.Read(theReadData);
    myStrings.Read(theReadData);
    myBytes.Read(theReadData);
    myIntArrays.Read(theReadData);
    myRealArrays.Read(theReadData);
  }

  //! Write persistent data to a file.
  inline void Write(StdObjMgt_WriteData& theWriteData) const
  {
    theWriteData << myDimensions;
    myInts.Write(theWriteData);
    myReals.Write(theWriteData);
    myStrings.Write(theWriteData);
    myBytes.Write(theWriteData);
    myIntArrays.Write(theWriteData);
    myRealArrays.Write(theWriteData);
  }

  //! Gets persistent child objects
  void PChildren(StdObjMgt_Persistent::SequenceOfPersistent&) const {}

  //! Returns persistent type name
  Standard_CString PName() const { return "PDataStd_NamedData"; }

  //! Import transient attribute from the persistent data.
  void Import(const Handle(TDataStd_NamedData)& theAttribute) const;

private:
  inline Standard_Integer lower(Standard_Integer theIndex) const;
  inline Standard_Integer upper(Standard_Integer theIndex) const;

private:
  Handle(StdLPersistent_HArray2::Integer)      myDimensions;
  pMapData<StdLPersistent_HArray1::Integer>    myInts;
  pMapData<StdLPersistent_HArray1::Real>       myReals;
  pMapData<StdLPersistent_HArray1::Persistent> myStrings;
  pMapData<StdLPersistent_HArray1::Byte>       myBytes;
  pMapData<StdLPersistent_HArray1::Persistent> myIntArrays;
  pMapData<StdLPersistent_HArray1::Persistent> myRealArrays;
};

#endif
