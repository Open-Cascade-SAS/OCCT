// Chamfers_Presentation.cpp: implementation of the Chamfers_Presentation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Chamfers_Presentation.h"

#ifdef WNT
#pragma warning (disable : 4786)
#endif

#include <TopoDS_Edge.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgo_Fuse.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepBuild_HBuilder.hxx>
#include <Precision.hxx>

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define BOX_SIDE_LEN 500

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Chamfers_Presentation;

// Initialization of array of samples
const Chamfers_Presentation::PSampleFuncType Chamfers_Presentation::SampleFuncs[] =
{
  &Chamfers_Presentation::sampleBoxChamfer,
  &Chamfers_Presentation::sampleBoxCylChamfer,
  &Chamfers_Presentation::sampleBoxCylAngChamfer,
  &Chamfers_Presentation::sampleBoxFillet,
  &Chamfers_Presentation::sampleBoxCylFillet,
  &Chamfers_Presentation::sampleBoxCylAngFillet
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Chamfers_Presentation::Chamfers_Presentation()
{
  myIndex = 0;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Chamfers and Fillets");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Chamfers_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : createDiverseChamferOnBox
// Purpose  : makes chamfers of different size on 3 converging edges 
//            of the given box using given distances d1, d2, d3; 
//            outputs its own source code into result dialog
//================================================================
Standard_Boolean Chamfers_Presentation::createDiverseChamferOnBox(
                         TopoDS_Solid& aBox, 
                         Standard_Real d1, // chamfer on edge 1
                         Standard_Real d2, // chamfer on edge 2
                         Standard_Real d3, // chamfer on edge 3
                         Handle(AIS_InteractiveObject)& thePrevShape)// for smoth drawing
                         // we display the new shape first, then erase the previous one
{
  // initializing a class that builds chamfer
  BRepFilletAPI_MakeChamfer aMakeChamfer(aBox);

  // a map of "edge-to-list of faces".  Key = edge, value = list of 2 faces 
  // of the box that "touch" the edge
  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;
  // filling the map with edges and faces of the box
  TopExp::MapShapesAndAncestors(aBox, TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);
  
  // array of indexes of edges to be chamfered
  int aEdgeInd[] = {6, 2, 10, 12, 5};

  // creating chamfer on one of an edge of the top face, using d1
  if (d1 > Precision::Confusion())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[0]));
    TopoDS_Face aFace = TopoDS::Face(aMapOfEdgeToFaces.FindFromKey(anEdge).First());
    aMakeChamfer.Add(d1, d1, anEdge, aFace);
  }

  // creating chamfer on the other 3 edges of the top face, using d2
  if (d2 > Precision::Confusion())
  {
    for (Standard_Integer i = 1; i <= 3; i++)
    {
      TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[i]));
      TopoDS_Face aFace = TopoDS::Face(aMapOfEdgeToFaces.FindFromKey(anEdge).First());
      aMakeChamfer.Add(d2, d2, anEdge, aFace);
    }
  }

  // creating chamfer on a vertical edge of box, using d3
  if (d3 > Precision::Confusion())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[4]));
    TopoDS_Face aFace = TopoDS::Face(aMapOfEdgeToFaces.FindFromKey(anEdge).First());
    aMakeChamfer.Add(d3, d3, anEdge, aFace);
  }

  aMakeChamfer.Build();
  if (aMakeChamfer.IsDone())
  {
    Handle(AIS_InteractiveObject) aNewShape = new AIS_Shape(aMakeChamfer.Shape());
    getAISContext()->Erase(thePrevShape, Standard_False);
    thePrevShape = aNewShape;
    getAISContext()->Display(aNewShape);
  }

  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a box and centering it on the screen" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(gp_Pnt(-a/2, -a/2, -a/2), a, a, a);" EOL EOL
    
    "  // initializing a class that builds chamfer" EOL
    "  BRepFilletAPI_MakeChamfer aMakeChamfer(aBox);" EOL EOL
    
    "  // a map of \"edge-to-list of faces\".  Key = edge, value = list of 2 faces " EOL
    "  // of the box that \"touch\" the edge" EOL
    "  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;" EOL
    "  // filling the map with edges and faces of the box" EOL
    "  TopExp::MapShapesAndAncestors(aBox, TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);" EOL
    "" EOL
    "  Standard_Real d1 = "+TCollection_AsciiString(d1)+";" EOL
    "  Standard_Real d2 = "+TCollection_AsciiString(d2)+";" EOL
    "  Standard_Real d3 = "+TCollection_AsciiString(d3)+";" EOL
    "" EOL
    "  // array of indexes of edges to be chamfered" EOL
    "  int aEdgeInd[] = {6, 2, 10, 12, 5};" EOL
    "" EOL
    "  // creating chamfer on one of an edge of the top face, using d1" EOL
    "  if (d1 > Precision::Confusion())" EOL
    "  {" EOL
    "    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[0]));" EOL
    "    TopoDS_Face aFace = TopoDS::Face(aMapOfEdgeToFaces.FindFromKey(anEdge).First());" EOL
    "    aMakeChamfer.Add(d1, d1, anEdge, aFace);" EOL
    "  }" EOL
    "" EOL
    "  // creating chamfer on the other 3 edges of the top face, using d2" EOL
    "  if (d2 > Precision::Confusion())" EOL
    "  {" EOL
    "    for (Standard_Integer i = 1; i <= 3; i++)" EOL
    "    {" EOL
    "      TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[i]));" EOL
    "      TopoDS_Face aFace = TopoDS::Face(aMapOfEdgeToFaces.FindFromKey(anEdge).First());" EOL
    "      aMakeChamfer.Add(d2, d2, anEdge, aFace);" EOL
    "    }" EOL
    "  }" EOL
    "" EOL
    "  // creating chamfer on a vertical edge of box, using d3" EOL
    "  if (d3 > Precision::Confusion())" EOL
    "  {" EOL
    "    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[4]));" EOL
    "    TopoDS_Face aFace = TopoDS::Face(aMapOfEdgeToFaces.FindFromKey(anEdge).First());" EOL
    "    aMakeChamfer.Add(d3, d3, anEdge, aFace);" EOL
    "  }" EOL
    "    " EOL
    "  aMakeChamfer.Build();" EOL
    "  if (aMakeChamfer.IsDone())" EOL
    "    TopoDS_Shape aChamferedShape = aMakeChamfer.Shape();" EOL;  
  setResultText(aText.ToCString());

  return WAIT_A_SECOND;
}

