// Created on: 1991-03-12
// Created by: Michel CHAUVAT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _GProp_HeaderFile
#define _GProp_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class gp_Pnt;
class gp_Mat;



//! This package defines algorithms to compute the global properties
//! of a set of points, a curve, a surface, a solid (non infinite
//! region of space delimited with geometric entities), a compound
//! geometric system (heterogeneous composition of the previous
//! entities).
//!
//! Global properties are :
//! . length, area, volume,
//! . centre of mass,
//! . axis of inertia,
//! . moments of inertia,
//! . radius of gyration.
//!
//! It provides  also a class to  compile the average point or
//! line of a set of points.
class GProp 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! methods of package
  //! Computes the matrix Operator, referred to as the
  //! "Huyghens Operator" of a geometric system at the
  //! point Q of the space, using the following data :
  //! - Mass, i.e. the mass of the system,
  //! - G, the center of mass of the system.
  //! The "Huyghens Operator" is used to compute
  //! Inertia/Q, the matrix of inertia of the system at
  //! the point Q using Huyghens' theorem :
  //! Inertia/Q = Inertia/G + HOperator (Q, G, Mass)
  //! where Inertia/G is the matrix of inertia of the
  //! system relative to its center of mass as returned by
  //! the function MatrixOfInertia on any GProp_GProps object.
  Standard_EXPORT static void HOperator (const gp_Pnt& G, const gp_Pnt& Q, const Standard_Real Mass, gp_Mat& Operator);




protected:





private:




friend class GProp_GProps;
friend class GProp_PGProps;
friend class GProp_CelGProps;
friend class GProp_SelGProps;
friend class GProp_VelGProps;
friend class GProp_PrincipalProps;
friend class GProp_PEquation;

};







#endif // _GProp_HeaderFile
