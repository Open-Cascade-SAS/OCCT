// File:	BRepAlgo_Section.cxx
// Created:	Fri Feb 18 11:14:56 1994
// Author:	Remi LEQUETTE
//		<rle@nonox>

#include <BRepAlgo_Section.ixx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <TopOpeBRepDS_BuildTool.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <TopOpeBRepBuild_Tools.hxx>

static TopoDS_Shape MakeShape(const Handle(Geom_Surface)& );

//=======================================================================
//function : BRepAlgo_Section
//purpose  : 
//=======================================================================
  BRepAlgo_Section::BRepAlgo_Section(const TopoDS_Shape& S1, 
				     const TopoDS_Shape& S2,
				     const Standard_Boolean PerformNow)
: BRepAlgo_BooleanOperation(S1, S2)
{
  InitParameters();
  myS1Changed = Standard_True;
  myS2Changed = Standard_True;  
  if(myS1.IsNull() || S2.IsNull()) {
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : BRepAlgo_Section
//purpose  : 
//=======================================================================
  BRepAlgo_Section::BRepAlgo_Section(const TopoDS_Shape& S1, 
				     const gp_Pln& Pl,
				     const Standard_Boolean PerformNow) 
: BRepAlgo_BooleanOperation(S1, MakeShape(new Geom_Plane(Pl)))
{ 
  InitParameters();
  myS1Changed = Standard_True;
  myS2Changed = Standard_True;  
  if(S1.IsNull() || myS2.IsNull()) {
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : BRepAlgo_Section
//purpose  : 
//=======================================================================
  BRepAlgo_Section::BRepAlgo_Section(const TopoDS_Shape& S1, 
				     const Handle(Geom_Surface)& Sf,
				     const Standard_Boolean PerformNow)
: BRepAlgo_BooleanOperation(S1, MakeShape(Sf))
{
  InitParameters();
  myS1Changed = Standard_True;
  myS2Changed = Standard_True;  
  if(S1.IsNull() || myS2.IsNull()) {
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : BRepAlgo_Section
//purpose  : 
//=======================================================================
  BRepAlgo_Section::BRepAlgo_Section(const Handle(Geom_Surface)& Sf, 
				     const TopoDS_Shape& S2,
				     const Standard_Boolean PerformNow)
: BRepAlgo_BooleanOperation(MakeShape(Sf), S2)
{
  InitParameters();
  myS1Changed = Standard_True;
  myS2Changed = Standard_True;  
  if(myS1.IsNull() || S2.IsNull()) {
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : BRepAlgo_Section
//purpose  : 
//=======================================================================
  BRepAlgo_Section::BRepAlgo_Section(const Handle(Geom_Surface)& Sf1,
				     const Handle(Geom_Surface)& Sf2,
				     const Standard_Boolean PerformNow)
: BRepAlgo_BooleanOperation(MakeShape(Sf1), MakeShape(Sf2))
{
  InitParameters();
  myS1Changed = Standard_True;
  myS2Changed = Standard_True;  
  if(myS1.IsNull() || myS2.IsNull()) {
    myshapeisnull = Standard_True;
  }
  if (PerformNow)
    Build();
}

//=======================================================================
//function : Init1
//purpose  : initialize the first Shape
//=======================================================================
  void BRepAlgo_Section::Init1(const TopoDS_Shape& S1)
{
  if(!S1.IsNull()) {
    if (!S1.IsEqual(myS1)) {
      myS1 = S1;
      myS1Changed = Standard_True;
    }
  } else {
    if(!myS1.IsNull()) {
      myS1 = S1;
      myS1Changed = Standard_True;
    }      
  }
  if (myS1Changed || myS2Changed)
    NotDone();
}

//=======================================================================
//function : Init1
//purpose  : initialize the first Shape
//=======================================================================
  void BRepAlgo_Section::Init1(const gp_Pln& Pl)
{
  Init1(MakeShape(new Geom_Plane(Pl)));
}

//=======================================================================
//function : Init1
//purpose  : initialize the first Shape
//=======================================================================
  void BRepAlgo_Section::Init1(const Handle(Geom_Surface)& Sf)
{
  Init1(MakeShape(Sf));
}

//=======================================================================
//function : Init2
//purpose  : initialize the second Shape
//=======================================================================
  void BRepAlgo_Section::Init2(const TopoDS_Shape& S2)
{
  if(!S2.IsNull()) {
    if (!S2.IsEqual(myS2)) {
      myS2 = S2;
      myS2Changed = Standard_True;
    }
  } else {
    if(!myS2.IsNull()) {
      myS2 = S2;
      myS2Changed = Standard_True;
    }      
  }
  if (myS1Changed || myS2Changed)
    NotDone();
}

//=======================================================================
//function : Init2
//purpose  : initialize the second Shape
//=======================================================================
  void BRepAlgo_Section::Init2(const gp_Pln& Pl)
{
  Init2(MakeShape(new Geom_Plane(Pl)));
}

//=======================================================================
//function : Init2
//purpose  : initialize the second Shape
//=======================================================================
  void BRepAlgo_Section::Init2(const Handle(Geom_Surface)& Sf)
{
  Init2(MakeShape(Sf));
}

//=======================================================================
//function : Approximation
//purpose  : To learn if an approximation of the geometry is calculated
//=======================================================================
  void BRepAlgo_Section::Approximation(const Standard_Boolean Approx)
{
  TopOpeBRepDS_BuildTool& BTofBuilder = myHBuilder->ChangeBuildTool();
  TopOpeBRepTool_GeomTool& GTofBTofBuilder = BTofBuilder.ChangeGeomTool();

  TopOpeBRepTool_OutCurveType OCT = 
    (Approx) ? TopOpeBRepTool_APPROX :TopOpeBRepTool_BSPLINE1;  
  if (GTofBTofBuilder.TypeC3D() != OCT) {
    myApproxChanged = Standard_True;
    GTofBTofBuilder.Define(OCT);
  }  
  
  if (myApproxChanged)
    NotDone();
}

//=======================================================================
//function : ComputePCurveOn1
//purpose  : To learn if PCurves are calculated on the faces of the 1st Shape
//=======================================================================
  void BRepAlgo_Section::ComputePCurveOn1(const Standard_Boolean ComputePCurve1)
{
  TopOpeBRepDS_BuildTool& BTofBuilder = myHBuilder->ChangeBuildTool();
  TopOpeBRepTool_GeomTool& GTofBTofBuilder = BTofBuilder.ChangeGeomTool();

  if( GTofBTofBuilder.CompPC1() != ComputePCurve1) {
    GTofBTofBuilder.DefinePCurves1(ComputePCurve1);
    myPCurve1Changed = Standard_True;
  }

  if(myPCurve1Changed)
    NotDone();
}

//=======================================================================
//function : ComputePCurve2
//purpose  : To learn if PCurves are calculated on the faces of the 2nd Shape
//=======================================================================
  void BRepAlgo_Section::ComputePCurveOn2(const Standard_Boolean ComputePCurve2)
{
  TopOpeBRepDS_BuildTool& BTofBuilder = myHBuilder->ChangeBuildTool();
  TopOpeBRepTool_GeomTool& GTofBTofBuilder = BTofBuilder.ChangeGeomTool();
  
  if( GTofBTofBuilder.CompPC2() != ComputePCurve2) {
    GTofBTofBuilder.DefinePCurves2(ComputePCurve2);
    myPCurve2Changed = Standard_True;
  }
  
  if(myPCurve2Changed)
    NotDone();
}

//=======================================================================
//function : Build
//purpose  : compute the section
//=======================================================================
  void BRepAlgo_Section::Build()
{
  if (myS1Changed || 
      myS2Changed ||
      myApproxChanged ||
      myPCurve1Changed ||
      myPCurve2Changed) {
    PerformDS();
    Standard_Boolean bcw = BuilderCanWork();
    if ( ! bcw || myshapeisnull) return;
    BRep_Builder BB;
    BB.MakeCompound(TopoDS::Compound(myShape));
    Handle(TopOpeBRepBuild_HBuilder) HB = Builder();
    TopTools_ListIteratorOfListOfShape itloe = HB->Section();
    for(; itloe.More(); itloe.Next()) BB.Add(myShape,itloe.Value());
    
    TopOpeBRepBuild_Tools::CorrectTolerances(myShape);
    
    
    Done();
  }


}

//=======================================================================
//function : HasAncestorFaceOn1
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgo_Section::HasAncestorFaceOn1(const TopoDS_Shape& E, 
							   TopoDS_Shape& F)const
{
  TopoDS_Shape F1,F2; Standard_Integer iC;
  Standard_Boolean res = myHBuilder->EdgeCurveAncestors(E,F1,F2,iC);
  if ( res ) F = F1;
  return res;
}

//=======================================================================
//function : HasAncestorFaceOn2
//purpose  : 
//=======================================================================
  Standard_Boolean BRepAlgo_Section::HasAncestorFaceOn2(const TopoDS_Shape& E, 
							   TopoDS_Shape& F)const
{
  TopoDS_Shape F1,F2; Standard_Integer iC;
  Standard_Boolean res = myHBuilder->EdgeCurveAncestors(E,F1,F2,iC);
  if ( res ) F = F2;
  return res;
}

//=======================================================================
//function : PCurve1
//purpose  : 
//=======================================================================
  Handle(Geom2d_Curve) BRepAlgo_Section::PCurveOn1(const TopoDS_Shape& E) const
{
  Handle(Geom2d_Curve) PC;
  const TopOpeBRepTool_GeomTool& GT = myHBuilder->BuildTool().GetGeomTool();
  if (!GT.CompPC1()) return PC;

  TopoDS_Shape F1,F2; Standard_Integer iC;
  Standard_Boolean res = myHBuilder->EdgeCurveAncestors(E,F1,F2,iC);
  if ( !res ) return PC;

  const TopoDS_Edge& EE = TopoDS::Edge(E);
  const TopoDS_Face& FF = TopoDS::Face(F1);
  Standard_Real f,l; PC = BRep_Tool::CurveOnSurface(EE,FF,f,l);
  Handle(Geom2d_TrimmedCurve) TC = new Geom2d_TrimmedCurve(PC,f,l);

  return TC;
}

//=======================================================================
//function : PCurve2
//purpose  : 
//=======================================================================
  Handle(Geom2d_Curve) BRepAlgo_Section::PCurveOn2(const TopoDS_Shape& E) const
{
  Handle(Geom2d_Curve) PC;
  const TopOpeBRepTool_GeomTool& GT = myHBuilder->BuildTool().GetGeomTool();
  if (!GT.CompPC2()) return PC;

  TopoDS_Shape F1,F2;
  Standard_Integer iC;
  Standard_Boolean res = myHBuilder->EdgeCurveAncestors(E,F1,F2,iC);
  if ( !res ) return PC;

  const TopoDS_Edge& EE = TopoDS::Edge(E);
  const TopoDS_Face& FF = TopoDS::Face(F2);
  Standard_Real f,l; PC = BRep_Tool::CurveOnSurface(EE,FF,f,l);
  Handle(Geom2d_TrimmedCurve) TC = new Geom2d_TrimmedCurve(PC,f,l);

  return TC;
}

//=======================================================================
//function : InitParameters
//purpose  : initialize the fields of the class
//=======================================================================
  void BRepAlgo_Section::InitParameters()
{
  TopOpeBRepDS_BuildTool& BTofBuilder = myHBuilder->ChangeBuildTool();
  TopOpeBRepTool_GeomTool& GTofBTofBuilder = BTofBuilder.ChangeGeomTool();
  
  GTofBTofBuilder.Define(TopOpeBRepTool_BSPLINE1);
  GTofBTofBuilder.DefineCurves(Standard_True);
  GTofBTofBuilder.DefinePCurves1(Standard_False);
  GTofBTofBuilder.DefinePCurves2(Standard_False);
  
  myS1Changed = Standard_False;
  myS2Changed = Standard_False;
  // 
  myApproxChanged = Standard_False;
  // 
  myPCurve1Changed = Standard_False;
  //  
  myPCurve2Changed = Standard_False;
  myshapeisnull = Standard_False;
}
//=======================================================================
//function : MakeShape
//purpose  : 
//=======================================================================
TopoDS_Shape MakeShape(const Handle(Geom_Surface)& S)
{
  GeomAbs_Shape c = S->Continuity();
  if (c >= GeomAbs_C2) return BRepBuilderAPI_MakeFace(S, Precision::Confusion());
  else return BRepBuilderAPI_MakeShell(S);
}
