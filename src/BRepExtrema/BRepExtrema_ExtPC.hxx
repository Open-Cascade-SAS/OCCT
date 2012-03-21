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

#ifndef _BRepExtrema_ExtPC_HeaderFile
#define _BRepExtrema_ExtPC_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineAlloc_HeaderFile
#include <Standard_DefineAlloc.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _Extrema_ExtPC_HeaderFile
#include <Extrema_ExtPC.hxx>
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
class TopoDS_Vertex;
class TopoDS_Edge;
class gp_Pnt;


class BRepExtrema_ExtPC
{
 public:

  DEFINE_STANDARD_ALLOC
  
  Standard_EXPORT BRepExtrema_ExtPC()
  {
  }
  //! It calculates all the distances. <br>
  Standard_EXPORT BRepExtrema_ExtPC(const TopoDS_Vertex& V,const TopoDS_Edge& E);
  
  Standard_EXPORT void Initialize(const TopoDS_Edge& E);
  //! An exception is raised if the fields have not been initialized. <br>
  Standard_EXPORT void Perform(const TopoDS_Vertex& V);
  //! True if the distances are found. <br>
  Standard_EXPORT Standard_Boolean IsDone() const
  {
    return myExtPC.IsDone();
  }
  //! Returns the number of extremum distances. <br>
  Standard_EXPORT Standard_Integer NbExt() const
  {
    return myExtPC.NbExt();
  }
  //! Returns True if the <N>th extremum distance is a minimum. <br>
  Standard_EXPORT Standard_Boolean IsMin(const Standard_Integer N) const
  {
    return myExtPC.IsMin(N);
  }
  //! Returns the value of the <N>th extremum square distance. <br>
  Standard_EXPORT Standard_Real SquareDistance(const Standard_Integer N) const
  {
    return myExtPC.SquareDistance(N);
  }
  //! Returns the parameter on the edge of the <N>th extremum distance. <br>
  Standard_EXPORT Standard_Real Parameter(const Standard_Integer N) const
  {
    return myExtPC.Point(N).Parameter();
  }
  //! Returns the Point of the <N>th extremum distance. <br>
  Standard_EXPORT gp_Pnt Point(const Standard_Integer N) const
  {
    return myExtPC.Point(N).Value();
  }
  //! if the curve is a trimmed curve, <br>
  //! dist1 is a square distance between <P> and the point <br>
  //! of parameter FirstParameter <pnt1> and <br>
  //! dist2 is a square distance between <P> and the point <br>
  //! of parameter LastParameter <pnt2>. <br>
  Standard_EXPORT void TrimmedSquareDistances(Standard_Real& dist1,Standard_Real& dist2,gp_Pnt& pnt1,gp_Pnt& pnt2) const
  {
    myExtPC.TrimmedSquareDistances(dist1,dist2,pnt1,pnt2);
  }

 private:

  Extrema_ExtPC myExtPC;
  Handle_BRepAdaptor_HCurve myHC;
};

#endif
