// Created on: 2002-04-23
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

// Purpose:     Single hashed Map. This  Map is used  to store and
//              retrieve keys in linear time.
//              The ::Iterator class can be  used to explore  the
//              content of the map. It is not  wise to iterate and
//              modify a map in parallel.
//              To compute  the hashcode of  the key the  function
//              ::HashCode must be defined in the global namespace
//              To compare two keys the function ::IsEqual must be
//              defined in the global namespace.
//              The performance of  a Map is conditionned  by  its
//              number of buckets that  should be kept greater  to
//              the number   of keys.  This  map has  an automatic
//              management of the number of buckets. It is resized
//              when  the number of Keys  becomes greater than the
//              number of buckets.
//              If you have a fair  idea of the number of  objects
//              you  can save on automatic   resizing by giving  a
//              number of buckets  at creation or using the ReSize
//              method. This should be  consider only for  crucial
//              optimisation issues.

#ifndef NCollection_DefineMap_HeaderFile
#define NCollection_DefineMap_HeaderFile

#include <NCollection_Map.hxx>

// *********************************************** Class Map *****************

#define DEFINE_MAP(_ClassName_, _BaseCollection_, TheKeyType)                  \
typedef NCollection_Map <TheKeyType > _ClassName_;

#endif
