//--------------------------------------------------------------------
//
//  File Name : IGESSolid_PlaneSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_PlaneSurface.ixx>


    IGESSolid_PlaneSurface::IGESSolid_PlaneSurface ()    {  }


    void  IGESSolid_PlaneSurface::Init
  (const Handle(IGESGeom_Point)& aLocation,
   const Handle(IGESGeom_Direction)& aNormal,
   const Handle(IGESGeom_Direction)& Refdir)
{
  theLocationPoint = aLocation;
  theNormal        = aNormal;
  theRefDir        = Refdir;
  InitTypeAndForm(190, (theRefDir.IsNull() ? 0 : 1));
}

    Handle(IGESGeom_Point)  IGESSolid_PlaneSurface::LocationPoint () const
{
  return theLocationPoint;
}

    Handle(IGESGeom_Direction)  IGESSolid_PlaneSurface::Normal () const
{
  return theNormal;
}

    Handle(IGESGeom_Direction)  IGESSolid_PlaneSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean  IGESSolid_PlaneSurface::IsParametrised () const
{
  return (!theRefDir.IsNull());
}
