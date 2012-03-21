// Created on: 1997-11-14
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
#define MTBhb  Handle(TopOpeBRepBuild_HBuilder)

#endif
