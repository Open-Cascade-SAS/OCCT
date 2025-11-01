// Created on: 1999-03-23
// Created by: data exchange team
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

#ifndef _STEPSelections_SelectInstances_HeaderFile
#define _STEPSelections_SelectInstances_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SelectExplore.hxx>
#include <Standard_Integer.hxx>
class Interface_EntityIterator;
class Interface_Graph;
class Standard_Transient;
class TCollection_AsciiString;

class STEPSelections_SelectInstances;
DEFINE_STANDARD_HANDLE(STEPSelections_SelectInstances, IFSelect_SelectExplore)

class STEPSelections_SelectInstances : public IFSelect_SelectExplore
{

public:
  Standard_EXPORT STEPSelections_SelectInstances();

  Standard_EXPORT Interface_EntityIterator
    RootResult(const Interface_Graph& G) const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean
    Explore(const Standard_Integer            level,
            const Handle(Standard_Transient)& ent,
            const Interface_Graph&            G,
            Interface_EntityIterator&         explored) const Standard_OVERRIDE;

  //! Returns a text defining the criterium : "Instances"
  Standard_EXPORT TCollection_AsciiString ExploreLabel() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(STEPSelections_SelectInstances, IFSelect_SelectExplore)

protected:
  Standard_EXPORT virtual Standard_Boolean HasUniqueResult() const Standard_OVERRIDE;

private:
};

#endif // _STEPSelections_SelectInstances_HeaderFile
