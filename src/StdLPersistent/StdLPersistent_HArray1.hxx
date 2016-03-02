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

#include <StdObjMgt_Persistent.hxx>
#include <StdLPersistent_HArray1OfPersistent.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfByte.hxx>

class TCollection_HExtendedString;
class TDF_Label;
class TDF_Data;


class StdLPersistent_HArray1 : protected StdObjMgt_ContentTypes
{
  class commonBase : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

  protected:
    virtual void createArray (const Standard_Integer theLowerBound,
                              const Standard_Integer theUpperBound) = 0;

    virtual void readValue (StdObjMgt_ReadData& theReadData,
                            const Standard_Integer theIndex) = 0;
  };

protected:
  template <class ArrayClass>
  class base : public commonBase
  {
  public:
    typedef Handle(ArrayClass)              ArrayHandle;
    typedef typename ArrayClass::value_type ValueType;
    typedef typename ArrayClass::Iterator   Iterator;

  public:
    //! Get the array.
    const Handle(ArrayClass)& Array() const  { return myArray; }

  protected:
    virtual void createArray (const Standard_Integer theLowerBound,
                              const Standard_Integer theUpperBound)
      { myArray = new ArrayClass (theLowerBound, theUpperBound); }

  protected:
    Handle(ArrayClass) myArray;
  };

private:
  template <class ArrayClass, class ValueClass = Value<typename ArrayClass::value_type> >
  class instance : public base<ArrayClass>
  {
  protected:
    Standard_EXPORT virtual void readValue (StdObjMgt_ReadData& theReadData,
                                            const Standard_Integer theIndex);
  };

public:
  typedef instance<TColStd_HArray1OfInteger>                         Integer;
  typedef instance<TColStd_HArray1OfReal>                            Real;
  typedef instance<TColStd_HArray1OfByte, Value<Standard_Character> >Byte;
  typedef instance<StdLPersistent_HArray1OfPersistent, Reference<> >        Persistent;
};

#endif
