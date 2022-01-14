// Created on: 1998-01-13
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IGESSelect_SelectBypassSubfigure_HeaderFile
#define _IGESSelect_SelectBypassSubfigure_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExplore.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;
class TCollection_AsciiString;


class IGESSelect_SelectBypassSubfigure;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectBypassSubfigure, IFSelect_SelectExplore)

//! Selects a list built as follows :
//! Subfigures correspond to
//! * Definition (basic : type 308, or Network : type 320)
//! * Instance (Singular : type 408, or Network : 420, or
//! patterns : 412,414)
//!
//! Entities which are not Subfigure are taken as such
//! For Subfigures Instances, their definition is taken, then
//! explored itself
//! For Subfigures Definitions, the list of "Associated Entities"
//! is explored
//! Hence, level 0 (D) recursively explores a Subfigure if some of
//! its Elements are Subfigures. level 1 explores just at first
//! level (i.e. for an instance, returns its definition)
class IGESSelect_SelectBypassSubfigure : public IFSelect_SelectExplore
{

public:

  
  //! Creates a SelectBypassSubfigure, by default all level
  //! (level = 1 explores at first level)
  Standard_EXPORT IGESSelect_SelectBypassSubfigure(const Standard_Integer level = 0);
  
  //! Explores an entity : for a Subfigure, gives its elements
  //! Else, takes the entity itself
  Standard_EXPORT Standard_Boolean Explore (const Standard_Integer level, const Handle(Standard_Transient)& ent, const Interface_Graph& G, Interface_EntityIterator& explored) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Content of Subfigure"
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectBypassSubfigure,IFSelect_SelectExplore)

protected:




private:




};







#endif // _IGESSelect_SelectBypassSubfigure_HeaderFile
