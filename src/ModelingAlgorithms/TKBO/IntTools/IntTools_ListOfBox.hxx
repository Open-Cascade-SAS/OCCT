// Created on: 2000-05-18
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef IntTools_ListOfBox_HeaderFile
#define IntTools_ListOfBox_HeaderFile

#include <Bnd_Box.hxx>
#include <NCollection_List.hxx>

typedef NCollection_List<Bnd_Box>           IntTools_ListOfBox;
typedef NCollection_List<Bnd_Box>::Iterator IntTools_ListIteratorOfListOfBox;

#endif
