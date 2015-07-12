// Created on: 1995-02-09
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _SelectBasics_EntityOwner_HeaderFile
#define _SelectBasics_EntityOwner_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Boolean.hxx>
class TopLoc_Location;


class SelectBasics_EntityOwner;
DEFINE_STANDARD_HANDLE(SelectBasics_EntityOwner, MMgt_TShared)

//! defines an abstract owner of sensitive primitives.
//! Owners are typically used to establish a connection
//! between sensitive entities and high-level objects (e.g. presentations).
//!
//! Priority : It's possible to give a priority:
//! the scale : [0-9] ; the default priority is 0
//! it allows the predominance of one selected object upon
//! another one if many objects are selected at the same time
//!
//! example : Selection of shapes : the owners are
//! selectable objects (presentations)
//!
//! a user can give vertex priority [3], edges [2] faces [1] shape [0],
//! so that if during selection one vertex one edge and one face are
//! simultaneously detected, the vertex will only be hilighted.
class SelectBasics_EntityOwner : public MMgt_TShared
{

public:

  
  //! sets the selectable priority of the owner
    void Set (const Standard_Integer aPriority);
  
    Standard_Integer Priority() const;
  
  Standard_EXPORT virtual Standard_Boolean HasLocation() const = 0;
  
  Standard_EXPORT virtual void SetLocation (const TopLoc_Location& aLoc) = 0;
  
  Standard_EXPORT virtual void ResetLocation() = 0;
  
  Standard_EXPORT virtual TopLoc_Location Location() const = 0;




  DEFINE_STANDARD_RTTI(SelectBasics_EntityOwner,MMgt_TShared)

protected:

  
  Standard_EXPORT SelectBasics_EntityOwner(const Standard_Integer aPriority = 0);

  Standard_Integer mypriority;


private:




};


#include <SelectBasics_EntityOwner.lxx>





#endif // _SelectBasics_EntityOwner_HeaderFile
