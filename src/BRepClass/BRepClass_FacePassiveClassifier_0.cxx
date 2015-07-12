// Created on: 1992-11-18
// Created by: Remi LEQUETTE
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

#include <BRepClass_FacePassiveClassifier.hxx>

#include <Standard_DomainError.hxx>
#include <BRepClass_Edge.hxx>
#include <BRepClass_Intersector.hxx>
#include <gp_Lin2d.hxx>
 

#define TheEdge BRepClass_Edge
#define TheEdge_hxx <BRepClass_Edge.hxx>
#define TheIntersector BRepClass_Intersector
#define TheIntersector_hxx <BRepClass_Intersector.hxx>
#define TopClass_Classifier2d BRepClass_FacePassiveClassifier
#define TopClass_Classifier2d_hxx <BRepClass_FacePassiveClassifier.hxx>
#include <TopClass_Classifier2d.gxx>

