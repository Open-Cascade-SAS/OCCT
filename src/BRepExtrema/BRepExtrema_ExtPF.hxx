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

#ifndef _BRepExtrema_ExtPF_HeaderFile
#define _BRepExtrema_ExtPF_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Extrema_ExtPS.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>

class TopoDS_Vertex;
class TopoDS_Face;
class gp_Pnt;


class BRepExtrema_ExtPF
{
 public:

  DEFINE_STANDARD_ALLOC

  BRepExtrema_ExtPF()
  {}
  //! It calculates all the distances. <br>
  Standard_EXPORT BRepExtrema_ExtPF(const TopoDS_Vertex& TheVertex,const TopoDS_Face& TheFace,
                                    const Extrema_ExtFlag TheFlag = Extrema_ExtFlag_MINMAX,
                                    const Extrema_ExtAlgo TheAlgo = Extrema_ExtAlgo_Grad);
  
  Standard_EXPORT void Initialize(const TopoDS_Face& TheFace,
                                  const Extrema_ExtFlag TheFlag = Extrema_ExtFlag_MINMAX,
                                  const Extrema_ExtAlgo TheAlgo = Extrema_ExtAlgo_Grad);

  //! An exception is raised if the fields have not been initialized. <br>
  //! Be careful: this method uses the Face only for classify not for the fields. <br>
  Standard_EXPORT void Perform(const TopoDS_Vertex& TheVertex,const TopoDS_Face& TheFace);
  //! True if the distances are found. <br>
  Standard_Boolean IsDone() const
  {
    return myExtPS.IsDone();
  }
  //! Returns the number of extremum distances. <br>
  Standard_Integer NbExt() const
  {
    return myPoints.Length();
  }
  //! Returns the value of the <N>th extremum square distance. <br>
  Standard_Real SquareDistance(const Standard_Integer N) const
  {
    return mySqDist.Value(N);
  }
  //! Returns the parameters on the Face of the <N>th extremum distance. <br>
  void Parameter(const Standard_Integer N,Standard_Real& U,Standard_Real& V) const
  {
    myPoints.Value(N).Parameter(U, V);
  }
  //! Returns the Point of the <N>th extremum distance. <br>
  gp_Pnt Point(const Standard_Integer N) const
  {
    return myPoints.Value(N).Value();
  }

  void SetFlag(const Extrema_ExtFlag F)
  {
    myExtPS.SetFlag(F);
  }

  void SetAlgo(const Extrema_ExtAlgo A)
  {
    myExtPS.SetAlgo(A);
  }

 private:

  Extrema_ExtPS myExtPS;
  TColStd_SequenceOfReal mySqDist;
  Extrema_SequenceOfPOnSurf myPoints;
  BRepAdaptor_Surface mySurf;
};

#endif
