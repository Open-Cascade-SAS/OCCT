//--------------------------------------------------------------------
//
//  File Name : IGESBasic_SingularSubfigure.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_SingularSubfigure.ixx>
#include <gp_GTrsf.hxx>


    IGESBasic_SingularSubfigure::IGESBasic_SingularSubfigure ()    {  }


    void  IGESBasic_SingularSubfigure::Init
  (const Handle(IGESBasic_SubfigureDef)& aSubfigureDef,
   const gp_XYZ& aTranslation, const Standard_Boolean hasScale,
   const Standard_Real aScale)
{
  theSubfigureDef = aSubfigureDef;
  theTranslation  = aTranslation;
  hasScaleFactor  = hasScale;
  theScaleFactor  = aScale;
  InitTypeAndForm(408,0);
}

    Handle(IGESBasic_SubfigureDef) IGESBasic_SingularSubfigure::Subfigure () const
{
  return theSubfigureDef;
}

    gp_XYZ  IGESBasic_SingularSubfigure::Translation () const
{
  return theTranslation;
}

    Standard_Boolean  IGESBasic_SingularSubfigure::HasScaleFactor () const
{
  return hasScaleFactor;
}

    Standard_Real  IGESBasic_SingularSubfigure::ScaleFactor () const
{
  return theScaleFactor;
}

    gp_XYZ  IGESBasic_SingularSubfigure::TransformedTranslation () const
{
  gp_XYZ tmp = theTranslation;
  if (HasTransf()) Location().Transforms(tmp);
  return tmp;
}
