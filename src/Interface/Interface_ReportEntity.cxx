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

#include <Interface_ReportEntity.ixx>


//=======================================================================
//function : Interface_ReportEntity
//purpose  : 
//=======================================================================

Interface_ReportEntity::Interface_ReportEntity
  (const Handle(Standard_Transient)& unknown)
{
  theconcerned = unknown;
  thecontent = unknown;
}


//=======================================================================
//function : Interface_ReportEntity
//purpose  : 
//=======================================================================

Interface_ReportEntity::Interface_ReportEntity
  (const Handle(Interface_Check)& acheck,
   const Handle(Standard_Transient)& concerned)
:  thecheck(acheck)
{
  theconcerned = concerned;
  thecheck->SetEntity(concerned);
}


//=======================================================================
//function : SetContent
//purpose  : 
//=======================================================================

void Interface_ReportEntity::SetContent(const Handle(Standard_Transient)& content)
{
  thecontent = content;
}

//  ....                        CONSULTATION                        ....


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

const Handle(Interface_Check)& Interface_ReportEntity::Check () const
{
  return thecheck;
}


//=======================================================================
//function : CCheck
//purpose  : 
//=======================================================================

Handle(Interface_Check)& Interface_ReportEntity::CCheck ()
{
  return thecheck;
}


//=======================================================================
//function : Concerned
//purpose  : 
//=======================================================================

Handle(Standard_Transient) Interface_ReportEntity::Concerned  () const
{
  return theconcerned;
}


//=======================================================================
//function : HasContent
//purpose  : 
//=======================================================================

Standard_Boolean Interface_ReportEntity::HasContent () const 
{
  return (!thecontent.IsNull());
}


//=======================================================================
//function : HasNewContent
//purpose  : 
//=======================================================================

Standard_Boolean Interface_ReportEntity::HasNewContent () const 
{
  return (!thecontent.IsNull() && thecontent != theconcerned);
}


//=======================================================================
//function : Content
//purpose  : 
//=======================================================================

Handle(Standard_Transient) Interface_ReportEntity::Content () const
{
  return thecontent;
}


//=======================================================================
//function : IsError
//purpose  : 
//=======================================================================

Standard_Boolean Interface_ReportEntity::IsError () const
{
  return (thecheck->NbFails() > 0);
}


//=======================================================================
//function : IsUnknown
//purpose  : 
//=======================================================================

Standard_Boolean Interface_ReportEntity::IsUnknown () const
{
  return ((thecheck->NbFails() == 0) && (thecheck->NbWarnings() == 0)
	  && (theconcerned == thecontent));
}
