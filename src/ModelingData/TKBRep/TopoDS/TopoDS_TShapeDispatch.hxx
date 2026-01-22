// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _TopoDS_TShapeDispatch_HeaderFile
#define _TopoDS_TShapeDispatch_HeaderFile

#include <TopoDS_TShape.hxx>
#include <TopoDS_TEdge.hxx>
#include <TopoDS_TWire.hxx>
#include <TopoDS_TFace.hxx>
#include <TopoDS_TShell.hxx>
#include <TopoDS_TSolid.hxx>
#include <TopoDS_TCompSolid.hxx>
#include <TopoDS_TCompound.hxx>

//! @brief Type-switch dispatch helper for TopoDS_TShape hierarchy.
//!
//! This helper provides std::visit-like functionality for TShape types,
//! enabling devirtualization of NbChildren() and GetChild() calls when
//! combined with the 'final' specifier on derived class methods.
//!
//! @section Usage Example
//! @code
//! TopoDS_TShape* aTShape = ...;
//! int aNbChildren = TopoDS_TShapeDispatch::Apply(aTShape,
//!   [](auto* theTyped) { return theTyped->NbChildren(); });
//! @endcode
//!
//! The lambda receives a pointer to the concrete type (TopoDS_TEdge*, etc.),
//! allowing the compiler to devirtualize calls to 'final' methods.
class TopoDS_TShapeDispatch
{
public:
  //! Dispatch a functor based on TShape type (non-const version).
  //! @tparam Func callable type (lambda, function object)
  //! @param theTShape pointer to TShape (non-null)
  //! @param theFunc functor to apply, receives concrete type pointer
  //! @return result of theFunc invocation
  //! @note TopoDS_TEdge* is used for return type deduction as a representative
  //!       derived type. Cannot use base class since lambdas may access
  //!       members (mySubShapes) that only exist in derived classes.
  template <typename Func>
  static auto Apply(TopoDS_TShape* theTShape, Func&& theFunc)
      -> decltype(theFunc(static_cast<TopoDS_TEdge*>(nullptr)))
  {
    using ReturnType = decltype(theFunc(static_cast<TopoDS_TEdge*>(nullptr)));
    switch (theTShape->ShapeType())
    {
      case TopAbs_EDGE:
        return theFunc(static_cast<TopoDS_TEdge*>(theTShape));
      case TopAbs_WIRE:
        return theFunc(static_cast<TopoDS_TWire*>(theTShape));
      case TopAbs_FACE:
        return theFunc(static_cast<TopoDS_TFace*>(theTShape));
      case TopAbs_SHELL:
        return theFunc(static_cast<TopoDS_TShell*>(theTShape));
      case TopAbs_SOLID:
        return theFunc(static_cast<TopoDS_TSolid*>(theTShape));
      case TopAbs_COMPSOLID:
        return theFunc(static_cast<TopoDS_TCompSolid*>(theTShape));
      case TopAbs_COMPOUND:
        return theFunc(static_cast<TopoDS_TCompound*>(theTShape));
      default:
        // Vertex has no children - return default-constructed value
        return ReturnType{};
    }
  }

  //! Dispatch with cached shape type (avoids re-reading ShapeType()).
  //! Use when you've already retrieved the shape type.
  //! @tparam Func callable type
  //! @param theTShape pointer to TShape (non-null)
  //! @param theType cached shape type
  //! @param theFunc functor to apply
  //! @return result of theFunc invocation
  template <typename Func>
  static auto ApplyWithType(TopoDS_TShape* theTShape, TopAbs_ShapeEnum theType, Func&& theFunc)
      -> decltype(theFunc(static_cast<TopoDS_TEdge*>(nullptr)))
  {
    using ReturnType = decltype(theFunc(static_cast<TopoDS_TEdge*>(nullptr)));
    switch (theType)
    {
      case TopAbs_EDGE:
        return theFunc(static_cast<TopoDS_TEdge*>(theTShape));
      case TopAbs_WIRE:
        return theFunc(static_cast<TopoDS_TWire*>(theTShape));
      case TopAbs_FACE:
        return theFunc(static_cast<TopoDS_TFace*>(theTShape));
      case TopAbs_SHELL:
        return theFunc(static_cast<TopoDS_TShell*>(theTShape));
      case TopAbs_SOLID:
        return theFunc(static_cast<TopoDS_TSolid*>(theTShape));
      case TopAbs_COMPSOLID:
        return theFunc(static_cast<TopoDS_TCompSolid*>(theTShape));
      case TopAbs_COMPOUND:
        return theFunc(static_cast<TopoDS_TCompound*>(theTShape));
      default:
        return ReturnType{};
    }
  }
};

#endif // _TopoDS_TShapeDispatch_HeaderFile
