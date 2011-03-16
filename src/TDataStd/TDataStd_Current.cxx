// File:	TDataStd_Current.cxx
// Created:	Mon Aug  2 09:58:09 1999
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <TDataStd_Current.ixx>
#include <TDF_Data.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Current::GetID () 
{
  static Standard_GUID TDataStd_CurrentID("2a96b623-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_CurrentID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDataStd_Current::Set (const TDF_Label& current)
{ 
  Handle(TDataStd_Current) A;
  Handle(TDF_Data) D = current.Data();
  if (!D->Root().FindAttribute (TDataStd_Current::GetID(), A)) {
    A = new TDataStd_Current (); 
    D->Root().AddAttribute(A);
  }
  A->SetLabel (current);
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

TDF_Label TDataStd_Current::Get (const TDF_Label& access)
{ 
//  TDF_Label current;
  Handle(TDataStd_Current) A;
  if (!access.Data()->Root().FindAttribute (TDataStd_Current::GetID(), A)) {
    Standard_DomainError::Raise("TDataStd_Current::Get : not setted");
  }  
  return A->GetLabel();
}

//=======================================================================
//function : Has
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_Current::Has (const TDF_Label& access)
{ 
  return (access.Data()->Root().IsAttribute (TDataStd_Current::GetID()));
}

//=======================================================================
//function : TDataStd_Current
//purpose  : 
//=======================================================================

TDataStd_Current::TDataStd_Current () { }



//=======================================================================
//function : SetLabel
//purpose  : 
//=======================================================================

void TDataStd_Current::SetLabel (const TDF_Label& current) 
{
  // OCC2932 correction
  if(myLabel == current) return;

  Backup();
  myLabel = current;
}

//=======================================================================
//function : GetLabel
//purpose  : 
//=======================================================================

TDF_Label TDataStd_Current::GetLabel () const
{
  return myLabel;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Current::ID() const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Current::NewEmpty () const
{  
  return new TDataStd_Current(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Current::Restore(const Handle(TDF_Attribute)& With) 
{
  myLabel = Handle(TDataStd_Current)::DownCast (With)->GetLabel ();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Current::Paste (const Handle(TDF_Attribute)& Into,
				const Handle(TDF_RelocationTable)& RT) const
{
  TDF_Label tLab;
  if (!myLabel.IsNull()) {
    if (!RT->HasRelocation(myLabel,tLab)) tLab = myLabel;
  }
  Handle(TDataStd_Current)::DownCast(Into)->SetLabel(tLab);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Current::Dump (Standard_OStream& anOS) const
{  
  anOS << "Current";
  return anOS;
}


