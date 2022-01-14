// Created on: 1993-04-22
// Created by: Christian CAILLET
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

#ifndef _Interface_HGraph_HeaderFile
#define _Interface_HGraph_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Interface_Graph.hxx>
#include <Standard_Transient.hxx>
class Interface_InterfaceModel;
class Interface_GeneralLib;
class Interface_Protocol;
class Interface_GTool;


class Interface_HGraph;
DEFINE_STANDARD_HANDLE(Interface_HGraph, Standard_Transient)

//! This class allows to store a redefinable Graph, via a Handle
//! (useful for an Object which can work on several successive
//! Models, with the same general conditions)
class Interface_HGraph : public Standard_Transient
{

public:

  
  //! Creates an HGraph directly from a Graph.
  //! Remark that the starting Graph is duplicated
  Standard_EXPORT Interface_HGraph(const Interface_Graph& agraph);
  
  //! Creates an HGraph with a Graph created from <amodel> and <lib>
  Standard_EXPORT Interface_HGraph(const Handle(Interface_InterfaceModel)& amodel, const Interface_GeneralLib& lib, const Standard_Boolean theModeStats = Standard_True);
  
  //! Creates an HGraph with a graph itself created from <amodel>
  //! and <protocol>
  Standard_EXPORT Interface_HGraph(const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_Protocol)& protocol, const Standard_Boolean theModeStats = Standard_True);
  
  //! Creates an HGraph with a graph itself created from <amodel>
  //! and <protocol>
  Standard_EXPORT Interface_HGraph(const Handle(Interface_InterfaceModel)& amodel, const Handle(Interface_GTool)& gtool, const Standard_Boolean theModeStats = Standard_True);
  
  //! Same a above, but works with the GTool in the model
  Standard_EXPORT Interface_HGraph(const Handle(Interface_InterfaceModel)& amodel, const Standard_Boolean theModeStats = Standard_True);
  
  //! Returns the Graph contained in <me>, for Read Only Operations
  //! Remark that it is returns as "const &"
  //! Getting it in a new variable instead of a reference would be
  //! a pity, because all the graph's content would be duplicated
  Standard_EXPORT const Interface_Graph& Graph() const;
  
  //! Same as above, but for Read-Write Operations
  //! Then, The Graph will be modified in the HGraph itself
  Standard_EXPORT Interface_Graph& CGraph();




  DEFINE_STANDARD_RTTIEXT(Interface_HGraph,Standard_Transient)

protected:




private:


  Interface_Graph thegraph;


};







#endif // _Interface_HGraph_HeaderFile
