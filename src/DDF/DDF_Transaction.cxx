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

