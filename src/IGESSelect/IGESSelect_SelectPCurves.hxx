// Created on: 1999-02-26
// Created by: Christian CAILLET
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

#ifndef _IGESSelect_SelectPCurves_HeaderFile
#define _IGESSelect_SelectPCurves_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExplore.hxx>
#include <Standard_Integer.hxx>
class Standard_Transient;
class Interface_Graph;
class Interface_EntityIterator;
class TCollection_AsciiString;


class IGESSelect_SelectPCurves;
DEFINE_STANDARD_HANDLE(IGESSelect_SelectPCurves, IFSelect_SelectExplore)

//! This Selection returns the pcurves which lie on a face
//! In two modes : global (i.e. a CompositeCurve is not explored)
//! or basic (all the basic curves are listed)
class IGESSelect_SelectPCurves : public IFSelect_SelectExplore
{

public:

  
  //! Creates a SelectPCurves
  //! basic True  : lists all the components of pcurves
  //! basic False : lists the uppest level definitions
  //! (i.e. stops at CompositeCurve)
  Standard_EXPORT IGESSelect_SelectPCurves(const Standard_Boolean basic);
  
  //! Explores an entity, to take its contained PCurves
  //! An independent curve is IGNORED : only faces are explored
  Standard_EXPORT Standard_Boolean Explore (const Standard_Integer level, const Handle(Standard_Transient)& ent, const Interface_Graph& G, Interface_EntityIterator& explored) const Standard_OVERRIDE;
  
  //! Returns a text defining the criterium : "Basic PCurves" or
  //! "Global PCurves"
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESSelect_SelectPCurves,IFSelect_SelectExplore)

protected:




private:


  Standard_Boolean thebasic;


};







#endif // _IGESSelect_SelectPCurves_HeaderFile
