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

#ifndef _BRepExtrema_ExtPC_HeaderFile
#define _BRepExtrema_ExtPC_HeaderFile

#include <Extrema_ExtPC.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Vertex;
class TopoDS_Edge;

class BRepExtrema_ExtPC
{
public:
  DEFINE_STANDARD_ALLOC

  BRepExtrema_ExtPC() {}

  //! It calculates all the distances. <br>
  Standard_EXPORT BRepExtrema_ExtPC(const TopoDS_Vertex& V, const TopoDS_Edge& E);

  Standard_EXPORT void Initialize(const TopoDS_Edge& E);
  //! An exception is raised if the fields have not been initialized. <br>
  Standard_EXPORT void Perform(const TopoDS_Vertex& V);

  //! True if the distances are found. <br>
  Standard_Boolean IsDone() const { return myExtPC.IsDone(); }

  //! Returns the number of extremum distances. <br>
  Standard_Integer NbExt() const { return myExtPC.NbExt(); }

  //! Returns True if the <N>th extremum distance is a minimum. <br>
  Standard_Boolean IsMin(const Standard_Integer N) const { return myExtPC.IsMin(N); }

  //! Returns the value of the <N>th extremum square distance. <br>
  Standard_Real SquareDistance(const Standard_Integer N) const { return myExtPC.SquareDistance(N); }

  //! Returns the parameter on the edge of the <N>th extremum distance. <br>
  Standard_Real Parameter(const Standard_Integer N) const { return myExtPC.Point(N).Parameter(); }

  //! Returns the Point of the <N>th extremum distance. <br>
  gp_Pnt Point(const Standard_Integer N) const { return myExtPC.Point(N).Value(); }

  //! if the curve is a trimmed curve, <br>
  //! dist1 is a square distance between <P> and the point <br>
  //! of parameter FirstParameter <pnt1> and <br>
  //! dist2 is a square distance between <P> and the point <br>
  //! of parameter LastParameter <pnt2>. <br>
  void TrimmedSquareDistances(Standard_Real& dist1,
                              Standard_Real& dist2,
                              gp_Pnt&        pnt1,
                              gp_Pnt&        pnt2) const
  {
    myExtPC.TrimmedSquareDistances(dist1, dist2, pnt1, pnt2);
  }

private:
  Extrema_ExtPC             myExtPC;
  Handle(BRepAdaptor_Curve) myHC;
};

#endif
