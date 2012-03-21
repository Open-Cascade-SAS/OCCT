// Created on: 1993-12-15
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// modified by MPS (june 96) : on utilise BRepClass_FaceClassifier seulement 
//  si IsDone de Extrema est vrai  

#include <BRepExtrema_ExtPF.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <gp_Pnt2d.hxx>
#include <BRepAdaptor_Surface.hxx>

//=======================================================================
//function : BRepExtrema_ExtPF
//purpose  : 
//=======================================================================

BRepExtrema_ExtPF::BRepExtrema_ExtPF(const TopoDS_Vertex& TheVertex, const TopoDS_Face& TheFace,
                                     const Extrema_ExtFlag TheFlag, const Extrema_ExtAlgo TheAlgo)
{
  Initialize(TheFace,TheFlag,TheAlgo);
  Perform(TheVertex,TheFace);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPF::Initialize(const TopoDS_Face& TheFace,
                                   const Extrema_ExtFlag TheFlag, const Extrema_ExtAlgo TheAlgo)
{
  // cette surface doit etre en champ. Extrema ne fait
  // pas de copie et prend seulement un pointeur dessus.
  mySurf.Initialize(TheFace, Standard_False); 
  const Standard_Real Tol = BRep_Tool::Tolerance(TheFace);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(TheFace, U1, U2, V1, V2);
  myExtPS.SetFlag(TheFlag);
  myExtPS.SetAlgo(TheAlgo);
  myExtPS.Initialize(mySurf, U1, U2, V1, V2, Tol, Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtPF::Perform(const TopoDS_Vertex& TheVertex, const TopoDS_Face& TheFace)
{
  mySqDist.Clear();
  myPoints.Clear();

  const gp_Pnt P = BRep_Tool::Pnt(TheVertex);
  myExtPS.Perform(P);

  // Exploration of points and classification
  if (myExtPS.IsDone())
  {
    BRepClass_FaceClassifier classifier;
    Standard_Real U1, U2;
    const Standard_Real Tol = BRep_Tool::Tolerance(TheFace);
    for (Standard_Integer i = 1; i <= myExtPS.NbExt(); i++)
    {
      myExtPS.Point(i).Parameter(U1, U2);
      const gp_Pnt2d Puv(U1, U2);
      classifier.Perform(TheFace, Puv, Tol);
      const TopAbs_State state = classifier.State();
      if(state == TopAbs_ON || state == TopAbs_IN)
      {
        mySqDist.Append(myExtPS.SquareDistance(i));
        myPoints.Append(myExtPS.Point(i));
      }
    }
  }
}
