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

#ifndef _BRepExtrema_ExtCC_HeaderFile
#define _BRepExtrema_ExtCC_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _Extrema_ExtCC_HeaderFile
#include <Extrema_ExtCC.hxx>
#endif
#ifndef _Handle_BRepAdaptor_HCurve_HeaderFile
#include <Handle_BRepAdaptor_HCurve.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
class BRepAdaptor_HCurve;
class TopoDS_Edge;
class gp_Pnt;


class BRepExtrema_ExtCC
{
 public:

  DEFINE_STANDARD_ALLOC
  
  Standard_EXPORT BRepExtrema_ExtCC()
  {
  }
  //! It calculates all the distances. <br>
  Standard_EXPORT BRepExtrema_ExtCC(const TopoDS_Edge& E1,const TopoDS_Edge& E2);

  Standard_EXPORT void Initialize(const TopoDS_Edge& E2);
  //! An exception is raised if the fields have not been initialized. <br>
  Standard_EXPORT void Perform(const TopoDS_Edge& E1);
  //! True if the distances are found. <br>
  Standard_EXPORT Standard_Boolean IsDone() const
  {
    return myExtCC.IsDone();
  }
  //! Returns the number of extremum distances. <br>
  Standard_EXPORT Standard_Integer NbExt() const
  {
    return myExtCC.NbExt();
  }
  //! Returns True if E1 and E2 are parallel. <br>
  Standard_EXPORT Standard_Boolean IsParallel() const
  {
    return myExtCC.IsParallel();
  }
  //! Returns the value of the <N>th extremum square distance. <br>
  Standard_EXPORT Standard_Real SquareDistance(const Standard_Integer N) const
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
  Handle_BRepAdaptor_HCurve myHC;
};

#endif