//================================================================
// Function : createChamferOnBox
// Purpose  : makes chamfers on edges of the top face of the given box
//            using given distances theDistA and theDistB, outputs its own 
//            source code into result dialog
//================================================================
Standard_Boolean Chamfers_Presentation::createChamferOnBox(
                         TopoDS_Solid& aBox, 
                         Standard_Real theDistA, // distance along first face 
                         Standard_Real theDistB, // distance along second face
                         Handle(AIS_InteractiveObject)& thePrevShape)// for smoth drawing
                         // we display the new shape first, then erase the previous one
{
  // initializing a class that builds chamfer
  BRepFilletAPI_MakeChamfer aMakeChamfer(aBox);

  // a map of "edge-to-list of faces".  Key = edge, value = list of 2 faces 
  // of the box that "touch" the edge
  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;
  // filling the map with edges and faces of the box
  TopExp::MapShapesAndAncestors(aBox, TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);
  
  // array of indexes of edges to be chamfered
  int aEdgeInd[] = {2, 6, 10, 12};

  // building chamfers
  for (int i = 0; i <= sizeof(aEdgeInd)/sizeof(int)-1; i++)
  //for (int i = 1; i <= 12; i++) // uncomment to iterate through ALL edges of the box
  {
    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[i]));
    //TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(i));
    // uncomment to iterate through ALL edges of the box
    const TopTools_ListOfShape& aFaces = aMapOfEdgeToFaces.FindFromKey(anEdge);
    TopoDS_Face aFace = TopoDS::Face(aFaces.First());
    aMakeChamfer.Add(theDistA, theDistB, anEdge, aFace);
  }

  aMakeChamfer.Build();
  if (aMakeChamfer.IsDone())
  {
    Handle(AIS_InteractiveObject) aNewShape = new AIS_Shape(aMakeChamfer.Shape());
    getAISContext()->Erase(thePrevShape, Standard_False);
    getAISContext()->Display(aNewShape, Standard_True);
    thePrevShape = aNewShape;
  }

  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a box and centering it on the screen" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(gp_Pnt(-a/2, -a/2, -a/2), a, a, a);" EOL EOL
    
    "  // initializing a class that builds chamfer" EOL
    "  BRepFilletAPI_MakeChamfer aMakeChamfer(aBox);" EOL EOL
    
    "  // a map of \"edge-to-list of faces\".  Key = edge, value = list of 2 faces " EOL
    "  // of the box that \"touch\" the edge" EOL
    "  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;" EOL
    "  // filling the map with edges and faces of the box" EOL
    "  TopExp::MapShapesAndAncestors(aBox, TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);" EOL
    "" EOL
    "  Standard_Real aDistA = "+TCollection_AsciiString(theDistA)+";" EOL
    "  Standard_Real aDistB = "+TCollection_AsciiString(theDistB)+";" EOL
    "" EOL
    "  // array of indexes of edges to be chamfered" EOL
    "  int aEdgeInd[] = {2, 6, 10, 12};" EOL EOL
    
    "  // building chamfers" EOL
    "  for (int i = 0; i <= sizeof(aEdgeInd)/sizeof(int)-1; i++)" EOL
    "  {" EOL
    "    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdgeToFaces.FindKey(aEdgeInd[i]));" EOL
    "    const TopTools_ListOfShape& aFaces = aMapOfEdgeToFaces.FindFromKey(anEdge);" EOL
    "    TopoDS_Face aFace = TopoDS::Face(aFaces.First());" EOL
    "    // creating a chamfer with lengths aDistA, aDistB" EOL 
    "    aMakeChamfer.Add(aDistA, aDistB, anEdge, aFace);" EOL
    "  }" EOL EOL
    
    "  aMakeChamfer.Build();" EOL
    "  if (aMakeChamfer.IsDone())" EOL
    "    TopoDS_Shape aChamferedShape = aMakeChamfer.Shape();" EOL;  
  setResultText(aText.ToCString());

  return WAIT_A_SECOND;
}

