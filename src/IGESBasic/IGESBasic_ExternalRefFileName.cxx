// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_ExternalRefFileName.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_ExternalRefFileName,IGESData_IGESEntity)

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
