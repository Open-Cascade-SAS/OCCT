// File:	MDataXtd_PatternStdRetrievalDriver.cxx
// Created:	Mon Feb 16 14:37:06 1998
// Author:	Jing Cheng MEI
//		<mei@pinochiox.paris1.matra-dtv.fr>


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
