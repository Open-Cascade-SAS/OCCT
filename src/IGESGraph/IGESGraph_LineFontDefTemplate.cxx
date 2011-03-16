//--------------------------------------------------------------------
//
//  File Name : IGESGraph_LineFontDefTemplate.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_LineFontDefTemplate.ixx>


    IGESGraph_LineFontDefTemplate::IGESGraph_LineFontDefTemplate ()    {  }


// This class inherits from IGESData_LineFontEntity

    void IGESGraph_LineFontDefTemplate::Init
  (const Standard_Integer                anOrientation,
   const Handle(IGESBasic_SubfigureDef)& aTemplate,
   const Standard_Real                   aDistance,
   const Standard_Real                   aScale)
{
  theOrientation    = anOrientation;
  theTemplateEntity = aTemplate;
  theDistance       = aDistance;
  theScale          = aScale;
  InitTypeAndForm(304,1);
}

    Standard_Integer IGESGraph_LineFontDefTemplate::Orientation () const
{
  return theOrientation;
}

    Handle(IGESBasic_SubfigureDef) IGESGraph_LineFontDefTemplate::TemplateEntity
  () const
{
  return theTemplateEntity;
}

    Standard_Real IGESGraph_LineFontDefTemplate::Distance () const
{
  return theDistance;
}

    Standard_Real IGESGraph_LineFontDefTemplate::Scale () const
{
  return theScale;
}
