// File:	TopOpeBRepDS_DSX.hxx
// Created:	Mon Jun 22 16:34:44 1998
// Author:	Jean Yves LEBEY
//		<jyl@langdox.paris1.matra-dtv.fr>


#ifndef _TopOpeBRepDS_tsx_HeaderFile
#define _TopOpeBRepDS_tsx_HeaderFile

#ifdef DEB
#include <TopOpeBRepDS_define.hxx>

Standard_EXPORT void TopOpeBRepDS_SettraceSPSX(const Standard_Boolean b,Standard_Integer n,char** a);
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const Standard_Integer i);
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSX(const TopoDS_Shape& S);
Standard_EXPORT Standard_Integer TopOpeBRepDS_GetindexSPSX(const TopoDS_Shape& S);
Standard_EXPORT void TopOpeBRepDS_SettraceSPSX(const Standard_Integer i,const Standard_Boolean b);

Standard_EXPORT void TopOpeBRepDS_SettraceSPSXX(const Standard_Boolean b, Standard_Integer n, char** a);
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSPSXX(const Standard_Integer i1,const Standard_Integer i2);

Standard_EXPORT void TopOpeBRepDS_SettraceSPSX_SS(const TopoDS_Shape&,const TopoDS_Shape&);
Standard_EXPORT void TopOpeBRepDS_SettraceSPSX_HDS(const Handle(TopOpeBRepDS_HDataStructure)& HDS);
Standard_EXPORT void TopOpeBRepDS_SettraceSPSX_SSHDS(const TopoDS_Shape&,const TopoDS_Shape&, const Handle(TopOpeBRepDS_HDataStructure)& HDS);
#endif

#endif
