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

#include <ShapeProcess.hxx>

#include <NCollection_DataMap.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressScope.hxx>
#include <ShapeProcess_Context.hxx>
#include <ShapeProcess_Operator.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TCollection_AsciiString.hxx>

static NCollection_DataMap<TCollection_AsciiString, Handle(ShapeProcess_Operator)> aMapOfOperators;

namespace
{
//! Simple RAII class to lock the scope of the current operation.
class ScopeLock
{
public:
  //! Constructor.
  //! Locks the scope of the current operation.
  //! @param theContext the context to lock.
  //! @param theScopeName the name of the scope to lock.
  ScopeLock(ShapeProcess_Context& theContext, const char* theScopeName)
      : myContext(theContext)
  {
    myContext.SetScope(theScopeName);
  }

  //! Destructor.
  //! Unlocks the scope of the current operation.
  ~ScopeLock() { myContext.UnSetScope(); }

private:
  ShapeProcess_Context& myContext; //!< The context to lock.
};
} // namespace

//=================================================================================================

Standard_Boolean ShapeProcess::RegisterOperator(const Standard_CString               name,
                                                const Handle(ShapeProcess_Operator)& op)
{
  if (aMapOfOperators.IsBound(name))
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: operator with name " << name << " is already registered!" << std::endl;
#endif
    return Standard_False;
  }
  aMapOfOperators.Bind(name, op);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean ShapeProcess::FindOperator(const Standard_CString         name,
                                            Handle(ShapeProcess_Operator)& op)
{
  if (!aMapOfOperators.IsBound(name))
  {
#ifdef OCCT_DEBUG
    std::cout << "Error: no operator with name " << name << " registered!" << std::endl;
#endif
    return Standard_False;
  }
  op = aMapOfOperators.ChangeFind(name);
  return !op.IsNull();
}

//=================================================================================================

Standard_Boolean ShapeProcess::Perform(const Handle(ShapeProcess_Context)& context,
                                       const Standard_CString              seq,
                                       const Message_ProgressRange&        theProgress)
{
  ScopeLock aSequenceScope(*context, seq);

  // get description of the sequence
  TCollection_AsciiString sequence;
  if (!context->GetString("exec.op", sequence))
  {
#ifdef OCCT_DEBUG
    std::cout << "Error: ShapeProcess_Performer::Perform: sequence not defined for " << seq
              << std::endl;
#endif
    if (context->TraceLevel() > 0)
    {
      Message_Msg SMSG3("SP.Sequence.Warn.NoSeq"); // Sequence %s not found
      context->Messenger()->Send(SMSG3 << seq, Message_Warning);
    }
    return Standard_False;
  }
  TColStd_SequenceOfAsciiString sequenceOfOperators;
  TCollection_AsciiString       oper;
  Standard_Integer              i;
  for (i = 1;; i++)
  {
    oper = sequence.Token(" \t,;", i);
    if (oper.Length() <= 0)
      break;
    sequenceOfOperators.Append(oper);
  }

  // put a message
  if (context->TraceLevel() >= 2)
  {
    Message_Msg             SMSG0("SP.Sequence.Info.Seq"); // Sequence of operators: %s
    TCollection_AsciiString Seq;
    for (Standard_Integer i1 = 1; i1 <= sequenceOfOperators.Length(); i1++)
    {
      if (i1 > 1)
        Seq += ",";
      Seq += sequenceOfOperators.Value(i1);
    }
    SMSG0.Arg(Seq.ToCString());
    context->Messenger()->Send(SMSG0, Message_Info);
  }

  // iterate on operators in the sequence
  Standard_Boolean      isDone = Standard_False;
  Message_ProgressScope aPS(theProgress, NULL, sequenceOfOperators.Length());
  for (i = 1; i <= sequenceOfOperators.Length() && aPS.More(); i++)
  {
    oper                         = sequenceOfOperators.Value(i);
    Message_ProgressRange aRange = aPS.Next();

    if (context->TraceLevel() >= 2)
    {
      Message_Msg SMSG5("SP.Sequence.Info.Operator"); // Operator %d/%d: %s
      SMSG5 << i << sequenceOfOperators.Length() << oper.ToCString();
      context->Messenger()->Send(SMSG5, Message_Alarm);
    }

    Handle(ShapeProcess_Operator) op;
    if (!ShapeProcess::FindOperator(oper.ToCString(), op))
    {
      if (context->TraceLevel() > 0)
      {
        Message_Msg SMSG1("SP.Sequence.Error.NoOp"); // Operator %s not found
        context->Messenger()->Send(SMSG1 << oper, Message_Alarm);
      }
      continue;
    }

    ScopeLock anOperationScope(*context, oper.ToCString());
    try
    {
      OCC_CATCH_SIGNALS
      if (op->Perform(context, aRange))
        isDone = Standard_True;
    }
    catch (Standard_Failure const& anException)
    {
      Message_Msg SMSG2("SP.Sequence.Error.Except"); // Operator %s failed with exception %s
      SMSG2 << oper << anException.GetMessageString();
      context->Messenger()->Send(SMSG2, Message_Alarm);
    }
  }

  return isDone;
}

