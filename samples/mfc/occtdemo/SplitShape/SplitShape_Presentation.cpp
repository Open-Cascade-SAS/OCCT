// SplitShape_Presentation.cpp: implementation of the SplitShape_Presentation class.
// Split shape by wire
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplitShape_Presentation.h"

#include <BRepAlgoAPI_Section.hxx>
#include <BRepFeat_SplitShape.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <TopLoc_Location.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepAdaptor_Curve.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new SplitShape_Presentation;

// Initialization of array of samples
const SplitShape_Presentation::PSampleFuncType SplitShape_Presentation::SampleFuncs[] =
{
  &SplitShape_Presentation::sample1,
  &SplitShape_Presentation::sample2
};

#ifdef WNT
 #define EOL "\r\n"
 #define DELIM "\\"
#else
 #define EOL "\n"
 #define DELIM "/"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SplitShape_Presentation::SplitShape_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("SplitShape");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void SplitShape_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Auxiliary functions
//////////////////////////////////////////////////////////////////////

Standard_Boolean SplitShape_Presentation::readShell(TCollection_AsciiString& theText)
{
  myShell.Nullify();
  TCollection_AsciiString aPath;
  aPath = aPath + GetDataDir() + DELIM + "shell1.brep";
  BRep_Builder aBld;
  Standard_Boolean isRead = BRepTools::Read (myShell, aPath.ToCString(), aBld);
  if (!isRead)
  {
    theText = aPath + " is not found";
    setResultText(theText.ToCString());
  }
  return isRead;
}

Standard_Boolean SplitShape_Presentation::computeSection
        (BRepAlgoAPI_Section& aSecAlgo, 
         TopoDS_Wire& aSecWire,
         TCollection_AsciiString& aText)
{
  aText +=
    "  aSecAlgo.Approximation(Standard_True);    // approximate new geometries" EOL
    "  aSecAlgo.ComputePCurveOn1(Standard_True); // pcurves must be computed on shell" EOL
    "  aSecAlgo.Build();" EOL
    "  if (!aSecAlgo.IsDone())" EOL
    "    return;" EOL
    "  TopoDS_Shape aSection = aSecAlgo.Shape(); // get compound of section edges" EOL EOL
    
    "  // connect section edges to a wire" EOL
    "  TopTools_ListOfShape aEdges;" EOL
    "  TopExp_Explorer ex(aSection, TopAbs_EDGE);" EOL
    "  if (!ex.More())  // is section empty?" EOL
    "    return;" EOL
    "  for (; ex.More(); ex.Next())" EOL
    "    aEdges.Append(ex.Current());" EOL
    "  BRepBuilderAPI_MakeWire aWMaker;" EOL
    "  aWMaker.Add(aEdges);" EOL
    "  if (!aWMaker.IsDone())" EOL
    "    return;" EOL
    "  TopoDS_Wire aSecWire = aWMaker.Wire();" EOL EOL;
    setResultText(aText.ToCString());

  // define parameters and perform section
  aSecAlgo.Approximation(Standard_True);    // approximate new geometries
  aSecAlgo.ComputePCurveOn1(Standard_True); // pcurves must be computed on shell
  aSecAlgo.Build();
  if (!aSecAlgo.IsDone())
  {
    aText += EOL "Error: section algo has failed" EOL;
    setResultText(aText.ToCString());
    return Standard_False;
  }
  TopoDS_Shape aSection = aSecAlgo.Shape(); // get compound of section edges

  // connect section edges to a wire
  TopTools_ListOfShape aEdges;
  TopExp_Explorer ex(aSection, TopAbs_EDGE);
  if (!ex.More())  // is section empty?
  {
    aText += EOL "Error: no intersection found" EOL;
    setResultText(aText.ToCString());
    return Standard_False;
  }
  for (; ex.More(); ex.Next())
    aEdges.Append(ex.Current());
  BRepBuilderAPI_MakeWire aWMaker;
  aWMaker.Add(aEdges);
  if (!aWMaker.IsDone())
  {
    aText += EOL "Error: connecting of section edges has failed" EOL;
    setResultText(aText.ToCString());
    return Standard_False;
  }
  aSecWire = aWMaker.Wire();

  return Standard_True;
}

