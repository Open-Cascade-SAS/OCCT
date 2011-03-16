// File:	TDocStd_Owner.cxx
// Created:	Mon Jul 12 08:40:14 1999
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


#include <TDocStd_Owner.ixx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_Owner::GetID() 
{ 
  static Standard_GUID TDocStd_OwnerID ("2a96b617-ec8b-11d0-bee7-080009dc3333");
  return TDocStd_OwnerID; 
}


//=======================================================================
//function : SetDocument
//purpose  : 
//=======================================================================

void TDocStd_Owner::SetDocument (const Handle(TDF_Data)& indata,
				 const Handle(TDocStd_Document)& D) 
{
  Handle(TDocStd_Owner) A;
  if (!indata->Root().FindAttribute (TDocStd_Owner::GetID(), A)) {
    A = new TDocStd_Owner (); 
    A->SetDocument(D);
    indata->Root().AddAttribute(A);
  }
  else {  
    Standard_DomainError::Raise("TDocStd_Owner::SetDocument : already called");
  }
}

//=======================================================================
//function : GetDocument
//purpose  : 
//=======================================================================

Handle(TDocStd_Document) TDocStd_Owner::GetDocument (const Handle(TDF_Data)& ofdata)
{
  Handle(TDocStd_Owner) A;
  if (!ofdata->Root().FindAttribute (TDocStd_Owner::GetID(), A)) {
    Standard_DomainError::Raise("TDocStd_Owner::GetDocument : document not found");
  }
  return A->GetDocument();
}

//=======================================================================
//function : TDocStd_Owner
//purpose  : 
//=======================================================================

TDocStd_Owner::TDocStd_Owner () { }


//=======================================================================
//function : SetDocument
//purpose  : 
//=======================================================================

void TDocStd_Owner::SetDocument(const Handle( TDocStd_Document)& D) 
{
  myDocument = D;
}


//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Handle(TDocStd_Document) TDocStd_Owner::GetDocument () const 
{ return myDocument; 
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_Owner::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDocStd_Owner::NewEmpty () const
{
  Handle(TDF_Attribute) dummy;
  return dummy;
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDocStd_Owner::Restore(const Handle(TDF_Attribute)& With) 
{
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDocStd_Owner::Paste (const Handle(TDF_Attribute)& Into,
			      const Handle(TDF_RelocationTable)& RT) const
{
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDocStd_Owner::Dump (Standard_OStream& anOS) const
{  
  anOS << "Owner";
  return anOS;
}

