//--------------------------------------------------------------------
//
//  File Name : IGESSolid_SphericalSurface.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_SphericalSurface.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_SphericalSurface::IGESSolid_SphericalSurface ()    {  }


    void  IGESSolid_SphericalSurface::Init
  (const Handle(IGESGeom_Point)& aCenter,
   const Standard_Real aRadius,
   const Handle(IGESGeom_Direction)& anAxis,
   const Handle(IGESGeom_Direction)& aRefdir)
{
  theCenter     = aCenter;
  theRadius     = aRadius;
  theAxis       = anAxis;
  theRefDir     = aRefdir;
  InitTypeAndForm(196, (theRefDir.IsNull() ? 0 : 1) );
}

    Handle(IGESGeom_Point)  IGESSolid_SphericalSurface::Center () const
{
  return theCenter;
}

    gp_Pnt  IGESSolid_SphericalSurface::TransformedCenter () const
{
  if (!HasTransf()) return theCenter->Value();
  else
    {
      gp_XYZ tmp = (theCenter->Value()).XYZ();
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    Standard_Real  IGESSolid_SphericalSurface::Radius () const
{
  return theRadius;
}

    Handle(IGESGeom_Direction)  IGESSolid_SphericalSurface::Axis () const
{
  return theAxis;
}

    Handle(IGESGeom_Direction)  IGESSolid_SphericalSurface::ReferenceDir () const
{
  return theRefDir;
}

    Standard_Boolean  IGESSolid_SphericalSurface::IsParametrised () const
{
  return !(theRefDir.IsNull());
}
