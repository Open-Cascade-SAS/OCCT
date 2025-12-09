// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _GeomGridEvaluator_Sphere_HeaderFile
#define _GeomGridEvaluator_Sphere_HeaderFile

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for sphere grid points.
//!
//! Uses analytical formula:
//! P(u,v) = Center + R * (cos(v) * cos(u) * XDir + cos(v) * sin(u) * YDir + sin(v) * ZDir)
//!
//! Where U is longitude (0 to 2*PI) and V is latitude (-PI/2 to PI/2).
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_Sphere anEvaluator;
//!   anEvaluator.Initialize(mySphere);
//!   anEvaluator.SetUParams(myUParams);
//!   anEvaluator.SetVParams(myVParams);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_Sphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Default constructor - creates uninitialized evaluator.
  GeomGridEvaluator_Sphere();

  //! Initialize with sphere geometry.
  //! @param theSphere the sphere to evaluate
  void Initialize(const gp_Sphere& theSphere);

  //! Set U parameters for grid evaluation.
  //! @param theParams array of U parameter values (longitude, 1-based)
  void SetUParams(const TColStd_Array1OfReal& theParams);

  //! Set V parameters for grid evaluation.
  //! @param theParams array of V parameter values (latitude, 1-based)
  void SetVParams(const TColStd_Array1OfReal& theParams);

  //! Returns true if the evaluator is properly initialized.
  bool IsInitialized() const { return myIsInitialized; }

  //! Returns number of U parameters.
  int NbUParams() const { return myUParams.Size(); }

  //! Returns number of V parameters.
  int NbVParams() const { return myVParams.Size(); }

  //! Evaluate all grid points.
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if not initialized or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid() const;

private:
  gp_Pnt                     myCenter;
  gp_Dir                     myXDir;
  gp_Dir                     myYDir;
  gp_Dir                     myZDir;
  double                     myRadius;
  NCollection_Array1<double> myUParams;
  NCollection_Array1<double> myVParams;
  bool                       myIsInitialized;
};

#endif // _GeomGridEvaluator_Sphere_HeaderFile
