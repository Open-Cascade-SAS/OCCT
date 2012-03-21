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

#include <StdPrs_ToolRFace.ixx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Adaptor2d_Curve2d.hxx>


#define OCC316

//=======================================================================
//function : StdPrs_ToolRFace
//purpose  : 
//=======================================================================

StdPrs_ToolRFace::StdPrs_ToolRFace()
{
}

//=======================================================================
//function : StdPrs_ToolRFace
//purpose  : 
//=======================================================================

StdPrs_ToolRFace::StdPrs_ToolRFace(const Handle(BRepAdaptor_HSurface)& aSurface) :
       myFace(((BRepAdaptor_Surface*)&(aSurface->Surface()))->Face())
{
  myFace.Orientation(TopAbs_FORWARD);
}

//=======================================================================
//function : IsOriented
//purpose  : 
//=======================================================================

Standard_Boolean StdPrs_ToolRFace::IsOriented () const {
  
  return Standard_True;

}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StdPrs_ToolRFace::Init() 
{
  myExplorer.Init(myFace,TopAbs_EDGE);
  if (myExplorer.More()) {
    Standard_Real U1,U2;
    const Handle(Geom2d_Curve)& C = 
      BRep_Tool::CurveOnSurface(TopoDS::Edge(myExplorer.Current()),
				myFace,
				U1,U2);
    DummyCurve.Load(C,U1,U2);
  }
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean StdPrs_ToolRFace::More() const
{
  return myExplorer.More();
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void StdPrs_ToolRFace::Next()
{
  myExplorer.Next();

  if (myExplorer.More()) {
    // skip INTERNAL and EXTERNAL edges
    while (myExplorer.More() && (myExplorer.Current().Orientation() == TopAbs_INTERNAL 
                              || myExplorer.Current().Orientation() == TopAbs_EXTERNAL)) 
	myExplorer.Next();
    if (myExplorer.More()) {
      Standard_Real U1,U2;
      const Handle(Geom2d_Curve)& C = 
	BRep_Tool::CurveOnSurface(TopoDS::Edge(myExplorer.Current()),
				  myFace,
				  U1,U2);
#ifdef OCC316
      if ( !C.IsNull() )
#endif
	DummyCurve.Load(C,U1,U2);
    }
  }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Adaptor2d_Curve2dPtr StdPrs_ToolRFace::Value() const
{
  return (Adaptor2d_Curve2dPtr)&DummyCurve;
}

//=======================================================================
//function : Orientation
//purpose  : 
//=======================================================================

TopAbs_Orientation StdPrs_ToolRFace::Orientation () const {
  return myExplorer.Current().Orientation();
}