//================================================================
// Function : createFilletOnBox
// Purpose  : creates a box with side=10, makes fillets on some of its edges
//            using given value of radius and shape of fillet, outputs its own 
//            source code into theText parameter
//================================================================
Standard_Boolean Chamfers_Presentation::createFilletOnBox(
                       TopoDS_Solid& aBox,
                       Standard_Real theRad, // radius of fillet
                       ChFi3d_FilletShape theFShape, // shape of fillet
                       Handle(AIS_InteractiveObject)& thePrevObj) // to erase 
                       // the previous shape
{
  // initializing a class that builds fillet
  // second parameter is a shape of fillet:
  // enum ChFi3d_FilletShape { ChFi3d_Rational, ChFi3d_QuasiAngular, ChFi3d_Polynomial };
  BRepFilletAPI_MakeFillet aMakeFillet(aBox, theFShape);

  // a map of edges
  TopTools_IndexedMapOfShape aMapOfEdges;
  // filling the map with edges of the box
  TopExp::MapShapes(aBox, TopAbs_EDGE, aMapOfEdges);
  
  // array of indexes of edges to be chamfered
  int aEdgeInd[] = {2, 6, 10, 12};

  // adding edges that will have fillet
  for (int i = 0; i <= sizeof(aEdgeInd)/sizeof(int)-1; i++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdges(aEdgeInd[i]));
    aMakeFillet.Add(theRad, anEdge);
  }

  aMakeFillet.Build();
  if (aMakeFillet.IsDone())
  {
    Handle(AIS_InteractiveObject) aNewShape = new AIS_Shape(aMakeFillet.Shape());
    getAISContext()->Erase(thePrevObj, Standard_False);
    getAISContext()->Display(aNewShape, Standard_True);
    thePrevObj = aNewShape;
  }

  // calculating the string corresponding to theFShape 
  TCollection_AsciiString aFShapeStr;
  aFShapeStr = ((theFShape==ChFi3d_Rational)?     "ChFi3d_Rational":
               ((theFShape==ChFi3d_QuasiAngular)? "ChFi3d_QuasiAngular":
                                                  "ChFi3d_Polynomial"));
  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a box and centering it on the screen" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(gp_Pnt(-a/2, -a/2, -a/2), a, a, a);" EOL
    "  " EOL
    "  // initializing a class that builds fillet" EOL
    "  // second parameter is a shape of fillet:" EOL
    "  // enum ChFi3d_FilletShape { ChFi3d_Rational, ChFi3d_QuasiAngular, ChFi3d_Polynomial };" EOL
    "  BRepFilletAPI_MakeFillet aMakeFillet(aBox, "+aFShapeStr+");" EOL
    "  " EOL
    "  // a map of edges" EOL
    "  TopTools_IndexedMapOfShape aMapOfEdges;" EOL
    "  // filling the map with edges of the box" EOL
    "  TopExp::MapShapes(aBox, TopAbs_EDGE, aMapOfEdges);" EOL EOL
  
    "  // array of indexes of edges to be chamfered" EOL
    "  int aEdgeInd[] = {2, 6, 10, 12};" EOL EOL

    "  // adding edges that will have fillet" EOL
    "  Standard_Real r = "+TCollection_AsciiString(theRad)+";" EOL
    "  for (int i = 0; i <= sizeof(aEdgeInd)/sizeof(int)-1; i++)" EOL
    "  {" EOL
    "    TopoDS_Edge anEdge = TopoDS::Edge(aMapOfEdges(aEdgeInd[i]));" EOL
    "    aMakeFillet.Add(r, anEdge);" EOL
    "  }" EOL
  
    "  aMakeFillet.Build();" EOL
    "  if (aMakeFillet.IsDone())" EOL
    "    TopoDS_Shape aShapeWithFillet = aMakeFillet.Shape();" EOL;
  setResultText(aText.ToCString());

  return WAIT_A_SECOND;
}



