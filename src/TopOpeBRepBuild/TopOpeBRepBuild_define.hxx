// File:	TopOpeBRepBuild_define.hxx
// Created:	Fri Nov 14 18:49:04 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>


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
