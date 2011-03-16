// File:      BinMDF_ReferenceDriver.cxx
// Created:   30.10.02 12:52:35
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

#include <BinMDF_ReferenceDriver.ixx>
#include <TDF_Reference.hxx>

//=======================================================================
//function : BinMDF_ReferenceDriver
//purpose  : Constructor
//=======================================================================

BinMDF_ReferenceDriver::BinMDF_ReferenceDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDF_Reference)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMDF_ReferenceDriver::NewEmpty() const
{
  return new TDF_Reference();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDF_ReferenceDriver::Paste
                (const BinObjMgt_Persistent&   theSource,
                 const Handle(TDF_Attribute)&  theTarget,
                 BinObjMgt_RRelocationTable&   ) const
{
  Handle(TDF_Reference) aRef = Handle(TDF_Reference)::DownCast(theTarget);

  TDF_Label tLab; // Null label.
  if (!theSource.GetLabel(aRef->Label().Data(), tLab))
    return Standard_False;

  // set referenced label
  aRef->Set(tLab);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMDF_ReferenceDriver::Paste (const Handle(TDF_Attribute)&  theSource,
                                    BinObjMgt_Persistent&         theTarget,
                                    BinObjMgt_SRelocationTable&   ) const
{
  Handle(TDF_Reference) aRef = Handle(TDF_Reference)::DownCast(theSource);
  if (!aRef.IsNull()) {
    const TDF_Label& lab = aRef->Label();
    const TDF_Label& refLab = aRef->Get();
    if (!lab.IsNull() && !refLab.IsNull()) {
      if (lab.IsDescendant(refLab.Root())) {   // Internal reference
        theTarget << refLab;
      }
    }
  }
}
