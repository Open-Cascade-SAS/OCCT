// Glue_Presentation.cpp: implementation of the Glue_Presentation class.
// Glue two solid shapes with contiguous faces
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Glue_Presentation.h"
#include <OCCDemoDoc.h>

#include <TopoDS_Solid.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepFeat_Gluer.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Glue_Presentation;

// Initialization of array of samples
const Glue_Presentation::PSampleFuncType Glue_Presentation::SampleFuncs[] =
{
  //&Glue_Presentation::sampleBoxes1,
  &Glue_Presentation::sampleBoxes,
  &Glue_Presentation::sampleCylinder,
  //&Glue_Presentation::sampleMoreBoxes
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define RESULT_SHAPE_COLOR Quantity_NOC_ORANGE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Glue_Presentation::Glue_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Glue shapes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Glue_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////
//================================================================
// Function : Glue_Presentation::sampleBoxes
// Purpose  : 
//================================================================
void Glue_Presentation::sampleBoxes()
{
  setResultTitle("Glue two solids");
  setResultText(
    "  // Create shapes" EOL
    "  TopoDS_Solid aShape1;" EOL
    "  TopoDS_Solid aShape2; " EOL
    "  //aShape1 = ..." EOL
    "  //aShape2 = ..." EOL
    "" EOL
    "  // Define same domain faces on aShape1 and aShape2" EOL
    "  TopoDS_Face aFace1;" EOL
    "  TopoDS_Face aFace2;" EOL
    "  //aFace1 = ..." EOL
    "  //aFace2 = ..." EOL
    "" EOL
    "  // Define same domain edges on aShape1 and aShape2" EOL
    "  TopoDS_Edge aEdge1;" EOL
    "  TopoDS_Edge aEdge2;" EOL
    "  //aEdge1 = ..." EOL
    "  //aEdge2 = ..." EOL
    "" EOL
    "  BRepFeat_Gluer aGluer(aShape2, aShape1);" EOL
    "  aGluer.Bind(aFace2, aFace1);" EOL
    "  aGluer.Bind(aEdge2, aEdge1);" EOL
    "  aGluer.Build();" EOL
    "" EOL
    "  TopoDS_Shape aGluedShape;" EOL
    "  if (aGluer.IsDone())" EOL
    "    aGluedShape = aGluer;" EOL);

  Standard_Real a = 10.0;
  TopoDS_Solid aShape1 = BRepPrimAPI_MakeBox(a,a,a);
  TopoDS_Solid aShape2 = BRepPrimAPI_MakeBox(gp_Pnt(a,a/2,a/4),a/2,a/2,a/2);

  TopTools_IndexedMapOfShape aShape1Faces, aShape2Faces;
  TopTools_IndexedMapOfShape aShape1Edges, aShape2Edges;
  TopExp::MapShapes(aShape1, TopAbs_FACE, aShape1Faces);
  TopExp::MapShapes(aShape2, TopAbs_FACE, aShape2Faces);
  TopExp::MapShapes(aShape1, TopAbs_EDGE, aShape1Edges);
  TopExp::MapShapes(aShape2, TopAbs_EDGE, aShape2Edges);
  Standard_Integer fi1 = 2, fi2 = 1, ei1 = 7, ei2 = 3;
  TopoDS_Face aFace1 = TopoDS::Face(aShape1Faces(fi1));
  TopoDS_Face aFace2 = TopoDS::Face(aShape2Faces(fi2));
  TopoDS_Edge aEdge1 = TopoDS::Edge(aShape1Edges(ei1));
  TopoDS_Edge aEdge2 = TopoDS::Edge(aShape2Edges(ei2));

  BRepFeat_Gluer aGluer(aShape2, aShape1);
  aGluer.Bind(aFace2, aFace1);
  aGluer.Bind(aEdge2, aEdge1);
  aGluer.Build();
  
  TopoDS_Shape aGluedShape;
  if (aGluer.IsDone())
    aGluedShape = aGluer;

  TopTools_ListOfShape aEdges1,aEdges2,aFaces1,aFaces2;
  aEdges1.Append(aEdge1);  aEdges2.Append(aEdge2);
  aFaces1.Append(aFace1);  aFaces2.Append(aFace2);

  displayShapesFaces(aShape1,aShape2,aGluedShape,aFaces1,aFaces2,aEdges1,aEdges2);
}

//================================================================
// Function : Glue_Presentation::sampleCylinder
// Purpose  : 
//================================================================
void Glue_Presentation::sampleCylinder()
{
  setResultTitle("Glue two solids");
  setResultText(
    "  // Create shapes" EOL
    "  TopoDS_Solid aShape1;" EOL
    "  TopoDS_Solid aShape2; " EOL
    "  //aShape1 = ..." EOL
    "  //aShape2 = ..." EOL
    "" EOL
    "  // Define same domain faces on aShape1 and aShape2" EOL
    "  TopoDS_Face aFace1;" EOL
    "  TopoDS_Face aFace2;" EOL
    "  //aFace1 = ..." EOL
    "  //aFace2 = ..." EOL
    "" EOL
    "  BRepFeat_Gluer aGluer(aShape2, aShape1);" EOL
    "  aGluer.Bind(aFace2, aFace1);" EOL
    "  aGluer.Build();" EOL
    "" EOL
    "  TopoDS_Shape aGluedShape;" EOL
    "  if (aGluer.IsDone())" EOL
    "    aGluedShape = aGluer;" EOL);

  TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,0),gp_Pnt(5,0,0));
  TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(gp_Pnt(19,0,0),gp_Pnt(24,0,0));
  TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(gp_Pnt(24,0,0),gp_Pnt(24,15,0));
  TopoDS_Edge e5 = BRepBuilderAPI_MakeEdge(gp_Pnt(24,15,0),gp_Pnt(0,15,0));
  TopoDS_Edge e6 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,15,0),gp_Pnt(0,0,0));
  Handle(Geom_Circle) e2C = new Geom_Circle(gp_Ax2(gp_Pnt(12,0,0), gp_Dir(0,0,1)), 7);
  e2C->Rotate(gp_Ax1(gp_Pnt(12,0,0),gp_Dir(0,0,1)),PI);
  Handle(Geom_TrimmedCurve) e2A = new Geom_TrimmedCurve(e2C, PI, 2*PI);
  TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(e2A);

  BRepBuilderAPI_MakeWire wireMaker(e1,e2,e3,e4);
  wireMaker.Add(e5);
  wireMaker.Add(e6);

  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(wireMaker);

  TopoDS_Shape aShape1 = BRepPrimAPI_MakePrism(aFace, gp_Vec(gp_Pnt(0,0,0),gp_Pnt(0,0,30)));

  TopoDS_Shape aShape2 = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(12,0,-10),gp_Dir(0,0,1), gp_Dir(-0.5,-0.5,0)), 7, 50);

  TopTools_IndexedMapOfShape aShape1Faces, aShape2Faces;
  TopExp::MapShapes(aShape1, TopAbs_FACE, aShape1Faces);
  TopExp::MapShapes(aShape2, TopAbs_FACE, aShape2Faces);
  Standard_Integer fi1 = 2;
  Standard_Integer fi2 = 1;
  TopoDS_Face aFace1 = TopoDS::Face(aShape1Faces(fi1));
  TopoDS_Face aFace2 = TopoDS::Face(aShape2Faces(fi2));

  BRepFeat_Gluer aGluer(aShape1, aShape2);
  aGluer.Bind(aFace1, aFace2);
  aGluer.Build();

  TopoDS_Shape aGluedShape;
  if (aGluer.IsDone())
    aGluedShape = aGluer;

  TopTools_ListOfShape aEdges1,aEdges2,aFaces1,aFaces2;
  aFaces1.Append(aFace1);  aFaces2.Append(aFace2);

  displayShapesFaces(aShape1, aShape2, aGluedShape, aFaces1, aFaces2, aEdges1, aEdges2);
}

