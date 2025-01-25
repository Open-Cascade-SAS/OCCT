// Created on: 2015-06-26
// Created by: Andrey Betenev
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

#ifndef NCollection_Shared_HeaderFile
#define NCollection_Shared_HeaderFile

#include <NCollection_DefineAlloc.hxx>

//! Template defining a class derived from the specified base class and
//! Standard_Transient, and supporting OCCT RTTI.
//!
//! This provides possibility to use Handes for types not initially intended
//! to be dynamically allocated.
//!
//! Current limitation is that only copy and constructors with 1-3 arguments are defined,
//! calling those of the argument class (default constructor must be available).
//! It can be improved when perfect forwarding of template arguments is supported
//! by all compilers used for OCCT.
//!
//! The intent is similar to std::make_shared<> in STL, except that this
//! implementation defines a separate type.

template <class T,
          typename = typename opencascade::std::enable_if<
            !opencascade::std::is_base_of<Standard_Transient, T>::value>::type>
class NCollection_Shared : public Standard_Transient, public T
{
public:
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

  //! Default constructor
  NCollection_Shared() {}

  //! Constructor with single argument
  template <typename T1>
  NCollection_Shared(const T1& arg1)
      : T(arg1)
  {
  }

  //! Constructor with single argument
  template <typename T1>
  NCollection_Shared(T1& arg1)
      : T(arg1)
  {
  }

  //! Constructor with two arguments
  template <typename T1, typename T2>
  NCollection_Shared(const T1& arg1, const T2& arg2)
      : T(arg1, arg2)
  {
  }

  //! Constructor with two arguments
  template <typename T1, typename T2>
  NCollection_Shared(T1& arg1, const T2& arg2)
      : T(arg1, arg2)
  {
  }

  //! Constructor with two arguments
  template <typename T1, typename T2>
  NCollection_Shared(const T1& arg1, T2& arg2)
      : T(arg1, arg2)
  {
  }

  //! Constructor with two arguments
  template <typename T1, typename T2>
  NCollection_Shared(T1& arg1, T2& arg2)
      : T(arg1, arg2)
  {
  }

  /* this could work...
    //! Forwarding constructor
    template<typename... Args>
    NCollection_Shared (Args&&... args)
    : T (std::forward<Args>(args)...)
    {}
  */
};

#endif
