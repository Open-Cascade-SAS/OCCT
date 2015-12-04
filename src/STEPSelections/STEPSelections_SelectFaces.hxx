// Created on: 1999-02-11
// Created by: Pavel DURANDIN
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

#ifndef _STEPSelections_SelectFaces_HeaderFile
#define _STEPSelections_SelectFaces_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExplore.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;
class TCollection_AsciiString;


class STEPSelections_SelectFaces;
DEFINE_STANDARD_HANDLE(STEPSelections_SelectFaces, IFSelect_SelectExplore)

//! This selection returns "STEP faces"
class STEPSelections_SelectFaces : public IFSelect_SelectExplore
{

public:

  
  Standard_EXPORT STEPSelections_SelectFaces();
  
  //! Explores an entity, to take its faces
  //! Works recursively
  Standard_EXPORT Standard_Boolean Explore (const Standard_Integer level, const Handle(Standard_Transient)& ent, const Interface_Graph& G, Interface_EntityIterator& explored) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Faces"
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(STEPSelections_SelectFaces,IFSelect_SelectExplore)

protected:




private:




};







#endif // _STEPSelections_SelectFaces_HeaderFile
