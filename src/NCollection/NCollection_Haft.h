/*
 Created on: 2011-06-02
 Created by: Andrey BETENEV
 Copyright (c) 2011-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#if ! defined(_MSC_VER) || ! defined(_MANAGED)
#error This file is usable only in C++/CLI (.NET) programs
#endif

#pragma once

using namespace System;
using namespace System::Collections::Generic;

//! Template CLI class providing the way to encapsulate instance of C++ 
//! class as a field in the C++/CLI (ref) class. 
//!
//! It can be helpful to encapsulate OCCT Handles, maps, arrays, etc.
//!
//! Use of variable of the Haft type is very similar to that of encapsulated 
//! class:
//! - Default constructor creates default-constructed C++ instance
//! - Non-default construction is possible by copy or by initialization from
//!   compatible pointer (e.g. Haft for Handle can be initialized by pointer 
//!   returned by operator new for a handled class)
//! - Underlying C++ instance is accessed by operator ()

template <class CPPClass> 
public ref class NCollection_Haft 
{
public:
  //! Initialize CLI Haft object by default-constructed C++ object
  NCollection_Haft ()
  {
    myPtr = new CPPClass;
  }

  //! Initialize CLI Haft object by compatible C++ pointer
  template <class T>
  NCollection_Haft (const T* aPtr)
  {
    myPtr = new CPPClass (aPtr);
  }

  //! Initialize CLI Haft object by C++ class object
  NCollection_Haft (const CPPClass& aPtr)
  {
    myPtr = new CPPClass (aPtr);
  }

  //! Destructor - invoked explicitly by delete, or automatically 
  //! when local variable is scoped out
  ~NCollection_Haft ()
  {
    this->Nullify();
  }

  //! Finalizer - called undeterministically by garbage collector
  !NCollection_Haft ()
  {
    this->Nullify();
  }

  //! Function call operator is provided to access underlying C++ object
  CPPClass& operator () () { return *myPtr; }

protected:
  //! Invalidate the haft
  void Nullify ()
  {
    delete myPtr;
    myPtr = 0;
  }

protected:
  CPPClass* myPtr;
};