// ===============================================================
//                          CHAMFER CASES
// ===============================================================
//================================================================
// Function : Chamfers_Presentation::sampleBoxChamfer1,2,3
// Purpose  : 
//================================================================
void Chamfers_Presentation::sampleBoxChamfer()
{
  setResultTitle("Creating Chamfer");
  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a box and centering it on the screen" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(gp_Pnt(-a/2, -a/2, -a/2), a, a, a);" EOL;
  setResultText(aText.ToCString());

  // show initial shape
  Standard_Real a = BOX_SIDE_LEN;
  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(gp_Pnt(-a/2, -a/2, -a/2), a, a, a);
  Handle(AIS_InteractiveObject) aPrevShape = new AIS_Shape(aBox);
  getAISContext()->Display(aPrevShape, Standard_False);
  COCCDemoDoc::Fit();

  if (WAIT_A_SECOND) return;

  // show several chamfers
  if (createChamferOnBox(aBox, a/10, a/10, aPrevShape)) return;
  if (createChamferOnBox(aBox, a/24, a/10, aPrevShape)) return;
  if (createChamferOnBox(aBox, a/10, a/10, aPrevShape)) return;
  if (createChamferOnBox(aBox, a/10, a/24, aPrevShape)) return;
  if (createChamferOnBox(aBox, a/10, a/10, aPrevShape)) return;
  if (createDiverseChamferOnBox(aBox, a/6, a/10, 0, aPrevShape)) return;
  if (createDiverseChamferOnBox(aBox, a/6, a/10, a/24, aPrevShape)) return;
}

