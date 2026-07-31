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

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetTol3d(const double Tol3d)
{
  myTol3d = Tol3d;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetTol2d(const double Tol2d)
{
  myTol2d = Tol2d;
}

//=================================================================================================

inline bool& ShapeCustom_BSplineRestriction::ModifyApproxSurfaceFlag()
{

  return myApproxSurfaceFlag;
}

//=================================================================================================

inline bool& ShapeCustom_BSplineRestriction::ModifyApproxCurve3dFlag()
{
  return myApproxCurve3dFlag;
}

//=================================================================================================

inline bool& ShapeCustom_BSplineRestriction::ModifyApproxCurve2dFlag()
{
  return myApproxCurve2dFlag;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetContinuity3d(const GeomAbs_Shape Continuity3d)
{
  myContinuity3d = Continuity3d;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetContinuity2d(const GeomAbs_Shape Continuity2d)
{
  myContinuity2d = Continuity2d;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetMaxDegree(const int MaxDegree)
{
  myMaxDegree = MaxDegree;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetMaxNbSegments(const int MaxNbSegments)
{
  myNbMaxSeg = MaxNbSegments;
}

//=================================================================================================

inline double ShapeCustom_BSplineRestriction::Curve3dError() const
{
  return myCurve3dError;
}

//=================================================================================================

inline double ShapeCustom_BSplineRestriction::Curve2dError() const
{
  return myCurve2dError;
}

//=================================================================================================

inline double ShapeCustom_BSplineRestriction::SurfaceError() const
{
  return mySurfaceError;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetPriority(const bool Degree)
{
  myDeg = Degree;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetConvRational(const bool Rational)
{
  myRational = Rational;
}

//=================================================================================================

inline occ::handle<ShapeCustom_RestrictionParameters> ShapeCustom_BSplineRestriction::
  GetRestrictionParameters() const
{
  return myParameters;
}

//=================================================================================================

inline void ShapeCustom_BSplineRestriction::SetRestrictionParameters(
  const occ::handle<ShapeCustom_RestrictionParameters>& aModes)
{
  myParameters = aModes;
}
