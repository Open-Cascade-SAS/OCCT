//--------------------------------------------------------------------
//
//  File Name : IGESAppli_LineWidening.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESAppli_LineWidening.ixx>
#include <IGESData_LevelListEntity.hxx>


    IGESAppli_LineWidening::IGESAppli_LineWidening ()    {  }


    void  IGESAppli_LineWidening::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real    aWidth,    const Standard_Integer aCornering,
   const Standard_Integer aExtnFlag, const Standard_Integer aJustifFlag,
   const Standard_Real    aExtnVal)
{
  theNbPropertyValues  = nbPropVal;
  theWidth             = aWidth;
  theCorneringCode     = aCornering;
  theExtensionFlag     = aExtnFlag;
  theJustificationFlag = aJustifFlag;
  theExtensionValue    = aExtnVal;
  InitTypeAndForm(406,5);
}


    Standard_Integer  IGESAppli_LineWidening::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real  IGESAppli_LineWidening::WidthOfMetalization () const
{
  return theWidth;
}

    Standard_Integer  IGESAppli_LineWidening::CorneringCode () const
{
  return theCorneringCode;
}

    Standard_Integer  IGESAppli_LineWidening::ExtensionFlag () const
{
  return theExtensionFlag;
}

    Standard_Integer  IGESAppli_LineWidening::JustificationFlag () const
{
  return theJustificationFlag;
}

    Standard_Real  IGESAppli_LineWidening::ExtensionValue () const
{
  return theExtensionValue;
}
