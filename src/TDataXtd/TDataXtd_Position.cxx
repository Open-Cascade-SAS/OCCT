// File:	TDataXtd_Position.cxx
// Created:	Mon Apr  6 18:31:09 2009
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
//Copyright:    Open CasCade SA 2009


#include <TDataXtd_Position.ixx>
#include <TDF_Label.hxx>

//=======================================================================
//function : Set (class method)
//purpose  : 
//=======================================================================
void TDataXtd_Position::Set(const TDF_Label& aLabel, const gp_Pnt& aPos) 
{
  Handle(TDataXtd_Position) pos;
  if (!aLabel.FindAttribute(TDataXtd_Position::GetID(), pos)) {
    pos = new TDataXtd_Position();
    aLabel.AddAttribute(pos);
  }
  pos->SetPosition( aPos ); 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataXtd_Position) TDataXtd_Position::Set (const TDF_Label& L)
{
  Handle(TDataXtd_Position) POS;
  if (!L.FindAttribute (TDataXtd_Position::GetID (), POS)) {    
    POS = new TDataXtd_Position;
    L.AddAttribute(POS);
  }
  return POS;
}

//=======================================================================
//function : Get (class method)
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Position::Get(const TDF_Label& aLabel, gp_Pnt& aPos) 
{
  Handle(TDataXtd_Position) pos;
  if( aLabel.FindAttribute( TDataXtd_Position::GetID(), pos) ) {
    aPos = pos->GetPosition();
    return Standard_True; 
  }
  return Standard_False; 
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Position::GetID() 
{
  static Standard_GUID TDataXtd_Position_guid("55553252-ce0c-11d1-b5d8-00a0c9064368");
  return TDataXtd_Position_guid;
}

//=======================================================================
//function : TDataXtd_Position
//purpose  : 
//=======================================================================
TDataXtd_Position::TDataXtd_Position()
  :myPosition(gp_Pnt(0.,0.,0.))
{
}

//=======================================================================
//function : GetPosition
//purpose  : 
//=======================================================================
const gp_Pnt& TDataXtd_Position::GetPosition() const
{
  return myPosition;
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================
void TDataXtd_Position::SetPosition(const gp_Pnt& aPos) 
{
  // OCC2932 correction
  if(myPosition.X() == aPos.X() &&
     myPosition.Y() == aPos.Y() &&
     myPosition.Z() == aPos.Z())
    return;

  Backup();
  myPosition = aPos;
}

 

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Position::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataXtd_Position::Restore(const Handle(TDF_Attribute)& anAttribute) 
{
  myPosition = Handle(TDataXtd_Position)::DownCast(anAttribute)->GetPosition();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataXtd_Position::NewEmpty() const
{
  return new TDataXtd_Position; 
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataXtd_Position::Paste(const Handle(TDF_Attribute)& intoAttribute,
			     const Handle(TDF_RelocationTable)&) const
{
  Handle(TDataXtd_Position)::DownCast(intoAttribute)->SetPosition(myPosition);
}
