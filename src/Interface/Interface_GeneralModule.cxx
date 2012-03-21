// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Interface_GeneralModule.ixx>


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
