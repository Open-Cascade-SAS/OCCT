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

#ifndef _BRepExtrema_ExtFF_HeaderFile
#define _BRepExtrema_ExtFF_HeaderFile

#include <Extrema_ExtSS.hxx>
#include <NCollection_Sequence.hxx>
#include <Extrema_POnSurf.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Face;

class BRepExtrema_ExtFF
{
public:
  DEFINE_STANDARD_ALLOC

  BRepExtrema_ExtFF() = default;

  //! It calculates all the distances.
  Standard_EXPORT BRepExtrema_ExtFF(const TopoDS_Face& F1, const TopoDS_Face& F2);

  Standard_EXPORT void Initialize(const TopoDS_Face& F2);
  //! An exception is raised if the fields have not been initialized.
  //! Be careful: this method uses the Face F2 only for classify, not for the fields.
  Standard_EXPORT void Perform(const TopoDS_Face& F1, const TopoDS_Face& F2);

  //! True if the distances are found.
  bool IsDone() const { return myExtSS.IsDone(); }

  //! Returns True if the surfaces are parallel.
  bool IsParallel() const { return myExtSS.IsParallel(); }

  //! Returns the number of extremum distances.
  int NbExt() const { return mySqDist.Length(); }

  //! Returns the value of the <N>th extremum square distance.
  double SquareDistance(const int N) const { return mySqDist.Value(N); }

  //! Returns the parameters on the Face F1 of the <N>th extremum distance.
  void ParameterOnFace1(const int N, double& U, double& V) const
  {
    myPointsOnS1.Value(N).Parameter(U, V);
  }

  //! Returns the parameters on the Face F2 of the <N>th extremum distance.
  void ParameterOnFace2(const int N, double& U, double& V) const
  {
    myPointsOnS2.Value(N).Parameter(U, V);
  }

  //! Returns the Point of the <N>th extremum distance.
  gp_Pnt PointOnFace1(const int N) const { return myPointsOnS1.Value(N).Value(); }

  //! Returns the Point of the <N>th extremum distance.
  gp_Pnt PointOnFace2(const int N) const { return myPointsOnS2.Value(N).Value(); }

private:
  Extrema_ExtSS                         myExtSS;
  NCollection_Sequence<double>          mySqDist;
  NCollection_Sequence<Extrema_POnSurf> myPointsOnS1;
  NCollection_Sequence<Extrema_POnSurf> myPointsOnS2;
  occ::handle<BRepAdaptor_Surface>      myHS;
};

#endif
