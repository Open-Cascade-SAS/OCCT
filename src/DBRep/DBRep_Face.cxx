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


#include <DBRep_Face.hxx>
#include <Draw_Color.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DBRep_Face,Standard_Transient)

//=======================================================================
//function : DBRep_Face
//purpose  : 
//=======================================================================
DBRep_Face::DBRep_Face(const TopoDS_Face& F, 
		       const Standard_Integer N,
		       const Draw_Color& C) :
       myFace(F),
       myColor(C),
       myTypes(N ? 1 : 0,N),
       myParams(N ? 1 : 0,3*N)
{
}


