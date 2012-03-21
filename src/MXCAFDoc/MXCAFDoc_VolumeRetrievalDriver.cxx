// Created on: 2000-09-08
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <MXCAFDoc_VolumeRetrievalDriver.ixx>
#include <PXCAFDoc_Volume.hxx>
#include <XCAFDoc_Volume.hxx>

//=======================================================================
//function : MCAFDoc_VolumeRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_VolumeRetrievalDriver::MXCAFDoc_VolumeRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_VolumeRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

 Handle(Standard_Type) MXCAFDoc_VolumeRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_Volume);
  return sourceType;
}
//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

 Handle(TDF_Attribute) MXCAFDoc_VolumeRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_Volume();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

 void MXCAFDoc_VolumeRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
						    const Handle(TDF_Attribute)& Target,
						    const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_Volume) S = Handle(PXCAFDoc_Volume)::DownCast (Source);
  Handle(XCAFDoc_Volume) T = Handle(XCAFDoc_Volume)::DownCast (Target);
  
  T->Set(S->Get());
}
