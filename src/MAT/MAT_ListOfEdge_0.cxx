// Created on: 1992-09-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <MAT_ListOfEdge.hxx>

#include <Standard_Type.hxx>

#include <MAT_TListNodeOfListOfEdge.hxx>
#include <MAT_Edge.hxx>

 








#define Item Handle(MAT_Edge)
#define Item_hxx <MAT_Edge.hxx>
#define MAT_TListNode MAT_TListNodeOfListOfEdge
#define MAT_TListNode_hxx <MAT_TListNodeOfListOfEdge.hxx>
#define Handle_MAT_TListNode Handle(MAT_TListNodeOfListOfEdge)
#define MAT_TList MAT_ListOfEdge
#define MAT_TList_hxx <MAT_ListOfEdge.hxx>
#define Handle_MAT_TList Handle(MAT_ListOfEdge)
#include <MAT_TList.gxx>

