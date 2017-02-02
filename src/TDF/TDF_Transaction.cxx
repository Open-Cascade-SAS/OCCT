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
//Version	Date		Purpose
//		0.0	Oct  1 1997	Creation

#include <Standard_DomainError.hxx>
#include <Standard_NullObject.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Data.hxx>
#include <TDF_Delta.hxx>
#include <TDF_Transaction.hxx>

#undef DEB_TRANSACTION
#ifdef OCCT_DEBUG
#define DEB_TRANSACTION
#endif
#undef DEB_TRANSACTION_DUMP

#include <TDF_Tool.hxx>

//=======================================================================
//function : TDF_Transaction
//purpose  : 
//=======================================================================

TDF_Transaction::TDF_Transaction
(const TCollection_AsciiString& aName)
: myUntilTransaction(0),
  myName(aName)
{}


//=======================================================================
//function : TDF_Transaction
//purpose  : Private copy constructor.
//=======================================================================

TDF_Transaction::TDF_Transaction(const TDF_Transaction& /*aTrans*/)
{}


//=======================================================================
//function : TDF_Transaction
//purpose  : 
//=======================================================================

TDF_Transaction::TDF_Transaction
(const Handle(TDF_Data)& aDF,
 const TCollection_AsciiString& aName)
: myDF(aDF),
  myUntilTransaction(0),
  myName(aName)
{}




//=======================================================================
//function : Initialize
//purpose  : Initializes a transaction ready to be opened.
//=======================================================================

void TDF_Transaction::Initialize(const Handle(TDF_Data)& aDF)
{
  if (IsOpen()) myDF->AbortUntilTransaction(myUntilTransaction);
  myDF = aDF;
  myUntilTransaction = 0;
}


//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

Standard_Integer TDF_Transaction::Open()
{
#ifdef OCCT_DEBUG_TRANSACTION
  cout<<"Transaction "<<myName<<" opens #"<<myDF->Transaction()+1<<endl;
#endif
  if (IsOpen())
    throw Standard_DomainError("This transaction is already open.");
  if (myDF.IsNull())
    throw Standard_NullObject("Null TDF_Data.");
  return myUntilTransaction = myDF->OpenTransaction();
}


//=======================================================================
//function : Commit
//purpose  : 
//=======================================================================

Handle(TDF_Delta) TDF_Transaction::Commit(const Standard_Boolean withDelta)
{
#ifdef OCCT_DEBUG_TRANSACTION
  cout<<"Transaction "<<myName<<" commits ";
#endif
  Handle(TDF_Delta) delta;
  if (IsOpen()) {
#ifdef OCCT_DEBUG_TRANSACTION
    cout<<"from #"<<myDF->Transaction()<<" until #"<<myUntilTransaction<<" while current is #"<<myDF->Transaction()<<endl;
#endif
#ifdef OCCT_DEBUG_TRANSACTION_DUMP
    cout<<"DF before commit"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
    Standard_Integer until = myUntilTransaction;
    myUntilTransaction = 0;
    delta = myDF->CommitUntilTransaction(until, withDelta);
#ifdef OCCT_DEBUG_TRANSACTION_DUMP
    cout<<"DF after commit"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
  }
#ifdef OCCT_DEBUG_TRANSACTION
  else cout<<"but this transaction is not open!"<<endl;
#endif
  return delta;
}


//=======================================================================
//function : Abort
//purpose  : alias ~
//=======================================================================

void TDF_Transaction::Abort()
{
  if (IsOpen()) {
#ifdef OCCT_DEBUG_TRANSACTION
    cout<<"Transaction "<<myName<<" aborts from #"<<myDF->Transaction()<<" until #"<<myUntilTransaction<<" while current is #"<<myDF->Transaction()<<endl;
#endif
#ifdef OCCT_DEBUG_TRANSACTION_DUMP
    cout<<"DF before abort"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
    myDF->AbortUntilTransaction(myUntilTransaction);
    myUntilTransaction = 0;
#ifdef OCCT_DEBUG_TRANSACTION_DUMP
    cout<<"DF after abort"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
  }
}

