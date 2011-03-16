// File:	MNaming_NamingStorageDriver.cxx
// Created:	Wed Sep 17 17:17:38 1997
// Author:	Isabelle GRIGNON
//		<isg@bigbox.paris1.matra-dtv.fr>


#include <MNaming_NamingStorageDriver.ixx>
#include <PNaming_Naming_1.hxx>
#include <PCollection_HAsciiString.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Naming.hxx>
#include <TCollection_AsciiString.hxx>
#include <TNaming_Name.hxx>
#include <PNaming_Name_1.hxx>
#include <PNaming_NamedShape.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <CDM_MessageDriver.hxx>
#include <PNaming_HArray1OfNamedShape.hxx>

//=======================================================================
//function : IntegerToShapeEnum
//purpose  : 
//=======================================================================

static Standard_Integer ShapeEnumToInteger (const TopAbs_ShapeEnum   I)
{
  switch(I)
    {
    case  TopAbs_COMPOUND  : return 0;
    case  TopAbs_COMPSOLID : return 1;
    case  TopAbs_SOLID     : return 2;
    case  TopAbs_SHELL     : return 3;
    case  TopAbs_FACE      : return 4;
    case  TopAbs_WIRE      : return 5;
    case  TopAbs_EDGE      : return 6;
    case  TopAbs_VERTEX    : return 7;
    case  TopAbs_SHAPE     : return 8;  
    }
  return 8;
}

//=======================================================================
//function : IntegerToNameType
//purpose  : 
//=======================================================================

static  Standard_Integer NameTypeToInteger (const TNaming_NameType I) 
{
  switch(I)
    { 
    case  TNaming_UNKNOWN             : return 0;  
    case  TNaming_IDENTITY            : return 1;
    case  TNaming_MODIFUNTIL          : return 2;
    case  TNaming_GENERATION          : return 3;
    case  TNaming_INTERSECTION        : return 4;
    case  TNaming_UNION               : return 5;
    case  TNaming_SUBSTRACTION        : return 6;
    case  TNaming_CONSTSHAPE          : return 7;
    case  TNaming_FILTERBYNEIGHBOURGS : return 8;
    case  TNaming_ORIENTATION         : return 9;
    case  TNaming_WIREIN              : return 10;
      default :
	Standard_DomainError::Raise("TNaming_NameType; enum term unknown ");
    }
  return 0;
}

//=======================================================================
//function : MNaming_NamingStorageDriver
//purpose  : 
//=======================================================================

MNaming_NamingStorageDriver::MNaming_NamingStorageDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ASDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MNaming_NamingStorageDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MNaming_NamingStorageDriver::SourceType() const
{ return STANDARD_TYPE(TNaming_Naming);  }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(PDF_Attribute) MNaming_NamingStorageDriver::NewEmpty() const
{ return new PNaming_Naming_1 (); }


//=======================================================================
//function : TNamingToPNaming
//purpose  : 
//=======================================================================

static void  TNamingToPNaming  (const TNaming_Name&   TN,
				Handle(PNaming_Name_1)& PN,
				const Handle(MDF_SRelocationTable)& RelocTable)
{
  PN = new PNaming_Name_1();
  PN->Type     (NameTypeToInteger (TN.Type     ()));
  PN->ShapeType(ShapeEnumToInteger(TN.ShapeType()));
  
  Handle (PNaming_NamedShape) PNS;
  Standard_Integer NbArgs = TN.Arguments().Extent();
  
  if (NbArgs > 0) {
    Handle(PNaming_HArray1OfNamedShape) TabArgs = new PNaming_HArray1OfNamedShape(1,NbArgs);
    PN->Arguments(TabArgs);
    
    Standard_Integer i = 1;
    for (TNaming_ListIteratorOfListOfNamedShape it (TN.Arguments()); it.More(); it.Next()) {
      RelocTable->HasRelocation(it.Value(),PNS);
      TabArgs->SetValue(i++,PNS);
    }
  }

  Handle (PNaming_NamedShape) StopPNS;
  if (!TN.StopNamedShape().IsNull())
    RelocTable->HasRelocation(TN.StopNamedShape(),StopPNS);
  PN->StopNamedShape(StopPNS);

  PN->Index(TN.Index());
  Handle(PCollection_HAsciiString) pEntry; // Null handle.
  if(!TN.ContextLabel().IsNull()) {
     TCollection_AsciiString entry;
     TDF_Tool::Entry(TN.ContextLabel(),entry);
     pEntry = new PCollection_HAsciiString(entry);

  }
  PN->ContextLabel(pEntry);
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MNaming_NamingStorageDriver::Paste (
  const Handle(TDF_Attribute)&        Source,
  const Handle(PDF_Attribute)&        Target,
  const Handle(MDF_SRelocationTable)& RelocTable) const
{
  Handle(PNaming_Naming_1) PF = Handle(PNaming_Naming_1)::DownCast(Target);
  Handle(TNaming_Naming) F  = Handle(TNaming_Naming)::DownCast(Source);  
  Handle(PNaming_Name_1) PN ;
  TNamingToPNaming (F->GetName(), PN ,RelocTable);
  PF->SetName(PN);
}

