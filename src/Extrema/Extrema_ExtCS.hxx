// Created on: 1991-02-21
// Created by: Isabelle GRIGNON
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

#ifndef _Extrema_ExtCS_HeaderFile
#define _Extrema_ExtCS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Extrema_ExtElCS.hxx>
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <GeomAbs_SurfaceType.hxx>

class Adaptor3d_Curve;
class Adaptor3d_Surface;
class Extrema_POnCurv;
class Extrema_POnSurf;
class gp_Pnt;


//! It calculates all the extremum distances
//! between a curve and a surface.
//! These distances can be minimum or maximum.
class Extrema_ExtCS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtCS();
  
  //! It calculates all the distances between C and S.
  Standard_EXPORT Extrema_ExtCS(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S, const Standard_Real TolC, const Standard_Real TolS);
  
  //! It calculates all the distances between C and S.
  //! UCinf and UCmax are the start and end parameters
  //! of the curve.
  Standard_EXPORT Extrema_ExtCS(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S, const Standard_Real UCinf, const Standard_Real UCsup, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup, const Standard_Real TolC, const Standard_Real TolS);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S, const Standard_Real TolC, const Standard_Real TolS);

  //! Initializes the fields of the algorithm.
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real Vinf, const Standard_Real Vsup, const Standard_Real TolC, const Standard_Real TolS);
  
  //! Computes the distances.
  //! An exception is raised if the fieds have not been
  //! initialized.
  Standard_EXPORT void Perform (const Adaptor3d_Curve& C, const Standard_Real Uinf, const Standard_Real Usup);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns True if the curve is on a parallel surface.
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnCurv& P1, Extrema_POnSurf& P2) const;

private:

  Standard_EXPORT Standard_Boolean AddSolution (const Adaptor3d_Curve& Curve, const Standard_Real T, const Standard_Real U, const Standard_Real V, const gp_Pnt& PointOnCurve, const gp_Pnt& PointOnSurf, const Standard_Real SquareDist);

private:

  // disallow copies
  Extrema_ExtCS            (Extrema_ExtCS& ) Standard_DELETE;
  Extrema_ExtCS& operator= (Extrema_ExtCS& ) Standard_DELETE;

private:

  const Adaptor3d_Surface* myS;
  Standard_Boolean myDone;
  Standard_Boolean myIsPar;
  Extrema_ExtElCS myExtElCS;
  Extrema_SequenceOfPOnSurf myPOnS;
  Extrema_SequenceOfPOnCurv myPOnC;
  Standard_Real myuinf;
  Standard_Real myusup;
  Standard_Real myvinf;
  Standard_Real myvsup;
  Standard_Real mytolC;
  Standard_Real mytolS;
  Standard_Real myucinf;
  Standard_Real myucsup;
  TColStd_SequenceOfReal mySqDist;
  GeomAbs_SurfaceType myStype;

};

#endif // _Extrema_ExtCS_HeaderFile
