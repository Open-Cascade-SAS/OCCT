// File:	TopOpeBRep_PointClassifier.cxx
// Created:	Thu Nov 18 10:35:11 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>

#include <TopOpeBRep_PointClassifier.ixx>

#include <BRepAdaptor_Surface.hxx>

//=======================================================================
//function : TopOpeBRep_PointClassifier
//purpose  : 
//=======================================================================

TopOpeBRep_PointClassifier::TopOpeBRep_PointClassifier()
{
  myHSurface = new BRepAdaptor_HSurface();
  Init();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void TopOpeBRep_PointClassifier::Init() 
{
  myTopolToolMap.Clear();
  myState = TopAbs_UNKNOWN;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void TopOpeBRep_PointClassifier::Load(const TopoDS_Face& F) 
{
  Standard_Boolean found = myTopolToolMap.IsBound(F);
  if ( ! found ) {
    myHSurface->ChangeSurface().Initialize(F);
    myTopolTool = new BRepTopAdaptor_TopolTool(myHSurface);
    myTopolToolMap.Bind(F,myTopolTool);
  }
  else {
    myTopolTool = myTopolToolMap.Find(F);
  }
}

//=======================================================================
//function : Classify
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRep_PointClassifier::Classify
  (const TopoDS_Face& F, 
   const gp_Pnt2d& P2d, 
   const Standard_Real Tol)
{
  myState = TopAbs_UNKNOWN;
  Load(F);
  myState = myTopolTool->Classify(P2d,Tol);

  return myState;
}


//=======================================================================
//function : State
//purpose  : 
//=======================================================================

TopAbs_State TopOpeBRep_PointClassifier::State() const
{
  return myState;
}
