/*
 Created on: 2011-06-02
 Created by: Andrey BETENEV
 Copyright (c) 2011-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
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
