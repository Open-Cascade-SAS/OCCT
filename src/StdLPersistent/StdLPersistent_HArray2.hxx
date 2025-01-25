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

#ifndef _StdLPersistent_HArray2_HeaderFile
#define _StdLPersistent_HArray2_HeaderFile

#include <Standard_NotImplemented.hxx>
#include <Standard_NullValue.hxx>

#include <StdObjMgt_Persistent.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>

#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>

DEFINE_HARRAY2(StdLPersistent_HArray2OfPersistent, NCollection_Array2<Handle(StdObjMgt_Persistent)>)

class StdLPersistent_HArray2
{
  class base : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read(StdObjMgt_ReadData& theReadData);

    //! Read persistent data from a file.
    Standard_EXPORT virtual void Write(StdObjMgt_WriteData& theWriteData) const;

  protected:
    virtual void lowerBound(Standard_Integer& theRow, Standard_Integer& theCol) const = 0;
    virtual void upperBound(Standard_Integer& theRow, Standard_Integer& theCol) const = 0;
    virtual void createArray(const Standard_Integer theLowerRow,
                             const Standard_Integer theLowerCol,
                             const Standard_Integer theUpperRow,
                             const Standard_Integer theUpperCol)                      = 0;

    virtual void readValue(StdObjMgt_ReadData&    theReadData,
                           const Standard_Integer theRow,
                           const Standard_Integer theCol)        = 0;
    virtual void writeValue(StdObjMgt_WriteData&   theWriteData,
                            const Standard_Integer theRow,
                            const Standard_Integer theCol) const = 0;
  };

protected:
  template <class ArrayClass>
  class instance : public base
  {
    friend class StdLPersistent_HArray2;

  public:
    typedef Handle(ArrayClass) ArrayHandle;

  public:
    //! Get the array.
    const Handle(ArrayClass)& Array() const { return myArray; }

  protected:
    virtual void lowerBound(Standard_Integer& theRow, Standard_Integer& theCol) const
    {
      theRow = myArray->LowerRow();
      theCol = myArray->LowerCol();
    }

    virtual void upperBound(Standard_Integer& theRow, Standard_Integer& theCol) const
    {
      theRow = myArray->UpperRow();
      theCol = myArray->UpperCol();
    }

    virtual void createArray(const Standard_Integer theLowerRow,
                             const Standard_Integer theLowerCol,
                             const Standard_Integer theUpperRow,
                             const Standard_Integer theUpperCol)
    {
      myArray = new ArrayClass(theLowerRow, theUpperRow, theLowerCol, theUpperCol);
    }

    virtual void readValue(StdObjMgt_ReadData&    theReadData,
                           const Standard_Integer theRow,
                           const Standard_Integer theCol)
    {
      theReadData >> myArray->ChangeValue(theRow, theCol);
    }

    virtual void writeValue(StdObjMgt_WriteData&   theWriteData,
                            const Standard_Integer theRow,
                            const Standard_Integer theCol) const
    {
      theWriteData << myArray->Value(theRow, theCol);
    }

    virtual void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      return PChildrenT(theChildren);
    }

    virtual Standard_CString PName() const { return PNameT(); }

    Standard_CString PNameT() const
    {
      Standard_NotImplemented::Raise("StdLPersistent_HArray2::instance::PName - not implemented");
      return "";
    }

    void PChildrenT(StdObjMgt_Persistent::SequenceOfPersistent&) const {}

  protected:
    Handle(ArrayClass) myArray;
  };

  template <class ArrayClass>
  class named_instance : public instance<ArrayClass>
  {
    friend class StdLPersistent_HArray2;

  public:
    virtual Standard_CString PName() const
    {
      Standard_NullValue_Raise_if(!myPName,
                                  "StdLPersistent_HArray2::named_instance::PName - name not set");
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
  typedef instance<TColStd_HArray2OfInteger>           Integer;
  typedef instance<TColStd_HArray2OfReal>              Real;
  typedef instance<StdLPersistent_HArray2OfPersistent> Persistent;

public:
  template <class ArrayClass>
  static Handle(instance<ArrayClass>) Translate(const ArrayClass& theArray)
  {
    Handle(instance<ArrayClass>) aPArray = new instance<ArrayClass>;
    aPArray->myArray                     = new ArrayClass(theArray.LowerRow(),
                                      theArray.UpperRow(),
                                      theArray.LowerCol(),
                                      theArray.UpperCol());
    for (Standard_Integer i = theArray.LowerRow(); i <= theArray.UpperRow(); ++i)
      for (Standard_Integer j = theArray.LowerCol(); j <= theArray.UpperCol(); ++j)
        aPArray->myArray->ChangeValue(i, j) = theArray.Value(i, j);
    return aPArray;
  }

  template <class ArrayClass>
  static Handle(instance<ArrayClass>) Translate(Standard_CString  thePName,
                                                const ArrayClass& theArray)
  {
    Handle(named_instance<ArrayClass>) aPArray = new named_instance<ArrayClass>(thePName);
    aPArray->myArray                           = new ArrayClass(theArray.LowerRow(),
                                      theArray.UpperRow(),
                                      theArray.LowerCol(),
                                      theArray.UpperCol());
    for (Standard_Integer i = theArray.LowerRow(); i <= theArray.UpperRow(); ++i)
      for (Standard_Integer j = theArray.LowerCol(); j <= theArray.UpperCol(); ++j)
        aPArray->myArray->ChangeValue(i, j) = theArray.Value(i, j);
    return aPArray;
  }
};

template <>
inline Standard_CString StdLPersistent_HArray2::instance<TColStd_HArray2OfInteger>::PNameT() const
{
  return "PColStd_HArray2OfInteger";
}

template <>
inline Standard_CString StdLPersistent_HArray2::instance<TColStd_HArray2OfReal>::PNameT() const
{
  return "PColStd_HArray2OfReal";
}

template <>
inline void StdLPersistent_HArray2::instance<StdLPersistent_HArray2OfPersistent>::PChildrenT(
  StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  for (Standard_Integer i = myArray->LowerRow(); i <= myArray->UpperRow(); ++i)
    for (Standard_Integer j = myArray->LowerCol(); j <= myArray->UpperCol(); ++j)
      theChildren.Append(myArray->Value(i, j));
}

#endif
