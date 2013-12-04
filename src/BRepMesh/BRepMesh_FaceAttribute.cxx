// Created by: Ekaterina SMIRNOVA
// Copyright (c) 2008-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_FaceAttribute.ixx>

//=======================================================================
//function : BRepMesh_FaceAttribute
//purpose  : 
//=======================================================================
BRepMesh_FaceAttribute::BRepMesh_FaceAttribute():
 mydefface(0.), myumin(0.), myumax(0.), myvmin(0.), myvmax(0.),
 mydeltaX(1.), mydeltaY(1.), myminX(0.), myminY(0.)
{
}
