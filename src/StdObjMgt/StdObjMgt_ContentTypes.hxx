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

#ifndef _StdObjMgt_ContentTypes_HeaderFile
#define _StdObjMgt_ContentTypes_HeaderFile


#include <StdObjMgt_ReadData.hxx>

class StdObjMgt_Persistent;


//! Root class for an object containing other obects.
class StdObjMgt_ContentTypes
{
  template <typename Type>
  class holder
  {
  public:
    //! Return the value.
    operator Type() const { return myValue; }

  protected:
    Type myValue;
  };

  template <typename Type>
  struct enum_ : holder<Type>
  {
    //! Read the value from a file.
    void Read (StdObjMgt_ReadData& theReadData)
      { theReadData.ReadEnum (this->myValue); }
  };

  template <typename Type>
  struct value : holder<Type>
  {
    //! Read the value from a file.
    void Read (StdObjMgt_ReadData& theReadData)
      { theReadData.ReadValue (this->myValue); }
  };

  template <class Class>
  struct object : Class
  {
    //! Read object data from a file.
    void Read (StdObjMgt_ReadData& theReadData)
      { theReadData.ReadObject ((Class&)*this); }
  };

  template <class Persistent>
  struct referenceT : Handle(Persistent)
  {
    //! Read object data from a file.
    void Read (StdObjMgt_ReadData& theReadData)
      { theReadData.ReadReference (*this); }
  };

  struct reference : Handle(StdObjMgt_Persistent)
  {
    //! Read object data from a file.
    void Read (StdObjMgt_ReadData& theReadData)
      { Handle(StdObjMgt_Persistent)::operator= (theReadData.ReadReference()); }

    //! Cast the reference to a target type.
    template <class Persistent>
    bool Cast (Handle(Persistent)& theTarget) const
    {
      theTarget = Handle(Persistent)::DownCast(*this);
      return ! theTarget.IsNull();
    }
  };

public:
  template <class Type>
  struct Enum      : StdObjMgt_ReadData::Content <enum_<Type> > {};

  template <class Type>
  struct Value     : StdObjMgt_ReadData::Content <value<Type> > {};

  template <class Class>
  struct Object    : StdObjMgt_ReadData::Content <object<Class> > {};

  template <class Persistent = StdObjMgt_Persistent>
  struct Reference : StdObjMgt_ReadData::Content <referenceT<Persistent> > {};
};

template<>
struct StdObjMgt_ContentTypes::Reference<StdObjMgt_Persistent>
    : StdObjMgt_ReadData::Content <reference> {};

#endif // _StdObjMgt_ContentTypes_HeaderFile
