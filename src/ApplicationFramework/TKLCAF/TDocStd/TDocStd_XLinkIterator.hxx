// Created by: DAUTRY Philippe
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

#ifndef _TDocStd_XLinkIterator_HeaderFile
#define _TDocStd_XLinkIterator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TDocStd_XLinkPtr.hxx>
#include <Standard_Boolean.hxx>
class TDocStd_Document;

//! Iterates on Reference attributes.
//! This is an iterator giving all the external references
//! of a Document.
class TDocStd_XLinkIterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns an empty iterator;
  Standard_EXPORT TDocStd_XLinkIterator();

  //! Creates an iterator on Reference of <D>.
  Standard_EXPORT TDocStd_XLinkIterator(const Handle(TDocStd_Document)& D);

  //! Restarts an iteration with <D>.
  Standard_EXPORT void Initialize(const Handle(TDocStd_Document)& D);

  //! Returns True if there is a current Item in the
  //! iteration.
  Standard_Boolean More() const;

  //! Move to the next item; raises if there is no more item.
  Standard_EXPORT void Next();

  //! Returns the current item; a null handle if there is none.
  TDocStd_XLinkPtr Value() const;

protected:
private:
  Standard_EXPORT void Init(const Handle(TDocStd_Document)& D);

  TDocStd_XLinkPtr myValue;
};

#include <TDocStd_XLinkIterator.lxx>

#endif // _TDocStd_XLinkIterator_HeaderFile
