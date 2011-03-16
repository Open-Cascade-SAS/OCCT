//--------------------------------------------------------------------
//
//  File Name : IGESSolid_Cylinder.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_Cylinder.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_Cylinder::IGESSolid_Cylinder ()    {  }


    void  IGESSolid_Cylinder::Init
  (const Standard_Real aHeight, const Standard_Real aRadius,
   const gp_XYZ& aCenter, const gp_XYZ& anAxis)
{
  theHeight     = aHeight;
  theRadius     = aRadius;
  theFaceCenter = aCenter;
  theAxis       = anAxis;
  InitTypeAndForm(154,0);
}

    Standard_Real  IGESSolid_Cylinder::Height () const
{
  return theHeight;
}

    Standard_Real  IGESSolid_Cylinder::Radius () const
{
  return theRadius;
}

    gp_Pnt  IGESSolid_Cylinder::FaceCenter () const
{
  return gp_Pnt(theFaceCenter);
}

    gp_Pnt  IGESSolid_Cylinder::TransformedFaceCenter () const
{
  if (!HasTransf()) return gp_Pnt(theFaceCenter);
  else
    {
      gp_XYZ tmp = theFaceCenter;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_Cylinder::Axis () const
{
  return gp_Dir(theAxis);
}

    gp_Dir  IGESSolid_Cylinder::TransformedAxis () const
{
  if (!HasTransf()) return gp_Dir(theAxis);
  else
    {
      gp_XYZ tmp = theAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}
