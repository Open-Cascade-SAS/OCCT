// Created on: 1995-12-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepCheck_HeaderFile
#define _BRepCheck_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_Status.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Edge;
class BRepCheck_Result;
class BRepCheck_Vertex;
class BRepCheck_Edge;
class BRepCheck_Wire;
class BRepCheck_Face;
class BRepCheck_Shell;
class BRepCheck_Solid;
class BRepCheck_Analyzer;


//! This package  provides tools to check the validity
//! of the BRep.
class BRepCheck 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void Add (BRepCheck_ListOfStatus& List, const BRepCheck_Status Stat);
  
  Standard_EXPORT static void Print (const BRepCheck_Status Stat, Standard_OStream& OS);
  
  Standard_EXPORT static Standard_Boolean SelfIntersection (const TopoDS_Wire& W, const TopoDS_Face& F, TopoDS_Edge& E1, TopoDS_Edge& E2);




protected:





private:




friend class BRepCheck_Result;
friend class BRepCheck_Vertex;
friend class BRepCheck_Edge;
friend class BRepCheck_Wire;
friend class BRepCheck_Face;
friend class BRepCheck_Shell;
friend class BRepCheck_Solid;
friend class BRepCheck_Analyzer;

};







#endif // _BRepCheck_HeaderFile
