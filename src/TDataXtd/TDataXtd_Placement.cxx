// File:	TDataXtd_Placement.cxx
// Created:	Mon Apr  6 18:14:13 2009
// Author:	Sergey ZARITCHNY
//		<szy@covox>


#include <TDataXtd_Placement.ixx>
//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Placement::GetID () 
{
  static Standard_GUID TDataXtd_PlacementID ("2a96b60b-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_PlacementID; 
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataXtd_Placement) TDataXtd_Placement::Set (const TDF_Label& L)
{
  Handle (TDataXtd_Placement) A;
  if (!L.FindAttribute (TDataXtd_Placement::GetID (), A)) {
    A = new TDataXtd_Placement ();
    L.AddAttribute(A);
  }
  return A;
}


//=======================================================================
//function : TDataXtd_Placement
//purpose  : 
//=======================================================================

TDataXtd_Placement::TDataXtd_Placement () { }



//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Placement::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataXtd_Placement::NewEmpty() const
{ 
  return new TDataXtd_Placement(); 
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataXtd_Placement::Restore (const Handle(TDF_Attribute)& other) 
{

}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataXtd_Placement::Paste(const Handle(TDF_Attribute)& Into,
			       const Handle(TDF_RelocationTable)& RT) const
{

}    



//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataXtd_Placement::Dump (Standard_OStream& anOS) const
{ 
  anOS << "Placement";
  return anOS;
}
