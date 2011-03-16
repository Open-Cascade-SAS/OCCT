// File:	Draft_FaceInfo.cxx
// Created:	Wed Aug 31 15:30:06 1994
// Author:	Jacques GOUSSARD
//		<jag@topsn2>


#include <Draft_FaceInfo.ixx>
#include <Geom_RectangularTrimmedSurface.hxx>


//=======================================================================
//function : Draft_FaceInfo
//purpose  : 
//=======================================================================

Draft_FaceInfo::Draft_FaceInfo ()
{
}

//=======================================================================
//function : Draft_FaceInfo
//purpose  : 
//=======================================================================

Draft_FaceInfo::Draft_FaceInfo (const Handle(Geom_Surface)& S,\
				const Standard_Boolean HasNewGeometry):
       myNewGeom(HasNewGeometry)
{
  Handle(Geom_RectangularTrimmedSurface) T = Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if (!T.IsNull()) myGeom = T->BasisSurface();
  else             myGeom = S;
}


//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

void Draft_FaceInfo::RootFace(const TopoDS_Face& F)
{
  myRootFace = F;
}



//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void Draft_FaceInfo::Add(const TopoDS_Face& F)
{
  if (myF1.IsNull()) {
    myF1 = F;
  }
  else if (myF2.IsNull()) {
    myF2 = F;
  }
}


//=======================================================================
//function : FirstFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_FaceInfo::FirstFace () const
{
  return myF1;
}


//=======================================================================
//function : SecondFace
//purpose  : 
//=======================================================================

const TopoDS_Face& Draft_FaceInfo::SecondFace () const
{
  return myF2;
}


//=======================================================================
//function : NewGeometry
//purpose  : 
//=======================================================================

Standard_Boolean Draft_FaceInfo::NewGeometry() const
{
  return myNewGeom;
}

//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)& Draft_FaceInfo::Geometry() const
{
  return myGeom;
}

//=======================================================================
//function : ChangeGeometry
//purpose  : 
//=======================================================================

Handle(Geom_Surface)& Draft_FaceInfo::ChangeGeometry()
{
  return myGeom;
}

//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)& Draft_FaceInfo::Curve() const
{
  return myCurv;
}

//=======================================================================
//function : ChangeCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve)& Draft_FaceInfo::ChangeCurve()
{
  return myCurv;
}

//=======================================================================
//function : RootFace
//purpose  : 
//=======================================================================

const TopoDS_Face & Draft_FaceInfo::RootFace() const
{
  return myRootFace;
}


