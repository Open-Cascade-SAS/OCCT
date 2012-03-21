// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//      	-------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Oct  1 1997	Creation



#include <TDF_Transaction.ixx>

#undef DEB_TRANSACTION
#ifdef DEB
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

TDF_Transaction::TDF_Transaction(const TDF_Transaction& aTrans)
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
#ifdef DEB_TRANSACTION
  cout<<"Transaction "<<myName<<" opens #"<<myDF->Transaction()+1<<endl;
#endif
  if (IsOpen())
    Standard_DomainError::Raise("This transaction is already open.");
  if (myDF.IsNull())
    Standard_NullObject::Raise("Null TDF_Data.");
  return myUntilTransaction = myDF->OpenTransaction();
}


//=======================================================================
//function : Commit
//purpose  : 
//=======================================================================

Handle(TDF_Delta) TDF_Transaction::Commit(const Standard_Boolean withDelta)
{
#ifdef DEB_TRANSACTION
  cout<<"Transaction "<<myName<<" commits ";
#endif
  Handle(TDF_Delta) delta;
  if (IsOpen()) {
#ifdef DEB_TRANSACTION
    cout<<"from #"<<myDF->Transaction()<<" until #"<<myUntilTransaction<<" while current is #"<<myDF->Transaction()<<endl;
#endif
#ifdef DEB_TRANSACTION_DUMP
    cout<<"DF before commit"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
    Standard_Integer until = myUntilTransaction;
    myUntilTransaction = 0;
    delta = myDF->CommitUntilTransaction(until, withDelta);
#ifdef DEB_TRANSACTION_DUMP
    cout<<"DF after commit"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
  }
#ifdef DEB_TRANSACTION
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
#ifdef DEB_TRANSACTION
    cout<<"Transaction "<<myName<<" aborts from #"<<myDF->Transaction()<<" until #"<<myUntilTransaction<<" while current is #"<<myDF->Transaction()<<endl;
#endif
#ifdef DEB_TRANSACTION_DUMP
    cout<<"DF before abort"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
    myDF->AbortUntilTransaction(myUntilTransaction);
    myUntilTransaction = 0;
#ifdef DEB_TRANSACTION_DUMP
    cout<<"DF after abort"<<endl;
    TDF_Tool::DeepDump(cout,myDF);
#endif
  }
}

