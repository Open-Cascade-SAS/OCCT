// Copyright (c) 1996-1999 Matra Datavision
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

//#include <GCPnts_QuasiUniformAbscissa.ixx>

#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ConstructionError.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <gp_Pnt.hxx>

#ifdef DEB
//#include <DrawTrSurf.hxx>

//static Standard_Integer compteur = 0;
#endif

//=======================================================================
//function : GCPnts_QuasiUniformAbscissa
//purpose  : 
//=======================================================================

GCPnts_QuasiUniformAbscissa::GCPnts_QuasiUniformAbscissa ()
     :myDone(Standard_False) 
{
} 

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>

#define TheCurve                 Adaptor3d_Curve
#define Handle_TheBezierCurve   Handle(Geom_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom_BSplineCurve)
#define TheArray1OfPnt          TColgp_Array1OfPnt
#define ThePnt                  gp_Pnt

#include <GCPnts_QuasiUniformAbscissa.gxx>

#undef TheCurve
#undef Handle_TheBezierCurve
#undef Handle_TheBSplineCurve
#undef TheArray1OfPnt     
#undef ThePnt

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

#define TheCurve                 Adaptor2d_Curve2d
#define Handle_TheBezierCurve   Handle(Geom2d_BezierCurve)
#define Handle_TheBSplineCurve  Handle(Geom2d_BSplineCurve)
#define TheArray1OfPnt          TColgp_Array1OfPnt2d
#define ThePnt                  gp_Pnt2d

#include <GCPnts_QuasiUniformAbscissa.gxx>



