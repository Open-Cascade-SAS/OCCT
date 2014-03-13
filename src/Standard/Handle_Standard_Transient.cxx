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

#include <Standard_Transient.hxx>
#include <Standard_Atomic.hxx>

//============================================================================

void Handle(Standard_Transient)::Dump(Standard_OStream& out) const
{ 
  out << Access();
}

//============================================================================

void Handle(Standard_Transient)::Assign (const Standard_Transient *anItem)
{
  Standard_Transient *anIt = (Standard_Transient*)anItem;
  if ( anIt == entity ) return;
  EndScope();
  entity = anIt;
  BeginScope();
}

//============================================================================

void Handle(Standard_Transient)::BeginScope()
{
  if (entity != 0)
  {
    Standard_Atomic_Increment (&entity->count);
  }
}

//============================================================================

void Handle(Standard_Transient)::EndScope()
{
  if (entity == 0)
    return;
  if (Standard_Atomic_Decrement (&entity->count) == 0)
    entity->Delete();
  entity = 0;
}
