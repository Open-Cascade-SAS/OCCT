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

#ifndef _BRepExtrema_ExtCC_HeaderFile
#define _BRepExtrema_ExtCC_HeaderFile

#include <Extrema_ExtCC.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Edge;
class gp_Pnt;

class BRepExtrema_ExtCC
{
public:
  DEFINE_STANDARD_ALLOC

  BRepExtrema_ExtCC() {}

  //! It calculates all the distances.
  Standard_EXPORT BRepExtrema_ExtCC(const TopoDS_Edge& E1, const TopoDS_Edge& E2);

  Standard_EXPORT void Initialize(const TopoDS_Edge& E2);
  //! An exception is raised if the fields have not been initialized.
  Standard_EXPORT void Perform(const TopoDS_Edge& E1);

  //! True if the distances are found.
  bool IsDone() const { return myExtCC.IsDone(); }

  //! Returns the number of extremum distances.
  int NbExt() const { return myExtCC.NbExt(); }

  //! Returns True if E1 and E2 are parallel.
  bool IsParallel() const { return myExtCC.IsParallel(); }

  //! Returns the value of the <N>th extremum square distance.
  double SquareDistance(const int N) const { return myExtCC.SquareDistance(N); }

  //! Returns the parameter on the first edge of the <N>th extremum distance.
  Standard_EXPORT double ParameterOnE1(const int N) const;
  //! Returns the Point of the <N>th extremum distance on the edge E1.
  Standard_EXPORT gp_Pnt PointOnE1(const int N) const;
  //! Returns the parameter on the second edge of the <N>th extremum distance.
  Standard_EXPORT double ParameterOnE2(const int N) const;
  //! Returns the Point of the <N>th extremum distance on the edge E2.
  Standard_EXPORT gp_Pnt PointOnE2(const int N) const;
  //! if the edges is a trimmed curve,
  //! dist11 is a square distance between the point on E1
  //! of parameter FirstParameter and the point of
  //! parameter FirstParameter on E2.
  Standard_EXPORT void TrimmedSquareDistances(double& dist11,
                                              double& distP12,
                                              double& distP21,
                                              double& distP22,
                                              gp_Pnt& P11,
                                              gp_Pnt& P12,
                                              gp_Pnt& P21,
                                              gp_Pnt& P22) const;

private:
  Extrema_ExtCC                  myExtCC;
  occ::handle<BRepAdaptor_Curve> myHC;
};

#endif
