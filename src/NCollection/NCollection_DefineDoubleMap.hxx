// Created on: 2002-04-24
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Purpose:     The DoubleMap  is used to  bind  pairs (Key1,Key2)
//              and retrieve them in linear time.
//              See Map from NCollection for a discussion about the number
//              of buckets

#ifndef NCollection_DefineDoubleMap_HeaderFile
#define NCollection_DefineDoubleMap_HeaderFile

#include <NCollection_DoubleMap.hxx>

// *********************************************** Class DoubleMap ************

#define DEFINE_DOUBLEMAP(_ClassName_, _BaseCollection_, TheKey1Type, TheKey2Type) \
typedef NCollection_DoubleMap <TheKey1Type, TheKey2Type > _ClassName_;

#endif