//================================================================
// Function : Glue_Presentation::sampleMoreBoxes
// Purpose  : 
//================================================================
void Glue_Presentation::sampleMoreBoxes()
{
  setResultTitle("Glue two solids");
  setResultText(
    "  // Create shapes" EOL
    "  TopoDS_Solid aShape1;" EOL
    "  TopoDS_Solid aShape2; " EOL
    "  //aShape1 = ..." EOL
    "  //aShape2 = ..." EOL
    "" EOL
    "  // Define same domain faces on aShape1 and aShape2" EOL
    "  TopoDS_Face aFace11,aFace12,aFace13;" EOL
    "  TopoDS_Face aFace21,aFace22,aFace23;" EOL
    "  //aFace11 = ..." EOL
    "  //aFace21 = ..." EOL
    "" EOL
    "  // Define same domain edges on aShape1 and aShape2" EOL
    "  TopoDS_Edge aEdge11,aEdge12,aEdge13;" EOL
    "  TopoDS_Edge aEdge21,aEdge22,aEdge23;" EOL
    "  //aEdge11 = ..." EOL
    "  //aEdge21 = ..." EOL
    "" EOL
    "  BRepFeat_Gluer aGluer(aShape2, aShape1);" EOL
    "  aGluer.Bind(aFace21, aFace11);" EOL
    "  aGluer.Bind(aFace22, aFace12);" EOL
    "  aGluer.Bind(aFace23, aFace13);" EOL
    "  aGluer.Bind(aEdge21, aEdge11);" EOL
    "  aGluer.Bind(aEdge22, aEdge12);" EOL
    "  aGluer.Bind(aEdge23, aEdge13);" EOL
    "  aGluer.Build();" EOL
    "" EOL
    "  TopoDS_Shape aGluedShape;" EOL
    "  if (aGluer.IsDone())" EOL
    "    aGluedShape = aGluer;" EOL);

  TopoDS_Shape aShape1;
  TopoDS_Shape aShape2;

  Standard_Real a = 10.0;
  TopoDS_Solid aBox1 = BRepPrimAPI_MakeBox(a,a,a);
  TopoDS_Solid aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(a/2,a/2,a/2),a,a,a);

  BRepAlgoAPI_Cut aCutter(aBox1,aBox2);
  aCutter.Build();
  if (aCutter.IsDone())
    aShape1 = aCutter.Shape();

  aShape2 = aBox2;

  TopTools_IndexedMapOfShape aShape1Faces, aShape2Faces;
  TopTools_IndexedMapOfShape aShape1Edges, aShape2Edges;
  TopExp::MapShapes(aShape1, TopAbs_FACE, aShape1Faces);
  TopExp::MapShapes(aShape2, TopAbs_FACE, aShape2Faces);
  TopExp::MapShapes(aShape1, TopAbs_EDGE, aShape1Edges);
  TopExp::MapShapes(aShape2, TopAbs_EDGE, aShape2Edges);
  Standard_Integer ei11 = 15, ei12 = 14, ei13 = 18;
  Standard_Integer ei21 = 1, ei22 = 9, ei23 = 4;
  Standard_Integer fi11 = 9, fi12 = 4, fi13 = 6;
  Standard_Integer fi21 = 1, fi22 = 3, fi23 = 5;
  TopoDS_Face f11 = TopoDS::Face(aShape1Faces(fi11));
  TopoDS_Face f12 = TopoDS::Face(aShape1Faces(fi12));
  TopoDS_Face f13 = TopoDS::Face(aShape1Faces(fi13));
  TopoDS_Edge e11 = TopoDS::Edge(aShape1Edges(ei11));
  TopoDS_Edge e12 = TopoDS::Edge(aShape1Edges(ei12));
  TopoDS_Edge e13 = TopoDS::Edge(aShape1Edges(ei13));
  TopoDS_Face f21 = TopoDS::Face(aShape2Faces(fi21));
  TopoDS_Face f22 = TopoDS::Face(aShape2Faces(fi22));
  TopoDS_Face f23 = TopoDS::Face(aShape2Faces(fi23));
  TopoDS_Edge e21 = TopoDS::Edge(aShape2Edges(ei21));
  TopoDS_Edge e22 = TopoDS::Edge(aShape2Edges(ei22));
  TopoDS_Edge e23 = TopoDS::Edge(aShape2Edges(ei23));

  BRepFeat_Gluer aGluer(aShape1, aShape2);
  aGluer.Bind(e11, e21);
  aGluer.Bind(e12, e22);
  aGluer.Bind(e13, e23);
  aGluer.Bind(f11, f21);
  aGluer.Bind(f12, f22);
  aGluer.Bind(f13, f23);
  aGluer.Build();
  
  TopoDS_Shape aGluedShape;
  if (aGluer.IsDone())
    aGluedShape = aGluer;

  TopTools_ListOfShape aEdges1,aEdges2,aFaces1,aFaces2;
  aEdges1.Append(e11);  aEdges1.Append(e12);
  aEdges1.Append(e13);  aEdges2.Append(e21);
  aEdges2.Append(e22);  aEdges2.Append(e23);
  aFaces1.Append(f11);  aFaces1.Append(f12);
  aFaces1.Append(f13);  aFaces2.Append(f21);
  aFaces2.Append(f22);  aFaces2.Append(f23);

  displayShapesFaces(aShape1,aShape2,aGluedShape,aFaces1,aFaces2,aEdges1,aEdges2);
}


