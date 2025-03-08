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

//      	-------------------
// Version:	0.0
// Version	Date		Purpose
//		0.0	Nov  4 1997	Creation

#include <DDF_Transaction.hxx>
#include <Standard_Type.hxx>
#include <TDF_Data.hxx>
#include <TDF_Delta.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DDF_Transaction, Standard_Transient)

//=================================================================================================

DDF_Transaction::DDF_Transaction()
    : myTransaction(TCollection_AsciiString())
{
}

//=================================================================================================

DDF_Transaction::DDF_Transaction(const Handle(TDF_Data)& aDF)
    : myTransaction(TCollection_AsciiString())
{
  myTransaction.Initialize(aDF);
}

//=================================================================================================

Standard_Integer DDF_Transaction::Open()
{
  return myTransaction.Open();
}

//=================================================================================================

Handle(TDF_Delta) DDF_Transaction::Commit(const Standard_Boolean withDelta)
{
  return myTransaction.Commit(withDelta);
}

//=================================================================================================

void DDF_Transaction::Abort()
{
  myTransaction.Abort();
}

//=================================================================================================

Handle(TDF_Data) DDF_Transaction::Data() const
{
  return myTransaction.Data();
}

//=================================================================================================

Standard_Integer DDF_Transaction::Transaction() const
{
  return myTransaction.Transaction();
}

//=================================================================================================

Standard_Boolean DDF_Transaction::IsOpen() const
{
  return myTransaction.IsOpen();
}
