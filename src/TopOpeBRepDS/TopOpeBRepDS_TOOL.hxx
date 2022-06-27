// Created on: 1999-01-25
// Created by: Xuan PHAM PHU
// Copyright (c) 1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_TOOL_HeaderFile
#define _TopOpeBRepDS_TOOL_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State.hxx>
class TopOpeBRepDS_HDataStructure;
class TopoDS_Edge;
class TopoDS_Shape;



class TopOpeBRepDS_TOOL 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Integer EShareG (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopoDS_Edge& E, TopTools_ListOfShape& lEsd);
  
  Standard_EXPORT static Standard_Boolean ShareG (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const Standard_Integer is1, const Standard_Integer is2);
  
  Standard_EXPORT static Standard_Boolean GetEsd (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopoDS_Shape& S, const Standard_Integer ie, Standard_Integer& iesd);
  
  Standard_EXPORT static Standard_Boolean ShareSplitON (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MspON, const Standard_Integer i1, const Standard_Integer i2, TopoDS_Shape& spON);
  
  Standard_EXPORT static Standard_Boolean GetConfig (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopOpeBRepDS_DataMapOfShapeListOfShapeOn1State& MEspON, const Standard_Integer ie, const Standard_Integer iesd, Standard_Integer& conf);




protected:





private:





};







#endif // _TopOpeBRepDS_TOOL_HeaderFile
