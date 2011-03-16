//--------------------------------------------------------------------
//
//  File Name : IGESBasic_ExternalRefLibName.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefLibName.ixx>


    IGESBasic_ExternalRefLibName::IGESBasic_ExternalRefLibName ()    {  }


    void  IGESBasic_ExternalRefLibName::Init
  (const Handle(TCollection_HAsciiString)& aLibName,
   const Handle(TCollection_HAsciiString)& anExtName)
{
  theLibName = aLibName;
  theExtRefEntitySymbName = anExtName;
  InitTypeAndForm(416,4);
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefLibName::LibraryName () const
{
  return theLibName;
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefLibName::ReferenceName () const
{
  return theExtRefEntitySymbName;
}
