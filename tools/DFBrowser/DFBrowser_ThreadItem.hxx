// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef DFBrowser_ThreadItem_H
#define DFBrowser_ThreadItem_H

#include <Standard_Macro.hxx>

//! \class DFBrowser_ThreadItem
//! An abstract interface for candidates to be processed in a separate thread.
//! To do this, new thread item should inherit this interface and be started in DFBrowser_Thread.
class DFBrowser_ThreadItem
{
public:

  //! Constructor
  DFBrowser_ThreadItem() {}

  //! Destructor
  virtual ~DFBrowser_ThreadItem() {}

  //! Algorithm of this item work. It will be performed only once
  virtual void Run() = 0;

  //! Applying values accepted by algorithm
  virtual void ApplyValues() = 0;
};


#endif
