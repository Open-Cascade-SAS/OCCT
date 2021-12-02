// Created on: 2019-03-27
// Created by: Timur Izmaylov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Standard_Std_HeaderFile
#define _Standard_Std_HeaderFile


#include <type_traits>


//! Namespace opencascade is intended for low-level template classes and functions
namespace opencascade
{

  //! Namespace opencascade::std includes templates from C++11 std namespace used by
  //! OCCT classes. These definitions are imported from std namespace, plus (on older
  //! compilers) from std::tr1, or implemented by custom code where neither std
  //! not std::tr1 provide necessary definitions.
  namespace std
  {
    // import all available standard stuff from std namespace
    using namespace ::std;

  } // namespace std

  //! Trait yielding true if class T1 is base of T2 but not the same
  template <class T1, class T2, class Dummy = void>
  struct is_base_but_not_same : opencascade::std::is_base_of<T1, T2>
  {
  };

  //! Explicit specialization of is_base_of trait to workaround the
  //! requirement of type to be complete when T1 and T2 are the same.
  template <class T1, class T2>
  struct is_base_but_not_same<T1,
                              T2,
                              typename opencascade::std::enable_if<opencascade::std::is_same<T1, T2>::value>::type>
  : opencascade::std::false_type
  {
  };

  //! The type trait that checks if the passed type is integer (it must be integral and not boolean)
  //! @tparam TheInteger the checked type
  template <typename TheInteger>
  struct is_integer : std::integral_constant<bool,
                                             opencascade::std::is_integral<TheInteger>::value
                                               && !opencascade::std::is_same<TheInteger, bool>::value>
  {
  };

  //! The auxiliary template that is used for template argument deduction in function templates. A function argument
  //! which type is a template type parameter and it is not needed to be deducted must be declared using this class
  //! template based on the type of some other template type parameter of a function template
  //! @tparam TheType the type that is used as a function argument type to prevent its deduction
  template <typename TheType>
  struct disable_deduction
  {
    typedef TheType type;
  };

} // namespace opencascade

#endif
