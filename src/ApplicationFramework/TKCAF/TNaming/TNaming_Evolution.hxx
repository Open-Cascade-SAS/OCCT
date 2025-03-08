// Created on: 1997-03-17
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TNaming_Evolution_HeaderFile
#define _TNaming_Evolution_HeaderFile

//! Defines the type of evolution in old shape - new shape pairs.
//! The definitions - in the form of the terms of
//! the enumeration - are needed by the
//! TNaming_NamedShape attribute and
//! indicate what entities this attribute records as follows:
//! -   PRIMITIVE
//! -   New entities; in each pair, old shape is a
//! null shape and new shape is a created
//! entity.
//! -   GENERATED
//! -   Entities created from other entities; in
//! each pair, old shape is the generator and
//! new shape is the created entity.
//! -   MODIFY
//! -   Split or merged entities, in each pair, old
//! shape is the entity before the operation
//! and new shape is the new entity after the
//! operation.
//! -   DELETE
//! -   Deletion of entities; in each pair, old
//! shape is a deleted entity and new shape is null.
//! -   SELECTED
//! -   Named topological entities; in each pair,
//! the new shape is a named entity and the
//! old shape is not used.
//!
//! For a split which generates multiple faces, the
//! attribute will contain many pairs with the same
//! old shape; for a merge, it will contain many
//! pairs with the same new shape.
//! Finally, an example of delete would be a face
//! removed by a Boolean operation.
enum TNaming_Evolution
{
  TNaming_PRIMITIVE,
  TNaming_GENERATED,
  TNaming_MODIFY,
  TNaming_DELETE,
  TNaming_REPLACE,
  TNaming_SELECTED
};

#endif // _TNaming_Evolution_HeaderFile
