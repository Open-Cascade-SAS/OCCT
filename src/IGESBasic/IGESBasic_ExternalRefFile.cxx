//--------------------------------------------------------------------
//
//  File Name : IGESBasic_ExternalRefFile.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefFile.ixx>


    IGESBasic_ExternalRefFile::IGESBasic_ExternalRefFile ()    {  }


    void  IGESBasic_ExternalRefFile::Init
  (const Handle(TCollection_HAsciiString)& aFileIdent)
{
  theExtRefFileIdentifier = aFileIdent;
  InitTypeAndForm(416,1);
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefFile::FileId () const
{
  return theExtRefFileIdentifier;
}
