// Created on: 1992-10-02
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IFGraph_AllConnected_HeaderFile
#define _IFGraph_AllConnected_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Interface_Graph.hxx>
#include <Interface_GraphContent.hxx>
class Standard_Transient;


//! this class gives content of the CONNECTED COMPONENT(S)
//! which include specific Entity(ies)
class IFGraph_AllConnected  : public Interface_GraphContent
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates an AllConnected from a graph, empty ready to be filled
  Standard_EXPORT IFGraph_AllConnected(const Interface_Graph& agraph);
  
  //! creates an AllConnected which memorizes Entities Connected to
  //! a given one, at any level : that is, itself, all Entities
  //! Shared by it and Sharing it, and so on.
  //! In other terms, this is the content of the CONNECTED COMPONENT
  //! which include a specific Entity
  Standard_EXPORT IFGraph_AllConnected(const Interface_Graph& agraph, const Handle(Standard_Transient)& ent);
  
  //! adds an entity and its Connected ones to the list (allows to
  //! cumulate all Entities Connected by some ones)
  //! Note that if "ent" is in the already computed list,, no entity
  //! will be added, but if "ent" is not already in the list, a new
  //! Connected Component will be cumulated
  Standard_EXPORT void GetFromEntity (const Handle(Standard_Transient)& ent);
  
  //! Allows to restart on a new data set
  Standard_EXPORT void ResetData();
  
  //! does the specific evaluation (Connected entities atall levels)
  Standard_EXPORT virtual void Evaluate() Standard_OVERRIDE;




protected:





private:



  Interface_Graph thegraph;


};







#endif // _IFGraph_AllConnected_HeaderFile
