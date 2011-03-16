//--------------------------------------------------------------------
//
//  File Name : IGESSolid_ConicalSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_ConicalSurface.ixx>


    IGESSolid_ConicalSurface::IGESSolid_ConicalSurface ()    {  }


    void  IGESSolid_ConicalSurface::Init
  (const Handle(IGESGeom_Point)&     aLocation,
   const Handle(IGESGeom_Direction)& anAxis,
   const Standard_Real aRadius,  const Standard_Real anAngle,
   const Handle(IGESGeom_Direction)& aRefdir)
{
  theLocationPoint = aLocation;
  theAxis          = anAxis;
  theRadius        = aRadius;
  theAngle         = anAngle;
  theRefDir        = aRefdir;
  InitTypeAndForm(194, (theRefDir.IsNull() ? 0 : 1));
}

    Handle(IGESGeom_Point) IGESSolid_ConicalSurface::LocationPoint () const
{
  return theLocationPoint;
}

    Handle(IGESGeom_Direction) IGESSolid_ConicalSurface::Axis () const
{
  return theAxis;
}

    Standard_Real IGESSolid_ConicalSurface::Radius () const
{
  return theRadius;
}

    Standard_Real IGESSolid_ConicalSurface::SemiAngle () const
{
  return theAngle;
}

    Handle(IGESGeom_Direction) IGESSolid_ConicalSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean IGESSolid_ConicalSurface::IsParametrised () const
{
  return (!theRefDir.IsNull());
}
