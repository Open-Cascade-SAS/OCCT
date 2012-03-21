// Created on: 1998-01-15
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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



#include <MDataStd_CommentStorageDriver.ixx>

#include <PDataStd_Comment.hxx>
#include <TDataStd_Comment.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <CDM_MessageDriver.hxx>


MDataStd_CommentStorageDriver::MDataStd_CommentStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataStd_CommentStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataStd_CommentStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataStd_Comment);}

Handle(PDF_Attribute) MDataStd_CommentStorageDriver::NewEmpty() const
{ return new PDataStd_Comment; }

//void MDataStd_CommentStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataStd_CommentStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& ) const
{
  Handle(TDataStd_Comment) S = Handle(TDataStd_Comment)::DownCast (Source);
  Handle(PDataStd_Comment) T = Handle(PDataStd_Comment)::DownCast (Target);
  Handle(PCollection_HExtendedString) Comment = new PCollection_HExtendedString (S->Get());
  T->Set (Comment);
}

