// Created on: 1998-06-22
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_tsx_HeaderFile
#define _TopOpeBRepDS_tsx_HeaderFile

#ifdef OCCT_DEBUG
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
