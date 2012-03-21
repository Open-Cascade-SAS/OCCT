// Created on: 1997-09-17
// Created by: Isabelle GRIGNON
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



#include <MNaming_NamingRetrievalDriver.ixx>
#include <PNaming_Naming.hxx>
#include <PNaming_NamedShape.hxx>
#include <PNaming_HArray1OfNamedShape.hxx>
#include <PNaming_Name.hxx>
#include <TNaming_NamedShape.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NameType.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : IntegerToShapeEnum
//purpose  : 
//=======================================================================

static TopAbs_ShapeEnum IntegerToShapeEnum (const Standard_Integer I)
{
  switch(I)
    {
    case  0 : return TopAbs_COMPOUND;
    case  1 : return TopAbs_COMPSOLID;
    case  2 : return TopAbs_SOLID;
    case  3 : return TopAbs_SHELL;
    case  4 : return TopAbs_FACE;
    case  5 : return TopAbs_WIRE;
    case  6 : return TopAbs_EDGE;
    case  7 : return TopAbs_VERTEX;
      default :
	return TopAbs_SHAPE;
    }
  return TopAbs_SHAPE;
}

//=======================================================================
//function : IntegerToNameType
//purpose  : 
//=======================================================================

static  TNaming_NameType IntegerToNameType (const Standard_Integer I) 
{
  switch(I)
    {
    case  0 : return TNaming_UNKNOWN;  
    case  1 : return TNaming_IDENTITY;
    case  2 : return TNaming_MODIFUNTIL;
    case  3 : return TNaming_GENERATION;
    case  4 : return TNaming_INTERSECTION;
    case  5 : return TNaming_UNION;
    case  6 : return TNaming_SUBSTRACTION;
    case  7 : return TNaming_CONSTSHAPE; 
    case  8 : return TNaming_FILTERBYNEIGHBOURGS;
      default :
	Standard_DomainError::Raise("TNaming_NameType; enum term unknown ");
    }
  return TNaming_INTERSECTION;
}

//=======================================================================
//function : MNaming_Naming
//purpose  : 
//=======================================================================

MNaming_NamingRetrievalDriver::MNaming_NamingRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MNaming_NamingRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MNaming_NamingRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PNaming_Naming);  }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MNaming_NamingRetrievalDriver::NewEmpty() const
{ return new TNaming_Naming (); }

//=======================================================================
//function : PNamingToTNaming
//purpose  : 
//=======================================================================

static void PNamingToTNaming (const Handle(PNaming_Name)& PN,
			      TNaming_Name&               TN,
			      const Handle(MDF_RRelocationTable)& RelocTable)

{
  TN.Type     (IntegerToNameType (PN->Type     ()));
  TN.ShapeType(IntegerToShapeEnum(PN->ShapeType()));
  Handle(TNaming_NamedShape) NS;
  
  if (!PN->Arguments().IsNull()) {
    Standard_Integer NbArgs = PN->Arguments()->Length();
    for (Standard_Integer i = 1; i <= NbArgs; i++) {
      RelocTable->HasRelocation(PN->Arguments()->Value(i),NS);
      TN.Append(NS);
    }
  }

  Handle(TNaming_NamedShape) StopNS;
  if (!PN->StopNamedShape().IsNull()) 
    RelocTable->HasRelocation(PN->StopNamedShape(),StopNS);
  TN.StopNamedShape(StopNS);  
  TN.Index(PN->Index());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MNaming_NamingRetrievalDriver::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
  const Handle(MDF_RRelocationTable)& RelocTable) const
{
  //MDesign_FunctionRetrievalDriver::Paste (Source,Target,RelocTable);
  Handle(PNaming_Naming) PF = Handle(PNaming_Naming)::DownCast(Source);
  Handle(TNaming_Naming) F  = Handle(TNaming_Naming)::DownCast(Target);  
  PNamingToTNaming (PF->GetName(),F->ChangeName(),RelocTable);
}



