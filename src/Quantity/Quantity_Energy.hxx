// Created on: 1994-02-08
// Created by: Gilles DEBARBOUILLE
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

#ifndef _Quantity_Energy_HeaderFile
#define _Quantity_Energy_HeaderFile

#include <Standard_Real.hxx>

//! Defined as the capacity of a system to do work.
//! In mechanical terms this can be quantified as the
//! product of a force and a distance moved by its point of
//! application.
//! In kinetic terms it is the product of half the mass
//! and the square of the speed .
//! In a rotating system it is the product of half the
//! moment of inertia and the square of the angular velocity.
//! In potential terms it is the product of a mass,
//! a height, and the free fall acceleration.
//! In molecular terms it is the sum of the kinetic and
//! potential energies of the molecules.
//! In electric terms it is the product of charge and
//! the electric potential traversed.
//! In relativistic terms it is the difference between
//! the observed mass and the rest mass of a body multiplied
//! by the square of the speed of light.
//! It is measured in joules (newton-metres).
Standard_DEPRECATED("This type is deprecated - Standard_Real should be used instead")
typedef Standard_Real Quantity_Energy;

#endif // _Quantity_Energy_HeaderFile
