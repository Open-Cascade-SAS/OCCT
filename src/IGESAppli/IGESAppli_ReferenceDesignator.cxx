//--------------------------------------------------------------------
//
//  File Name : IGESAppli_ReferenceDesignator.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_ReferenceDesignator.ixx>
#include <IGESData_LevelListEntity.hxx>


    IGESAppli_ReferenceDesignator::IGESAppli_ReferenceDesignator ()    {  }


    void  IGESAppli_ReferenceDesignator::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& aText)
{
  theRefDesigText     = aText;
  theNbPropertyValues = nbPropVal;
  InitTypeAndForm(406,7);
}


    Standard_Integer  IGESAppli_ReferenceDesignator::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)
    IGESAppli_ReferenceDesignator::RefDesignatorText () const
{
  return theRefDesigText;
}
