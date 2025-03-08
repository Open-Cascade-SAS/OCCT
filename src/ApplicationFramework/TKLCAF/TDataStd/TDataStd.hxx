// Created on: 1995-05-10
// Created by: Denis PASCAL
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

#ifndef _TDataStd_HeaderFile
#define _TDataStd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_IDList.hxx>
#include <Standard_OStream.hxx>
#include <TDataStd_RealEnum.hxx>

//! This  package  defines   standard attributes for
//! modelling.
//! These allow you to create and modify labels
//! and attributes for many basic data types.
//! Standard topological and visualization
//! attributes have also been created.
//! To find an attribute attached to a specific label,
//! you use the GUID of the type of attribute you
//! are looking for. To do this, first find this
//! information using the method GetID as follows: Standard_GUID anID =
//! MyAttributeClass::GetID();
//! Then, use the method Find for the label as follows:
//! Standard_Boolean HasAttribute
//! =
//! aLabel.Find(anID,anAttribute);
//! Note
//! For information on the relations between this
//! component of OCAF and the others, refer to the OCAF User's Guide.
class TDataStd
{
public:
  DEFINE_STANDARD_ALLOC

  //! Appends to <anIDList> the list of the attributes
  //! IDs of this package. CAUTION: <anIDList> is NOT
  //! cleared before use.
  Standard_EXPORT static void IDList(TDF_IDList& anIDList);

  //! Prints the name of the real dimension <DIM> as a String on
  //! the Stream <S> and returns <S>.
  Standard_EXPORT static Standard_OStream& Print(const TDataStd_RealEnum DIM, Standard_OStream& S);
};

#endif // _TDataStd_HeaderFile
