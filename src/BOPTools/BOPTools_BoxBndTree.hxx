// Created by: Eugeny MALTCHIKOV
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

#ifndef BOPTools_BoxBndTree_HeaderFile
#define BOPTools_BoxBndTree_HeaderFile

#include <Bnd_Box.hxx>
#include <BOPTools_BoxSelector.hxx>
#include <NCollection_UBTree.hxx>
#include <Standard_Integer.hxx>

typedef NCollection_UBTree<Standard_Integer, Bnd_Box> BOPTools_BoxBndTree;
typedef BOPTools_BoxSelector<Bnd_Box> BOPTools_BoxBndTreeSelector;

#endif 
