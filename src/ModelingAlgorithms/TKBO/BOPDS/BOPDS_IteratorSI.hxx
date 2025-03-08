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

#ifndef _BOPDS_IteratorSI_HeaderFile
#define _BOPDS_IteratorSI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPDS_Iterator.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard_Integer.hxx>

//! The class BOPDS_IteratorSI is
//! 1.to compute self-intersections between BRep sub-shapes
//! of each argument of an operation (see the class BOPDS_DS)
//! in terms of theirs bounding boxes
//! 2.provides interface to iterare the pairs of
//! intersected sub-shapes of given type
class BOPDS_IteratorSI : public BOPDS_Iterator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BOPDS_IteratorSI();

  Standard_EXPORT virtual ~BOPDS_IteratorSI();

  //! Constructor
  //! @param theAllocator the allocator to manage the memory
  Standard_EXPORT BOPDS_IteratorSI(const Handle(NCollection_BaseAllocator)& theAllocator);

  //! Updates the lists of possible intersections
  //! according to the value of <theLevel>.
  //! It defines which interferferences will be checked:
  //! 0 - only V/V;
  //! 1 - V/V and V/E;
  //! 2 - V/V, V/E and E/E;
  //! 3 - V/V, V/E, E/E and V/F;
  //! 4 - V/V, V/E, E/E, V/F and E/F;
  //! other - all interferences.
  Standard_EXPORT void UpdateByLevelOfCheck(const Standard_Integer theLevel);

protected:
  Standard_EXPORT virtual void Intersect(
    const Handle(IntTools_Context)& theCtx        = Handle(IntTools_Context)(),
    const Standard_Boolean          theCheckOBB   = Standard_False,
    const Standard_Real             theFuzzyValue = Precision::Confusion()) Standard_OVERRIDE;

private:
};

#endif // _BOPDS_IteratorSI_HeaderFile
