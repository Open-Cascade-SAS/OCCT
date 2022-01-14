// Created on: 1992-11-17
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

#ifndef _IFSelect_SelectExtract_HeaderFile
#define _IFSelect_SelectExtract_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectDeduct.hxx>
#include <Standard_Integer.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class Standard_Transient;
class Interface_InterfaceModel;
class TCollection_AsciiString;


class IFSelect_SelectExtract;
DEFINE_STANDARD_HANDLE(IFSelect_SelectExtract, IFSelect_SelectDeduct)

//! A SelectExtract determines a list of Entities from an Input
//! Selection, as a sub-list of the Input Result
//! It works by applying a sort criterium on each Entity of the
//! Input. This criterium can be applied Direct to Pick Items
//! (default case) or Reverse to Remove Item
//!
//! Basic features (the unique Input) are inherited from SelectDeduct
class IFSelect_SelectExtract : public IFSelect_SelectDeduct
{

public:

  
  //! Returns True if Sort criterium is Direct, False if Reverse
  Standard_EXPORT Standard_Boolean IsDirect() const;
  
  //! Sets Sort criterium sense to a new value
  //! (True : Direct , False : Reverse)
  Standard_EXPORT void SetDirect (const Standard_Boolean direct);
  
  //! Returns the list of selected entities. Works by calling the
  //! method Sort on each input Entity : the Entity is kept as
  //! output if Sort returns the same value as Direct status
  Standard_EXPORT virtual Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Returns True for an Entity if it satisfies the Sort criterium
  //! It receives :
  //! - <rank>, the rank of the Entity in the Iteration,
  //! - <ent> , the Entity itself, and
  //! - <model>, the Starting Model
  //! Hence, the Entity to check is "model->Value(num)" (but an
  //! InterfaceModel allows other checks)
  //! This method is specific to each class of SelectExtract
  Standard_EXPORT virtual Standard_Boolean Sort (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const = 0;
  
  //! Works as Sort but works on the Graph
  //! Default directly calls Sort, but it can be redefined
  //! If SortInGraph is redefined, Sort should be defined even if
  //! not called (to avoid deferred methods in a final class)
  Standard_EXPORT virtual Standard_Boolean SortInGraph (const Standard_Integer rank, const Handle(Standard_Transient)& ent, const Interface_Graph& G) const;
  
  //! Returns a text saying "Picked" or "Removed", plus the
  //! specific criterium returned by ExtractLabel (see below)
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium for extraction
  Standard_EXPORT virtual TCollection_AsciiString ExtractLabel() const = 0;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectExtract,IFSelect_SelectDeduct)

protected:

  
  //! Initializes a SelectExtract : enforces the sort to be Direct
  Standard_EXPORT IFSelect_SelectExtract();



private:


  Standard_Boolean thesort;


};







#endif // _IFSelect_SelectExtract_HeaderFile
