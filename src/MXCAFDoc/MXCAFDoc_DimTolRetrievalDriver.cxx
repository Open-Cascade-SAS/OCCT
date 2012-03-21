// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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
