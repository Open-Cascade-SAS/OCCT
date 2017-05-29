// Created by: Peter Kurnev
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

#ifndef _BOPAlgo_CheckerSI_HeaderFile
#define _BOPAlgo_CheckerSI_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <BOPAlgo_PaveFiller.hxx>


//! Checks the shape on self-interference.
//!
//! The algorithm can set the following errors:
//! - *BOPAlgo_AlertMultipleArguments* - The number of the input arguments is not one;
//! - *BOPALgo_ErrorIntersectionFailed* - The check has been aborted during intersection of sub-shapes.
//! In case the error has occurred during intersection of sub-shapes, i.e.
//! in BOPAlgo_PaveFiller::PerformInternal() method, the errors from this method
//! directly will be returned.

class BOPAlgo_CheckerSI  : public BOPAlgo_PaveFiller
{
public:

  DEFINE_STANDARD_ALLOC


  Standard_EXPORT BOPAlgo_CheckerSI();
  Standard_EXPORT virtual ~BOPAlgo_CheckerSI();
  
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
  //! Sets the level of checking shape on self-interference.<br>
  //! It defines which interferences will be checked:<br>
  //! 0 - only V/V;<br>
  //! 1 - V/V and V/E;<br>
  //! 2 - V/V, V/E and E/E;<br>
  //! 3 - V/V, V/E, E/E and V/F;<br>
  //! 4 - V/V, V/E, E/E, V/F and E/F;<br>
  //! 5 - V/V, V/E, E/E, V/F, E/F and F/F;<br>
  //! 6 - V/V, V/E, E/E, V/F, E/F, F/F and V/S;<br>
  //! 7 - V/V, V/E, E/E, V/F, E/F, F/F, V/S and E/S;<br>
  //! 8 - V/V, V/E, E/E, V/F, E/F, F/F, V/S, E/S and F/S;<br>
  //! 9 - V/V, V/E, E/E, V/F, E/F, F/F, V/S, E/S, F/S and S/S - all interferences (Default value)
  Standard_EXPORT void SetLevelOfCheck (const Standard_Integer theLevel);

protected:

  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Treats the intersection results
  Standard_EXPORT void PostTreat();

  Standard_EXPORT void CheckFaceSelfIntersection();

  //! Methods for intersection with solids

  //! Vertex/Solid intersection
  Standard_EXPORT virtual void PerformVZ();

  //! Edge/Solid intersection
  Standard_EXPORT virtual void PerformEZ();

  //! Face/Solid intersection
  Standard_EXPORT virtual void PerformFZ();

  //! Solid/Solid intersection
  Standard_EXPORT virtual void PerformZZ();

  //! Used for intersection of edges and faces with solids
  Standard_EXPORT virtual void PerformSZ(const TopAbs_ShapeEnum aTS);

  Standard_Integer myLevelOfCheck;

private:

};

#endif // _BOPAlgo_CheckerSI_HeaderFile
