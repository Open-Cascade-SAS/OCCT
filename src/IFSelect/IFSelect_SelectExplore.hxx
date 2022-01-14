// Created on: 1996-09-25
// Created by: Christian CAILLET
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _IFSelect_SelectExplore_HeaderFile
#define _IFSelect_SelectExplore_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IFSelect_SelectDeduct.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class Standard_Transient;
class TCollection_AsciiString;


class IFSelect_SelectExplore;
DEFINE_STANDARD_HANDLE(IFSelect_SelectExplore, IFSelect_SelectDeduct)

//! A SelectExplore determines from an input list of Entities,
//! a list obtained by a way of exploration. This implies the
//! possibility of recursive exploration : the output list is
//! itself reused as input, etc...
//! Examples : Shared Entities, can be considered at one level
//! (immediate shared) or more, or max level
//!
//! Then, for each input entity, if it is not rejected, it can be
//! either taken itself, or explored : it then produces a list.
//! According to a level, either the produced lists or taken
//! entities give the result (level one), or lists are themselves
//! considered and for each item, is it taken or explored.
//!
//! Remark that rejection is just a safety : normally, an input
//! entity is, either taken itself, or explored
//! A maximum level can be specified. Else, the process continues
//! until all entities have been either taken or rejected
class IFSelect_SelectExplore : public IFSelect_SelectDeduct
{

public:

  
  //! Returns the required exploring level
  Standard_EXPORT Standard_Integer Level() const;
  
  //! Returns the list of selected entities. Works by calling the
  //! method Explore on each input entity : it can be rejected,
  //! taken for output, or to explore. If the maximum level has not
  //! yet been attained, or if no max level is specified, entities
  //! to be explored are themselves used as if they were input
  Standard_EXPORT Interface_EntityIterator RootResult (const Interface_Graph& G) const Standard_OVERRIDE;
  
  //! Analyses and, if required, Explores an entity, as follows :
  //! The explored list starts as empty, it has to be filled by this
  //! method.
  //! If it returns False, <ent> is rejected for result (this is to
  //! be used only as safety)
  //! If it returns True and <explored> remains empty, <ent> is
  //! taken itself for result, not explored
  //! If it returns True and <explored> is not empty, the content
  //! of this list is considered :
  //! If maximum level is attained, it is taken for result
  //! Else (or no max), each of its entity will be itself explored
  Standard_EXPORT virtual Standard_Boolean Explore (const Standard_Integer level, const Handle(Standard_Transient)& ent, const Interface_Graph& G, Interface_EntityIterator& explored) const = 0;
  
  //! Returns a text saying "(Recursive)" or "(Level nn)" plus
  //! specific criterium returned by ExploreLabel (see below)
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  //! Returns a text defining the way of exploration
  Standard_EXPORT virtual TCollection_AsciiString ExploreLabel() const = 0;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SelectExplore,IFSelect_SelectDeduct)

protected:

  
  //! Initializes a SelectExplore : the level must be specified on
  //! starting. 0 means all levels, 1 means level one only, etc...
  Standard_EXPORT IFSelect_SelectExplore(const Standard_Integer level);



private:


  Standard_Integer thelevel;


};







#endif // _IFSelect_SelectExplore_HeaderFile
