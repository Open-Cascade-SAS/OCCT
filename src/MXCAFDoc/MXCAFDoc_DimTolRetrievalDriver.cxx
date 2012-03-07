// File:      MXCAFDoc_DimTolRetrievalDriver.cxx
// Created:   10.12.08 09:46:46
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <MXCAFDoc_DimTolRetrievalDriver.ixx>
#include <PXCAFDoc_DimTol.hxx>
#include <XCAFDoc_DimTol.hxx>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <PCollection_HAsciiString.hxx>
#include <PColStd_HArray1OfReal.hxx>


//=======================================================================
//function : MCAFDoc_DimTolRetrievalDriver
//purpose  : 
//=======================================================================

MXCAFDoc_DimTolRetrievalDriver::MXCAFDoc_DimTolRetrievalDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver) : MDF_ARDriver (theMsgDriver)
{}

//=======================================================================
//function : VersionNumber
//purpose  : 
//=======================================================================

Standard_Integer MXCAFDoc_DimTolRetrievalDriver::VersionNumber() const
{ return 0; }

//=======================================================================
//function : SourceType
//purpose  : 
//=======================================================================

Handle(Standard_Type) MXCAFDoc_DimTolRetrievalDriver::SourceType() const
{
  static Handle(Standard_Type) sourceType = STANDARD_TYPE(PXCAFDoc_DimTol);
  return sourceType;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) MXCAFDoc_DimTolRetrievalDriver::NewEmpty() const
{
  return new XCAFDoc_DimTol();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void MXCAFDoc_DimTolRetrievalDriver::Paste(const Handle(PDF_Attribute)& Source,
                                           const Handle(TDF_Attribute)& Target,
                                           const Handle(MDF_RRelocationTable)& RelocTable) const
{
  Handle(PXCAFDoc_DimTol) S = Handle(PXCAFDoc_DimTol)::DownCast (Source);
  Handle(XCAFDoc_DimTol) T = Handle(XCAFDoc_DimTol)::DownCast (Target);
  
  Handle(TColStd_HArray1OfReal) aVals;
  Handle(PColStd_HArray1OfReal) aPArr = S->GetVal();
  if ( !aPArr.IsNull() )
  {
    Standard_Integer i = aPArr->Lower(), n = aPArr->Upper();
    aVals = new TColStd_HArray1OfReal( i, n );
     for( ; i <= n; i++ )
       aVals->SetValue( i, aPArr->Value( i ) );
  }
  Handle(TCollection_HAsciiString) aName, aDescr;
  if ( !S->GetName().IsNull() )
    aName = new TCollection_HAsciiString( (S->GetName())->Convert() );
  if ( !S->GetDescription().IsNull() )
    aDescr = new TCollection_HAsciiString( (S->GetDescription())->Convert() );

  T->Set(S->GetKind(), aVals, aName, aDescr);
}
