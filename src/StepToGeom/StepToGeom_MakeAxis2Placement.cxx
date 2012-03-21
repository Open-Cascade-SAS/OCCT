// Created on: 1993-07-01
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

// sln 22.10.2001. CTS23496: If problems with creation of direction occur default direction is used (StepToGeom_MakeAxis2Placement(...) function)

#include <StepToGeom_MakeAxis2Placement.ixx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <StepToGeom_MakeDirection.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Direction.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax2.hxx>

//=============================================================================
// Creation d' un Axis2Placement de Geom a partir d' un axis2_placement_3d
// de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeAxis2Placement::Convert (const Handle(StepGeom_Axis2Placement3d)& SA, Handle(Geom_Axis2Placement)& CA)
{
  Handle(Geom_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint::Convert(SA->Location(),P))
  {
    const gp_Pnt Pgp = P->Pnt();
  
    // sln 22.10.2001. CTS23496: If problems with creation of direction occur default direction is used (StepToGeom_MakeLine(...) function)
    gp_Dir Ngp(0.,0.,1.);
    if (SA->HasAxis())
    {
      Handle(Geom_Direction) D;
      if (StepToGeom_MakeDirection::Convert(SA->Axis(),D))
        Ngp = D->Dir();
    }
  
    gp_Ax2 gpAx2;
    Standard_Boolean isDefaultDirectionUsed = Standard_True;
    if (SA->HasRefDirection())
    {
      Handle(Geom_Direction) D;
      if (StepToGeom_MakeDirection::Convert(SA->RefDirection(),D))
      {
        const gp_Dir Vxgp = D->Dir();
        if (!Ngp.IsParallel(Vxgp,Precision::Angular()))
        {
          gpAx2 = gp_Ax2(Pgp, Ngp, Vxgp);
          isDefaultDirectionUsed = Standard_False;
        }
      }
    }
    if(isDefaultDirectionUsed)
      gpAx2 = gp_Ax2(Pgp, Ngp);

    CA = new Geom_Axis2Placement(gpAx2);
    return Standard_True;
  }
  return Standard_False;
}
