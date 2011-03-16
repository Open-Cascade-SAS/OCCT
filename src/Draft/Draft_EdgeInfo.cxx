// File:	Draft_EdgeInfo.cxx
// Created:	Wed Aug 31 16:36:09 1994
// Author:	Jacques GOUSSARD
//		<jag@topsn2>


#include <Draft_EdgeInfo.ixx>

#include <Standard_DomainError.hxx>
#include <BRep_Tool.hxx>


//=======================================================================
//function : Draft_EdgeInfo
//purpose  : 
//=======================================================================

Draft_EdgeInfo::Draft_EdgeInfo():
       myNewGeom(Standard_False),myTgt(Standard_False),myTol(0)
{}


//=======================================================================
//function : Draft_EdgeInfo
//purpose  : 
//=======================================================================

Draft_EdgeInfo::Draft_EdgeInfo(const Standard_Boolean HasNewGeometry):
       myNewGeom(HasNewGeometry),myTgt(Standard_False),myTol(0)
{}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::Add(const TopoDS_Face& F)
{
  if (myFirstF.IsNull()) {
    myFirstF = F;
  }
  else if (!myFirstF.IsSame(F) && mySeconF.IsNull()) {
    mySeconF = F;
  }
  myTol=Max(myTol, BRep_Tool::Tolerance(F));
}

//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::RootFace(const TopoDS_Face& F)
{
  myRootFace = F;
}



//=======================================================================
//function : Tangent
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::Tangent(const gp_Pnt& P)
{
  myTgt = Standard_True;
  myPt  = P;
}

//=======================================================================
//function : IsTangent
//purpose  : 
//=======================================================================

Standard_Boolean Draft_EdgeInfo::IsTangent(gp_Pnt& P) const
{
  P = myPt;
  return myTgt;
}

//=======================================================================
//function : NewGeometry
//purpose  : 
//=======================================================================

Standard_Boolean Draft_EdgeInfo::NewGeometry() const
{
  return myNewGeom;
}

//=======================================================================
//function : SetNewGeometry
//purpose  : 
//=======================================================================

void Draft_EdgeInfo::SetNewGeometry( const Standard_Boolean NewGeom )
{
  myNewGeom = NewGeom;
}

//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)& Draft_EdgeInfo::Geometry() const
{
  return myGeom;
}


//=======================================================================
//function : FirstFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_EdgeInfo::FirstFace () const
{
  return myFirstF;
}


//=======================================================================
//function : SecondFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_EdgeInfo::SecondFace () const
{
  return mySeconF;
}


//=======================================================================
//function : ChangeGeometry
//purpose  : 
//=======================================================================

Handle(Geom_Curve)& Draft_EdgeInfo::ChangeGeometry()
{
  return myGeom;
}

//=======================================================================
//function : Handle_Geom2d_Curve&
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& Draft_EdgeInfo::FirstPC() const 
{
  return myFirstPC;
}


//=======================================================================
//function : Handle_Geom2d_Curve&
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)& Draft_EdgeInfo::SecondPC() const 
{
  return mySeconPC;
}

//=======================================================================
//function : ChangeFirstPC
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)& Draft_EdgeInfo::ChangeFirstPC()
{
  return myFirstPC;
}


//=======================================================================
//function : ChangeSecondPC
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve)& Draft_EdgeInfo::ChangeSecondPC()
{
  return mySeconPC;
}


//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

const TopoDS_Face & Draft_EdgeInfo::RootFace() const
{
  return myRootFace;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

void  Draft_EdgeInfo::Tolerance(const Standard_Real tol)
{
  myTol=tol;
}
Standard_Real  Draft_EdgeInfo::Tolerance() const
{
  return myTol;
}


