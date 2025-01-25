// Created on: 1997-11-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TopOpeBRepBuild_define_HeaderFile
#define _TopOpeBRepBuild_define_HeaderFile

#include <TopOpeBRepDS_define.hxx>

#include <TopOpeBRepBuild_PWireEdgeSet.hxx>
#include <TopOpeBRepBuild_WireEdgeSet.hxx>
#include <TopOpeBRepBuild_ShellFaceSet.hxx>
#include <TopOpeBRepBuild_GTopo.hxx>
#include <TopOpeBRepBuild_PaveClassifier.hxx>
#include <TopOpeBRepBuild_PaveSet.hxx>
#include <TopOpeBRepBuild_Pave.hxx>
#include <TopOpeBRepBuild_SolidBuilder.hxx>
#include <TopOpeBRepBuild_FaceBuilder.hxx>
#include <TopOpeBRepBuild_EdgeBuilder.hxx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_PBuilder.hxx>
#include <TopOpeBRepBuild_DataMapIteratorOfDataMapOfShapeListOfShapeListOfShape.hxx>
#include <TopOpeBRepBuild_DataMapOfShapeListOfShapeListOfShape.hxx>
#include <TopOpeBRepBuild_ListIteratorOfListOfShapeListOfShape.hxx>
#include <TopOpeBRepBuild_ListOfShapeListOfShape.hxx>
#include <TopOpeBRepBuild_ShapeListOfShape.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>

#define MTBpwes TopOpeBRepBuild_PWireEdgeSet
#define MTBwes TopOpeBRepBuild_WireEdgeSet
#define MTBsfs TopOpeBRepBuild_ShellFaceSet
#define MTBgt TopOpeBRepBuild_GTopo
#define MTBpvc TopOpeBRepBuild_PaveClassifier
#define MTBpvs TopOpeBRepBuild_PaveSet
#define MTBhpv Handle(TopOpeBRepBuild_Pave)
#define MTBpv TopOpeBRepBuild_Pave
#define MTBsb TopOpeBRepBuild_SolidBuilder
#define MTBfb TopOpeBRepBuild_FaceBuilder
#define MTBeb TopOpeBRepBuild_EdgeBuilder
#define MTBbON TopOpeBRepBuild_BuilderON
#define MTBb TopOpeBRepBuild_Builder
#define MTBpb TopOpeBRepBuild_PBuilder
#define MTBdmiodmosloslos TopOpeBRepBuild_DataMapIteratorOfDataMapOfShapeListOfShapeListOfShape
#define MTBdmosloslos TopOpeBRepBuild_DataMapOfShapeListOfShapeListOfShape
#define MTBlioloslos TopOpeBRepBuild_ListIteratorOfListOfShapeListOfShape
#define MTBloslos TopOpeBRepBuild_ListOfShapeListOfShape
#define MTBslos TopOpeBRepBuild_ShapeListOfShape
#define MTBhb Handle(TopOpeBRepBuild_HBuilder)

#endif
