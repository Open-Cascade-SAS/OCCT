// Created on: 1995-02-01
// Created by: Marie Jose MARTZ
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

//#53 rln 24.12.98 CCI60005
//#57 rln 25.12.98 avoid code duplication

#include <Geom2dToIGES_Geom2dCurve.ixx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>

//#include <Geom2dConvert.hxx>

#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>

#include <IGESData_IGESEntity.hxx>
#include <IGESData_ToolLocation.hxx>

#include <IGESGeom_BSplineCurve.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_ConicArc.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_TransformationMatrix.hxx>

#include <Interface_Macros.hxx>

#include <Precision.hxx>

#include <GeomToIGES_GeomCurve.hxx>
#include <GeomAPI.hxx>
#include <gp_Pln.hxx>


//=============================================================================
// Geom2dToIGES_Geom2dCurve
//=============================================================================

Geom2dToIGES_Geom2dCurve::Geom2dToIGES_Geom2dCurve()
:Geom2dToIGES_Geom2dEntity()
{
}


//=============================================================================
// Geom2dToIGES_Geom2dCurve
//=============================================================================

Geom2dToIGES_Geom2dCurve::Geom2dToIGES_Geom2dCurve
(const Geom2dToIGES_Geom2dEntity& G2dE)
:Geom2dToIGES_Geom2dEntity(G2dE)
{
}


//=============================================================================
// Transfer des Entites Curve de Geom2d vers IGES
// Transfer2dCurve
//=============================================================================

Handle(IGESData_IGESEntity) Geom2dToIGES_Geom2dCurve::Transfer2dCurve
(const Handle(Geom2d_Curve)& start, const Standard_Real Udeb, const Standard_Real Ufin)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull()) {
    return res;
  }

  //#57 rln 25.12.98 avoid code duplication
  GeomToIGES_GeomCurve GC;
  GC.SetModel (GetModel());
  GC.SetUnit (1.); //not scale 2D curves
  return GC.TransferCurve (GeomAPI::To3d (start, gp_Pln (0, 0, 1, 0)), Udeb, Ufin);
}

