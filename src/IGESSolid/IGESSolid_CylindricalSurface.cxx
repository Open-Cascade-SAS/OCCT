//--------------------------------------------------------------------
//
//  File Name : IGESSolid_CylindricalSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_CylindricalSurface.ixx>


    IGESSolid_CylindricalSurface::IGESSolid_CylindricalSurface ()    {  }


    void  IGESSolid_CylindricalSurface::Init
  (const Handle(IGESGeom_Point)&     aLocation,
   const Handle(IGESGeom_Direction)& anAxis,
   const Standard_Real    aRadius,
   const Handle(IGESGeom_Direction)& aRefdir)
{
  theLocationPoint = aLocation;
  theAxis          = anAxis;
  theRadius        = aRadius;
  theRefDir        = aRefdir;
  InitTypeAndForm(192, (theRefDir.IsNull() ? 0 : 1));
}

    Handle(IGESGeom_Point) IGESSolid_CylindricalSurface::LocationPoint () const
{
  return theLocationPoint;
}

    Handle(IGESGeom_Direction) IGESSolid_CylindricalSurface::Axis () const
{
  return theAxis;
}

    Standard_Real IGESSolid_CylindricalSurface::Radius () const
{
  return theRadius;
}

    Handle(IGESGeom_Direction)  IGESSolid_CylindricalSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean  IGESSolid_CylindricalSurface::IsParametrised () const
{
  return (!theRefDir.IsNull());
}

