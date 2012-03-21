// Created on: 1997-11-19
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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



#include <MDataXtd_GeometryRetrievalDriver.ixx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TDataXtd_Geometry.hxx>
#include <PDataXtd_Geometry.hxx>
#include <MDataXtd.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : MDataXtd_GeometryRetrievalDriver
//purpose  : 
//=======================================================================

MDataXtd_GeometryRetrievalDriver::MDataXtd_GeometryRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_GeometryRetrievalDriver::VersionNumber() const
{ return 0; }


//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_GeometryRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataXtd_Geometry);  }




//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataXtd_GeometryRetrievalDriver::NewEmpty() const
{ return new TDataXtd_Geometry (); }



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_GeometryRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
//  const Handle(MDF_RRelocationTable)& RelocTable) const
  const Handle(MDF_RRelocationTable)& ) const
{
  Handle(PDataXtd_Geometry) S = 
     Handle(PDataXtd_Geometry)::DownCast (Source);
  Handle(TDataXtd_Geometry) T = 
    Handle(TDataXtd_Geometry)::DownCast (Target);

  T->SetType (MDataXtd::IntegerToGeometryType (S->GetType ()));
}



