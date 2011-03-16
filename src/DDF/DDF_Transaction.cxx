// File:	DDF_Transaction.cxx
//      	-------------------
// Author:	DAUTRY Philippe
//		<fid@fox.paris1.matra-dtv.fr>
// Copyright:	Matra Datavision 1997

// Version:	0.0
// History:	Version	Date		Purpose
//		0.0	Nov  4 1997	Creation



#include <DDF_Transaction.ixx>


//=======================================================================
//function : DDF_Transaction
//purpose  : 
//=======================================================================

DDF_Transaction::DDF_Transaction()
: myTransaction( TCollection_AsciiString() )
{}


//=======================================================================
//function : DDF_Transaction
//purpose  : 
//=======================================================================

DDF_Transaction::DDF_Transaction(const Handle(TDF_Data)& aDF)
: myTransaction( TCollection_AsciiString() )
{ myTransaction.Initialize(aDF); }


//=======================================================================
//function : Open
//purpose  : 
//=======================================================================

Standard_Integer DDF_Transaction::Open()
{ return myTransaction.Open(); }


//=======================================================================
//function : Commit
//purpose  : 
//=======================================================================

Handle(TDF_Delta) DDF_Transaction::Commit(const Standard_Boolean withDelta)
{ return myTransaction.Commit(withDelta); }


//=======================================================================
//function : Abort
//purpose  : alias ~
//=======================================================================

void DDF_Transaction::Abort()
{ myTransaction.Abort(); }


//=======================================================================
//function : Data
//purpose  : 
//=======================================================================

Handle(TDF_Data) DDF_Transaction::Data() const
{ return myTransaction.Data(); }


//=======================================================================
//function : Transaction
//purpose  : 
//=======================================================================

Standard_Integer DDF_Transaction::Transaction() const
{ return myTransaction.Transaction(); }


//=======================================================================
//function : IsOpen
//purpose  : 
//=======================================================================

Standard_Boolean DDF_Transaction::IsOpen() const
{ return myTransaction.IsOpen(); }

