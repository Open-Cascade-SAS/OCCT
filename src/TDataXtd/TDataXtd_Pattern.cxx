// File:	TDataXtd_Pattern.cxx
// Created:	Mon Apr  6 18:00:08 2009
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
//Copyright:    Open CasCade SA 2009


#include <TDataXtd_Pattern.ixx>
//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Pattern::GetID()
{
  static Standard_GUID myID("2a96b618-ec8b-11d0-bee7-080009dc3333");
  return myID;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Pattern::ID() const
{ return GetID(); }