//================================================================
// Function : Chamfers_Presentation::sampleBoxCylChamfer
// Purpose  : creates a more complecated chamfer on a shape created
//            by fusing box and cylinder
//================================================================
void Chamfers_Presentation::sampleBoxCylChamfer()
{
  setResultTitle("Creating Chamfer");
  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a shape, a result of fusing of a box and cylinder" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  Standard_Real b = a/2;" EOL
    "  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);" EOL
    "  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), a/4, a);" EOL
    "  BRepAlgoAPI_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());" EOL
    "  aFuser.Build();" EOL EOL
    
    "  // initializing a class that builds chamfer" EOL
    "  BRepFilletAPI_MakeChamfer aMakeChamfer(aFuser.Shape());" EOL EOL
    
    "  // a map of \"edge-to-list of faces\".  Key = edge, value = list of 2 faces " EOL
    "  // of the box that \"touch\" the edge" EOL
    "  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;" EOL
    "  // filling the map with edges and faces of the box" EOL
    "  TopExp::MapShapesAndAncestors(aFuser.Shape(), TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);" EOL EOL
    
    "  // retrieving list of edges to build a chamfer on - edges of " EOL
    "  // intersection of the cylinder and the box" EOL
    "  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();" EOL
    "  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);" EOL
    "" EOL
    "  // creating fillet on edge intersection edges" EOL
    "  Standard_Real d = a/20;" EOL
    "  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())" EOL
    "  {" EOL
    "    TopoDS_Edge anEdge = TopoDS::Edge(anIntersectionEdgesIt.Value());" EOL
    "    if (!aMapOfEdgeToFaces.Contains(anEdge)) continue;" EOL
    "    const TopTools_ListOfShape& aFaces = aMapOfEdgeToFaces.FindFromKey(anEdge);" EOL
    "    TopoDS_Face aFace = TopoDS::Face(aFaces.First());" EOL
    "    aMakeChamfer.Add(d, d, anEdge, aFace);" EOL
    "  }" EOL
    "    " EOL
    "  aMakeChamfer.Build();" EOL
    "  if (aMakeChamfer.IsDone())" EOL
    "    TopoDS_Shape aChamferedShape = aMakeChamfer.Shape();" EOL;
  setResultText(aText.ToCString());

  // creating a shape, a result of fusing of a box and cylinder
  Standard_Real a = BOX_SIDE_LEN;
  Standard_Real b = a/2;
  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);
  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), a/4, a);
  BRepAlgo_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());
  aFuser.Build();

  Handle(AIS_InteractiveObject) aPrevObj = new AIS_Shape(aFuser.Shape());
  getAISContext()->Display(aPrevObj, Standard_False);
  COCCDemoDoc::Fit();

  if (WAIT_A_SECOND) return;

  // initializing a class that builds chamfer
  BRepFilletAPI_MakeChamfer aMakeChamfer(aFuser.Shape());

  // a map of "edge-to-list of faces".  Key = edge, value = list of 2 faces 
  // of the box that "touch" the edge
  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;
  // filling the map with edges and faces of the box
  TopExp::MapShapesAndAncestors(aFuser.Shape(), TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);

  // retrieving list of edges to build a chamfer on - edges of 
  // intersection of the cylinder and the box
  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();
  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);

  // creating fillet on edge intersection edges
  Standard_Real d = a/20;
  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(anIntersectionEdgesIt.Value());
    if (!aMapOfEdgeToFaces.Contains(anEdge)) continue;
    const TopTools_ListOfShape& aFaces = aMapOfEdgeToFaces.FindFromKey(anEdge);
    TopoDS_Face aFace = TopoDS::Face(aFaces.First());
    aMakeChamfer.Add(d, d, anEdge, aFace);
  }

  aMakeChamfer.Build();

  if (aMakeChamfer.IsDone())
  {
    getAISContext()->Erase(aPrevObj, Standard_False);
    getAISContext()->Display(new AIS_Shape(aMakeChamfer.Shape()));
    //aPrevObj = aNewShape;// uncomment if further thePrevObj is used
  }
}

