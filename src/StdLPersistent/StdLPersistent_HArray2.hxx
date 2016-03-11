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

#include <StdObjMgt_Persistent.hxx>
#include <StdObjMgt_ReadData.hxx>

#include <NCollection_DefineHArray2.hxx>

#include <TColStd_HArray2OfInteger.hxx>
#include <TColStd_HArray2OfReal.hxx>


DEFINE_HARRAY2 (StdLPersistent_HArray2OfPersistent,
                NCollection_Array2<Handle(StdObjMgt_Persistent)>)


class StdLPersistent_HArray2
{
  class base : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

  protected:
    virtual void createArray (
      const Standard_Integer theLowerRow, const Standard_Integer theLowerCol,
      const Standard_Integer theUpperRow, const Standard_Integer theUpperCol)
        = 0;

    virtual void readValue (StdObjMgt_ReadData&    theReadData,
                            const Standard_Integer theRow,
                            const Standard_Integer theCol) = 0;
  };

protected:
  template <class ArrayClass>
  class instance : public base
  {
  public:
    typedef Handle(ArrayClass) ArrayHandle;

  public:
    //! Get the array.
    const Handle(ArrayClass)& Array() const  { return myArray; }

  protected:
    virtual void createArray (
      const Standard_Integer theLowerRow, const Standard_Integer theLowerCol,
      const Standard_Integer theUpperRow, const Standard_Integer theUpperCol)
    {
      myArray = new ArrayClass (theLowerRow, theUpperRow,
                                theLowerCol, theUpperCol);
    }

    virtual void readValue (StdObjMgt_ReadData&    theReadData,
                            const Standard_Integer theRow,
                            const Standard_Integer theCol)
      { theReadData >> myArray->ChangeValue (theRow, theCol); }

  protected:
    Handle(ArrayClass) myArray;
  };

public:
  typedef instance<TColStd_HArray2OfInteger>           Integer;
  typedef instance<TColStd_HArray2OfReal>              Real;
  typedef instance<StdLPersistent_HArray2OfPersistent> Persistent;
};

#endif
