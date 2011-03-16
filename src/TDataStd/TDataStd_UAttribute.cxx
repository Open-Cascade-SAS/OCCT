// File:	TDataStd_UAttribute.cxx
// Created:	Fri Jun 11 14:39:43 1999
// Author:	Sergey RUIN
//		<s-ruin@nnov.matra-dtv.fr>


#include <TDataStd_UAttribute.ixx>
#include <TDataStd.hxx>
#include <TDF_Label.hxx>

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_UAttribute) TDataStd_UAttribute::Set (const TDF_Label& label, const Standard_GUID& guid )
{
  Handle(TDataStd_UAttribute) A;  
  if (!label.FindAttribute(guid, A)) {
    A = new TDataStd_UAttribute ();
    A->SetID(guid);
    label.AddAttribute(A);                      
  }
  return A;
}


//=======================================================================
//function : TDataStd_UAttribute
//purpose  : 
//=======================================================================

TDataStd_UAttribute::TDataStd_UAttribute()
{
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_UAttribute::ID() const
{ return myID; }


//=======================================================================
//function : SetID
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::SetID( const Standard_GUID&  guid)
{
  // OCC2932 correction
  if(myID == guid) return;

  Backup();
  myID = guid;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_UAttribute::NewEmpty () const
{ 
  Handle(TDataStd_UAttribute) A = new TDataStd_UAttribute();
  A->SetID(myID);
  return A; 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::Restore(const Handle(TDF_Attribute)& with) 
{  
  Handle(TDataStd_UAttribute) A = Handle(TDataStd_UAttribute)::DownCast(with);
  SetID( A->ID() );
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::Paste (const Handle(TDF_Attribute)& into,
			   const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_UAttribute) A = Handle(TDataStd_UAttribute)::DownCast(into);
  A->SetID( myID );
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::References (const Handle(TDF_DataSet)& DS) const
{  
}
 
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_UAttribute::Dump (Standard_OStream& anOS) const
{  
  anOS << "UAttribute";
  TDF_Attribute::Dump(anOS);
  return anOS;
}


