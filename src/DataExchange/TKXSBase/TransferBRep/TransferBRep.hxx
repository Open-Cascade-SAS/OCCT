// Created on: 1994-10-03
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _TransferBRep_HeaderFile
#define _TransferBRep_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopAbs_Orientation.hxx>
#include <TransferBRep_TransferResultInfo.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;
class Transfer_Binder;
class Transfer_TransientProcess;
class Standard_Transient;
class Transfer_FinderProcess;
class TransferBRep_ShapeMapper;
class Message_Printer;
class Message_Msg;
class TransferBRep_TransferResultInfo;
class Interface_CheckIterator;
class Interface_InterfaceModel;

//! This package gathers services to simply read files and convert
//! them to Shapes from CasCade. IE. it can be used in conjunction
//! with purely CasCade software
class TransferBRep
{
public:
  DEFINE_STANDARD_ALLOC

  //! Get the Shape recorded in a Binder
  //! If the Binder brings a multiple result, search for the Shape
  Standard_EXPORT static TopoDS_Shape ShapeResult(const occ::handle<Transfer_Binder>& binder);

  //! Get the Shape recorded in a TransientProcess as result of the
  //! Transfer of an entity. I.E. in the binder bound to that Entity
  //! If no result or result not a single Shape, returns a Null Shape
  Standard_EXPORT static TopoDS_Shape ShapeResult(const occ::handle<Transfer_TransientProcess>& TP,
                                                  const occ::handle<Standard_Transient>&        ent);

  //! Sets a Shape as a result for a starting entity <ent>
  //! (reverse of ShapeResult)
  //! It simply creates a ShapeBinder then binds it to the entity
  Standard_EXPORT static void SetShapeResult(const occ::handle<Transfer_TransientProcess>& TP,
                                             const occ::handle<Standard_Transient>&        ent,
                                             const TopoDS_Shape&                      result);

  //! Gets the Shapes recorded in a TransientProcess as result of a
  //! Transfer, considers roots only or all results according
  //! <rootsonly>, returns them as a HSequence
  Standard_EXPORT static occ::handle<NCollection_HSequence<TopoDS_Shape>> Shapes(
    const occ::handle<Transfer_TransientProcess>& TP,
    const bool                   rootsonly = true);

  //! Gets the Shapes recorded in a TransientProcess as result of a
  //! Transfer, for a given list of starting entities, returns
  //! the shapes as a HSequence
  Standard_EXPORT static occ::handle<NCollection_HSequence<TopoDS_Shape>> Shapes(
    const occ::handle<Transfer_TransientProcess>&    TP,
    const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>& list);

  //! Returns a Status regarding a Shape in a FinderProcess
  //! - FORWARD means bound with SAME Orientation
  //! - REVERSED means bound with REVERSE Orientation
  //! - EXTERNAL means NOT BOUND
  //! - INTERNAL is not used
  Standard_EXPORT static TopAbs_Orientation ShapeState(const occ::handle<Transfer_FinderProcess>& FP,
                                                       const TopoDS_Shape&                   shape);

  //! Returns the result (as a Binder) attached to a given Shape
  //! Null if none
  Standard_EXPORT static occ::handle<Transfer_Binder> ResultFromShape(
    const occ::handle<Transfer_FinderProcess>& FP,
    const TopoDS_Shape&                   shape);

  //! Returns the result as pure Transient attached to a Shape
  //! first one if multiple result
  Standard_EXPORT static occ::handle<Standard_Transient> TransientFromShape(
    const occ::handle<Transfer_FinderProcess>& FP,
    const TopoDS_Shape&                   shape);

  //! Binds a Transient Result to a Shape in a FinderProcess
  //! (as first result if multiple : does not add it to existing one)
  Standard_EXPORT static void SetTransientFromShape(const occ::handle<Transfer_FinderProcess>& FP,
                                                    const TopoDS_Shape&                   shape,
                                                    const occ::handle<Standard_Transient>&     result);