//=================================================================================================

Standard_Boolean ShapeProcess::Perform(const Handle(ShapeProcess_Context)& theContext,
                                       const OperationsFlags&              theOperations,
                                       const Message_ProgressRange&        theProgress)
{
  if (!theContext)
  {
    return Standard_False;
  }

  std::vector<std::pair<const char*, Handle(ShapeProcess_Operator)>> anOperators =
    getOperators(theOperations);
  if (anOperators.empty())
  {
    return Standard_False;
  }

  Standard_Boolean      anIsAnySuccess = Standard_False;
  Message_ProgressScope aProgressScope(theProgress,
                                       nullptr,
                                       static_cast<Standard_Real>(anOperators.size()));
  for (const auto& anOperator : anOperators)
  {
    const char*                          anOperationName = anOperator.first;
    const Handle(ShapeProcess_Operator)& anOperation     = anOperator.second;
    Message_ProgressRange                aProgressRange  = aProgressScope.Next();
    ScopeLock anOperationScope(*theContext, anOperationName); // Set operation scope.
    try
    {
      OCC_CATCH_SIGNALS;
      anIsAnySuccess |= anOperation->Perform(theContext, aProgressRange);
    }
    catch (const Standard_Failure& anException)
    {
      Message_Msg aMessage("SP.Sequence.Error.Except"); // Operator %s failed with exception %s
      aMessage << anOperationName << anException.GetMessageString();
      theContext->Messenger()->Send(aMessage, Message_Alarm);
    }
  }
  return anIsAnySuccess;
}

//=================================================================================================

std::pair<ShapeProcess::Operation, bool> ShapeProcess::ToOperationFlag(const char* theName)
{
  if (!theName)
  {
    return {Operation::First, false};
  }

  for (std::underlying_type<Operation>::type anOperation = Operation::First;
       anOperation <= Operation::Last;
       ++anOperation)
  {
    const char* anOperationName = toOperationName(static_cast<Operation>(anOperation));
    if (anOperationName && !strcmp(theName, anOperationName))
    {
      return {static_cast<Operation>(anOperation), true};
    }
  }
  return {Operation::First, false};
}

//=================================================================================================

std::vector<std::pair<const char*, Handle(ShapeProcess_Operator)>> ShapeProcess::getOperators(
  const OperationsFlags& theFlags)
{
  std::vector<std::pair<const char*, Handle(ShapeProcess_Operator)>> aResult;
  for (std::underlying_type<Operation>::type anOperation = Operation::First;
       anOperation <= Operation::Last;
       ++anOperation)
  {
    if (theFlags.test(anOperation))
    {
      const char* anOperationName = toOperationName(static_cast<Operation>(anOperation));
      if (!anOperationName)
      {
        continue;
      }
      Handle(ShapeProcess_Operator) anOperator;
      if (FindOperator(anOperationName, anOperator))
      {
        aResult.emplace_back(anOperationName, anOperator);
      }
    }
  }
  return aResult;
}

//=================================================================================================

const char* ShapeProcess::toOperationName(const Operation theOperation)
{
  switch (theOperation)
  {
    case Operation::DirectFaces:
      return "DirectFaces";

    case Operation::SameParameter:
      return "SameParameter";

    case Operation::SetTolerance:
      return "SetTolerance";

    case Operation::SplitAngle:
      return "SplitAngle";

    case Operation::BSplineRestriction:
      return "BSplineRestriction";

    case Operation::ElementaryToRevolution:
      return "ElementaryToRevolution";

    case Operation::SweptToElementary:
      return "SweptToElementary";

    case Operation::SurfaceToBSpline:
      return "SurfaceToBSpline";

    case Operation::ToBezier:
      return "ToBezier";

    case Operation::SplitContinuity:
      return "SplitContinuity";

    case Operation::SplitClosedFaces:
      return "SplitClosedFaces";

    case Operation::FixWireGaps:
      return "FixWireGaps";

    case Operation::FixFaceSize:
      return "FixFaceSize";

    case Operation::DropSmallSolids:
      return "DropSmallSolids";

    case Operation::DropSmallEdges:
      return "DropSmallEdges";

    case Operation::FixShape:
      return "FixShape";

    case Operation::SplitClosedEdges:
      return "SplitClosedEdges";

    case Operation::SplitCommonVertex:
      return "SplitCommonVertex";
  }
  return nullptr;
}
