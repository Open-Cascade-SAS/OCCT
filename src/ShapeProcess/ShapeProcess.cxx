// Created on: 2000-08-21
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <NCollection_DataMap.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <ShapeProcess.hxx>
#include <ShapeProcess_Context.hxx>
#include <ShapeProcess_Operator.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

static NCollection_DataMap<TCollection_AsciiString, Handle(ShapeProcess_Operator)> aMapOfOperators;
//=======================================================================
//function : RegisterOperator
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess::RegisterOperator (const Standard_CString name,
                                                 const Handle(ShapeProcess_Operator)& op)
{
  if (aMapOfOperators.IsBound(name)) {
#ifdef OCCT_DEBUG
    cout << "Warning: operator with name " << name << " is already registered!" << endl;
#endif
    return Standard_False;
  }
  aMapOfOperators.Bind( name, op );
  return Standard_True;
}

//=======================================================================
//function : FindOperator
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess::FindOperator (const Standard_CString name,
                                             Handle(ShapeProcess_Operator)& op)
{
  if (!aMapOfOperators.IsBound(name)) {
#ifdef OCCT_DEBUG
    cout << "Error: no operator with name " << name << " registered!" << endl;
#endif
    return Standard_False;
  }
  op = aMapOfOperators.ChangeFind(name);
  return !op.IsNull();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess::Perform (const Handle(ShapeProcess_Context)& context,
                                        const Standard_CString seq)
{
  context->SetScope ( seq );
  
  // get description of the sequence
  TCollection_AsciiString sequence;
  if ( ! context->GetString ( "exec.op", sequence ) ) {
#ifdef OCCT_DEBUG
    cout << "Error: ShapeProcess_Performer::Perform: sequence not defined for " << seq << endl;
#endif
    if ( context->TraceLevel() >0 ) {
      Message_Msg SMSG3 ("SP.Sequence.Warn.NoSeq"); // Sequence %s not found
      context->Messenger()->Send (SMSG3 << seq, Message_Warning);
    }
    context->UnSetScope();
    return Standard_False;
  }
  TColStd_SequenceOfAsciiString sequenceOfOperators;
  TCollection_AsciiString oper;
  Standard_Integer i;
  for ( i=1; ; i++ ) {
    oper = sequence.Token ( " \t,;", i );
    if ( oper.Length() <=0 ) break;
    sequenceOfOperators.Append(oper);
  }
  
  // put a message
  if ( context->TraceLevel() >=2 ) {
    Message_Msg SMSG0 ("SP.Sequence.Info.Seq"); //Sequence of operators: %s
    TCollection_AsciiString Seq;
    for ( Standard_Integer i1=1; i1 <= sequenceOfOperators.Length(); i1++ ) {
      if (i1 > 1) Seq += ",";
      Seq += sequenceOfOperators.Value(i1);
    }
    SMSG0.Arg (Seq.ToCString());
    context->Messenger()->Send (SMSG0, Message_Info);
  }

  // iterate on operators in the sequence
  Standard_Boolean isDone = Standard_False;
  for (i=1; i<=sequenceOfOperators.Length(); i++) {
    oper = sequenceOfOperators.Value(i);
    
    if ( context->TraceLevel() >=2 ) {
      Message_Msg SMSG5 ("SP.Sequence.Info.Operator"); //Operator %d/%d: %s
      SMSG5 << i << sequenceOfOperators.Length() << oper.ToCString();
      context->Messenger()->Send (SMSG5, Message_Alarm);
    }
    
    Handle(ShapeProcess_Operator) op;
    if ( ! ShapeProcess::FindOperator ( oper.ToCString(), op ) ) {
      if ( context->TraceLevel() >0 ) {
        Message_Msg SMSG1 ("SP.Sequence.Error.NoOp"); //Operator %s not found
        context->Messenger()->Send (SMSG1 << oper, Message_Alarm);
      }
      continue;
    }
    
    context->SetScope ( oper.ToCString() );
    try {
      OCC_CATCH_SIGNALS
      if ( op->Perform(context) )
        isDone = Standard_True;
    }
    catch (Standard_Failure const& anException) {
      Message_Msg SMSG2 ("SP.Sequence.Error.Except"); //Operator %s failed with exception %s
      SMSG2 << oper << anException.GetMessageString();
      context->Messenger()->Send (SMSG2, Message_Alarm);
    }
    context->UnSetScope();
  }
  
  context->UnSetScope();
  return isDone;
}
