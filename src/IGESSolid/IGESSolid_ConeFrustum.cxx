//--------------------------------------------------------------------
//
//  File Name : IGESSolid_ConeFrustum.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_ConeFrustum.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_ConeFrustum::IGESSolid_ConeFrustum ()    {  }


    void IGESSolid_ConeFrustum::Init
  (const Standard_Real Ht, const Standard_Real R1,     const Standard_Real R2,
   const gp_XYZ& Center,   const gp_XYZ&       anAxis)
{
  theHeight     = Ht;
  theR1         = R1;
  theR2         = R2;                 // default 0
  theFaceCenter = Center;             // default (0,0,0)
  theAxis       = anAxis;             // default (0,0,1)
  InitTypeAndForm(156,0);
}

    Standard_Real IGESSolid_ConeFrustum::Height () const
{
  return theHeight;
}

    Standard_Real IGESSolid_ConeFrustum::LargerRadius () const
{
  return theR1;
}

    Standard_Real IGESSolid_ConeFrustum::SmallerRadius () const
{
  return theR2;
}

    gp_Pnt IGESSolid_ConeFrustum::FaceCenter () const
{
  return gp_Pnt(theFaceCenter);
}

    gp_Pnt IGESSolid_ConeFrustum::TransformedFaceCenter () const
{
  if (!HasTransf()) return gp_Pnt(theFaceCenter);
  else
    {
      gp_XYZ tmp = theFaceCenter;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir IGESSolid_ConeFrustum::Axis () const
{
  return gp_Dir(theAxis);
}

    gp_Dir IGESSolid_ConeFrustum::TransformedAxis () const
{
  if (!HasTransf()) return gp_Dir(theAxis);
  else
    {
      gp_XYZ xyz = theAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(xyz);
      return gp_Dir(xyz);
    }
}
