//--------------------------------------------------------------------
//
//  File Name : IGESBasic_ExternalRefFileName.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefFileName.ixx>


    IGESBasic_ExternalRefFileName::IGESBasic_ExternalRefFileName ()    {  }


    void  IGESBasic_ExternalRefFileName::Init
  (const Handle(TCollection_HAsciiString)& aFileIdent,
   const Handle(TCollection_HAsciiString)& anExtName)
{
  theExtRefFileIdentifier = aFileIdent;
  theExtRefEntitySymbName = anExtName;
  InitTypeAndForm(416,FormNumber());
//  FormNumber 0-2 : sens pas clair. Pourrait etre 0:Definition  2:Entity
}

    void IGESBasic_ExternalRefFileName::SetForEntity (const Standard_Boolean F)
{
  InitTypeAndForm(416, (F ? 2 : 0));
}


    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefFileName::FileId () const
{
  return theExtRefFileIdentifier;
}

    Handle(TCollection_HAsciiString)  IGESBasic_ExternalRefFileName::ReferenceName () const
{
  return theExtRefEntitySymbName;
}