  //! Returns a ShapeMapper for a given Shape (location included)
  //! Either <shape> is already mapped, then its Mapper is returned
  //! Or it is not, then a new one is created then returned, BUT
  //! it is not mapped here (use Bind or FindElseBind to do this)
  Standard_EXPORT static occ::handle<TransferBRep_ShapeMapper> ShapeMapper(
    const occ::handle<Transfer_FinderProcess>& FP,
    const TopoDS_Shape&                   shape);

  //! Fills sequence of TransferResultInfo for each type of entity
  //! given in the EntityTypes (entity are given as objects).
  //! Method IsKind applied to the entities in TP is used to
  //! compare with entities in EntityTypes.
  //! TopAbs_ShapeEnum).
  Standard_EXPORT static void TransferResultInfo(
    const occ::handle<Transfer_TransientProcess>&            TP,
    const occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>>&         EntityTypes,
    occ::handle<NCollection_HSequence<occ::handle<TransferBRep_TransferResultInfo>>>& InfoSeq);

  //! Fills sequence of TransferResultInfo for each type of shape
  //! given in the ShapeTypes (which are in fact considered as
  //! TopAbs_ShapeEnum).
  //! The Finders in the FP are considered as ShapeMappers.
  Standard_EXPORT static void TransferResultInfo(
    const occ::handle<Transfer_FinderProcess>&               FP,
    const occ::handle<NCollection_HSequence<int>>&           ShapeTypes,
    occ::handle<NCollection_HSequence<occ::handle<TransferBRep_TransferResultInfo>>>& InfoSeq);

  //! Prints the results of transfer to given priner with given header.
  Standard_EXPORT static void PrintResultInfo(
    const occ::handle<Message_Printer>&                 Printer,
    const Message_Msg&                             Header,
    const occ::handle<TransferBRep_TransferResultInfo>& ResultInfo,
    const bool                         printEmpty = true);

  //! Performs a heavy check by calling the Analyser from BRepCheck
  //! This tool computes a lot of information about integrity of a
  //! Shape. This method uses it and converts its internal result
  //! to a classic check-list.
  //! <lev> allows to get more information :
  //! 0 : BRepCheck only
  //! 1(D) + Curves/Surfaces not C0  ;  2 + SameParameter on Edges
  //! Warning : entities to which checks are bound are the Shapes themselves,
  //! embedded in ShapeMapper
  Standard_EXPORT static Interface_CheckIterator BRepCheck(const TopoDS_Shape&    shape,
                                                           const int lev = 1);

  //! Takes a starting CheckIterator which brings checks bound with
  //! starting objects (Shapes, Transient from an Imagine appli ...)
  //! and converts it to a CheckIterator in which checks are bound
  //! with results in an InterfaceModel
  //! Mapping is recorded in the FinderProcess
  //! Starting objects for which no individual result is recorded
  //! remain in their state
  Standard_EXPORT static Interface_CheckIterator ResultCheckList(
    const Interface_CheckIterator&          chl,
    const occ::handle<Transfer_FinderProcess>&   FP,
    const occ::handle<Interface_InterfaceModel>& model);

  //! Returns the list of objects to which a non-empty Check is
  //! bound in a check-list. Objects are transients, they can then
  //! be either Imagine objects entities for an Interface Norm.
  //! <alsoshapes> commands Shapes to be returned too
  //! (as ShapeMapper), see also CheckedShapes
  Standard_EXPORT static occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> Checked(
    const Interface_CheckIterator& chl,
    const bool         alsoshapes = false);

  //! Returns the list of shapes to which a non-empty Check is bound
  //! in a check-list
  Standard_EXPORT static occ::handle<NCollection_HSequence<TopoDS_Shape>> CheckedShapes(
    const Interface_CheckIterator& chl);

  //! Returns the check-list bound to a given object, generally none
  //! (if OK) or one check. <obj> can be, either a true Transient
  //! object or entity, or a ShapeMapper, in that case the Shape is
  //! considered
  Standard_EXPORT static Interface_CheckIterator CheckObject(const Interface_CheckIterator&    chl,
                                                             const occ::handle<Standard_Transient>& obj);
};

#endif // _TransferBRep_HeaderFile
