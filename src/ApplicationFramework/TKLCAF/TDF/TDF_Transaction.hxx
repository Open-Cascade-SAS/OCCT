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

#ifndef _TDF_Transaction_HeaderFile
#define _TDF_Transaction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
class TDF_Data;
class TDF_Delta;

//! This class offers services to open, commit or
//! abort a transaction in a more secure way than
//! using Data from TDF. If you forget to close a
//! transaction, it will be automatically aborted at
//! the destruction of this object, at the closure of
//! its scope.
//!
//! In case of catching errors, the effect will be the
//! same: aborting transactions until the good current
//! one.
class TDF_Transaction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty transaction context, unable to be
  //! opened.
  Standard_EXPORT TDF_Transaction(const TCollection_AsciiString& aName = "");

  //! Creates a transaction context on <aDF>, ready to
  //! be opened.
  Standard_EXPORT TDF_Transaction(const Handle(TDF_Data)&        aDF,
                                  const TCollection_AsciiString& aName = "");

  //! Aborts all the transactions on <myDF> and sets
  //! <aDF> to build a transaction context on <aDF>,
  //! ready to be opened.
  Standard_EXPORT void Initialize(const Handle(TDF_Data)& aDF);

  //! If not yet done, opens a new transaction on
  //! <myDF>. Returns the index of the just opened
  //! transaction.
  //!
  //! It raises DomainError if the transaction is
  //! already open, and NullObject if there is no
  //! current Data framework.
  Standard_EXPORT Standard_Integer Open();

  //! Commits the transactions until AND including the
  //! current opened one.
  Standard_EXPORT Handle(TDF_Delta) Commit(const Standard_Boolean withDelta = Standard_False);

  //! Aborts the transactions until AND including the
  //! current opened one.
  Standard_EXPORT void Abort();

  ~TDF_Transaction() { Abort(); }

  //! Returns the Data from TDF.
  Handle(TDF_Data) Data() const;

  //! Returns the number of the transaction opened by <me>.
  Standard_Integer Transaction() const;

  //! Returns the transaction name.
  const TCollection_AsciiString& Name() const;

  //! Returns true if the transaction is open.
  Standard_Boolean IsOpen() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:
  //! Private to avoid copy.
  TDF_Transaction(const TDF_Transaction& aTrans);
  TDF_Transaction& operator=(const TDF_Transaction& theOther);

private:
  Handle(TDF_Data)        myDF;
  TCollection_AsciiString myName;
  Standard_Integer        myUntilTransaction;
};

#include <TDF_Transaction.lxx>

#endif // _TDF_Transaction_HeaderFile
