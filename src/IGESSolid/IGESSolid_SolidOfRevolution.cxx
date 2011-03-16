//--------------------------------------------------------------------
//
//  File Name : IGESSolid_SolidOfRevolution.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_SolidOfRevolution.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_SolidOfRevolution::IGESSolid_SolidOfRevolution ()    {  }


    void  IGESSolid_SolidOfRevolution::Init
  (const Handle(IGESData_IGESEntity)& aCurve, const Standard_Real Fract,
   const gp_XYZ& AxisPnt, const gp_XYZ& Direction)
{
  theCurve     = aCurve;
  theFraction  = Fract;           // default 1.0
  theAxisPoint = AxisPnt;         // default (0,0,0)
  theAxis      = Direction;       // default (0,0,1)
  InitTypeAndForm(162,FormNumber());
// Form 0 : Curve closed to Axis;   Form 1 : Curve closed to itself
}

    void  IGESSolid_SolidOfRevolution::SetClosedToAxis (const Standard_Boolean F)
{
  InitTypeAndForm(162, (F ? 0 : 1));
}

    Standard_Boolean IGESSolid_SolidOfRevolution::IsClosedToAxis () const
{
  return (FormNumber() == 0);
}


    Handle(IGESData_IGESEntity)  IGESSolid_SolidOfRevolution::Curve () const
{
  return theCurve;
}

    Standard_Real  IGESSolid_SolidOfRevolution::Fraction () const
{
  return theFraction;
}

    gp_Pnt  IGESSolid_SolidOfRevolution::AxisPoint () const
{
  return gp_Pnt(theAxisPoint);
}

    gp_Pnt  IGESSolid_SolidOfRevolution::TransformedAxisPoint () const
{
  if (!HasTransf()) return gp_Pnt(theAxisPoint);
  else
    {
      gp_XYZ tmp = theAxisPoint;
      Location().Transforms(tmp);
      return gp_Pnt(tmp);
    }
}

    gp_Dir  IGESSolid_SolidOfRevolution::Axis () const
{
  return gp_Dir(theAxis);
}

    gp_Dir  IGESSolid_SolidOfRevolution::TransformedAxis () const
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
