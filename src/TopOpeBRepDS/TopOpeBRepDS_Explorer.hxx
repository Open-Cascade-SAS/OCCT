// Created on: 1999-01-05
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepDS_Explorer_HeaderFile
#define _TopOpeBRepDS_Explorer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepDS_HDataStructure;
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Vertex;



class TopOpeBRepDS_Explorer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepDS_Explorer();
  
  Standard_EXPORT TopOpeBRepDS_Explorer(const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopAbs_ShapeEnum T = TopAbs_SHAPE, const Standard_Boolean findkeep = Standard_True);
  
  Standard_EXPORT void Init (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopAbs_ShapeEnum T = TopAbs_SHAPE, const Standard_Boolean findkeep = Standard_True);
  
  Standard_EXPORT TopAbs_ShapeEnum Type() const;
  
  Standard_EXPORT Standard_Boolean More() const;
  
  Standard_EXPORT void Next();
  
  Standard_EXPORT const TopoDS_Shape& Current() const;
  
  Standard_EXPORT Standard_Integer Index() const;
  
  Standard_EXPORT const TopoDS_Face& Face() const;
  
  Standard_EXPORT const TopoDS_Edge& Edge() const;
  
  Standard_EXPORT const TopoDS_Vertex& Vertex() const;




protected:





private:

  
  Standard_EXPORT void Find();


  Handle(TopOpeBRepDS_HDataStructure) myHDS;
  TopAbs_ShapeEnum myT;
  Standard_Integer myI;
  Standard_Integer myN;
  Standard_Boolean myB;
  Standard_Boolean myFK;


};







#endif // _TopOpeBRepDS_Explorer_HeaderFile
