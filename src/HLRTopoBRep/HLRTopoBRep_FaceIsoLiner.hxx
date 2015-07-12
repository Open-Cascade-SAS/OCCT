// Created on: 1995-01-06
// Created by: Christophe MARION
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

#ifndef _HLRTopoBRep_FaceIsoLiner_HeaderFile
#define _HLRTopoBRep_FaceIsoLiner_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class TopoDS_Face;
class HLRTopoBRep_Data;
class TopoDS_Vertex;
class TopoDS_Edge;
class gp_Pnt;
class Geom2d_Line;



class HLRTopoBRep_FaceIsoLiner 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static void Perform (const Standard_Integer FI, const TopoDS_Face& F, HLRTopoBRep_Data& DS, const Standard_Integer nbIsos);
  
  Standard_EXPORT static TopoDS_Vertex MakeVertex (const TopoDS_Edge& E, const gp_Pnt& P, const Standard_Real Par, const Standard_Real Tol, HLRTopoBRep_Data& DS);
  
  Standard_EXPORT static void MakeIsoLine (const TopoDS_Face& F, const Handle(Geom2d_Line)& Iso, TopoDS_Vertex& V1, TopoDS_Vertex& V2, const Standard_Real U1, const Standard_Real U2, const Standard_Real Tol, HLRTopoBRep_Data& DS);




protected:





private:





};







#endif // _HLRTopoBRep_FaceIsoLiner_HeaderFile
