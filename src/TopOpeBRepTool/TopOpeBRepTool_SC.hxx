// File:	TopOpeBRepTool_SC.hxx
// Created:	Wed Apr  1 15:25:56 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>

#ifndef _TopOpeBRepTool_SC_HeaderFile
#define _TopOpeBRepTool_SC_HeaderFile

#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_ShapeClassifier.hxx>
#include <TopOpeBRepTool_PShapeClassifier.hxx>

#define MTLsc  TopOpeBRepTool_ShapeClassifier
#define MTLpsc TopOpeBRepTool_PShapeClassifier

Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(void);
Standard_EXPORT TopOpeBRepTool_ShapeClassifier& FSC_GetPSC(const TopoDS_Shape& S);
// ----------------------------------------------------------------------
//  state point <P> on/in shapes (edge <E>,face <F>)
// ----------------------------------------------------------------------
Standard_EXPORT TopAbs_State FSC_StatePonFace(const gp_Pnt& P,const TopoDS_Shape& F,TopOpeBRepTool_ShapeClassifier& PSC);
Standard_EXPORT TopAbs_State FSC_StateEonFace(const TopoDS_Shape& E,const Standard_Real t,const TopoDS_Shape& F,TopOpeBRepTool_ShapeClassifier& PSC);

#endif
