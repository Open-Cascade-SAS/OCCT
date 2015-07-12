// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_EdgeBuilder_HeaderFile
#define _TopOpeBRepBuild_EdgeBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopOpeBRepBuild_Area1dBuilder.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopOpeBRepBuild_PaveSet;
class TopOpeBRepBuild_PaveClassifier;
class TopOpeBRepBuild_LoopSet;
class TopOpeBRepBuild_LoopClassifier;
class TopoDS_Shape;



class TopOpeBRepBuild_EdgeBuilder  : public TopOpeBRepBuild_Area1dBuilder
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_EdgeBuilder();
  
  //! Creates a EdgeBuilder to find the areas of
  //! the shapes described by <LS> using the classifier <LC>.
  Standard_EXPORT TopOpeBRepBuild_EdgeBuilder(TopOpeBRepBuild_PaveSet& LS, TopOpeBRepBuild_PaveClassifier& LC, const Standard_Boolean ForceClass = Standard_False);
  
  Standard_EXPORT void InitEdgeBuilder (TopOpeBRepBuild_LoopSet& LS, TopOpeBRepBuild_LoopClassifier& LC, const Standard_Boolean ForceClass = Standard_False);
  
  Standard_EXPORT void InitEdge();
  
  Standard_EXPORT Standard_Boolean MoreEdge() const;
  
  Standard_EXPORT void NextEdge();
  
  Standard_EXPORT void InitVertex();
  
  Standard_EXPORT Standard_Boolean MoreVertex() const;
  
  Standard_EXPORT void NextVertex();
  
  Standard_EXPORT const TopoDS_Shape& Vertex() const;
  
  Standard_EXPORT Standard_Real Parameter() const;




protected:





private:





};







#endif // _TopOpeBRepBuild_EdgeBuilder_HeaderFile
