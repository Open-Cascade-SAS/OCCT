// Created on: 1993-07-15
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <DBRep_Edge.hxx>
#include <Draw_Color.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Edge.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DBRep_Edge,Standard_Transient)

//=======================================================================
//function : DBRep_Edge
//purpose  : 
//=======================================================================
DBRep_Edge::DBRep_Edge(const TopoDS_Edge& E, const Draw_Color& C) :
       myEdge(E),
       myColor(C)
{
}


