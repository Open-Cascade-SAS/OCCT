// Created on: 1994-02-03
// Created by: Jean Marc LACHAUME
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

#include <Geom2dHatch_Classifier.hxx>

#include <Standard_DomainError.hxx>
#include <Geom2dHatch_Elements.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <Geom2dHatch_FClass2dOfClassifier.hxx>
#include <gp_Pnt2d.hxx>
 

#define TheFaceExplorer Geom2dHatch_Elements
#define TheFaceExplorer_hxx <Geom2dHatch_Elements.hxx>
#define TheEdge Geom2dAdaptor_Curve
#define TheEdge_hxx <Geom2dAdaptor_Curve.hxx>
#define TheIntersection2d Geom2dHatch_Intersector
#define TheIntersection2d_hxx <Geom2dHatch_Intersector.hxx>
#define TopClass_FClass2d Geom2dHatch_FClass2dOfClassifier
#define TopClass_FClass2d_hxx <Geom2dHatch_FClass2dOfClassifier.hxx>
#define TopClass_FaceClassifier Geom2dHatch_Classifier
#define TopClass_FaceClassifier_hxx <Geom2dHatch_Classifier.hxx>
#include <TopClass_FaceClassifier.gxx>

