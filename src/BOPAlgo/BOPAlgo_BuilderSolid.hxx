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

#ifndef _BOPAlgo_BuilderSolid_HeaderFile
#define _BOPAlgo_BuilderSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Solid.hxx>
#include <BOPAlgo_BuilderArea.hxx>
#include <BOPCol_BaseAllocator.hxx>
class TopoDS_Solid;


//! The algorithm to build solids from set of faces
class BOPAlgo_BuilderSolid  : public BOPAlgo_BuilderArea
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_BuilderSolid();
Standard_EXPORT virtual ~BOPAlgo_BuilderSolid();
  
  Standard_EXPORT BOPAlgo_BuilderSolid(const BOPCol_BaseAllocator& theAllocator);
  
  //! Sets the source solid <theSolid>
  Standard_EXPORT void SetSolid (const TopoDS_Solid& theSolid);
  
  //! Returns the source solid
  Standard_EXPORT const TopoDS_Solid& Solid() const;
  
  //! Performs the algorithm
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;

protected:
  
  //! Collect the faces that
  //! a) are internal
  //! b) are the same and have different orientation
  Standard_EXPORT virtual void PerformShapesToAvoid() Standard_OVERRIDE;
  
  //! Build draft shells
  //! a)myLoops - draft shells that consist of
  //! boundary faces
  //! b)myLoopsInternal - draft shells that contains
  //! inner faces
  Standard_EXPORT virtual void PerformLoops() Standard_OVERRIDE;
  
  //! Build draft solids that contains boundary faces
  Standard_EXPORT virtual void PerformAreas() Standard_OVERRIDE;
  
  //! Build finalized solids with internal shells
  Standard_EXPORT virtual void PerformInternalShapes() Standard_OVERRIDE;


  TopoDS_Solid mySolid;


private:





};







#endif // _BOPAlgo_BuilderSolid_HeaderFile
