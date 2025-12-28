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

#include <StdLPersistent_Collection.hxx>

#include <TColStd_HPackedMapOfInteger.hxx>
#include <TCollection_HExtendedString.hxx>

struct StdLPersistent_Collection::noConversion
{
  noConversion(const occ::handle<TDF_Data>&) {}

  template <class Type>
  Type operator()(Type theValue) const
  {
    return theValue;
  }
};

struct StdLPersistent_Collection::byteConverter
{
  byteConverter(const occ::handle<TDF_Data>&) {}

  uint8_t operator()(int theValue) const
  {
    return static_cast<uint8_t>(theValue);
  }
};

struct StdLPersistent_Collection::boolConverter
{
  boolConverter(const occ::handle<TDF_Data>&) {}

  bool operator()(int theValue) const { return theValue != 0; }
};

struct StdLPersistent_Collection::stringConverter
{
  stringConverter(const occ::handle<TDF_Data>&) {}

  const TCollection_ExtendedString& operator()(const occ::handle<StdObjMgt_Persistent>& theValue) const
  {
    if (theValue.IsNull())
      return TCollection_ExtendedString::EmptyString();

    occ::handle<TCollection_HExtendedString> aString = theValue->ExtString();
    return aString ? aString->String() : TCollection_ExtendedString::EmptyString();
  }
};

struct StdLPersistent_Collection::referenceConverter
{
  referenceConverter(const occ::handle<TDF_Data>& theDF)
      : myDF(theDF)
  {
  }

  TDF_Label operator()(const occ::handle<StdObjMgt_Persistent>& theValue) const
  {
    return theValue->Label(myDF);
  }

private:
  occ::handle<TDF_Data> myDF;
};

template <class Base>
template <class ArrayHandle, class Converter>
void StdLPersistent_Collection::booleanArrayBase<Base>::import(const ArrayHandle& theArray,
                                                               Converter theConverter) const
{
  occ::handle<NCollection_HArray1<uint8_t>> aByteArray =
    new NCollection_HArray1<uint8_t>(theArray->Lower(), theArray->Upper());

  for (int i = theArray->Lower(); i <= theArray->Upper(); i++)
    aByteArray->SetValue(i, theConverter(theArray->Value(i)));

  this->myTransient->Init(myLower, myUpper);
  this->myTransient->SetInternalArray(aByteArray);
}

template <class Base>
template <class ArrayHandle, class Converter>
void StdLPersistent_Collection::directArrayBase<Base>::import(const ArrayHandle& theArray,
                                                              Converter) const
{
  this->myTransient->ChangeArray(theArray);
}

template <class Base>
template <class ArrayHandle, class Converter>
void StdLPersistent_Collection::arrayBase<Base>::import(const ArrayHandle& theArray,
                                                        Converter          theConverter) const
{
  this->myTransient->Init(theArray->Lower(), theArray->Upper());
  for (int i = theArray->Lower(); i <= theArray->Upper(); i++)
    this->myTransient->SetValue(i, theConverter(theArray->Value(i)));
}

template <class Base>
template <class ArrayHandle, class Converter>
void StdLPersistent_Collection::listBase<Base>::import(const ArrayHandle& theArray,
                                                       Converter          theConverter) const
{
  for (int i = theArray->Lower(); i <= theArray->Upper(); i++)
    this->myTransient->Append(theConverter(theArray->Value(i)));
}

template <class Base>
template <class ArrayHandle, class Converter>
void StdLPersistent_Collection::mapBase<Base>::import(const ArrayHandle& theArray,
                                                      Converter          theConverter) const
{
  occ::handle<TColStd_HPackedMapOfInteger> anHMap = new TColStd_HPackedMapOfInteger;
  for (int i = theArray->Lower(); i <= theArray->Upper(); i++)
    anHMap->ChangeMap().Add(theConverter(theArray->Value(i)));
  this->myTransient->ChangeMap(anHMap);
}

