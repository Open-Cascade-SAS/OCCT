//--------------------------------------------------------------------
//
//  File Name : IGESSolid_Torus.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_Torus.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_Torus::IGESSolid_Torus ()    {  }


    void  IGESSolid_Torus::Init
  (const Standard_Real R1,    const Standard_Real R2,
   const gp_XYZ&       Point, const gp_XYZ&       Axisdir)
{
  theR1 = R1;
  theR2 = R2;
  thePoint = Point;               // default (0,0,0)
  theAxis  = Axisdir;             // default (0,0,1)
  InitTypeAndForm(160,0);
}

    Standard_Real  IGESSolid_Torus::MajorRadius () const
{
  return theR1;
}

    Standard_Real  IGESSolid_Torus::DiscRadius () const
{
  return theR2;
}

    gp_Pnt  IGESSolid_Torus::AxisPoint () const
{
  return gp_Pnt(thePoint);
}

    gp_Pnt  IGESSolid_Torus::TransformedAxisPoint () const
{
  if (!HasTransf()) return gp_Pnt(thePoint);
  else
    {
      gp_XYZ pnt = thePoint;
      Location().Transforms(pnt);
      return gp_Pnt(pnt);
    }
}

    gp_Dir  IGESSolid_Torus::Axis () const
{
  return gp_Dir(theAxis);
}

    gp_Dir  IGESSolid_Torus::TransformedAxis () const
{
  if (!HasTransf()) return gp_Dir(theAxis);
  else
    {
      gp_XYZ pnt = theAxis;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(pnt);
      return gp_Dir(pnt);
    }
}
