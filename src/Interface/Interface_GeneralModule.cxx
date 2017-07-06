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


#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_GeneralModule,Standard_Transient)

void  Interface_GeneralModule::FillShared
  (const Handle(Interface_InterfaceModel)& /*model*/,
   const Standard_Integer casenum,
   const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{  FillSharedCase (casenum,ent,iter);  }  // Par defaut, model ne sert pas


    void  Interface_GeneralModule::Share
  (Interface_EntityIterator& iter,
   const Handle(Standard_Transient)& shared) const 
{  iter.GetOneItem(shared);  }    // Plus joli d appeler Share


    void  Interface_GeneralModule::ListImplied
  (const Handle(Interface_InterfaceModel)& /*model*/,
   const Standard_Integer casenum,
   const Handle(Standard_Transient)& ent,
   Interface_EntityIterator& iter) const
{  ListImpliedCase (casenum,ent,iter);  }  // Par defaut, model ne sert pas


    void  Interface_GeneralModule::ListImpliedCase
  (const Standard_Integer /*casenum*/,
   const Handle(Standard_Transient)& /*ent*/,
   Interface_EntityIterator& /*iter*/) const
{  }  // Par defaut, pas d Imply


    Standard_Boolean  Interface_GeneralModule::CanCopy
  (const Standard_Integer /*CN*/, const Handle(Standard_Transient)& /*ent*/) const
      {  return Standard_False;  }

    Standard_Boolean  Interface_GeneralModule::Dispatch
  (const Standard_Integer, const Handle(Standard_Transient)& entfrom,
   Handle(Standard_Transient)& entto, Interface_CopyTool& ) const
      {  entto = entfrom;  return Standard_False;  }

    Standard_Boolean  Interface_GeneralModule::NewCopiedCase
  (const Standard_Integer, const Handle(Standard_Transient)&,
   Handle(Standard_Transient)&, Interface_CopyTool& ) const
      {  return Standard_False;  }


    void  Interface_GeneralModule::RenewImpliedCase
  (const Standard_Integer /*casenum*/,
   const Handle(Standard_Transient)& /*entfrom*/,
   const Handle(Standard_Transient)& /*entto*/,
   const Interface_CopyTool& /*TC*/) const 
{  }    // Par defaut, ne fait rien

    void  Interface_GeneralModule::WhenDeleteCase
  (const Standard_Integer /*casenum*/,
   const Handle(Standard_Transient)& /*ent*/,
   const Standard_Boolean /*dispatched*/) const
{  }    // par defaut, ne fait rien

    Standard_Integer  Interface_GeneralModule::CategoryNumber
  (const Standard_Integer , const Handle(Standard_Transient)& ,
   const Interface_ShareTool& ) const
      {  return 0;  }  // par defaut, non specifie

    Handle(TCollection_HAsciiString)  Interface_GeneralModule::Name
  (const Standard_Integer , const Handle(Standard_Transient)& ,
   const Interface_ShareTool& ) const
      {  Handle(TCollection_HAsciiString) str;  return str;  }  // par defaut, non specifie
