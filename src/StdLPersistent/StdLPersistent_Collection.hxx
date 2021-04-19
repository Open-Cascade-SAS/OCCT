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


#ifndef _StdLPersistent_Collection_HeaderFile
#define _StdLPersistent_Collection_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdLPersistent_HArray1.hxx>

#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDataStd_RealList.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDataStd_IntPackedMap.hxx>


class StdLPersistent_Collection
{
  // Converters
  struct noConversion;
  struct byteConverter;
  struct boolConverter;
  struct stringConverter;
  struct referenceConverter;

  // Base templates
  template <class Base>
  class booleanArrayBase : public Base
  {
  public:
    //! Empty constructor.
    booleanArrayBase()
    : myLower(0),
      myUpper(0)
    {
    }

    //! Read persistent data from a file.
    virtual void Read (StdObjMgt_ReadData& theReadData)
    {
      Base::Read (theReadData);
      theReadData >> myLower >> myUpper;
    }

  protected:
    template <class ArrayHandle, class Converter>
    inline void import (const ArrayHandle& theArray, Converter theConverter)
      const;

  protected:
    Standard_Integer myLower;
    Standard_Integer myUpper;
  };

  template <class Base>
  class directArrayBase : public Base
  {
  protected:
    template <class ArrayHandle, class Converter>
    inline void import (const ArrayHandle& theArray, Converter theConverter)
      const;
  };

  template <class Base>
  class arrayBase : public Base
  {
  protected:
    template <class ArrayHandle, class Converter>
    inline void import (const ArrayHandle& theArray, Converter theConverter)
      const;
  };

  template <class Base>
  class listBase : public Base
  {
  protected:
    template <class ArrayHandle, class Converter>
    inline void import (const ArrayHandle& theArray, Converter theConverter)
      const;
  };

  template <class Base>
  class mapBase : public Base
  {
  protected:
    template <class ArrayHandle, class Converter>
    inline void import (const ArrayHandle& theArray, Converter theConverter)
      const;
  };

  // Instance templates
  template <template<class> class BaseT,
            class HArrayClass,
            class AttribClass,
            class Converter>
  class instance
    : public BaseT <typename StdObjMgt_Attribute<AttribClass>::SingleRef>
  {
  public:
    //! Import transient attribute from the persistent data.
    Standard_EXPORT virtual void ImportAttribute();
  };

  template <class Instance>
  class instance_1 : public Instance
  {
  public:
    //! Empty constructor.
    instance_1()
    : myDelta(Standard_False)
    {
    }

    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);

    //! Import transient attribute from the persistent data.
    Standard_EXPORT virtual void ImportAttribute();

  private:
    Standard_Boolean myDelta;
  };


  // Collection type specialization
  template<class HArrayClass, class AttribClass, class Converter = noConversion>
  struct booleanArrayT
    : instance<booleanArrayBase, HArrayClass, AttribClass, Converter> {};

  template<class HArrayClass, class AttribClass, class Converter = noConversion>
  struct directArrayT
    : instance<directArrayBase, HArrayClass, AttribClass, Converter> {};

  template<class HArrayClass, class AttribClass, class Converter = noConversion>
  struct arrayT
    : instance<arrayBase, HArrayClass, AttribClass, Converter> {};

  template<class HArrayClass, class AttribClass, class Converter = noConversion>
  struct listT
    : instance<listBase, HArrayClass, AttribClass, Converter> {};

  template<class HArrayClass, class AttribClass, class Converter = noConversion>
  struct mapT
    : instance<mapBase, HArrayClass, AttribClass, Converter> {};


  // Internal array types
  typedef StdLPersistent_HArray1::Integer    integer;
  typedef StdLPersistent_HArray1::Real       real;
  typedef StdLPersistent_HArray1::Persistent persistent;

public:
  // Final specialization
  typedef booleanArrayT <integer, TDataStd_BooleanArray, byteConverter>
    BooleanArray;

  typedef directArrayT <integer, TDataStd_IntegerArray>
    IntegerArray;

  typedef directArrayT <real, TDataStd_RealArray>
    RealArray;

  typedef arrayT <integer, TDataStd_ByteArray, byteConverter>
    ByteArray;

  typedef arrayT <persistent, TDataStd_ExtStringArray, stringConverter>
    ExtStringArray;

  typedef arrayT <persistent, TDataStd_ReferenceArray, referenceConverter>
    ReferenceArray;

  typedef listT <integer, TDataStd_IntegerList>
    IntegerList;

  typedef listT <real, TDataStd_RealList>
    RealList;

  typedef listT <integer, TDataStd_BooleanList, boolConverter>
    BooleanList;

  typedef listT <persistent, TDataStd_ExtStringList, stringConverter>
    ExtStringList;

  typedef listT <persistent, TDataStd_ReferenceList, referenceConverter>
    ReferenceList;

  typedef mapT <integer, TDataStd_IntPackedMap>
    IntPackedMap;

  typedef instance_1<IntegerArray>    IntegerArray_1;
  typedef instance_1<RealArray>       RealArray_1;
  typedef instance_1<ByteArray>       ByteArray_1;
  typedef instance_1<ExtStringArray>  ExtStringArray_1;
  typedef instance_1<IntPackedMap>    IntPackedMap_1;
};

#endif
