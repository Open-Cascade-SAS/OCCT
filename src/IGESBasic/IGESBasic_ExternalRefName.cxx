//--------------------------------------------------------------------
//
//  File Name : IGESBasic_ExternalRefName.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefName.ixx>


    IGESBasic_ExternalRefName::IGESBasic_ExternalRefName ()    {  }


    void  IGESBasic_ExternalRefName::Init
  (const Handle(TCollection_HAsciiString)& anExtName)
{
  theExtRefEntitySymbName = anExtName;
  InitTypeAndForm(416,3);
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefName::ReferenceName
  () const
{
  return theExtRefEntitySymbName;
}
