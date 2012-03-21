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

// modified by mps (juillet 96 ): on utilise BRepAdaptor a la place de 
// GeomAdaptor dans Initialize et Perform.

#include <BRepExtrema_ExtFF.hxx>

#include <BRepExtrema_ExtCF.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <BRepAdaptor_HSurface.hxx>

//=======================================================================
//function : BRepExtrema_ExtFF
//purpose  : 
//=======================================================================

BRepExtrema_ExtFF::BRepExtrema_ExtFF(const TopoDS_Face& F1, const TopoDS_Face& F2)
{
 Initialize(F2);
 Perform(F1,F2);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::Initialize(const TopoDS_Face& F2)
{
  BRepAdaptor_Surface Surf(F2);
  myHS = new BRepAdaptor_HSurface(Surf);
  const Standard_Real Tol = BRep_Tool::Tolerance(F2);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F2, U1, U2, V1, V2);
  myExtSS.Initialize(myHS->Surface(), U1, U2, V1, V2, Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepExtrema_ExtFF::Perform(const TopoDS_Face& F1, const TopoDS_Face& F2)
{ 
  mySqDist.Clear();
  myPointsOnS1.Clear();
  myPointsOnS2.Clear();

  BRepAdaptor_Surface Surf1(F1);
  Handle(BRepAdaptor_HSurface) HS1 = new BRepAdaptor_HSurface(Surf1);
  const Standard_Real Tol1 = BRep_Tool::Tolerance(F1);
  Standard_Real U1, U2, V1, V2;
  BRepTools::UVBounds(F1, U1, U2, V1, V2);
  myExtSS.Perform(HS1->Surface(), U1, U2, V1, V2, Tol1);

  if (!myExtSS.IsDone())
    return;

  if (myExtSS.IsParallel())
    mySqDist.Append(myExtSS.SquareDistance(1));
  else
  {
    // Exploration of points and classification
    BRepClass_FaceClassifier classifier;
    const Standard_Real Tol2 = BRep_Tool::Tolerance(F2);
    Extrema_POnSurf P1, P2;

    Standard_Integer i;
    for (i = 1; i <= myExtSS.NbExt(); i++)
    {
      myExtSS.Points(i, P1, P2);
      P1.Parameter(U1, U2);
      const gp_Pnt2d Puv1(U1, U2);
      classifier.Perform(F1, Puv1, Tol1);
      const TopAbs_State state1 = classifier.State();
      if (state1 == TopAbs_ON || state1 == TopAbs_IN)
      {
        P2.Parameter(U1, U2);
        const gp_Pnt2d Puv2(U1, U2);
        classifier.Perform(F2, Puv2, Tol2);
        const TopAbs_State state2 = classifier.State();
        if (state2 == TopAbs_ON || state2 == TopAbs_IN)
        {
          mySqDist.Append(myExtSS.SquareDistance(i));
          myPointsOnS1.Append(P1);
          myPointsOnS2.Append(P2);
        }
      }
    }
  }
}
