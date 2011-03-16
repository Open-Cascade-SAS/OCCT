//--------------------------------------------------------------------
//
//  File Name : IGESSolid_SolidOfLinearExtrusion.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESSolid_SolidOfLinearExtrusion.ixx>
#include <gp_GTrsf.hxx>


    IGESSolid_SolidOfLinearExtrusion::IGESSolid_SolidOfLinearExtrusion ()   { }


    void  IGESSolid_SolidOfLinearExtrusion::Init
  (const Handle(IGESData_IGESEntity)& aCurve,
   const Standard_Real Length, const gp_XYZ& Direction)
{
  theCurve     = aCurve;
  theLength    = Length;
  theDirection = Direction;           // default (0,0,1)
  InitTypeAndForm(164,0);
}

    Handle(IGESData_IGESEntity)  IGESSolid_SolidOfLinearExtrusion::Curve () const
{
  return theCurve;
}

    Standard_Real  IGESSolid_SolidOfLinearExtrusion::ExtrusionLength () const
{
  return theLength;
}

    gp_Dir  IGESSolid_SolidOfLinearExtrusion::ExtrusionDirection () const
{
  return gp_Dir(theDirection);
}

    gp_Dir  IGESSolid_SolidOfLinearExtrusion::TransformedExtrusionDirection () const
{
  if (!HasTransf()) return gp_Dir(theDirection);
  else
    {
      gp_XYZ tmp = theDirection;
      gp_GTrsf loc = Location();
      loc.SetTranslationPart(gp_XYZ(0.,0.,0.));
      loc.Transforms(tmp);
      return gp_Dir(tmp);
    }
}