//=======================================================================
// function : ImportAttribute
// purpose  : Import transient attribute from the persistent data
//=======================================================================
template <template <class> class BaseT, class HArrayClass, class AttribClass, class Converter>
void StdLPersistent_Collection::instance<BaseT, HArrayClass, AttribClass, Converter>::
  ImportAttribute()
{
  occ::handle<HArrayClass> anHArray = occ::down_cast<HArrayClass>(this->myData);
  if (anHArray)
  {
    typename HArrayClass::ArrayHandle anArray = anHArray->Array();
    if (anArray)
      this->import(anArray, Converter(this->myTransient->Label().Data()));
    this->myData.Nullify();
  }
}

//=======================================================================
// function : Read
// purpose  : Read persistent data from a file
//=======================================================================
template <class Instance>
void StdLPersistent_Collection::instance_1<Instance>::Read(StdObjMgt_ReadData& theReadData)
{
  Instance::Read(theReadData);
  theReadData >> myDelta;
}

//=======================================================================
// function : ImportAttribute
// purpose  : Import transient attribute from the persistent data
//=======================================================================
template <class Instance>
void StdLPersistent_Collection::instance_1<Instance>::ImportAttribute()
{
  Instance::ImportAttribute();
  this->myTransient->SetDelta(myDelta);
}

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::booleanArrayBase,
                                                   StdLPersistent_Collection::integer,
                                                   TDataStd_BooleanArray,
                                                   StdLPersistent_Collection::byteConverter>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::directArrayBase,
                                                   StdLPersistent_Collection::integer,
                                                   TDataStd_IntegerArray,
                                                   StdLPersistent_Collection::noConversion>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::directArrayBase,
                                                   StdLPersistent_Collection::real,
                                                   TDataStd_RealArray,
                                                   StdLPersistent_Collection::noConversion>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::arrayBase,
                                                   StdLPersistent_Collection::integer,
                                                   TDataStd_ByteArray,
                                                   StdLPersistent_Collection::byteConverter>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::arrayBase,
                                                   StdLPersistent_Collection::persistent,
                                                   TDataStd_ExtStringArray,
                                                   StdLPersistent_Collection::stringConverter>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::arrayBase,
                                                   StdLPersistent_Collection::persistent,
                                                   TDataStd_ReferenceArray,
                                                   StdLPersistent_Collection::referenceConverter>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::listBase,
                                                   StdLPersistent_Collection::integer,
                                                   TDataStd_IntegerList,
                                                   StdLPersistent_Collection::noConversion>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::listBase,
                                                   StdLPersistent_Collection::real,
                                                   TDataStd_RealList,
                                                   StdLPersistent_Collection::noConversion>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::listBase,
                                                   StdLPersistent_Collection::integer,
                                                   TDataStd_BooleanList,
                                                   StdLPersistent_Collection::boolConverter>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::listBase,
                                                   StdLPersistent_Collection::persistent,
                                                   TDataStd_ExtStringList,
                                                   StdLPersistent_Collection::stringConverter>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::listBase,
                                                   StdLPersistent_Collection::persistent,
                                                   TDataStd_ReferenceList,
                                                   StdLPersistent_Collection::referenceConverter>;

template class StdLPersistent_Collection::instance<StdLPersistent_Collection::mapBase,
                                                   StdLPersistent_Collection::integer,
                                                   TDataStd_IntPackedMap,
                                                   StdLPersistent_Collection::noConversion>;

template class StdLPersistent_Collection::instance_1<StdLPersistent_Collection::IntegerArray>;

template class StdLPersistent_Collection::instance_1<StdLPersistent_Collection::RealArray>;

template class StdLPersistent_Collection::instance_1<StdLPersistent_Collection::ByteArray>;

template class StdLPersistent_Collection::instance_1<StdLPersistent_Collection::ExtStringArray>;

template class StdLPersistent_Collection::instance_1<StdLPersistent_Collection::IntPackedMap>;
