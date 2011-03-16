
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







