//--------------------------------------------------------------------
//
//  File Name : IGESGeom_Direction.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_Direction.ixx>
#include <gp_GTrsf.hxx>


    IGESGeom_Direction::IGESGeom_Direction ()    {  }


    void IGESGeom_Direction::Init
  (const gp_XYZ& aDirection)
{
  theDirection = aDirection;
  InitTypeAndForm(123,0);
}

    gp_Vec IGESGeom_Direction::Value () const
{
  gp_Vec direction(theDirection);
  return direction;
}

    gp_Vec IGESGeom_Direction::TransformedValue () const
{
  if (!HasTransf()) return gp_Vec(theDirection);
  gp_XYZ xyz (theDirection);
  gp_GTrsf loc = Location();
  loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
  loc.Transforms(xyz);
  return gp_Vec(xyz);
}