//================================================================
// Function : Chamfers_Presentation::sampleBoxCylAngChamfer
// Purpose  : creates a more complecated chamfer on a shape created
//            by fusing box and cylinder
//================================================================
void Chamfers_Presentation::sampleBoxCylAngChamfer()
{
  setResultTitle("Creating Chamfer");
  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a shape, a result of fusing of a box and cylinder" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  Standard_Real b = a/2;" EOL
    "  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);" EOL
    "  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,-0.4,0.6)), a/4, a);" EOL
    "  BRepAlgoAPI_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());" EOL
    "  aFuser.Build();" EOL EOL
    
    "  // initializing a class that builds chamfer" EOL
    "  BRepFilletAPI_MakeChamfer aMakeChamfer(aFuser.Shape());" EOL EOL
    
    "  // a map of \"edge-to-list of faces\".  Key = edge, value = list of 2 faces " EOL
    "  // of the box that \"touch\" the edge" EOL
    "  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;" EOL
    "  // filling the map with edges and faces of the box" EOL
    "  TopExp::MapShapesAndAncestors(aFuser.Shape(), TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);" EOL EOL
    
    "  // retrieving list of edges to build a chamfer on - edges of " EOL
    "  // intersection of the cylinder and the box" EOL
    "  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();" EOL
    "  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);" EOL EOL

    "  // creating chamfer on intersection edges" EOL
    "  Standard_Real d = a/20;" EOL
    "  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())" EOL
    "  {" EOL
    "    TopoDS_Edge anEdge = TopoDS::Edge(anIntersectionEdgesIt.Value());" EOL
    "    if (!aMapOfEdgeToFaces.Contains(anEdge)) continue;" EOL
    "    const TopTools_ListOfShape& aFaces = aMapOfEdgeToFaces.FindFromKey(anEdge);" EOL
    "    TopoDS_Face aFace = TopoDS::Face(aFaces.First());" EOL
    "    aMakeChamfer.Add(d, d, anEdge, aFace);" EOL
    "  }" EOL EOL
    
    "  aMakeChamfer.Build();" EOL
    "  if (aMakeChamfer.IsDone())" EOL
    "    TopoDS_Shape aChamferedShape = aMakeChamfer.Shape();" EOL;
  setResultText(aText.ToCString());

  // creating a shape, a result of fusing of a box and cylinder
  Standard_Real a = BOX_SIDE_LEN;
  Standard_Real b = a/2;
  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);
  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,-0.4,0.6)), a/4, a);
  BRepAlgo_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());
  aFuser.Build();

  Handle(AIS_InteractiveObject) aPrevObj = new AIS_Shape(aFuser.Shape());
  getAISContext()->Display(aPrevObj, Standard_False);
  COCCDemoDoc::Fit();

  if (WAIT_A_SECOND) return;

  // initializing a class that builds chamfer
  BRepFilletAPI_MakeChamfer aMakeChamfer(aFuser.Shape());

  // a map of "edge-to-list of faces".  Key = edge, value = list of 2 faces 
  // of the box that "touch" the edge
  TopTools_IndexedDataMapOfShapeListOfShape aMapOfEdgeToFaces;
  // filling the map with edges and faces of the box
  TopExp::MapShapesAndAncestors(aFuser.Shape(), TopAbs_EDGE, TopAbs_FACE, aMapOfEdgeToFaces);
  
  // retrieving list of edges to build a chamfer on - edges of 
  // intersection of the cylinder and the box
  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();
  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);

  // creating fillet on edge intersection edges
  Standard_Real d = a/20;
  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())
  {
    TopoDS_Edge anEdge = TopoDS::Edge(anIntersectionEdgesIt.Value());
    if (!aMapOfEdgeToFaces.Contains(anEdge)) continue;
    const TopTools_ListOfShape& aFaces = aMapOfEdgeToFaces.FindFromKey(anEdge);
    TopoDS_Face aFace = TopoDS::Face(aFaces.First());
    aMakeChamfer.Add(d, d, anEdge, aFace);
  }

  aMakeChamfer.Build();

  if (aMakeChamfer.IsDone())
  {
    getAISContext()->Erase(aPrevObj, Standard_False);
    getAISContext()->Display(new AIS_Shape(aMakeChamfer.Shape()));
    //thePrevShape = aNewShape;// uncomment if further thePrevObj is used
  }
}