/////////////////////////////////////////////////////////////////////

Standard_Boolean SplitShape_Presentation::splitAndComposeLeft
        (const BRepAlgoAPI_Section& aSecAlgo, 
         const TopoDS_Wire& aSecWire, TopoDS_Shell& aNewShell,
         TCollection_AsciiString& aText)
{
  aText +=
    "  // initialize splitting algo with the shell" EOL
    "  BRepFeat_SplitShape aSplitter(myShell);" EOL EOL
    
    "  // add splitting edges" EOL
    "  TopoDS_Iterator its(aSecWire);" EOL
    "  for (; its.More(); its.Next())" EOL
    "  {" EOL
    "    const TopoDS_Edge& aEdge = TopoDS::Edge(its.Value());" EOL
    "    // determine an ancestor face of this edge" EOL
    "    TopoDS_Face aFace;" EOL
    "    if (aSecAlgo.HasAncestorFaceOn1(aEdge, aFace))" EOL
    "      aSplitter.Add (aEdge, aFace);" EOL
    "  }" EOL EOL
    
    "  // perform splitting" EOL
    "  aSplitter.Build();" EOL
    "  if (!aSplitter.IsDone())" EOL
    "    return;" EOL EOL
    
    "  // compose a new shell from the left side faces" EOL
    "  TopoDS_Shell aNewShell;" EOL
    "  aBld.MakeShell(aNewShell);" EOL
    "  const TopTools_ListOfShape& aLF = aSplitter.Left();" EOL
    "  TopTools_ListIteratorOfListOfShape it(aLF);" EOL
    "  for (; it.More(); it.Next())" EOL
    "    aBld.Add (aNewShell, it.Value());" EOL EOL;
  setResultText(aText.ToCString());

  // initialize splitting algo with the shell
  BRepFeat_SplitShape aSplitter(myShell);

  // add splitting edges
  TopoDS_Iterator its(aSecWire);
  for (; its.More(); its.Next())
  {
    const TopoDS_Edge& aEdge = TopoDS::Edge(its.Value());
    // determine an ancestor face of this edge
    TopoDS_Face aFace;
    if (aSecAlgo.HasAncestorFaceOn1(aEdge, aFace))
      aSplitter.Add (aEdge, aFace);
  }

  // perform splitting
  aSplitter.Build();
  if (!aSplitter.IsDone())
  {
    aText += EOL "Error: splitting algo has failed" EOL;
    setResultText(aText.ToCString());
    return Standard_False;
  }

  // compose a new shell from the left side faces
  BRep_Builder aBld;
  aBld.MakeShell(aNewShell);
  const TopTools_ListOfShape& aLF = aSplitter.Left();
  TopTools_ListIteratorOfListOfShape it(aLF);
  for (; it.More(); it.Next())
    aBld.Add (aNewShell, it.Value());

  return Standard_True;
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

void SplitShape_Presentation::sample1()
{
  ResetView();
  SetViewCenter(60.457558475728, -20.351205260916);
  SetViewScale(29.468160110195); 
 
  setResultTitle ("Split shell by plane");
  TCollection_AsciiString aText =
    "  // define initial shell" EOL
    "  TopoDS_Shell myShell;" EOL
    "  // myShell = ...;" EOL EOL

    "  // define secant plane" EOL
    "  gp_Pln aPln(gp_Pnt(65,11,0), gp_Dir(1,-0.1,-0.3));" EOL EOL

    "  // make section of myShell by the plane" EOL
    "  Standard_Boolean performNow = Standard_False;" EOL
    "  BRepAlgoAPI_Section aSecAlgo (myShell, aPln, performNow);" EOL
    ;
    setResultText(aText.ToCString());

  // read shell
  if (myShell.IsNull() && !readShell(aText))
    return;

  Handle(AIS_InteractiveObject) aIShell = drawShape (myShell);
  if (WAIT_A_SECOND) return;

  // define secant plane
  gp_Pln aPln(gp_Pnt(65,11,0), gp_Dir(1,-0.1,-0.3));

  Handle(Geom_Surface) aPlane = new Geom_Plane(aPln);
  aPlane = new Geom_RectangularTrimmedSurface(aPlane,-5.,20.,-13.,13.);
  Handle(AIS_InteractiveObject) aIPlane = drawSurface(aPlane);
 
  if (WAIT_A_LITTLE) return;

  // make section of myShell by the plane
  TopoDS_Wire aSecWire;
  Standard_Boolean performNow = Standard_False;
  BRepAlgoAPI_Section aSecAlgo (myShell, aPln, performNow);
  if (!computeSection(aSecAlgo, aSecWire, aText))
    return;

  getAISContext()->Erase(aIPlane);
  Handle(AIS_InteractiveObject) aISecWire = drawShape (aSecWire,Quantity_NOC_GREEN,Standard_False);
  Handle(Prs3d_LineAspect) aLA = 
    new Prs3d_LineAspect(Quantity_NOC_GREEN,Aspect_TOL_SOLID,3.);
  aISecWire->Attributes()->SetWireAspect(aLA);
  getAISContext()->Display(aISecWire);
  if (WAIT_A_LITTLE) return;

  aText +=
    "  //aSecWire.Reverse();	// orient wire to have the left side as you need" EOL EOL;
  setResultText(aText.ToCString());
  //aSecWire.Reverse();	// orient wire to have the left side as you need

  // split myShell by aSecWire and compose the left side subshell
  TopoDS_Shell aNewShell;
  if (!splitAndComposeLeft(aSecAlgo, aSecWire, aNewShell, aText))
    return;

  drawShape (aNewShell);
  getAISContext()->Erase(aIShell);
  getAISContext()->Erase(aISecWire);
}

/////////////////////////////////////////////////////////////////////

void SplitShape_Presentation::sample2()
{
  setResultTitle ("Cut hole from shell");
  TCollection_AsciiString aText =
    "  // define initial shell" EOL
    "  TopoDS_Shell myShell;" EOL
    "  // myShell = ...;" EOL EOL

    "  // define contour of the hole and its normal direction" EOL
    "  TopoDS_Wire aCont;" EOL
    "  gp_Dir aNormal(-0.2, -0.7, 0.4);" EOL
    "  // aCont = ... ;" EOL EOL

    "  // project the contour on the shell along aNormal;" EOL
    "  // for that create prism from the contour" EOL
    "  // and find intersection with the shell" EOL
    "  Standard_Real dist = 8;" EOL
    "  gp_Vec aPrismVec(aNormal.Reversed().XYZ()*dist*2);" EOL
    "  BRepPrimAPI_MakePrism aSweeper(aCont,aPrismVec);" EOL
    "  if (!aSweeper.IsDone())" EOL
    "    return;" EOL
    "  TopoDS_Shape aPrism = aSweeper.Shape();" EOL EOL

    "  // make section of myShell by the prism" EOL
    "  TopoDS_Wire aSecWire;" EOL
    "  Standard_Boolean performNow = Standard_False;" EOL
    "  BRepAlgoAPI_Section aSecAlgo (myShell, aPrism, performNow);" EOL
    ;
  setResultText(aText.ToCString());

  // read shell
  if (myShell.IsNull() && !readShell(aText))
    return;

  Handle(AIS_InteractiveObject) aIShell = drawShape (myShell);
  if (WAIT_A_SECOND) return;

  // define contour of the hole and its normal direction
  TopoDS_Wire aCont;
  gp_Dir aNormal(-0.2, -0.7, 0.4);

  Standard_Real dx=2, dy=1.5, aOffValue=1.5;
  TopoDS_Wire aPol = BRepBuilderAPI_MakePolygon(gp_Pnt(-dx,-dy,0), gp_Pnt(dx,-dy,0),
                                     gp_Pnt(dx,dy,0), gp_Pnt(-dx,dy,0),
                                     Standard_True);
  // transform contour to a needed location
  gp_Pnt aOrigin(78,16,10);
  gp_Dir aXDir(1, 0, 0);
  gp_Ax2 aSystem(aOrigin, aNormal, aXDir);
  gp_Trsf aTrsf;
  aTrsf.SetDisplacement(gp::XOY(), aSystem);
  aPol.Location(TopLoc_Location(aTrsf));
  BRepOffsetAPI_MakeOffset aOffAlgo (aPol, GeomAbs_Arc);
  aOffAlgo.Perform (aOffValue);
  aCont = TopoDS::Wire(aOffAlgo.Shape());

  Handle(AIS_InteractiveObject) aICont = drawShape (aCont);
  if (WAIT_A_SECOND) return;

  // project the contour on the shell along aNormal;
  // for that create prism from the contour
  // and find intersection with the shell
  Standard_Real dist = 8;
  gp_Vec aPrismVec(aNormal.Reversed().XYZ()*dist);
  BRepPrimAPI_MakePrism aSweeper(aCont,aPrismVec);
  if (!aSweeper.IsDone())
  {
    aText += EOL "Error: sweeping algo has failed" EOL;
    setResultText(aText.ToCString());
    return;
  }
  TopoDS_Shape aPrism = aSweeper.Shape();

  Handle(AIS_InteractiveObject) aIPrism = drawShape(aPrism, Quantity_NOC_LEMONCHIFFON3);
  if (WAIT_A_LITTLE) return;

  // make section of myShell by the prism
  TopoDS_Wire aSecWire;
  Standard_Boolean performNow = Standard_False;
  BRepAlgoAPI_Section aSecAlgo (myShell, aPrism, performNow);
  if (!computeSection(aSecAlgo, aSecWire, aText))
    return;

  aText +=
    "  aSecWire.Reverse();	// orient wire to have the left side as you need" EOL EOL;
  setResultText(aText.ToCString());
  //aSecWire.Reverse();	// orient wire to have the left side as you need
  // get an edge from the initial contour and
  // determine the corresponding projection edge from the section wire;
  // check orientations of their first derivatives
  TopoDS_Shape aEdgeP, aEdge1, aFace, aEdge2;
  TopoDS_Iterator it(aPol);
  aEdgeP = it.Value();
  aEdge1 = aOffAlgo.Generated(aEdgeP).First();
  aFace = aSweeper.Generated(aEdge1).First();
  for (it.Initialize(aSecWire); it.More() && aEdge2.IsNull(); it.Next())
  {
    TopoDS_Shape aF;
    if (aSecAlgo.HasAncestorFaceOn2(it.Value(),aF) && aF.IsSame(aFace))
      aEdge2 = it.Value();
  }
  if (aEdge2.IsNull())
  {
    aText += EOL "Error: cannot orient projected contour due to" EOL
      "failure restoring history of modification" EOL;
    setResultText(aText.ToCString());
    return;
  }
  BRepAdaptor_Curve aC1(TopoDS::Edge(aEdge1)), aC2(TopoDS::Edge(aEdge2));
  gp_Pnt p1, p2;
  gp_Vec v1, v2;
  aC1.D1(aC1.FirstParameter(), p1, v1);
  aC2.D1(aC2.FirstParameter(), p2, v2);
  if (aEdge1.Orientation() == TopAbs_REVERSED) v1.Reverse();
  if (aEdge2.Orientation() == TopAbs_REVERSED) v2.Reverse();
  Standard_Real pr = v1 * v2;
  if (pr > 0.)
    aSecWire.Reverse();

  getAISContext()->Erase(aIPrism);
  Handle(AIS_InteractiveObject) aISecWire = drawShape (aSecWire,Quantity_NOC_GREEN,Standard_False);
  Handle(Prs3d_LineAspect) aLA = 
    new Prs3d_LineAspect(Quantity_NOC_GREEN,Aspect_TOL_SOLID,3.);
  aISecWire->Attributes()->SetWireAspect(aLA);
  getAISContext()->Display(aISecWire);
  if (WAIT_A_SECOND) return;

  // split myShell by aSecWire and compose the left side subshell
  TopoDS_Shell aNewShell;
  if (!splitAndComposeLeft(aSecAlgo, aSecWire, aNewShell, aText))
    return;

  drawShape (aNewShell);
  getAISContext()->Erase(aIShell);
  getAISContext()->Erase(aISecWire);
}
