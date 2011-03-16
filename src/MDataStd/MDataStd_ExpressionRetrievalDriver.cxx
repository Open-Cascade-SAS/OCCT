#include <MDataStd_ExpressionRetrievalDriver.ixx>
#include <PDataStd_Expression.hxx>
#include <TDataStd_Expression.hxx>
#include <TCollection_ExtendedString.hxx>
#include <PCollection_HExtendedString.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx> 
#include <PDF_HAttributeArray1.hxx> 
#include <Standard_NoSuchObject.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : MDataStd_ExpressionRetrievalDriver
//purpose  : 
//=======================================================================

MDataStd_ExpressionRetrievalDriver::MDataStd_ExpressionRetrievalDriver(const Handle(CDM_MessageDriver)& theMsgDriver):MDF_ARDriver(theMsgDriver)
{
}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MDataStd_ExpressionRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MDataStd_ExpressionRetrievalDriver::SourceType() const
{ return STANDARD_TYPE(PDataStd_Expression); }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MDataStd_ExpressionRetrievalDriver::NewEmpty() const
{ return new TDataStd_Expression (); }

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MDataStd_ExpressionRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,const Handle(TDF_Attribute)& Target,const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PDataStd_Expression) S = Handle(PDataStd_Expression)::DownCast (Source);
  Handle(TDataStd_Expression) T = Handle(TDataStd_Expression)::DownCast (Target);
  TCollection_ExtendedString Expression = (S->GetName())->Convert ();
  T->SetExpression (Expression);  
  
  Handle(PDF_Attribute) PV; 
  Handle(TDF_Attribute) TV;

  Handle(PDF_HAttributeArray1) PVARS = S->GetVariables ();
  if (PVARS.IsNull()) return;
  for (Standard_Integer i = 1; i <= PVARS->Length (); i++) {
    PV = PVARS->Value (i);
    if (!PV.IsNull ()) {
      Handle(TDF_Attribute) TV;
      if (!RelocTable->HasRelocation (PV, TV)) {
	Standard_NoSuchObject::Raise("MDataStd_ConstraintRetrievalDriver::Paste");
      }
      T->GetVariables().Append(TV);
    }
  }
}
