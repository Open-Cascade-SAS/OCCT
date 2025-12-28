// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
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

#ifndef _IGESGraph_Pick_HeaderFile
#define _IGESGraph_Pick_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>

//! defines IGESPick, Type <406> Form <21>
//! in package IGESGraph
//!
//! Attaches information that an entity may be picked
//! by whatever pick device is used in the receiving
//! system
class IGESGraph_Pick : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGraph_Pick();

  //! This method is used to set the fields of the class Pick
  //! - nbProps     : Number of property values (NP = 1)
  //! - aPickStatus : Pick Flag
  Standard_EXPORT void Init(const int nbProps, const int aPickStatus);

  //! returns the number of property values in <me>.
  Standard_EXPORT int NbPropertyValues() const;

  //! returns 0 if <me> is pickable(default),
  //! 1 if <me> is not pickable.
  Standard_EXPORT int PickFlag() const;

  //! returns True if thePick is 0.
  Standard_EXPORT bool IsPickable() const;

  DEFINE_STANDARD_RTTIEXT(IGESGraph_Pick, IGESData_IGESEntity)

private:
  int theNbPropertyValues;
  int thePick;
};

#endif // _IGESGraph_Pick_HeaderFile
