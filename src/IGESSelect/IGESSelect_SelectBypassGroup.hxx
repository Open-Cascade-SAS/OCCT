// Created on: 1994-06-01
// Created by: Christian CAILLET
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

#ifndef _IGESSelect_SelectBypassGroup_HeaderFile
#define _IGESSelect_SelectBypassGroup_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExplore.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;
class TCollection_AsciiString;


class IGESSelect_SelectBypassGroup;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectBypassGroup, IFSelect_SelectExplore)

//! Selects a list built as follows :
//! Groups are entities type 402, forms 1,7,14,15 (Group,
//! Ordered or not, "WithoutBackPointer" or not)
//!
//! Entities which are not GROUP are taken as such
//! For Groups, their list of Elements is explore
//! Hence, level 0 (D) recursively explores a Group if some of
//! its Elements are Groups. level 1 explores just at first level
class IGESSelect_SelectBypassGroup : public IFSelect_SelectExplore
{

public:

  
  //! Creates a SelectBypassGroup, by default all level
  //! (level = 1 explores at first level)
  Standard_EXPORT IGESSelect_SelectBypassGroup(const Standard_Integer level = 0);
  
  //! Explores an entity : for a Group, gives its elements
  //! Else, takes the entity itself
  Standard_EXPORT Standard_Boolean Explore (const Standard_Integer level, const Handle(Standard_Transient)& ent, const Interface_Graph& G, Interface_EntityIterator& explored) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Content of Group"
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectBypassGroup,IFSelect_SelectExplore)

protected:




private:




};







#endif // _IGESSelect_SelectBypassGroup_HeaderFile
