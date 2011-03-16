//--------------------------------------------------------------------
//
//  File Name : IGESSolid_Sphere.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_Sphere.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_Sphere::IGESSolid_Sphere ()    {  }


    void  IGESSolid_Sphere::Init
  (const Standard_Real aRadius, const gp_XYZ& aCenter)
{
  theRadius = aRadius;
  theCenter = aCenter;            // default (0,0,0)
  InitTypeAndForm(158,0);
}

    Standard_Real  IGESSolid_Sphere::Radius () const
{
  return theRadius;
}

    gp_Pnt  IGESSolid_Sphere::Center () const
{
  return gp_Pnt(theCenter);
}

    gp_Pnt  IGESSolid_Sphere::TransformedCenter () const
{
  if (!HasTransf()) return gp_Pnt(theCenter);
  else
    {
      gp_XYZ tmp = theCenter;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}
