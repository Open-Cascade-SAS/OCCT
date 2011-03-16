// File:        TDataStd_AsciiString.cxx
// Created:     Tue Jul 31 12:39:17 2007
// Author:      Sergey ZARITCHNY
//              <sergey.zaritchny@opencascade.com>
//Copyright:    Open CasCade SA 2007

#include <TDataStd_AsciiString.ixx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

//=======================================================================
//function : TDataStd_AsciiString
//purpose  : 
//=======================================================================

TDataStd_AsciiString::TDataStd_AsciiString()
{
  myString.Clear();
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_AsciiString::GetID()
{
  static Standard_GUID theGUID ("3bbefc60-e618-11d4-ba38-0060b0ee18ea");
  return theGUID;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_AsciiString::ID() const
{
  return GetID();
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_AsciiString) TDataStd_AsciiString::Set (
                             const TDF_Label& theLabel,
                             const TCollection_AsciiString& theAsciiString)
{
  Handle(TDataStd_AsciiString) A;
  if (!theLabel.FindAttribute(TDataStd_AsciiString::GetID(), A))
  {
    A = new TDataStd_AsciiString;
    theLabel.AddAttribute(A);
  }
  A->Set(theAsciiString);
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Set (const TCollection_AsciiString& theAsciiString)
{
  Backup();
  myString = theAsciiString;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

const TCollection_AsciiString& TDataStd_AsciiString::Get () const
{
  return myString;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_AsciiString::NewEmpty () const
{
  return new TDataStd_AsciiString();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Restore (const Handle(TDF_Attribute)& theWith)
{
  Handle(TDataStd_AsciiString) R = Handle(TDataStd_AsciiString)::DownCast(theWith);
  myString = R->Get();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Paste (const Handle(TDF_Attribute)& theInto,
                              const Handle(TDF_RelocationTable)& /* RT */) const
{ 
  Handle(TDataStd_AsciiString) R = Handle(TDataStd_AsciiString)::DownCast (theInto);
  R->Set(myString);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_AsciiString::IsEmpty () const
{
  return myString.IsEmpty();
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_AsciiString::Dump(Standard_OStream& theOS) const
{
  Standard_OStream& anOS = TDF_Attribute::Dump( theOS );
  anOS << myString;
  return anOS;
}
