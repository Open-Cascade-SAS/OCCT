// Created on: 1995-02-20
// Created by: Jacques GOUSSARD
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



#include <Draft.ixx>

#include <Geom_Surface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Plane.hxx>
#include <BRep_Tool.hxx>

#include <BRepTools.hxx>
#include <ElSLib.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>


//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real Draft::Angle(const TopoDS_Face& F,
			   const gp_Dir& D)
{

  TopLoc_Location Lo;
  Handle(Geom_Surface) S = BRep_Tool::Surface(F,Lo);
  Handle(Standard_Type) TypeS = S->DynamicType();
  if (TypeS == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
    S = Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface();
    TypeS = S->DynamicType();
  }

  if (TypeS != STANDARD_TYPE(Geom_Plane) &&
      TypeS != STANDARD_TYPE(Geom_ConicalSurface) &&
      TypeS != STANDARD_TYPE(Geom_CylindricalSurface)) {
    Standard_DomainError::Raise();
  }

  Standard_Real Angle;
  S = Handle(Geom_Surface)::DownCast(S->Transformed(Lo.Transformation()));
  if (TypeS == STANDARD_TYPE(Geom_Plane)) {
    gp_Ax3 ax3(Handle(Geom_Plane)::DownCast(S)->Pln().Position());
    gp_Vec normale(ax3.Direction());
    if (!ax3.Direct()) {
      normale.Reverse();
    }
    if (F.Orientation() == TopAbs_REVERSED) {
      normale.Reverse();
    }
    Angle = ASin(normale.Dot(D));
  }
  else if (TypeS == STANDARD_TYPE(Geom_CylindricalSurface)) {
    gp_Cylinder Cy(Handle(Geom_CylindricalSurface)::DownCast(S)->Cylinder());
    Standard_Real testdir = D.Dot(Cy.Axis().Direction());
    if (Abs(testdir) <= 1.-Precision::Angular()) {
      Standard_DomainError::Raise();
    }
    Angle = 0.;
  }
  else { // STANDARD_TYPE(Geom_ConicalSurface)
    gp_Cone Co(Handle(Geom_ConicalSurface)::DownCast(S)->Cone());
    Standard_Real testdir = D.Dot(Co.Axis().Direction());
    if (Abs(testdir) <= 1.-Precision::Angular()) {
      Standard_DomainError::Raise();
    }
    Standard_Real umin,umax,vmin,vmax;
    BRepTools::UVBounds(F,umin,umax,vmin,vmax);
    gp_Pnt ptbid;
    gp_Vec d1u,d1v;
    ElSLib::D1(umin+umax/2.,vmin+vmax/2.,Co,ptbid,d1u,d1v);
    d1u.Cross(d1v);
    d1u.Normalize();
    if (F.Orientation() == TopAbs_REVERSED) {
      d1u.Reverse();
    }
    Angle = ASin(d1u.Dot(D));
  }
  return Angle;
}
