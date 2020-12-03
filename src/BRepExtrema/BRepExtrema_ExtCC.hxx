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
  
  BRepExtrema_ExtCC ()
  {
  }
  //! It calculates all the distances. <br>
  Standard_EXPORT BRepExtrema_ExtCC(const TopoDS_Edge& E1,const TopoDS_Edge& E2);

  Standard_EXPORT void Initialize(const TopoDS_Edge& E2);
  //! An exception is raised if the fields have not been initialized. <br>
  Standard_EXPORT void Perform(const TopoDS_Edge& E1);
  //! True if the distances are found. <br>
  Standard_Boolean IsDone() const
  {
    return myExtCC.IsDone();
  }
  //! Returns the number of extremum distances. <br>
  Standard_Integer NbExt() const
  {
    return myExtCC.NbExt();
  }
  //! Returns True if E1 and E2 are parallel. <br>
  Standard_Boolean IsParallel() const
  {
    return myExtCC.IsParallel();
  }
  //! Returns the value of the <N>th extremum square distance. <br>
  Standard_Real SquareDistance(const Standard_Integer N) const
  {
    return myExtCC.SquareDistance(N);
  }
  //! Returns the parameter on the first edge of the <N>th extremum distance. <br>
  Standard_EXPORT Standard_Real ParameterOnE1(const Standard_Integer N) const;
  //! Returns the Point of the <N>th extremum distance on the edge E1. <br>
  Standard_EXPORT gp_Pnt PointOnE1(const Standard_Integer N) const;
  //! Returns the parameter on the second edge of the <N>th extremum distance. <br>
  Standard_EXPORT Standard_Real ParameterOnE2(const Standard_Integer N) const;
  //! Returns the Point of the <N>th extremum distance on the edge E2. <br>
  Standard_EXPORT gp_Pnt PointOnE2(const Standard_Integer N) const;
  //! if the edges is a trimmed curve, <br>
  //! dist11 is a square distance between the point on E1 <br>
  //! of parameter FirstParameter and the point of <br>
  //! parameter FirstParameter on E2. <br>
  Standard_EXPORT void TrimmedSquareDistances(Standard_Real& dist11,Standard_Real& distP12,Standard_Real& distP21,Standard_Real& distP22,gp_Pnt& P11,gp_Pnt& P12,gp_Pnt& P21,gp_Pnt& P22) const;

 private:

  Extrema_ExtCC myExtCC;
  Handle(BRepAdaptor_Curve) myHC;
};

#endif
