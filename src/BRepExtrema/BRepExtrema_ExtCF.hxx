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

#ifndef _BRepExtrema_ExtCF_HeaderFile
#define _BRepExtrema_ExtCF_HeaderFile

#include <Extrema_ExtCS.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <Standard_DefineAlloc.hxx>

class BRepAdaptor_HSurface;
class TopoDS_Edge;
class TopoDS_Face;
class gp_Pnt;


class BRepExtrema_ExtCF
{
 public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepExtrema_ExtCF()
  {
  }
  //! It calculates all the distances. <br>
  Standard_EXPORT BRepExtrema_ExtCF(const TopoDS_Edge& E,const TopoDS_Face& F);

  Standard_EXPORT void Initialize(const TopoDS_Edge& E, const TopoDS_Face& F);
  //! An exception is raised if the fields have not been initialized. <br>
  //! Be careful: this method uses the Face only for classify not for the fields. <br>
  Standard_EXPORT void Perform(const TopoDS_Edge& E,const TopoDS_Face& F);
  //! True if the distances are found. <br>
  Standard_EXPORT Standard_Boolean IsDone() const
  {
    return myExtCS.IsDone();
  }
  //! Returns the number of extremum distances. <br>
  Standard_EXPORT Standard_Integer NbExt() const
  {
    return mySqDist.Length();
  }
  //! Returns the value of the <N>th extremum square distance. <br>
  Standard_EXPORT Standard_Real SquareDistance(const Standard_Integer N) const
  {
    return mySqDist.Value(N);
  }
  //! Returns True if the curve is on a parallel surface. <br>
  Standard_EXPORT Standard_Boolean IsParallel() const
  {
    return myExtCS.IsParallel();
  }
  //! Returns the parameters on the Edge of the <N>th extremum distance. <br>
  Standard_EXPORT Standard_Real ParameterOnEdge(const Standard_Integer N) const
  {
    return myPointsOnC.Value(N).Parameter();
  }
  //! Returns the parameters on the Face of the <N>th extremum distance. <br>
  Standard_EXPORT void ParameterOnFace(const Standard_Integer N,Standard_Real& U,Standard_Real& V) const
  {
    myPointsOnS.Value(N).Parameter(U, V);
  }
  //! Returns the Point of the <N>th extremum distance. <br>
  Standard_EXPORT gp_Pnt PointOnEdge(const Standard_Integer N) const
  {
    return myPointsOnC.Value(N).Value();
  }
  //! Returns the Point of the <N>th extremum distance. <br>
  Standard_EXPORT gp_Pnt PointOnFace(const Standard_Integer N) const
  {
    return myPointsOnS.Value(N).Value();
  }

 private:

  Extrema_ExtCS myExtCS;
  TColStd_SequenceOfReal mySqDist;
  Extrema_SequenceOfPOnSurf myPointsOnS;
  Extrema_SequenceOfPOnCurv myPointsOnC;
  Handle(BRepAdaptor_HSurface) myHS;
};

#endif
