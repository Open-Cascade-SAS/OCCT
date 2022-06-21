// Created on: 1993-03-09
// Created by: Jean-Louis Frenkel
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

#ifndef _StdPrs_HLRToolShape_HeaderFile
#define _StdPrs_HLRToolShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRAlgo_EdgeIterator.hxx>
#include <Standard_Integer.hxx>
class HLRBRep_Data;
class TopoDS_Shape;
class HLRAlgo_Projector;
class BRepAdaptor_Curve;



class StdPrs_HLRToolShape 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT StdPrs_HLRToolShape(const TopoDS_Shape& TheShape, const HLRAlgo_Projector& TheProjector);
  
  Standard_EXPORT Standard_Integer NbEdges() const;
  
  Standard_EXPORT void InitVisible (const Standard_Integer EdgeNumber);
  
  Standard_EXPORT Standard_Boolean MoreVisible() const;
  
  Standard_EXPORT void NextVisible();
  
  Standard_EXPORT void Visible (BRepAdaptor_Curve& TheEdge, Standard_Real& U1, Standard_Real& U2);
  
  Standard_EXPORT void InitHidden (const Standard_Integer EdgeNumber);
  
  Standard_EXPORT Standard_Boolean MoreHidden() const;
  
  Standard_EXPORT void NextHidden();
  
  Standard_EXPORT void Hidden (BRepAdaptor_Curve& TheEdge, Standard_Real& U1, Standard_Real& U2);




protected:





private:



  Handle(HLRBRep_Data) MyData;
  HLRAlgo_EdgeIterator myEdgeIterator;
  Standard_Integer MyCurrentEdgeNumber;


};







#endif // _StdPrs_HLRToolShape_HeaderFile
