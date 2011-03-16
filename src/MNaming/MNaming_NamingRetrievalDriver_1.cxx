// File:	MNaming_NamingRetrievalDriver_1.cxx
// Created:	Fri Aug 15 17:09:32 2008
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:   Open CasCade SA 2008


#include <MNaming_NamingRetrievalDriver_1.ixx>
#include <PNaming_Naming_1.hxx>
#include <PNaming_NamedShape.hxx>
#include <PNaming_HArray1OfNamedShape.hxx>
#include <PNaming_Name_1.hxx>
#include <PCollection_HAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>
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
    case  9 : return TNaming_ORIENTATION;
    case  10 : return TNaming_WIREIN;
      default :
	Standard_DomainError::Raise("TNaming_NameType; enum term unknown ");
    }
  return TNaming_INTERSECTION;
}

//=======================================================================
//function : MNaming_Naming_1
//purpose  : 
//=======================================================================

MNaming_NamingRetrievalDriver_1::MNaming_NamingRetrievalDriver_1(
 const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MNaming_NamingRetrievalDriver_1::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MNaming_NamingRetrievalDriver_1::SourceType() const
{ return STANDARD_TYPE(PNaming_Naming_1);  }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MNaming_NamingRetrievalDriver_1::NewEmpty() const
{ return new TNaming_Naming (); }

//=======================================================================
//function : PNamingToTNaming
//purpose  : 
//=======================================================================

static void PNamingToTNaming (const Handle(PNaming_Name_1)& PN,
			      TNaming_Name&                 TN,
			      const Handle(MDF_RRelocationTable)& RelocTable)

{
  TN.Type     (IntegerToNameType (PN->Type     ()));
  TN.ShapeType(IntegerToShapeEnum(PN->ShapeType()));
  Handle(TNaming_NamedShape) NS;
  Handle(TDF_Data) aData;
  if (!PN->Arguments().IsNull()) {
    Standard_Integer NbArgs = PN->Arguments()->Length();
    for (Standard_Integer i = 1; i <= NbArgs; i++) {
      RelocTable->HasRelocation(PN->Arguments()->Value(i),NS);
      TN.Append(NS);
      if(aData.IsNull() && !NS.IsNull())
	aData = NS->Label().Data();
    }
  }

  Handle(TNaming_NamedShape) StopNS;
  if (!PN->StopNamedShape().IsNull()) 
    RelocTable->HasRelocation(PN->StopNamedShape(),StopNS);
  TN.StopNamedShape(StopNS);  
  TN.Index(PN->Index());

  TDF_Label tLab; // Null label.  
  Handle(PCollection_HAsciiString) pEntry = PN->ContextLabel();
  if (!pEntry.IsNull()) {
    TCollection_AsciiString entry = pEntry->Convert();
    if(!aData.IsNull())
      TDF_Tool::Label(aData,entry,tLab);
  }
  TN.ContextLabel(tLab);
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MNaming_NamingRetrievalDriver_1::Paste (
  const Handle(PDF_Attribute)&        Source,
  const Handle(TDF_Attribute)&        Target,
  const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PNaming_Naming_1) PF = Handle(PNaming_Naming_1)::DownCast(Source);
  Handle(TNaming_Naming) F  = Handle(TNaming_Naming)::DownCast(Target);  
  PNamingToTNaming (PF->GetName(),F->ChangeName(),RelocTable);
}

