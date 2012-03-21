// Created on: 2000-08-21
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <ShapeProcess.ixx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

#include <Message_Msg.hxx>
#include <Message_Messenger.hxx>

#include <ShapeProcess_Operator.hxx>
#include <ShapeProcess_DictionaryOfOperator.hxx>

static Handle(ShapeProcess_DictionaryOfOperator) dic;

//=======================================================================
//function : RegisterOperator
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess::RegisterOperator (const Standard_CString name,
                                                 const Handle(ShapeProcess_Operator)& op)
{
  if ( dic.IsNull() ) dic = new ShapeProcess_DictionaryOfOperator;
  if ( dic->HasItem ( name, Standard_True ) ) {
#ifdef DEB
    cout << "Warning: operator with name " << name << " is already registered!" << endl;
#endif
    return Standard_False;
  }
  dic->SetItem ( name, op );
  return Standard_True;
}

//=======================================================================
//function : FindOperator
//purpose  : 
//=======================================================================

Standard_Boolean ShapeProcess::FindOperator (const Standard_CString name,
                                             Handle(ShapeProcess_Operator)& op)
{
  if ( dic.IsNull() ) dic = new ShapeProcess_DictionaryOfOperator;
  if ( ! dic->HasItem ( name, Standard_True ) ) {
#ifdef DEB
    cout << "Error: no operator with name " << name << " registered!" << endl;
#endif
    return Standard_False;
  }
  op = dic->Item ( name );
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
#ifdef DEB
    cout << "Error: ShapeProcess_Performer::Perform: sequence not defined for " << seq << endl;
#endif
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
    Message_Msg SMSG0 ("Sequence.MSG0"); //Sequence of operators: %s
    TCollection_AsciiString Seq;
    for ( Standard_Integer i1=1; i1 <= sequenceOfOperators.Length(); i1++ ) {
      if (i1 > 1) Seq += ",";
      Seq += sequenceOfOperators.Value(i1);
    }
    SMSG0.Arg (Seq.ToCString());
    context->Messenger()->Send (SMSG0, Message_Info);
  }

  // iterate on operators in the sequence
  for (i=1; i<=sequenceOfOperators.Length(); i++) {
    oper = sequenceOfOperators.Value(i);
    
    if ( context->TraceLevel() >=2 ) {
      Message_Msg SMSG5 ("Sequence.MSG5"); //Operator %d/%d: %s
      SMSG5 << i << sequenceOfOperators.Length() << oper.ToCString();
      context->Messenger()->Send (SMSG5, Message_Alarm);
    }
    
    Handle(ShapeProcess_Operator) op;
    if ( ! ShapeProcess::FindOperator ( oper.ToCString(), op ) ) {
      if ( context->TraceLevel() >0 ) {
        Message_Msg SMSG1 ("Sequence.MSG1"); //Operator %s not found
        context->Messenger()->Send (SMSG1 << oper, Message_Alarm);
      }
      continue;
    }
    
    context->SetScope ( oper.ToCString() );
    try {
      OCC_CATCH_SIGNALS
      if ( !op->Perform(context) )
        return Standard_False;
    }
    catch (Standard_Failure) {
      Message_Msg SMSG2 ("Sequence.MSG2"); //Operator %s failed with exception %s
      SMSG2 << oper << Standard_Failure::Caught()->GetMessageString();
      context->Messenger()->Send (SMSG2, Message_Alarm);
    }
    context->UnSetScope();
  }
  
  context->UnSetScope();
  return Standard_True;
}