//================================================================
// Function : Glue_Presentation::drawShapeWf
// Purpose  : display a shape in wire frame mode
//================================================================
Standard_Boolean Glue_Presentation::drawShapeWf(TopoDS_Shape& aShape, 
                                                Handle(AIS_InteractiveObject)& io)
{
  io = drawShape(aShape, Graphic3d_NOM_BRASS, Standard_False);
  getAISContext()->SetDisplayMode(io, AIS_WireFrame, Standard_False);
  getAISContext()->Display(io);
  return !WAIT_A_LITTLE;
}


//================================================================
// Function : Glue_Presentation::drawShapeSh
// Purpose  : display a shape in shaded mode
//================================================================
Standard_Boolean Glue_Presentation::drawShapeSh(TopoDS_Shape& aShape, 
                                                Handle(AIS_InteractiveObject)& io)
{
  io = drawShape(aShape, Graphic3d_NOM_BRASS, Standard_False);
  getAISContext()->SetDisplayMode(io, AIS_Shaded, Standard_False);
  getAISContext()->Display(io, Standard_False);
  COCCDemoDoc::Fit();
  return !WAIT_A_LITTLE;
}

//================================================================
// Function : Glue_Presentation::displayShapesFaces
// Purpose  : displays shapes, same domain faces and edges if given
//================================================================
Standard_Boolean Glue_Presentation::displayShapesFaces(TopoDS_Shape& aShape1,
                                           TopoDS_Shape& aShape2,
                                           TopoDS_Shape& aGluedShape,
                                           TopTools_ListOfShape& aFaces1,
                                           TopTools_ListOfShape& aFaces2,
                                           TopTools_ListOfShape& aEdges1,
                                           TopTools_ListOfShape& aEdges2)
{
  Handle(AIS_Shape) io1,io2,io3,io4,io5,io6;
  if (!drawShapeSh(aShape1, io1)) return Standard_False;
  if (!drawShapeSh(aShape2, io2)) return Standard_False;

  getAISContext()->SetDisplayMode(io1, AIS_WireFrame, Standard_False);
  getAISContext()->SetDisplayMode(io2, AIS_WireFrame);
  if (WAIT_A_LITTLE) return Standard_False;

  TopTools_ListIteratorOfListOfShape it1,it2;
  for (it1.Initialize(aEdges1),it2.Initialize(aEdges2); 
       it1.More() && it2.More(); 
       it1.Next(),it2.Next())
  {
    if (!drawShapeWf(it2.Value(), io6)) return Standard_False;
    getAISContext()->Erase(io6);
    if (WAIT_A_LITTLE) return Standard_False;

    if (!drawShapeWf(it1.Value(), io6)) return Standard_False;
    getAISContext()->Erase(io6);
    if (WAIT_A_LITTLE) return Standard_False;
  }

  for (it1.Initialize(aFaces1),it2.Initialize(aFaces2); 
       it1.More() && it2.More(); 
       it1.Next(),it2.Next())
  {
    if (!drawShapeSh(it2.Value(), io6)) return Standard_False;
    getAISContext()->Erase(io6);
    if (WAIT_A_LITTLE) return Standard_False;

    if (!drawShapeSh(it1.Value(), io6)) return Standard_False;
    getAISContext()->Erase(io6);
    if (WAIT_A_LITTLE) return Standard_False;
  }

  getAISContext()->Erase(io1, Standard_False);
  getAISContext()->Erase(io2, Standard_False);  

  io1 = drawShape(aGluedShape, RESULT_SHAPE_COLOR, Standard_False);
  getAISContext()->SetDisplayMode(io1, AIS_WireFrame, Standard_False);
  getAISContext()->Display(io1);
  if (WAIT_A_LITTLE) return Standard_False;

  getAISContext()->SetDisplayMode(io1, AIS_Shaded);

  return Standard_True;
}