// ===============================================================
//                          FILLET CASES
// ===============================================================
//================================================================
// Function : Chamfers_Presentation::sampleBoxFillet1,2,3
// Purpose  : 
//================================================================
void Chamfers_Presentation::sampleBoxFillet()
{
  setResultTitle("Creating Fillet");
  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a box and centering it on the screen" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(gp_Pnt(-a/2, -a/2, -a/2), a, a, a);" EOL;
  setResultText(aText.ToCString());

  // show initial shape
  Standard_Real a = BOX_SIDE_LEN;
  TopoDS_Solid aBox = BRepPrimAPI_MakeBox(gp_Pnt(-a/2, -a/2, -a/2), a, a, a);

  Handle(AIS_InteractiveObject) aPrevObj = new AIS_Shape(aBox);
  getAISContext()->Display(aPrevObj, Standard_False);
  COCCDemoDoc::Fit();

  if (WAIT_A_SECOND) return;

  // show several fillets
  if (createFilletOnBox(aBox, a/10,  ChFi3d_Polynomial, aPrevObj))   return;
  if (createFilletOnBox(aBox, a/5,   ChFi3d_QuasiAngular, aPrevObj)) return;
  if (createFilletOnBox(aBox, a/3.3, ChFi3d_Rational, aPrevObj))     return;
}

//================================================================
// Function : Chamfers_Presentation::sampleBoxCylFillet
// Purpose  : creates a more complecated chamfer on a shape created
//            by fusing box and cylinder
//================================================================
void Chamfers_Presentation::sampleBoxCylFillet()
{
  setResultTitle("Creating Fillet");
  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a shape - result of fusing a box and a cylinder" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  Standard_Real b = a/2;" EOL
    "  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);" EOL
    "  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), a/4, a);" EOL
    "  BRepAlgoAPI_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());" EOL
    "  aFuser.Build();" EOL EOL
    
    "  // initializing a class that builds fillet" EOL
    "  // second parameter is a shape of fillet:" EOL
    "  // enum ChFi3d_FilletShape { ChFi3d_Rational, ChFi3d_QuasiAngular, ChFi3d_Polynomial };" EOL
    "  BRepFilletAPI_MakeFillet aMakeFillet(aFuser.Shape(), ChFi3d_Rational);" EOL EOL
  
    "  // retrieving list of edges to build a chamfer on - edges of " EOL
    "  // intersection of the cylinder and the box" EOL
    "  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();" EOL
    "  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);" EOL EOL

    "  // creating fillet on edge intersection edges" EOL
    "  Standard_Real d = a/10;" EOL
    "  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())" EOL
    "    aMakeFillet.Add(d, TopoDS::Edge(anIntersectionEdgesIt.Value()));" EOL
    "" EOL
    "  aMakeFillet.Build();" EOL EOL

    "  if (aMakeFillet.IsDone())" EOL
    "    TopoDS_Shape aShapeWithFillet = aMakeFillet.Shape();" EOL;
  setResultText(aText.ToCString());

  // creating a shape, a result of fusing of a box and cylinder
  Standard_Real a = BOX_SIDE_LEN;
  Standard_Real b = a/2;
  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);
  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), a/4, a);
  BRepAlgo_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());
  aFuser.Build();

  Handle(AIS_InteractiveObject) aPrevObj = new AIS_Shape(aFuser.Shape());
  getAISContext()->Display(aPrevObj, Standard_False);
  COCCDemoDoc::Fit();

  if (WAIT_A_SECOND) return;

  // initializing a class that builds fillet
  // second parameter is a shape of fillet:
  // enum ChFi3d_FilletShape { ChFi3d_Rational, ChFi3d_QuasiAngular, ChFi3d_Polynomial };
  BRepFilletAPI_MakeFillet aMakeFillet(aFuser.Shape(), ChFi3d_Rational);
  
  // retrieving list of edges to build a chamfer on - edges of 
  // intersection of the cylinder and the box
  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();
  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);

  // creating fillet on edge intersection edges
  Standard_Real d = a/10;
  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())
    aMakeFillet.Add(d, TopoDS::Edge(anIntersectionEdgesIt.Value()));

  aMakeFillet.Build();

  if (aMakeFillet.IsDone())
  {
    getAISContext()->Erase(aPrevObj, Standard_False);
    //aPrevObj = new AIS_Shape(aMakeFillet.Shape());// uncomment if aPrevObj is used further
    getAISContext()->Display(new AIS_Shape(aMakeFillet.Shape()));
  }
}


