// Created on: 1994-08-31
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Draft_VertexInfo_HeaderFile
#define _Draft_VertexInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Standard_DomainError;
class Standard_NoMoreObject;
class TopoDS_Edge;
class gp_Pnt;



class Draft_VertexInfo 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Draft_VertexInfo();
  
  Standard_EXPORT void Add (const TopoDS_Edge& E);
  
  Standard_EXPORT const gp_Pnt& Geometry() const;
  
  Standard_EXPORT Standard_Real Parameter (const TopoDS_Edge& E);
  
  Standard_EXPORT void InitEdgeIterator();
  
  Standard_EXPORT const TopoDS_Edge& Edge() const;
  
  Standard_EXPORT void NextEdge();
  
  Standard_EXPORT Standard_Boolean MoreEdge() const;
  
  Standard_EXPORT gp_Pnt& ChangeGeometry();
  
  Standard_EXPORT Standard_Real& ChangeParameter (const TopoDS_Edge& E);




protected:





private:



  gp_Pnt myGeom;
  TopTools_ListOfShape myEdges;
  TColStd_ListOfReal myParams;
  TopTools_ListIteratorOfListOfShape myItEd;


};







#endif // _Draft_VertexInfo_HeaderFile
