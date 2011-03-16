// File:	STEPCAFControl_ActorWrite.cxx
// Created:	Thu Oct  5 20:00:00 2000
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

#include <STEPCAFControl_ActorWrite.ixx>

//=======================================================================
//function : STEPCAFControl_ActorWrite
//purpose  : 
//=======================================================================

STEPCAFControl_ActorWrite::STEPCAFControl_ActorWrite () : myStdMode(Standard_True)
{
}

//=======================================================================
//function : ClearMap
//purpose  : 
//=======================================================================

void STEPCAFControl_ActorWrite::SetStdMode (const Standard_Boolean stdmode)
{
  myStdMode = stdmode;
  if ( myStdMode ) ClearMap();
}

//=======================================================================
//function : ClearMap
//purpose  : 
//=======================================================================

void STEPCAFControl_ActorWrite::ClearMap ()
{
  myMap.Clear();
}

//=======================================================================
//function : RegisterAssembly
//purpose  : 
//=======================================================================

void STEPCAFControl_ActorWrite::RegisterAssembly (const TopoDS_Shape &S)
{
  if ( ! myStdMode && S.ShapeType() == TopAbs_COMPOUND ) myMap.Add ( S );
}

//=======================================================================
//function : IsAssembly
//purpose  : 
//=======================================================================

Standard_Boolean STEPCAFControl_ActorWrite::IsAssembly (TopoDS_Shape &S) const
{
  if ( myStdMode ) return STEPControl_ActorWrite::IsAssembly ( S );
  return myMap.Contains ( S );
}

