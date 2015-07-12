// Created on: 1992-01-30
// Created by: Didier PIFFAULT
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

#ifndef _TopTrans_SurfaceTransition_HeaderFile
#define _TopTrans_SurfaceTransition_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir.hxx>
#include <Standard_Real.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TopTrans_Array2OfOrientation.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
class gp_Dir;


//! This algorithm  is used to  compute the transition
//! of a 3D surface intersecting a topological surfacic
//! boundary on a 3D curve ( intersection curve ).
//! The  boundary is  described  by a  set of faces
//! each face is described by
//! - its support surface,
//! - an orientation defining its matter side.
//! The geometric elements are described locally at the
//! intersection point by a second order development.
//! A surface is described by the normal vector, the
//! principal directions and the principal curvatures.
//! A curve is described  by the  tangent, the normal
//! and the curvature.
//! The  algorithm  keeps track of the two faces elements
//! closest to the part of the curve "before" and "after"
//! the intersection,  these  two elements are updated
//! for each new face.
//! The position of the  curve can be computed when at
//! least  one surface  element has   been given, this
//! position is "In","Out" or "On" for the part of the
//! curve "Before" or "After" the intersection.
class TopTrans_SurfaceTransition 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create an empty Surface Transition.
  Standard_EXPORT TopTrans_SurfaceTransition();
  
  //! Initialize  a  Surface Transition with the local
  //! description of the intersection curve and of the
  //! reference surface.
  //! PREQUESITORY : Norm oriented OUTSIDE "geometric matter"
  Standard_EXPORT void Reset (const gp_Dir& Tgt, const gp_Dir& Norm, const gp_Dir& MaxD, const gp_Dir& MinD, const Standard_Real MaxCurv, const Standard_Real MinCurv);
  
  //! Initialize  a  Surface Transition  with the  local
  //! description of a straight line.
  Standard_EXPORT void Reset (const gp_Dir& Tgt, const gp_Dir& Norm);
  
  //! Add a face element to the boundary.
  //!
  //! - S defines topological orientation for the face :
  //! S FORWARD means: along the intersection curve on the
  //! reference surface, transition states while crossing
  //! the face are OUT,IN.
  //! S REVERSED means states are IN,OUT.
  //! S INTERNAL means states are IN,IN.
  //!
  //! - O defines curve's position on face :
  //! O FORWARD means the face is before the intersection
  //! O REVERSED means the face is AFTER
  //! O INTERNAL means the curve intersection is in the face.
  //! PREQUESITORY : Norm oriented OUTSIDE "geometric matter"
  Standard_EXPORT void Compare (const Standard_Real Tole, const gp_Dir& Norm, const gp_Dir& MaxD, const gp_Dir& MinD, const Standard_Real MaxCurv, const Standard_Real MinCurv, const TopAbs_Orientation S, const TopAbs_Orientation O);
  
  //! Add a plane or a cylindric face to the boundary.
  Standard_EXPORT void Compare (const Standard_Real Tole, const gp_Dir& Norm, const TopAbs_Orientation S, const TopAbs_Orientation O);
  
  //! Returns the state of the reference surface before
  //! the interference, this is the position relative to
  //! the surface of a  point very close to the intersection
  //! on the negative side of the tangent.
  Standard_EXPORT TopAbs_State StateBefore() const;
  
  //! Returns the state of the reference surface after
  //! interference, this is the position relative to the
  //! surface of a point very  close to the intersection
  //! on the positive side of the tangent.
  Standard_EXPORT TopAbs_State StateAfter() const;
  
  Standard_EXPORT static TopAbs_State GetBefore (const TopAbs_Orientation Tran);
  
  Standard_EXPORT static TopAbs_State GetAfter (const TopAbs_Orientation Tran);




protected:





private:

  
  Standard_EXPORT void UpdateReference (const Standard_Real Tole, const Standard_Boolean isInfRef, Standard_Real& CosInf, Standard_Real& CosSup, const TopAbs_Orientation Tran, TopAbs_Orientation& TranRef);
  
  Standard_EXPORT Standard_Real ComputeCos (const Standard_Real Tole, const gp_Dir& Norm, const TopAbs_Orientation O, Standard_Boolean& isleft) const;


  gp_Dir myTgt;
  gp_Dir myNorm;
  gp_Dir beafter;
  Standard_Real myCurvRef;
  TColStd_Array2OfReal myAng;
  TColStd_Array2OfReal myCurv;
  TopTrans_Array2OfOrientation myOri;
  Standard_Boolean myTouchFlag;


};







#endif // _TopTrans_SurfaceTransition_HeaderFile
