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

#include <NCollection_HArray2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array2.hxx>

class StdLPersistent_HArray2
{
  class base : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT void Read(StdObjMgt_ReadData& theReadData) override;

    //! Read persistent data from a file.
    Standard_EXPORT void Write(StdObjMgt_WriteData& theWriteData) const override;

  protected:
    virtual void lowerBound(int& theRow, int& theCol) const = 0;
    virtual void upperBound(int& theRow, int& theCol) const = 0;
    virtual void createArray(const int theLowerRow,
                             const int theLowerCol,
                             const int theUpperRow,
                             const int theUpperCol)         = 0;

    virtual void readValue(StdObjMgt_ReadData& theReadData, const int theRow, const int theCol) = 0;
    virtual void writeValue(StdObjMgt_WriteData& theWriteData,
                            const int            theRow,
                            const int            theCol) const                                             = 0;
  };

protected:
  template <class ArrayClass>
  class instance : public base
  {
    friend class StdLPersistent_HArray2;

  public:
    typedef occ::handle<ArrayClass> ArrayHandle;

  public:
    //! Get the array.
    const occ::handle<ArrayClass>& Array() const { return myArray; }

  protected:
    void lowerBound(int& theRow, int& theCol) const override
    {
      theRow = myArray->LowerRow();
      theCol = myArray->LowerCol();
    }

    void upperBound(int& theRow, int& theCol) const override
    {
      theRow = myArray->UpperRow();
      theCol = myArray->UpperCol();
    }

    void createArray(const int theLowerRow,
                     const int theLowerCol,
                     const int theUpperRow,
                     const int theUpperCol) override
    {
      myArray = new ArrayClass(theLowerRow, theUpperRow, theLowerCol, theUpperCol);
    }

    void readValue(StdObjMgt_ReadData& theReadData, const int theRow, const int theCol) override
    {
      theReadData >> myArray->ChangeValue(theRow, theCol);
    }

    void writeValue(StdObjMgt_WriteData& theWriteData,
                    const int            theRow,
                    const int            theCol) const override
    {
      theWriteData << myArray->Value(theRow, theCol);
    }

    void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const override
    {
      return PChildrenT(theChildren);
    }

    const char* PName() const override { return PNameT(); }

    const char* PNameT() const
    {
      Standard_NotImplemented::Raise("StdLPersistent_HArray2::instance::PName - not implemented");
      return "";
    }

    void PChildrenT(StdObjMgt_Persistent::SequenceOfPersistent&) const {}

  protected:
    occ::handle<ArrayClass> myArray;
  };

  template <class ArrayClass>
  class named_instance : public instance<ArrayClass>
  {
    friend class StdLPersistent_HArray2;

  public:
    const char* PName() const override
    {
      Standard_NullValue_Raise_if(!myPName,
                                  "StdLPersistent_HArray2::named_instance::PName - name not set");
      return myPName;
    }

  protected:
    named_instance(const char* thePName)
        : myPName(thePName)
    {
    }

    const char* myPName;
  };

public:
  typedef instance<NCollection_HArray2<int>>                               Integer;
  typedef instance<NCollection_HArray2<double>>                            Real;
  typedef instance<NCollection_HArray2<occ::handle<StdObjMgt_Persistent>>> Persistent;

public:
  template <class ArrayClass>
  static Handle(instance<ArrayClass>) Translate(const ArrayClass& theArray)
  {
    Handle(instance<ArrayClass>) aPArray = new instance<ArrayClass>;
    aPArray->myArray                     = new ArrayClass(theArray.LowerRow(),
                                      theArray.UpperRow(),
                                      theArray.LowerCol(),
                                      theArray.UpperCol());
    for (int i = theArray.LowerRow(); i <= theArray.UpperRow(); ++i)
      for (int j = theArray.LowerCol(); j <= theArray.UpperCol(); ++j)
        aPArray->myArray->ChangeValue(i, j) = theArray.Value(i, j);
    return aPArray;
  }

  template <class ArrayClass>
  static Handle(instance<ArrayClass>) Translate(const char* thePName, const ArrayClass& theArray)
  {
    Handle(named_instance<ArrayClass>) aPArray = new named_instance<ArrayClass>(thePName);
    aPArray->myArray                           = new ArrayClass(theArray.LowerRow(),
                                      theArray.UpperRow(),
                                      theArray.LowerCol(),
                                      theArray.UpperCol());
    for (int i = theArray.LowerRow(); i <= theArray.UpperRow(); ++i)
      for (int j = theArray.LowerCol(); j <= theArray.UpperCol(); ++j)
        aPArray->myArray->ChangeValue(i, j) = theArray.Value(i, j);
    return aPArray;
  }
};

template <>
inline const char* StdLPersistent_HArray2::instance<NCollection_HArray2<int>>::PNameT() const
{
  return "PColStd_HArray2OfInteger";
}

template <>
inline const char* StdLPersistent_HArray2::instance<NCollection_HArray2<double>>::PNameT() const
{
  return "PColStd_HArray2OfReal";
}

template <>
inline void StdLPersistent_HArray2::
  instance<NCollection_HArray2<occ::handle<StdObjMgt_Persistent>>>::PChildrenT(
    StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  for (int i = myArray->LowerRow(); i <= myArray->UpperRow(); ++i)
    for (int j = myArray->LowerCol(); j <= myArray->UpperCol(); ++j)
      theChildren.Append(myArray->Value(i, j));
}

#endif
