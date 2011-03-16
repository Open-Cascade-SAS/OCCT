// File:	StepToGeom_MakeAxis2Placement.cxx
// Created:	Thu Jul  1 16:41:35 1993
// Author:	Martine LANGLOIS
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
