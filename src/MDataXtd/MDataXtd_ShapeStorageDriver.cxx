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

#include <MDataXtd_ShapeStorageDriver.ixx>
#include <PDataXtd_Shape.hxx>
#include <TDataXtd_Shape.hxx>
#include <CDM_MessageDriver.hxx>

MDataXtd_ShapeStorageDriver::MDataXtd_ShapeStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

Standard_Integer MDataXtd_ShapeStorageDriver::VersionNumber() const
{ return 0; }

Handle(Standard_Type) MDataXtd_ShapeStorageDriver::SourceType() const
{ return STANDARD_TYPE(TDataXtd_Shape); }

Handle(PDF_Attribute) MDataXtd_ShapeStorageDriver::NewEmpty() const
{ return new PDataXtd_Shape; }

//void MDataXtd_ShapeStorageDriver::Paste(const Handle(TDF_Attribute)& Source,const Handle(PDF_Attribute)& Target,const Handle(MDF_SRelocationTable)& RelocTable) const
void MDataXtd_ShapeStorageDriver::Paste(const Handle(TDF_Attribute)& ,const Handle(PDF_Attribute)& ,const Handle(MDF_SRelocationTable)& ) const
{
}

