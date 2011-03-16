//--------------------------------------------------------------------
//
//  File Name : IGESAppli_LevelFunction.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_LevelFunction.ixx>


    IGESAppli_LevelFunction::IGESAppli_LevelFunction ()    {  }


    void  IGESAppli_LevelFunction::Init
  (const Standard_Integer nbPropVal,
   const Standard_Integer aCode,
   const Handle(TCollection_HAsciiString)& aFuncDescrip)
{
  theNbPropertyValues = nbPropVal;
  theFuncDescripCode  = aCode;
  theFuncDescrip      = aFuncDescrip;
  InitTypeAndForm(406,3);
}


    Standard_Integer  IGESAppli_LevelFunction::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer  IGESAppli_LevelFunction::FuncDescriptionCode () const
{
  return theFuncDescripCode;
}

    Handle(TCollection_HAsciiString) IGESAppli_LevelFunction::FuncDescription () const
{
  return theFuncDescrip;
}
