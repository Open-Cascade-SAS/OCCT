// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _BRepExtrema_ExtPF_HeaderFile
#define _BRepExtrema_ExtPF_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _Extrema_ExtPS_HeaderFile
#include <Extrema_ExtPS.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _TColStd_SequenceOfReal_HeaderFile
#include <TColStd_SequenceOfReal.hxx>
#endif
#ifndef _Extrema_SequenceOfPOnSurf_HeaderFile
#include <Extrema_SequenceOfPOnSurf.hxx>
#endif
#ifndef _BRepAdaptor_Surface_HeaderFile
#include <BRepAdaptor_Surface.hxx>
#endif
#ifndef _Extrema_ExtFlag_HeaderFile
#include <Extrema_ExtFlag.hxx>
#endif
#ifndef _Extrema_ExtAlgo_HeaderFile
#include <Extrema_ExtAlgo.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
class TopoDS_Vertex;
class TopoDS_Face;
class gp_Pnt;


class BRepExtrema_ExtPF
{
 public:

  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepExtrema_ExtPF()
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
  Standard_EXPORT Standard_Boolean IsDone() const
  {
    return myExtPS.IsDone();
  }
  //! Returns the number of extremum distances. <br>
  Standard_EXPORT Standard_Integer NbExt() const
  {
    return myPoints.Length();
  }
  //! Returns the value of the <N>th extremum square distance. <br>
  Standard_EXPORT Standard_Real SquareDistance(const Standard_Integer N) const
  {
    return mySqDist.Value(N);
  }
  //! Returns the parameters on the Face of the <N>th extremum distance. <br>
  Standard_EXPORT void Parameter(const Standard_Integer N,Standard_Real& U,Standard_Real& V) const
  {
    myPoints.Value(N).Parameter(U, V);
  }
  //! Returns the Point of the <N>th extremum distance. <br>
  Standard_EXPORT gp_Pnt Point(const Standard_Integer N) const
  {
    return myPoints.Value(N).Value();
  }

  Standard_EXPORT void SetFlag(const Extrema_ExtFlag F)
  {
    myExtPS.SetFlag(F);
  }

  Standard_EXPORT void SetAlgo(const Extrema_ExtAlgo A)
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
