// Created on: 1994-04-18
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepClass3d_SolidPassiveClassifier.hxx>

#include <Standard_DomainError.hxx>
#include <BRepClass3d_Intersector3d.hxx>
#include <gp_Lin.hxx>
#include <TopoDS_Face.hxx>
 

#define TheIntersector BRepClass3d_Intersector3d
#define TheIntersector_hxx <BRepClass3d_Intersector3d.hxx>
#define TopClass_Classifier3d BRepClass3d_SolidPassiveClassifier
#define TopClass_Classifier3d_hxx <BRepClass3d_SolidPassiveClassifier.hxx>
#include <TopClass_Classifier3d.gxx>

