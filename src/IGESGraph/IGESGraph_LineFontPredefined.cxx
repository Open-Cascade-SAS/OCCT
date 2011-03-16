//--------------------------------------------------------------------
//
//  File Name : IGESGraph_LineFontPredefined.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGraph_LineFontPredefined.ixx>

    IGESGraph_LineFontPredefined::IGESGraph_LineFontPredefined ()    {  }


    void IGESGraph_LineFontPredefined::Init
  (const Standard_Integer nbProps, const Standard_Integer aLineFontPatternCode)
{
  theNbPropertyValues    = nbProps;
  theLineFontPatternCode = aLineFontPatternCode;
  InitTypeAndForm(406,19);
}

    Standard_Integer IGESGraph_LineFontPredefined::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESGraph_LineFontPredefined::LineFontPatternCode () const
{
  return theLineFontPatternCode;
}
