// Created on: 1993-04-13
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _gp_HeaderFile
#define _gp_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>

class gp_Pnt;
class gp_Dir;
class gp_Ax1;
class gp_Ax2;
class gp_Pnt2d;
class gp_Dir2d;
class gp_Ax2d;

//! The geometric processor package, called gp, provides an
//! implementation of entities used  :
//! . for algebraic calculation such as "XYZ" coordinates, "Mat"
//! matrix
//! . for basis analytic geometry such as Transformations, point,
//! vector, line, plane, axis placement, conics, and elementary
//! surfaces.
//! These entities are defined in 2d and 3d space.
//! All the classes of this package are non-persistent.
class gp
{
public:
  DEFINE_STANDARD_ALLOC

  //! Method of package gp
  //!
  //! In geometric computations, defines the tolerance criterion
  //! used to determine when two numbers can be considered equal.
  //! Many class functions use this tolerance criterion, for
  //! example, to avoid division by zero in geometric
  //! computations. In the documentation, tolerance criterion is
  //! always referred to as gp::Resolution().
  static Standard_Real Resolution() { return RealSmall(); }

  //! Identifies a Cartesian point with coordinates X = Y = Z = 0.0.0
  Standard_EXPORT static const gp_Pnt& Origin();

  //! Returns a unit vector with the combination (1,0,0)
  Standard_EXPORT static const gp_Dir& DX();

  //! Returns a unit vector with the combination (0,1,0)
  Standard_EXPORT static const gp_Dir& DY();

  //! Returns a unit vector with the combination (0,0,1)
  Standard_EXPORT static const gp_Dir& DZ();

  //! Identifies an axis where its origin is Origin
  //! and its unit vector coordinates  X = 1.0,  Y = Z = 0.0
  Standard_EXPORT static const gp_Ax1& OX();

  //! Identifies an axis where its origin is Origin
  //! and its unit vector coordinates Y = 1.0,  X = Z = 0.0
  Standard_EXPORT static const gp_Ax1& OY();

  //! Identifies an axis where its origin is Origin
  //! and its unit vector coordinates Z = 1.0,  Y = X = 0.0
  Standard_EXPORT static const gp_Ax1& OZ();

  //! Identifies a coordinate system where its origin is Origin,
  //! and its "main Direction" and "X Direction" coordinates
  //! Z = 1.0, X = Y =0.0 and X direction coordinates X = 1.0, Y = Z = 0.0
  Standard_EXPORT static const gp_Ax2& XOY();

  //! Identifies a coordinate system where its origin is Origin,
  //! and its "main Direction" and "X Direction" coordinates
  //! Y = 1.0, X = Z =0.0 and X direction coordinates Z = 1.0, X = Y = 0.0
  Standard_EXPORT static const gp_Ax2& ZOX();

  //! Identifies a coordinate system where its origin is Origin,
  //! and its "main Direction" and "X Direction" coordinates
  //! X = 1.0, Z = Y =0.0 and X direction coordinates Y = 1.0, X = Z = 0.0
  //! In 2D space
  Standard_EXPORT static const gp_Ax2& YOZ();

  //! Identifies a Cartesian point with coordinates X = Y = 0.0
  Standard_EXPORT static const gp_Pnt2d& Origin2d();

  //! Returns a unit vector with the combinations (1,0)
  Standard_EXPORT static const gp_Dir2d& DX2d();

  //! Returns a unit vector with the combinations (0,1)
  Standard_EXPORT static const gp_Dir2d& DY2d();

  //! Identifies an axis where its origin is Origin2d
  //! and its unit vector coordinates are: X = 1.0,  Y = 0.0
  Standard_EXPORT static const gp_Ax2d& OX2d();

  //! Identifies an axis where its origin is Origin2d
  //! and its unit vector coordinates are Y = 1.0,  X = 0.0
  Standard_EXPORT static const gp_Ax2d& OY2d();
};

#endif // _gp_HeaderFile
