// Created on: 2001-12-13
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <IntTools_PntOnFace.ixx>
//=======================================================================
//function : IntTools_PntOnFace::IntTools_PntOnFace
//purpose  : 
//=======================================================================
IntTools_PntOnFace::IntTools_PntOnFace()
:
  myIsValid(Standard_False),
  myU(99.),
  myV(99.)
{}
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::Init(const TopoDS_Face& aF,
				const gp_Pnt& aP,
				const Standard_Real anU,
				const Standard_Real aV)
{
  myFace=aF;
  myPnt=aP;
  myU=anU;
  myV=aV;
}
//=======================================================================
//function : SetFace
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetFace(const TopoDS_Face& aF)
{
  myFace=aF;
}

//=======================================================================
//function : SetPnt
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetPnt(const gp_Pnt& aP)
{
  myPnt=aP;
}
//=======================================================================
//function : SetParameters
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetParameters(const Standard_Real anU,
					 const Standard_Real aV)
{
  myU=anU;
  myV=aV;
}
//=======================================================================
//function : SetValid
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::SetValid(const Standard_Boolean bF)
{
  myIsValid=bF;
}

//=======================================================================
//function : Face
//purpose  : 
//=======================================================================
  const TopoDS_Face& IntTools_PntOnFace::Face()const
{
  return myFace;
}
//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================
  const gp_Pnt& IntTools_PntOnFace::Pnt()const
{
  return myPnt;
}
//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
  void IntTools_PntOnFace::Parameters(Standard_Real& anU,
				      Standard_Real& aV)const
{
  anU=myU;
  aV=myV;
}
//=======================================================================
//function : Valid
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_PntOnFace::Valid()const
{
  return myIsValid;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

