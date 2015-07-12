// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _HLRTopoBRep_DSFiller_HeaderFile
#define _HLRTopoBRep_DSFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepTopAdaptor_MapOfShapeTool.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopoDS_Shape;
class Contap_Contour;
class HLRTopoBRep_Data;
class TopoDS_Face;
class TopoDS_Vertex;
class Contap_Point;
class TopoDS_Edge;


//! Provides methods  to  fill a HLRTopoBRep_Data.
class HLRTopoBRep_DSFiller 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Stores in <DS> the outlines of  <S> using the current
  //! outliner and stores the isolines in <DS> using a Hatcher.
  Standard_EXPORT static void Insert (const TopoDS_Shape& S, Contap_Contour& FO, HLRTopoBRep_Data& DS, BRepTopAdaptor_MapOfShapeTool& MST, const Standard_Integer nbIso);




protected:





private:

  
  //! Stores in <DS> the outlines of  <F> using the current
  //! outliner.
  Standard_EXPORT static void InsertFace (const Standard_Integer FI, const TopoDS_Face& F, Contap_Contour& FO, HLRTopoBRep_Data& DS, const Standard_Boolean withPCurve);
  
  //! Make a  vertex  from an intersection  point <P>and
  //! store it in the data structure <DS>.
  Standard_EXPORT static TopoDS_Vertex MakeVertex (const Contap_Point& P, const Standard_Real tol, HLRTopoBRep_Data& DS);
  
  //! Insert a vertex    from an internal   intersection
  //! point <P> on restriction <E>  and store it in  the
  //! data structure <DS>.
  Standard_EXPORT static void InsertVertex (const Contap_Point& P, const Standard_Real tol, const TopoDS_Edge& E, HLRTopoBRep_Data& DS);
  
  //! Split all  the edges  with  vertices in   the data
  //! structure.
  Standard_EXPORT static void ProcessEdges (HLRTopoBRep_Data& DS);




};







#endif // _HLRTopoBRep_DSFiller_HeaderFile
