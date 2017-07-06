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

#ifndef _DDF_Transaction_HeaderFile
#define _DDF_Transaction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Transaction.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_DomainError;
class Standard_NullObject;
class TDF_Data;
class TDF_Delta;


class DDF_Transaction;
DEFINE_STANDARD_HANDLE(DDF_Transaction, Standard_Transient)

//! This class encapsulates TDF_Transaction.
class DDF_Transaction : public Standard_Transient
{

public:

  
  //! Creates an empty transaction context, unable to be
  //! opened.
  Standard_EXPORT DDF_Transaction();
  
  //! Creates a transaction context on <aDF>, ready to
  //! be opened.
  Standard_EXPORT DDF_Transaction(const Handle(TDF_Data)& aDF);
  
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
  Standard_EXPORT Handle(TDF_Delta) Commit (const Standard_Boolean withDelta = Standard_False);
  
  //! Aborts the transactions until AND including the
  //! current opened one.
  Standard_EXPORT void Abort();
~DDF_Transaction()
{
  Abort();
}
  
  //! Returns the Data from TDF.
  Standard_EXPORT Handle(TDF_Data) Data() const;
  
  //! Returns the number of the transaction opened by <me>.
  Standard_EXPORT Standard_Integer Transaction() const;
  
  //! Returns true if the transaction is open.
  Standard_EXPORT Standard_Boolean IsOpen() const;



  DEFINE_STANDARD_RTTIEXT(DDF_Transaction,Standard_Transient)

protected:




private:


  TDF_Transaction myTransaction;


};







#endif // _DDF_Transaction_HeaderFile
