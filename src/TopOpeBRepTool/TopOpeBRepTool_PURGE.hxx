// Created on: 1998-11-24
// Created by: Prestataire Xuan PHAM PHU
// Copyright (c) 1998-1999 Matra Datavision
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



#ifndef _TopOpeBRepTool_PURGE_HeaderFile
#define _TopOpeBRepTool_PURGE_HeaderFile
#include <TopOpeBRepTool_define.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <gp_Pnt2d.hxx>

#ifdef DEB
Standard_IMPORT void FUN_REINIT();
Standard_IMPORT Standard_Integer FUN_addepc(const TopoDS_Shape& ed,const TopoDS_Shape& f);
Standard_IMPORT Standard_Integer FUN_addcheckepc(const TopoDS_Shape& ed,const TopoDS_Shape& f);
Standard_IMPORT Standard_Integer FUN_adds(const TopoDS_Shape& s);
Standard_IMPORT void FUN_tool_trace(const Standard_Integer Index);
Standard_IMPORT  void FUN_tool_trace(const gp_Pnt2d p2d);
#endif
// ----------------------------------------------------------------------
// TopOpeBRepTool_closing.cxx
// ----------------------------------------------------------------------
//Standard_IMPORT Standard_Boolean FUN_tool_UVonclosing(const TopoDS_Edge& E, const TopoDS_Face& F, const Standard_Boolean onU,
//					 const Standard_Real xfirst, const Standard_Real xperiod,
//					 const Standard_Real toluv);
Standard_IMPORT Standard_Boolean FUN_tool_correctCLO(TopoDS_Edge& E, const TopoDS_Face& F);
//Standard_IMPORT Standard_Boolean FUN_tool_getEclo(const TopoDS_Face& F, TopoDS_Edge& Eclo);

// ----------------------------------------------------------------------
// TopOpeBRepTool_faulty.cxx
// ----------------------------------------------------------------------
//Standard_IMPORT gp_Pnt2d FUN_GetVParonF(const TopoDS_Edge& E, const TopoDS_Face& F, const Standard_Integer Index);
//Standard_IMPORT Standard_Real FUN_toluv(const GeomAdaptor_Surface& GAS, const Standard_Real& tol3d);
//Standard_IMPORT void FUN_tool_Vertices(const TopoDS_Edge& E, TopTools_Array1OfShape& vertices);
//Standard_IMPORT void FUN_mapVloe(const TopoDS_Shape& F, TopTools_IndexedDataMapOfShapeListOfShape& mapVloe);

//Standard_IMPORT Standard_Boolean FUN_DetectEdgeswithfaultyUV(const TopoDS_Face& Fin, const TopoDS_Shape& fF, const TopTools_ListOfShape& ISOEds,
//						const Standard_Boolean has2fybounds, TopTools_ListOfShape& lfyE, Standard_Integer& Ivfaulty,
//						const Standard_Boolean& stopatfirst=Standard_False);
//Standard_IMPORT Standard_Boolean FUN_DetectEdgewithfaultyUV(const TopoDS_Face& Fin, const TopoDS_Shape& fF, const TopTools_ListOfShape& ISOEds,
//					       const Standard_Boolean has2fybounds, TopoDS_Shape& fyE, Standard_Integer& Ivfaulty);
//Standard_IMPORT Standard_Boolean FUN_DetectFaultyClosingEdge(const TopoDS_Face& Fin,const TopTools_ListOfShape& Eds,const TopTools_ListOfShape& cEds,TopTools_ListOfShape& fyE);
//Standard_IMPORT Standard_Boolean FUN_isUVClosed(const TopoDS_Face& F, const TopoDS_Face& fF);

// ----------------------------------------------------------------------
// TopOpeBRepTool_PURGE.cxx
// ----------------------------------------------------------------------
//Standard_IMPORT void FUN_tool_ttranslate(const gp_Vec2d& tvector, const TopoDS_Face& fF, TopoDS_Edge& fyE);
#endif
