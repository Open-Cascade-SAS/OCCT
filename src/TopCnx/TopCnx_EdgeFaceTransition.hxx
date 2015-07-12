// Created on: 1992-08-11
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _TopCnx_EdgeFaceTransition_HeaderFile
#define _TopCnx_EdgeFaceTransition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTrans_CurveTransition.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_Orientation.hxx>
class gp_Dir;


//! TheEdgeFaceTransition is an algorithm to   compute
//! the  cumulated  transition for interferences on an
//! edge.
class TopCnx_EdgeFaceTransition 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an empty algorithm.
  Standard_EXPORT TopCnx_EdgeFaceTransition();
  
  //! Initialize  the     algorithm    with the    local
  //! description of the edge.
  Standard_EXPORT void Reset (const gp_Dir& Tgt, const gp_Dir& Norm, const Standard_Real Curv);
  
  //! Initialize the algorithm with a linear Edge.
  Standard_EXPORT void Reset (const gp_Dir& Tgt);
  
  //! Add a curve  element to the  boundary.  Or  is the
  //! orientation of   the interference on  the boundary
  //! curve. Tr is  the transition  of the interference.
  //! BTr     is   the    boundary  transition    of the
  //! interference.
  Standard_EXPORT void AddInterference (const Standard_Real Tole, const gp_Dir& Tang, const gp_Dir& Norm, const Standard_Real Curv, const TopAbs_Orientation Or, const TopAbs_Orientation Tr, const TopAbs_Orientation BTr);
  
  //! Returns the current cumulated transition.
  Standard_EXPORT TopAbs_Orientation Transition() const;
  
  //! Returns the current cumulated BoundaryTransition.
  Standard_EXPORT TopAbs_Orientation BoundaryTransition() const;




protected:





private:



  TopTrans_CurveTransition myCurveTransition;
  Standard_Integer nbBoundForward;
  Standard_Integer nbBoundReversed;


};







#endif // _TopCnx_EdgeFaceTransition_HeaderFile
