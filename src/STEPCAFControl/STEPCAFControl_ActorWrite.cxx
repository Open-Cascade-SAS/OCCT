// Created on: 2000-10-05
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

