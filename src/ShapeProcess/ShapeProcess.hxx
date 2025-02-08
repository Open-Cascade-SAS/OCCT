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

#ifndef _ShapeProcess_HeaderFile
#define _ShapeProcess_HeaderFile

#include <Message_ProgressRange.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <bitset>
#include <vector>

class ShapeProcess_Operator;
class ShapeProcess_Context;

//! Shape Processing module
//! allows to define and apply general Shape Processing as a
//! customizable sequence of Shape Healing operators. The
//! customization is implemented via user-editable resource
//! file which defines sequence of operators to be executed
//! and their parameters.
class ShapeProcess
{
public:
  DEFINE_STANDARD_ALLOC

  //! Describes all available operations.
  //! C++11 enum class is not used to allow implicit conversion to underlying type.
  enum Operation : uint8_t
  {
    First       = 0, // First operation index.
    DirectFaces = First,
    SameParameter,
    SetTolerance,
    SplitAngle,
    BSplineRestriction,
    ElementaryToRevolution,
    SweptToElementary,
    SurfaceToBSpline,
    ToBezier,
    SplitContinuity,
    SplitClosedFaces,
    FixWireGaps,
    FixFaceSize,
    DropSmallSolids,
    DropSmallEdges,
    FixShape,
    SplitClosedEdges,
    SplitCommonVertex,
    Last = SplitCommonVertex // Last operation index.
  };

  // Bitset of operations. It is used to specify which operations should be performed.
  // For example, to perform DirectFaces and SameParameter operations, use:
  // ShapeProcess::OperationsFlags flags;
  // flags.set(ShapeProcess::Operation::DirectFaces);
  // flags.set(ShapeProcess::Operation::SameParameter);
  // ShapeProcess::Perform(context, flags);
  using OperationsFlags = std::bitset<Operation::Last + 1>;

public:
  //! Registers operator to make it visible for Performer
  Standard_EXPORT static Standard_Boolean RegisterOperator(const Standard_CString name,
                                                           const Handle(ShapeProcess_Operator)& op);

  //! Finds operator by its name
  Standard_EXPORT static Standard_Boolean FindOperator(const Standard_CString         name,
                                                       Handle(ShapeProcess_Operator)& op);

  //! Performs a specified sequence of operators on Context
  //! Resource file and other data should be already loaded
  //! to Context (including description of sequence seq)
  Standard_EXPORT static Standard_Boolean Perform(
    const Handle(ShapeProcess_Context)& context,
    const Standard_CString              seq,
    const Message_ProgressRange&        theProgress = Message_ProgressRange());

  //! Performs a specified sequence of operators on @p theContext.
  //! @param theContext Context to perform operations on. Contains the shape to process
  //!        and processing parameters. If processing parameters are not set, default values are
  //!        used. Parameters should be in a scope of operation, for example, instead of
  //!        "FromSTEP.FixShape.Tolerance3d"	we should use just "FixShape.Tolerance3d".
  //! @param theOperations Bitset of operations to perform.
  //! @param theProgress Progress indicator.
  //! @return true if at least one operation was performed, false otherwise.
  Standard_EXPORT static Standard_Boolean Perform(
    const Handle(ShapeProcess_Context)& theContext,
    const OperationsFlags&              theOperations,
    const Message_ProgressRange&        theProgress = Message_ProgressRange());

  //! Converts operation name to operation flag.
  //! @param theName Operation name.
  //! @return Operation flag and true if the operation name is valid, false otherwise.
  Standard_EXPORT static std::pair<Operation, bool> ToOperationFlag(const char* theName);

private:
  //! Returns operators to be performed according to the specified flags.
  //! @param theFlags Bitset of operations flags.
  //! @return List of operators to perform: pairs of operator name and operator handle.
  static std::vector<std::pair<const char*, Handle(ShapeProcess_Operator)>> getOperators(
    const OperationsFlags& theFlags);

  //! Converts operation flag to its name.
  //! @param theOperation Operation flag.
  //! @return Operation name.
  static const char* toOperationName(const Operation theOperation);
};

#endif // _ShapeProcess_HeaderFile
