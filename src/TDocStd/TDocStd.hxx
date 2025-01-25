// Created on: 1999-04-07
// Created by: Denis PASCAL
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

#ifndef _TDocStd_HeaderFile
#define _TDocStd_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDF_IDList.hxx>

//! This package define  CAF main classes.
//!
//! * The standard application root class
//!
//! * The standard document which contains data
//!
//! * The external reference mechanism between documents
//!
//! * Attributes for Document management
//! Standard documents offer you a ready-to-use
//! document containing a TDF-based data
//! structure. The documents themselves are
//! contained in a class inheriting from
//! TDocStd_Application which manages creation,
//! storage and retrieval of documents.
//! You can implement undo and redo in your
//! document, and refer from the data framework of
//! one document to that of another one. This is
//! done by means of external link attributes, which
//! store the path and the entry of external links. To
//! sum up, standard documents alone provide
//! access to the data framework. They also allow
//! you to:
//! -   Update external links
//! -   Manage the saving and opening of data
//! -   Manage undo/redo functionality.
//! Note
//! For information on the relations between this
//! component of OCAF and the others, refer to the
//! OCAF User's Guide.
class TDocStd
{
public:
  DEFINE_STANDARD_ALLOC

  //! specific GUID of this package
  //! =============================
  //! Appends to <anIDList> the list of the attributes
  //! IDs of this package. CAUTION: <anIDList> is NOT
  //! cleared before use.
  Standard_EXPORT static void IDList(TDF_IDList& anIDList);
};

#endif // _TDocStd_HeaderFile
