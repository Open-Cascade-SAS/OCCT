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

#ifndef _StdLPersistent_HArray1_HeaderFile
#define _StdLPersistent_HArray1_HeaderFile

#include <Standard_NotImplemented.hxx>
#include <Standard_NullValue.hxx>

#include <StdObjMgt_Persistent.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfByte.hxx>

DEFINE_HARRAY1(StdLPersistent_HArray1OfPersistent, NCollection_Array1<Handle(StdObjMgt_Persistent)>)

class StdLPersistent_HArray1
{
  class base : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read(StdObjMgt_ReadData& theReadData);
    //! Write persistent data to a file.
    Standard_EXPORT virtual void Write(StdObjMgt_WriteData& theWriteData) const;

  protected:
    virtual Standard_Integer lowerBound() const                                = 0;
    virtual Standard_Integer upperBound() const                                = 0;
    virtual void             createArray(const Standard_Integer theLowerBound,
                                         const Standard_Integer theUpperBound) = 0;

    virtual void readValue(StdObjMgt_ReadData& theReadData, const Standard_Integer theIndex) = 0;
    virtual void writeValue(StdObjMgt_WriteData&   theWriteData,
                            const Standard_Integer theIndex) const                           = 0;
  };

protected:
  template <class ArrayClass>
  class instance : public base
  {
    friend class StdLPersistent_HArray1;

  public:
    typedef Handle(ArrayClass)              ArrayHandle;
    typedef typename ArrayClass::value_type ValueType;
    typedef typename ArrayClass::Iterator   Iterator;

  public:
    //! Get the array.
    const Handle(ArrayClass)& Array() const { return myArray; }

  protected:
    virtual Standard_Integer lowerBound() const { return myArray->Lower(); }

    virtual Standard_Integer upperBound() const { return myArray->Upper(); }

    virtual void createArray(const Standard_Integer theLowerBound,
                             const Standard_Integer theUpperBound)
    {
      myArray = new ArrayClass(theLowerBound, theUpperBound);
    }

    virtual void readValue(StdObjMgt_ReadData& theReadData, const Standard_Integer theIndex)
    {
      theReadData >> myArray->ChangeValue(theIndex);
    }

    virtual void writeValue(StdObjMgt_WriteData&   theWriteData,
                            const Standard_Integer theIndex) const
    {
      theWriteData << myArray->Value(theIndex);
    }

    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      return PChildrenT(theChildren);
    }

    virtual Standard_CString PName() const { return PNameT(); }

    Standard_CString PNameT() const
    {
      Standard_NotImplemented::Raise("StdLPersistent_HArray1::instance::PName - not implemented");
      return "";
    }

    void PChildrenT(StdObjMgt_Persistent::SequenceOfPersistent&) const {}

  protected:
    Handle(ArrayClass) myArray;
  };

  template <class ArrayClass>
  class named_instance : public instance<ArrayClass>
  {
    friend class StdLPersistent_HArray1;

  public:
    virtual Standard_CString PName() const
    {
      Standard_NullValue_Raise_if(!myPName,
                                  "StdLPersistent_HArray1::named_instance::PName - name not set");
      return myPName;
    }

  protected:
    named_instance(Standard_CString thePName)
        : myPName(thePName)
    {
    }

    Standard_CString myPName;
  };

public:
  typedef instance<TColStd_HArray1OfInteger>           Integer;
  typedef instance<TColStd_HArray1OfReal>              Real;
  typedef instance<TColStd_HArray1OfByte>              Byte;
  typedef instance<StdLPersistent_HArray1OfPersistent> Persistent;

public:
  template <class ArrayClass>
  static Handle(instance<ArrayClass>) Translate(const ArrayClass& theArray)
  {
    Handle(instance<ArrayClass>) aPArray = new instance<ArrayClass>;
    aPArray->myArray                     = new ArrayClass(theArray.Lower(), theArray.Upper());
    for (Standard_Integer i = theArray.Lower(); i <= theArray.Upper(); ++i)
      aPArray->myArray->ChangeValue(i) = theArray.Value(i);
    return aPArray;
  }

  template <class ArrayClass>
  static Handle(instance<ArrayClass>) Translate(Standard_CString  thePName,
                                                const ArrayClass& theArray)
  {
    Handle(named_instance<ArrayClass>) aPArray = new named_instance<ArrayClass>(thePName);
    aPArray->myArray                           = new ArrayClass(theArray.Lower(), theArray.Upper());
    for (Standard_Integer i = theArray.Lower(); i <= theArray.Upper(); ++i)
      aPArray->myArray->ChangeValue(i) = theArray.Value(i);
    return aPArray;
  }
};

template <>
inline Standard_CString StdLPersistent_HArray1::instance<TColStd_HArray1OfInteger>::PNameT() const
{
  return "PColStd_HArray1OfInteger";
}

template <>
inline Standard_CString StdLPersistent_HArray1::instance<TColStd_HArray1OfReal>::PNameT() const
{
  return "PColStd_HArray1OfReal";
}

template <>
inline Standard_CString StdLPersistent_HArray1::instance<TColStd_HArray1OfByte>::PNameT() const
{
  return "PColStd_HArray1OfByte";
}

inline StdObjMgt_ReadData& operator>>(StdObjMgt_ReadData& theReadData, Standard_Byte& theByte)
{
  return theReadData >> reinterpret_cast<Standard_Character&>(theByte);
}

inline StdObjMgt_WriteData& operator>>(StdObjMgt_WriteData& theWriteData,
                                       const Standard_Byte& theByte)
{
  return theWriteData << reinterpret_cast<const Standard_Character&>(theByte);
}

template <>
inline void StdLPersistent_HArray1::instance<StdLPersistent_HArray1OfPersistent>::PChildrenT(
  StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  for (Standard_Integer i = myArray->Lower(); i <= myArray->Upper(); ++i)
    theChildren.Append(myArray->Value(i));
}

#endif
