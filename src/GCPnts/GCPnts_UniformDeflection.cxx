// Copyright (c) 1995-1999 Matra Datavision
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

#include <GCPnts_UniformDeflection.ixx>
#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NotImplemented.hxx>
#include <gp_Pnt2d.hxx>
// mask the return of a Adaptor2d_Curve2d as a gp_Pnt 
static gp_Pnt Value(const Adaptor3d_Curve & C,
		    const Standard_Real Parameter) 
{
  return C.Value(Parameter) ;
}
static gp_Pnt Value(const Adaptor2d_Curve2d & C,
		    const Standard_Real Parameter) 
{
  gp_Pnt aPoint ;
  gp_Pnt2d a2dPoint =
  C.Value(Parameter) ;
  aPoint.SetX ( a2dPoint.X()) ;
  aPoint.SetY ( a2dPoint.Y()) ;
  aPoint.SetZ ( 0.0e0) ;
  return aPoint ;
}
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt GCPnts_UniformDeflection::Value
                      (const Standard_Integer Index) const
{ 
  StdFail_NotDone_Raise_if(!myDone, 
			 "GCPnts_UniformAbscissa::Parameter()");
  return myPoints.Value(Index) ;
}
//=======================================================================
//function : GCPnts_UniformDeflection
//purpose  : 
//=======================================================================

GCPnts_UniformDeflection::GCPnts_UniformDeflection ()
:myDone(Standard_False)
{
} 

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#define TheCurve                 Adaptor3d_Curve
#define Handle_TheBezierCurve   Handle(Geom_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom_BSplineCurve)
#include <GCPnts_UniformDeflection.gxx>
#undef TheCurve
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#define TheCurve                 Adaptor2d_Curve2d
#define Handle_TheBezierCurve   Handle(Geom2d_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom2d_BSplineCurve)
#include <GCPnts_UniformDeflection.gxx>
#undef TheCurve
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve






