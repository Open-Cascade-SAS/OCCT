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

