// File:	TopOpeBRepTool_GeomTool.cxx
// Created:	Thu Jun 24 18:45:58 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>


#include <TopOpeBRepTool_GeomTool.ixx>

#include <Precision.hxx>

//=======================================================================
//function : TopOpeBRepTool_GeomTool
//purpose  : 
//=======================================================================

TopOpeBRepTool_GeomTool::TopOpeBRepTool_GeomTool
   (const TopOpeBRepTool_OutCurveType TypeC3D, 
   const Standard_Boolean CompC3D,
   const Standard_Boolean CompPC1, 
   const Standard_Boolean CompPC2) :
   myTypeC3D(TypeC3D),
   myCompC3D(CompC3D), 
   myCompPC1(CompPC1), 
   myCompPC2(CompPC2),
   myTol3d(Precision::Approximation()),
   myTol2d(Precision::PApproximation()),
   myRelativeTol(Standard_True),
   myNbPntMax(30)
{
}

//=======================================================================
//function : Define
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::Define
  (const TopOpeBRepTool_OutCurveType TypeC3D, 
   const Standard_Boolean CompC3D,
   const Standard_Boolean CompPC1, 
   const Standard_Boolean CompPC2)
{
  myTypeC3D = TypeC3D;
  myCompC3D = CompC3D;
  myCompPC1 = CompPC1;
  myCompPC2 = CompPC2;
}

//=======================================================================
//function : Define
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::Define
  (const TopOpeBRepTool_OutCurveType TypeC3D)
{
  myTypeC3D = TypeC3D;
}

//=======================================================================
//function : DefineCurves
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::DefineCurves
  (const Standard_Boolean CompC3D)
{
  myCompC3D = CompC3D;
}

//=======================================================================
//function : DefinePCurves1
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::DefinePCurves1
  (const Standard_Boolean CompPC1)
{
  myCompPC1 = CompPC1;
}

//=======================================================================
//function : DefinePCurves2
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::DefinePCurves2
  (const Standard_Boolean CompPC2)
{
  myCompPC2 = CompPC2;
}

//=======================================================================
//function : Define
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::Define
  (const TopOpeBRepTool_GeomTool& GT)
{
  *this = GT;
}

//=======================================================================
//function : GetTolerances
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::GetTolerances
  (Standard_Real& tol3d, Standard_Real& tol2d) const
{
  tol3d = myTol3d;
  tol2d = myTol2d;
}


//=======================================================================
//function : SetTolerances
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::SetTolerances
  (const Standard_Real tol3d, const Standard_Real tol2d)
{
  myTol3d = tol3d;
  myTol2d = tol2d;
  myRelativeTol = Standard_True;
}

//=======================================================================
//function : GetTolerances
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::GetTolerances
  (Standard_Real& tol3d, Standard_Real& tol2d, Standard_Boolean& relative) const
{
  tol3d = myTol3d;
  tol2d = myTol2d;
  relative = myRelativeTol;
}


//=======================================================================
//function : SetTolerances
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::SetTolerances
  (const Standard_Real tol3d, const Standard_Real tol2d, const Standard_Boolean relative)
{
  myTol3d = tol3d;
  myTol2d = tol2d;
  myRelativeTol = relative;
}

//=======================================================================
//function : NbPntMax
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_GeomTool::NbPntMax()const
{
  return myNbPntMax;
}

//=======================================================================
//function : SetNbPntMax
//purpose  : 
//=======================================================================

void TopOpeBRepTool_GeomTool::SetNbPntMax (const Standard_Integer NbPntMax)
{
  myNbPntMax = NbPntMax ;
}

//=======================================================================
//function : CompC3D
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_GeomTool::CompC3D()const
{
  return myCompC3D;
}

//=======================================================================
//function : TypeC3D
//purpose  : 
//=======================================================================

TopOpeBRepTool_OutCurveType TopOpeBRepTool_GeomTool::TypeC3D()const
{
  return myTypeC3D;
}

//=======================================================================
//function : CompPC1
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_GeomTool::CompPC1()const
{
  return myCompPC1;
}


//=======================================================================
//function : CompPC2
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_GeomTool::CompPC2()const
{
  return myCompPC2;
}

