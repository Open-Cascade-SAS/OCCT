// Created on: 1998-02-16
// Created by: Jing Cheng MEI
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



#include <MDataXtd_PatternStdRetrievalDriver.ixx>
#include <PDataXtd_PatternStd.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : MDataXtd_PatternStd
//purpose  : 
//=======================================================================

MDataXtd_PatternStdRetrievalDriver::MDataXtd_PatternStdRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataXtd_PatternStdRetrievalDriver::VersionNumber() const
{
  return 0; 
}

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataXtd_PatternStdRetrievalDriver::SourceType() const
{
  return STANDARD_TYPE(PDataXtd_PatternStd);  
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataXtd_PatternStdRetrievalDriver::NewEmpty() const
{
  return new TDataXtd_PatternStd (); 
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataXtd_PatternStdRetrievalDriver::Paste (const Handle(PDF_Attribute)&        Source,
						const Handle(TDF_Attribute)&        Target,
						const Handle(MDF_RRelocationTable)& RRelocTable) const
{
  Handle(PDataXtd_PatternStd) S = Handle(PDataXtd_PatternStd)::DownCast (Source);
  Handle(TDataXtd_PatternStd) T = Handle(TDataXtd_PatternStd)::DownCast (Target);
  
  T->Signature(S->Signature());
  T->Axis1Reversed(S->Axis1Reversed());
  T->Axis2Reversed(S->Axis2Reversed());
  
  Standard_Integer signature = S->Signature();
  
  Handle(PNaming_NamedShape) PNS; 
  Handle(TNaming_NamedShape) TNS;
  Handle(PDataStd_Real) PReal;
  Handle(TDataStd_Real) TReal;
  Handle(PDataStd_Integer) PInt;
  Handle(TDataStd_Integer) TInt;
  
  if (signature < 5) {
    PNS = S->Axis1();
    RRelocTable->HasRelocation(PNS, TNS);
    T->Axis1(TNS);
    PReal = S->Value1();
    RRelocTable->HasRelocation(PReal, TReal);
    T->Value1(TReal);
    PInt = S->NbInstances1();
    RRelocTable->HasRelocation(PInt, TInt);
    T->NbInstances1(TInt);
    
    if (signature > 2) {
      PNS = S->Axis2();
      RRelocTable->HasRelocation(PNS, TNS);
      T->Axis2(TNS);
      PReal = S->Value2();
      RRelocTable->HasRelocation(PReal, TReal);
      T->Value2(TReal);
      PInt = S->NbInstances2();
      RRelocTable->HasRelocation(PInt, TInt);
      T->NbInstances2(TInt);
    }
  }
  else {
    PNS = S->Mirror();
    RRelocTable->HasRelocation(PNS, TNS);
    T->Mirror(TNS);
  }
}
