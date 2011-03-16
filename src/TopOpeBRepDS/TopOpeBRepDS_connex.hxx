// File:	TopOpeBRepDS_connex.hxx
// Created:	Tue Nov 25 11:22:42 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>


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