//================================================================
// Function : Chamfers_Presentation::sampleBoxCylAngFillet
// Purpose  : creates a fillet on a more complecated shape created
//            by fusing box and cylinder
//================================================================
void Chamfers_Presentation::sampleBoxCylAngFillet()
{
  setResultTitle("Creating Fillet");
  TCollection_AsciiString aText;
  aText = aText +
    "  // creating a shape - result of fusing a box and a cylinder" EOL
    "  Standard_Real a = "+TCollection_AsciiString(BOX_SIDE_LEN)+";" EOL
    "  Standard_Real b = a/2;" EOL
    "  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);" EOL
    "  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,-0.4,0.6)), a/4, a);" EOL
    "  BRepAlgoAPI_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());" EOL
    "  aFuser.Build();" EOL EOL
    
    "  // initializing a class that builds fillet" EOL
    "  // second parameter is a shape of fillet:" EOL
    "  // enum ChFi3d_FilletShape { ChFi3d_Rational, ChFi3d_QuasiAngular, ChFi3d_Polynomial };" EOL
    "  BRepFilletAPI_MakeFillet aMakeFillet(aFuser.Shape(), ChFi3d_Rational);" EOL EOL
  
    "  // retrieving list of edges to build a chamfer on - edges of " EOL
    "  // intersection of the cylinder and the box" EOL
    "  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();" EOL
    "  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);" EOL EOL

    "  // creating fillet on intersection edges" EOL
    "  Standard_Real d = a/10;" EOL
    "  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())" EOL
    "    aMakeFillet.Add(d, TopoDS::Edge(anIntersectionEdgesIt.Value()));" EOL
    "  aMakeFillet.Build();" EOL EOL

    "  if (aMakeFillet.IsDone())" EOL
    "    TopoDS_Shape aShapeWithFillet = aMakeFillet.Shape();" EOL;
  setResultText(aText.ToCString());

  // creating a shape, a result of fusing of a box and cylinder
  Standard_Real a = BOX_SIDE_LEN;
  Standard_Real b = a/2;
  BRepPrimAPI_MakeBox aBoxMaker (gp_Pnt(-b, -b, -b), a, a, a);
  BRepPrimAPI_MakeCylinder aCylMaker(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,-0.4,0.6)), a/4, a);
  BRepAlgo_Fuse aFuser(aBoxMaker.Solid(), aCylMaker.Solid());
  aFuser.Build();

  Handle(AIS_InteractiveObject) aPrevObj = new AIS_Shape(aFuser.Shape());
  getAISContext()->Display(aPrevObj, Standard_False);
  COCCDemoDoc::Fit();

  if (WAIT_A_SECOND) return;

  // initializing a class that builds fillet
  // second parameter is a shape of fillet:
  // enum ChFi3d_FilletShape { ChFi3d_Rational, ChFi3d_QuasiAngular, ChFi3d_Polynomial };
  BRepFilletAPI_MakeFillet aMakeFillet(aFuser.Shape(), ChFi3d_Rational);
  
  // retrieving list of edges to build a chamfer on - edges of 
  // intersection of the cylinder and the box
  const TopTools_ListOfShape& aEdgesOfIntersection = aFuser.Builder()->Section();
  TopTools_ListIteratorOfListOfShape anIntersectionEdgesIt(aEdgesOfIntersection);

  // creating fillet on edge intersection edges
  Standard_Real d = a/10;
  for (; anIntersectionEdgesIt.More(); anIntersectionEdgesIt.Next())
    aMakeFillet.Add(d, TopoDS::Edge(anIntersectionEdgesIt.Value()));

  aMakeFillet.Build();

  if (aMakeFillet.IsDone())
  {
    getAISContext()->Erase(aPrevObj, Standard_False);
    //aPrevObj = new AIS_Shape(aMakeFillet.Shape());// uncomment if aPrevObj is used further
    getAISContext()->Display(new AIS_Shape(aMakeFillet.Shape()));
  }
}
