// File:      HLRAppli_ReflectLines.cxx
// Created:   05.12.12 12:55:50
// Created by: Julia GERASIMOVA
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

#include <HLRAppli_ReflectLines.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <BRep_Builder.hxx>
#include <gp_Ax3.hxx>

//=======================================================================
//function : HLRAppli_ReflectLines
//purpose  : Constructor
//=======================================================================

HLRAppli_ReflectLines::HLRAppli_ReflectLines(const TopoDS_Shape& aShape)
{
  myShape = aShape;
}

//=======================================================================
//function : SetAxes
//purpose  :
//=======================================================================

void HLRAppli_ReflectLines::SetAxes(const Standard_Real Nx,
                                   const Standard_Real Ny,
                                   const Standard_Real Nz,
                                   const Standard_Real XAt,
                                   const Standard_Real YAt,
                                   const Standard_Real ZAt,
                                   const Standard_Real XUp,
                                   const Standard_Real YUp,
                                   const Standard_Real ZUp)
{
  Standard_Boolean IsPerspective = Standard_False;
  Standard_Real aFocus = 1;
  //Prs3d_Projector aPrs3dProjector(IsPerspective, aFocus, Nx, Ny, Nz, XAt, YAt, ZAt, XUp, YUp, ZUp);

  gp_Pnt At (XAt,YAt,ZAt);
  gp_Dir Zpers (Nx,Ny,Nz);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  gp_Trsf T;
  T.SetTransformation(Axe);

  //myProjector = aPrs3dProjector.Projector();
  myProjector = HLRAlgo_Projector(T,IsPerspective,aFocus);
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================

void HLRAppli_ReflectLines::Perform()
{
  Handle(HLRBRep_Algo) aHLRAlgo = new HLRBRep_Algo();
  aHLRAlgo->Add( myShape, 0 );
  aHLRAlgo->Projector( myProjector );
  aHLRAlgo->Update();
  aHLRAlgo->Hide();
  HLRBRep_HLRToShape aHLRToShape( aHLRAlgo );

  myCompound = aHLRToShape.OutLineVCompound3d();
}

//=======================================================================
//function : GetResult
//purpose  :
//=======================================================================

TopoDS_Shape HLRAppli_ReflectLines::GetResult() const
{
  return myCompound;
}
