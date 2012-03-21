// Created on: 1997-11-25
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



#ifndef _TopOpeBRepDS_connex_HeaderFile
#define _TopOpeBRepDS_connex_HeaderFile

#include <TopOpeBRepDS_define.hxx>

Standard_EXPORT const TopTools_ListOfShape& FDSCNX_EdgeConnexityShapeIndex(const TopoDS_Shape& E,const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer SI);
Standard_EXPORT const TopTools_ListOfShape& FDSCNX_EdgeConnexitySameShape(const TopoDS_Shape& E,const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FDSCNX_Prepare(const TopoDS_Shape& S1, const TopoDS_Shape& S2,const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT Standard_Boolean FDSCNX_HasConnexFace(const TopoDS_Shape& S,const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FDSCNX_FaceEdgeConnexFaces(const TopoDS_Shape& F,const TopoDS_Shape& E,const Handle(TopOpeBRepDS_HDataStructure)& HDS,TopTools_ListOfShape& LF);
Standard_EXPORT void FDSCNX_Dump(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void FDSCNX_Dump(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer I);
Standard_EXPORT void FDSCNX_DumpIndex(const Handle(TopOpeBRepDS_HDataStructure)& HDS,const Standard_Integer I);
#endif
