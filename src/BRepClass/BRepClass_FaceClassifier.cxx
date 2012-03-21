// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepClass_FaceClassifier.ixx>
#include <TopAbs_State.hxx>
#include <Extrema_ExtPS.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_HSurface.hxx>

//=======================================================================
//function : BRepClass_FaceClassifier
//purpose  : 
//=======================================================================
BRepClass_FaceClassifier::BRepClass_FaceClassifier()
{
}

//=======================================================================
//function : BRepClass_FaceClassifier
//purpose  : 
//=======================================================================
BRepClass_FaceClassifier::BRepClass_FaceClassifier(BRepClass_FaceExplorer& F, 
						   const gp_Pnt2d& P, 
						   const Standard_Real Tol) 
:
  BRepClass_FClassifier(F,P,Tol)
{
}
//=======================================================================
//function : BRepClass_FaceClassifier
//purpose  : 
//=======================================================================
BRepClass_FaceClassifier::BRepClass_FaceClassifier(const TopoDS_Face& F, 
						   const gp_Pnt& P, 
						   const Standard_Real Tol)
{
  Perform(F,P,Tol);
}
//=======================================================================
//function : BRepClass_FaceClassifier
//purpose  : 
//=======================================================================
BRepClass_FaceClassifier::BRepClass_FaceClassifier(const TopoDS_Face& F, 
						   const gp_Pnt2d& P, 
						   const Standard_Real Tol)
{
  Perform(F,P,Tol);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void  BRepClass_FaceClassifier::Perform(const TopoDS_Face& F, 
					const gp_Pnt2d& P, 
					const Standard_Real Tol)
{
  BRepClass_FaceExplorer Fex(F);
  BRepClass_FClassifier::Perform(Fex,P,Tol);
}






//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void  BRepClass_FaceClassifier::Perform(const TopoDS_Face& aF, 
					const gp_Pnt& aP, 
					const Standard_Real aTol)
{
  Standard_Integer aNbExt, aIndice, i; 
  Standard_Real aU1, aU2, aV1, aV2, aMaxDist, aD;
  gp_Pnt2d aPuv;
  Extrema_ExtPS aExtrema;
  //
  aMaxDist=RealLast();
  aIndice=0;
  //
  BRepAdaptor_Surface aSurf(aF);
  BRepTools::UVBounds(aF, aU1, aU2, aV1, aV2);
  aExtrema.Initialize(aSurf, aU1, aU2, aV1, aV2, aTol, aTol);
  //
  //modified by NIZNHY-PKV Wed Aug 13 11:28:47 2008f
  rejected=Standard_True;
  //modified by NIZNHY-PKV Wed Aug 13 11:28:49 2008t
  aExtrema.Perform(aP);
  if(!aExtrema.IsDone()) {
    return;
  }
  //
  aNbExt=aExtrema.NbExt();
  if(!aNbExt) {
    return;
  }
  //
  for (i=1; i<=aNbExt; ++i) {
    aD=aExtrema.SquareDistance(i);
    if(aD < aMaxDist) { 
      aMaxDist=aD;
      aIndice=i;
    }
  }
  //
  if(aIndice) { 
    aExtrema.Point(aIndice).Parameter(aU1, aU2);
    aPuv.SetCoord(aU1, aU2);
    Perform(aF, aPuv, aTol);
  }
}







