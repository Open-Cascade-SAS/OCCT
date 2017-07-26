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

#ifndef View_ConextType_H
#define View_ConextType_H

//! Type of context used in a tool library
enum View_ContextType
{
  View_ContextType_Own, //!< View widget context is used
  View_ContextType_None, //!< no context (is useful if visualization is not needed, for better performance)
  View_ContextType_External //!< context is set from outside
};

#endif
