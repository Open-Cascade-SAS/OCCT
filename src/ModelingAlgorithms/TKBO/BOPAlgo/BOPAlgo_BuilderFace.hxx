// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef _BOPAlgo_BuilderFace_HeaderFile
#define _BOPAlgo_BuilderFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>
#include <BOPAlgo_BuilderArea.hxx>
#include <NCollection_BaseAllocator.hxx>

//! The algorithm to build new faces from the given faces and
//! set of edges lying on this face.
//!
//! The algorithm returns the following Error statuses:
//! - *BOPAlgo_AlertNullInputShapes* - in case the given face is a null shape.
//!
class BOPAlgo_BuilderFace : public BOPAlgo_BuilderArea
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BOPAlgo_BuilderFace();
  Standard_EXPORT virtual ~BOPAlgo_BuilderFace();

  Standard_EXPORT BOPAlgo_BuilderFace(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Sets the face generatix
  Standard_EXPORT void SetFace(const TopoDS_Face& theFace);

  //! Returns the face generatix
  Standard_EXPORT const TopoDS_Face& Face() const;

  //! Performs the algorithm
  Standard_EXPORT virtual void Perform(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  Standard_EXPORT TopAbs_Orientation Orientation() const;

protected:
  //! Collect the edges that
  //! a) are internal
  //! b) are the same and have different orientation
  Standard_EXPORT virtual void PerformShapesToAvoid(const Message_ProgressRange& theRange)
    Standard_OVERRIDE;

  //! Build draft wires
  //! a)myLoops - draft wires that consist of
  //! boundary edges
  //! b)myLoopsInternal - draft wires that contains
  //! inner edges
  Standard_EXPORT virtual void PerformLoops(const Message_ProgressRange& theRange)
    Standard_OVERRIDE;

  //! Build draft faces that contains boundary edges
  Standard_EXPORT virtual void PerformAreas(const Message_ProgressRange& theRange)
    Standard_OVERRIDE;

  //! Build finalized faces with internals
  Standard_EXPORT virtual void PerformInternalShapes(const Message_ProgressRange& theRange)
    Standard_OVERRIDE;

  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;

protected:
  TopoDS_Face        myFace;
  TopAbs_Orientation myOrientation;
};

#endif // _BOPAlgo_BuilderFace_HeaderFile
