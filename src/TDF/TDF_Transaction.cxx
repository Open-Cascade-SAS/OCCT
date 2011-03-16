// File:	TDF_Transaction.cxx
//      	-------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
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

