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

#if defined(WOKC40)

//===============================================
//    AIS2D Objects Creation : Datums (axis,trihedrons,lines,planes)
//===============================================

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <QAViewer2dTest.hxx>

#include <string.h>

#include <Quantity_NameOfColor.hxx>

#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>
#include <V2d.hxx>

#include <QAViewer2dTest_DoubleMapOfInteractiveAndName.hxx>
#include <QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <QAViewer2dTest_EventManager.hxx>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>

#include <TopAbs.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <Draw_Window.hxx>

#include <AIS2D_ProjShape.hxx>
#include <AIS2D_InteractiveContext.hxx>

#include <Graphic2d_InfiniteLine.hxx>
#include <Graphic2d_Circle.hxx>

#include <Geom2d_Line.hxx>
#include <Geom2d_CartesianPoint.hxx>

#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_CartesianPoint.hxx>

#include <gp_Pln.hxx>
#include <gp_Circ.hxx>

#include <GC_MakePlane.hxx>

extern QAViewer2dTest_DoubleMapOfInteractiveAndName& GetMapOfAIS2D();
Standard_IMPORT int ViewerMainLoop2d (Standard_Integer argc, char** argv);
extern Handle(AIS2D_InteractiveContext)& TheAIS2DContext();

//==============================================================================
//function : SelectShape2d
//purpose  : 
//==============================================================================
TopoDS_Shape SelectShape2d (int theSelType) 
{
  TopoDS_Shape ShapeA;

//  if (theSelType != 5)
//    TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(theSelType));

  switch (theSelType)
  {
  case 1:
    cout << " Select a vertex ..." << endl;
    break;
  case 2:
    cout << " Select an edge ..." << endl;
    break;
  case 4:
    cout << " Select a face ..." << endl;
    break;
  case 5: // user defined text
    break;
  default:
    cout << " Incorrect selection mode" << endl;
    break;
  }
 
  Standard_Integer argcc = 5;
  char *buff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  char **argvv = (char **) buff;
  while (ViewerMainLoop2d(argcc, argvv)) {}

/*  for (TheAIS2DContext()->InitSelected();
       TheAIS2DContext()->MoreSelected();
       TheAIS2DContext()->NextSelected())
  {
    ShapeA = TheAIS2DContext()->SelectedShape();
  }*/

//  if (theSelType != 5)
//    TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(theSelType));

  return ShapeA;
}
#if 0
//==============================================================================
//function : V2dTrihedron2D
//purpose  : Create a plane with a 2D  trihedron from a faceselection
//Draw arg : v2dtri2d  name  
//==============================================================================
static int V2dTrihedron2D (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  if (argc != 2)
  {
    cout << argv[0] << " wrong arguments number" << endl;
    return 1;
  }
  
  // Declarations
  Standard_Integer myCurrentIndex;

  // Fermeture des contextes 
  TheAIS2DContext()->CloseAllContext();

  // Ouverture d'un contexte local et recuperation de son index.
  TheAIS2DContext()->OpenLocalContext();
  myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();

  TopoDS_Shape ShapeB = SelectShape2d(4); // Face
  TopoDS_Face FaceB = TopoDS::Face(ShapeB);
  
  // Construction du Plane
  // recuperation des edges des faces.
  TopExp_Explorer FaceExpB (FaceB, TopAbs_EDGE);
  
  TopoDS_Edge EdgeB = TopoDS::Edge(FaceExpB.Current());

  // declarations 
  gp_Pnt A,B,C;
  
  // si il y a plusieurs edges
  if (FaceExpB.More())
  {
    FaceExpB.Next();
    TopoDS_Edge EdgeC = TopoDS::Edge(FaceExpB.Current());
    BRepAdaptor_Curve theCurveB (EdgeB);
    BRepAdaptor_Curve theCurveC (EdgeC);
    A = theCurveC.Value(0.1);
    B = theCurveC.Value(0.9);
    C = theCurveB.Value(0.5);
  }
  else
  {
    // FaceB a 1 unique edge courbe
    BRepAdaptor_Curve theCurveB (EdgeB);
    A = theCurveB.Value(0.1);
    B = theCurveB.Value(0.9);
    C = theCurveB.Value(0.5);
  }
  // Construction du Geom_Plane
  GC_MakePlane MkPlane (A,B,C);
  Handle(Geom_Plane) theGeomPlane = MkPlane.Value();
  
  // Construction de l'AIS2D_PlaneTrihedron
//  Handle(AIS2D_PlaneTrihedron) theAIS2DPlaneTri = new AIS2D_PlaneTrihedron(theGeomPlane);

  // Fermeture du contexte local.
  TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
  
  // on le display & bind
//  TheAIS2DContext()->Display(theAIS2DPlaneTri);
//  GetMapOfAIS2D().Bind(theAIS2DPlaneTri, argv[1]);
  
  return 0; 
}
#endif

//==============================================================================
//function : V2dTrihedron
//purpose  : Create a trihedron. If no arguments are set, the default
//           trihedron (Oxyz) is created.
//Draw arg : vtrihedron  name  [Xo] [Yo] [Zo] [Zu] [Zv] [Zw] [Xu] [Xv] [Xw]
//==============================================================================
/*static int V2dTrihedron (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  // Verification des arguments
  if (argc < 2 || argc > 11)
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }

  TCollection_AsciiString name = argv[1];

  if (argc > 5 && argc != 11)
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }

  Standard_Real coord[9] = {0.,0.,0.,0.,0.,1.,1.,0.,0.};
  if (argc > 2)
  {
    Standard_Integer i;
    for (i = 0; i <= 2; i++)
      coord[i] = atof(argv[2+i]);
    
    if (argc > 5)
    {
      for (i = 0; i <= 2; i++)
      {
	coord[3+i] = atof(argv[6+i]);
	coord[6+i] = atof(argv[8+i]);
      }
    }
  }
  gp_Pnt ThePoint   (coord[0],coord[1],coord[2]);
  gp_Dir TheZVector (coord[3],coord[4],coord[5]);
  gp_Dir TheXVector (coord[6],coord[7],coord[8]);

  if (!TheZVector.IsNormal(TheXVector,M_PI/180))
  {
    cout << argv[0] << " VectorX is not normal to VectorZ" << endl;
    return 1;
  }

  Handle(Geom_Axis2Placement) OrigineAndAxii = new Geom_Axis2Placement(ThePoint,TheZVector,TheXVector);

  // Creation du triedre
  Handle(AIS2D_Trihedron) aShape = new AIS2D_Trihedron(OrigineAndAxii);
  GetMapOfAIS2D().Bind(aShape,name);
  TheAIS2DContext()->Display(aShape);

  return 0; 
}*/
#if 0
//==============================================================================
//function : V2dSize
//purpose  : Change the size of a named or selected trihedron
//           if no name : it affects the trihedrons witch are selected otherwise nothing is donne
//           if no value, the value is set at 100 by default
//Draw arg : v2dsize [name] [size]
//==============================================================================
static int V2dSize (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  Standard_Boolean ThereIsName;
  Standard_Real    value;
  //  Quantity_NameOfColor col = Quantity_NOC_BLACK;
  
  if (argc > 3)
  {
    cout << argv[0] << " Syntaxe error" << endl;
    return 1;
  }
  
  if (argc == 1)
  {
    ThereIsName = Standard_False;
    value = 100;
  }
  else if (argc == 2)
  {
    ThereIsName = Standard_False;
    value = atof(argv[1]);
  }
  else
  {
    ThereIsName = Standard_True;
    value = atof(argv[2]);
  }

  if (TheAIS2DContext()->HasOpenedContext()) TheAIS2DContext()->CloseLocalContext();
  
  if (!ThereIsName)
  {
//    if (TheAIS2DContext()->NbCurrents() <= 0) return -1;
    QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());

    while (it.More())
    {
      Handle(AIS2D_InteractiveObject) aShape = it.Key1();
      
      if (TheAIS2DContext()->IsCurrent(aShape))
      {
/*	if (aShape->Type() == AIS2D_KOI_Datum && aShape->Signature() == 3)
        {
	  if (aShape->HasColor())
          {
	    hascol = Standard_True;
	    col = aShape->Color();
          }
	  else hascol = Standard_False;
	  
	  Handle(AIS2D_Trihedron) aTrihedron = *(Handle(AIS2D_Trihedron)*) &aShape;
	  
	  aTrihedron->SetSize(value);
	  
	  if (hascol) aTrihedron->SetColor(col);
	  else        aTrihedron->UnsetColor();

	  // The trihedron hasn't be errased from the map
	  // so you just have to redisplay it
	  TheAIS2DContext()->Redisplay(aTrihedron, Standard_False);
	}*/
      }
      it.Next();
    }
    TheAIS2DContext()->UpdateCurrentViewer();
  }
  else
  {
    TCollection_AsciiString name = argv[1];
    
    if (GetMapOfAIS2D().IsBound2(name))
    {
      Handle(AIS2D_InteractiveObject) aShape = GetMapOfAIS2D().Find2(name);
      
/*      if (aShape->Type() == AIS2D_KOI_Datum && aShape->Signature() == 3)
      {
	if (aShape->HasColor())
        {
	  hascol = Standard_True;
	  col = aShape->Color();
        }
	else hascol = Standard_False;
	
	// On downcast aShape de AIS2D_InteractiveObject a AIS2D_Trihedron
	// pour lui appliquer la methode SetSize()
	Handle(AIS2D_Trihedron) aTrihedron = *(Handle(AIS2D_Trihedron)*) &aShape;
	
	aTrihedron->SetSize(value);
	
	if (hascol) aTrihedron->SetColor(col);
	else        aTrihedron->UnsetColor();
	
	// The trihedron hasn't be errased from the map
	// so you just have to redisplay it
	TheAIS2DContext()->Redisplay(aTrihedron, Standard_False);
      
	TheAIS2DContext()->UpdateCurrentViewer();
      }*/
    }
  }
  return 0; 
}
#endif

//==============================================================================
//function : V2dPlaneTrihedron
//purpose  : Create a plane from a trihedron selection. If no arguments are set, the default
//Draw arg : v2dplanetri  name  
//==============================================================================
/*static int V2dPlaneTrihedron (Draw_Interpretor& , Standard_Integer argc, char** argv)
{
  if (argc != 2)
  {
    cout << argv[0] << " error" << endl;
    return 1;
  }
  
  Standard_Integer myCurrentIndex;
  TheAIS2DContext()->CloseAllContexts();
  TheAIS2DContext()->OpenLocalContext(Standard_False);
  myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
  
  QAViewer2dTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it (GetMapOfAIS2D());
  while (it.More())
  {
    Handle(AIS2D_InteractiveObject) ShapeA = it.Key1();
    if (ShapeA->Type() == AIS2D_KOI_Datum && ShapeA->Signature() == 3)
    {
      Handle(AIS2D_Trihedron) TrihedronA = ((*(Handle(AIS2D_Trihedron)*)&ShapeA));
      TheAIS2DContext()->Load(TrihedronA,0,Standard_False);
      TheAIS2DContext()->Activate(TrihedronA,3);
    }
    it.Next();
  }
  
  cout<<" Select a plane."<<endl;
  Standard_Integer argccc = 5;
  char *bufff[] = { "VPick", "X", "VPickY","VPickZ", "VPickShape" };
  char **argvvv = bufff;
  while (ViewerMainLoop2d(argccc, argvvv)) {}
  
  Handle(AIS2D_InteractiveObject) theIOB;
  for (TheAIS2DContext()->InitSelected();
       TheAIS2DContext()->MoreSelected(); TheAIS2DContext()->NextSelected())
  {
    theIOB = TheAIS2DContext()->Interactive();
  }
  Handle(AIS2D_Plane) PlaneB = ((*(Handle(AIS2D_Plane)*)&theIOB));

  TheAIS2DContext()->CloseLocalContext(myCurrentIndex);

  // Construction de l'AIS2D_Plane
  //gp_Pnt A,B,C;
  //Handle(Geom_Plane) theGeomPlane;
  //PlaneB->PlaneAttributes(theGeomPlane,A,B,C);
  //Handle(AIS2D_PlaneTrihedron) theAIS2DPlaneTri= new AIS2D_PlaneTrihedron(theGeomPlane );

  // on le display & bind
  TheAIS2DContext()->Display(PlaneB);
  GetMapOfAIS2D().Bind(PlaneB, argv[1]);
  
  return 0; 
}*/

//==============================================================================
// Fonction        First click      2de click
// 
// v2daxis           vertex           vertex
//                   edge             None
// v2daxispara       edge             vertex
// v2daxisortho      edge             Vertex
// v2daxisinter      Face             Face
//==============================================================================

//==============================================================================
//function : V2dAxisBuilder
//purpose  : 
//Draw arg : v2daxis AxisName Xa Ya Xb Yb
//==============================================================================
static int V2dAxisBuilder (Draw_Interpretor& , Standard_Integer argc, char** argv) 
{
  Standard_Boolean HasArg;
  TCollection_AsciiString name;
  Standard_Integer MyCurrentIndex;
  
  if (argc < 2 || argc > 6)
  {
    cout << " Syntaxe error" << endl;
    return 1;
  }
  if (argc == 6) HasArg = Standard_True;
  else HasArg = Standard_False;
  
  name = argv[1];
  TheAIS2DContext()->CloseAllContext();
  
  Handle(V2d_View) V = QAViewer2dTest::CurrentView();

  if (HasArg)
  {
    Standard_Real coord [4];
    for (Standard_Integer i = 0; i <= 3; i++)
    {
      coord[i] = atof(argv[2+i]);
    }
//    gp_Pnt2d p1 (coord[0],coord[1]);
//    gp_Pnt2d p2 (coord[2],coord[3]);
    
    Handle(Graphic2d_GraphicObject) obj =
      new Graphic2d_GraphicObject(V->View());

    Handle(Graphic2d_InfiniteLine) line = new Graphic2d_InfiniteLine
      (obj, coord[0], coord[1], coord[2] - coord[0], coord[3] - coord[1]);

    obj->Display();
    V->Viewer()->Update();
//    gp_Vec2d myVect (p1, p2);
//    Handle(Geom2d_Line) myLine = new Geom2d_Line(p1, myVect);
//    Handle(AIS2D_Axis) TheAxis = new AIS2D_Axis(myLine);
//    GetMapOfAIS2D().Bind(TheAxis,name);
//    TheAIS2DContext()->Display(TheAxis);
  }
  else
  {
    // fonction vaxis
    // Purpose: Teste le constructeur AIS2D_Axis::AIS2D_Axis (x:Axis1Placement from Geom)
    if (!strcasecmp(argv[0], "v2daxis"))
    {
      TheAIS2DContext()->OpenLocalContext();
      MyCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
      
      // Active le mode edge et le mode vertex
//      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(1));
//      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(2));
      cout << " Select an edge or a vertex." << endl;
      
      TopoDS_Shape ShapeA = SelectShape2d(5);
      
      if (ShapeA.ShapeType() == TopAbs_VERTEX)
      {
//	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(2));
	cout << " Select a different vertex." << endl;
	
	TopoDS_Shape ShapeB;
	do
        {
          ShapeB = SelectShape2d(5);
        } while (ShapeB.IsSame(ShapeA));
	
	TheAIS2DContext()->CloseLocalContext(MyCurrentIndex);
	
	// Construction de l'axe 
	gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(ShapeA));
	gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
	gp_Vec aV (A,B);
	gp_Dir D (aV);
	Handle(Geom_Axis1Placement) OrigineAndVect = new Geom_Axis1Placement(A,D);
//	Handle(AIS2D_Axis) TheAxis = new AIS2D_Axis(OrigineAndVect);
//	GetMapOfAIS2D().Bind(TheAxis,name);
//	TheAIS2DContext()->Display(TheAxis);
      } 
      else
      {
	// Un unique edge (ShapeA) a ete picke
	// Fermeture du context local
	TheAIS2DContext()->CloseLocalContext(MyCurrentIndex);
	// Constuction de l'axe 
	TopoDS_Edge    ed = TopoDS::Edge(ShapeA);
	TopoDS_Vertex  Va,Vb;
	TopExp::Vertices(ed,Va,Vb );
	gp_Pnt A = BRep_Tool::Pnt(Va);
	gp_Pnt B = BRep_Tool::Pnt(Vb);
	gp_Vec aV (A,B);
	gp_Dir D (aV);
	Handle(Geom_Axis1Placement) OrigineAndVect = new Geom_Axis1Placement(A,D);
//	Handle(AIS2D_Axis) TheAxis = new AIS2D_Axis(OrigineAndVect);
//	GetMapOfAIS2D().Bind(TheAxis,name);
//	TheAIS2DContext()->Display(TheAxis);
      }
    }

    // Fonction axispara
    // Purpose: Teste le constructeur AIS2D_Axis::AIS2D_Axis(x: Axis2Placement from Geom, y: TypeOfAxis from AIS2D)
    else if (!strcasecmp(argv[0], "v2daxispara"))
    {
      TheAIS2DContext()->OpenLocalContext();
      MyCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
      
      TopoDS_Shape ShapeA = SelectShape2d(2); // Edge
      TopoDS_Shape ShapeB = SelectShape2d(1); // Vertex

      // Fermeture du context local
      TheAIS2DContext()->CloseLocalContext(MyCurrentIndex);
      
      // Construction de l'axe
      TopoDS_Edge ed = TopoDS::Edge(ShapeA);
      gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
      TopoDS_Vertex Va,Vc;
      TopExp::Vertices(ed,Va,Vc);
      gp_Pnt A = BRep_Tool::Pnt(Va);
      gp_Pnt C = BRep_Tool::Pnt(Vc);
      gp_Vec aV (A,C);
      gp_Dir D (aV);
      Handle(Geom_Axis1Placement) OrigineAndVect = new Geom_Axis1Placement(B,D);
//      Handle(AIS2D_Axis) TheAxis = new AIS2D_Axis(OrigineAndVect);
//      GetMapOfAIS2D().Bind(TheAxis,name);
//      TheAIS2DContext()->Display(TheAxis);
    }
    else // Fonction axisortho
    {
      TheAIS2DContext()->OpenLocalContext();
      MyCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
      
      TopoDS_Shape ShapeA = SelectShape2d(2); // Edge
      TopoDS_Shape ShapeB = SelectShape2d(1); // Vertex

      // Fermeture du context local
      TheAIS2DContext()->CloseLocalContext(MyCurrentIndex);
      
      // Construction de l'axe
      TopoDS_Edge ed = TopoDS::Edge(ShapeA);
      gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
      TopoDS_Vertex Va,Vc;
      TopExp::Vertices(ed,Va,Vc);
      gp_Pnt A = BRep_Tool::Pnt(Va);
      gp_Pnt C = BRep_Tool::Pnt(Vc);
      gp_Pnt E (A.Y()+A.Z()-C.Y()-C.Z(), C.X()-A.X(), C.X()-A.X());
      // W = AE est un vecteur orthogonal a AC
      gp_Vec aV (A,E);
      gp_Dir D (aV);
      Handle(Geom_Axis1Placement) OrigineAndVect = new Geom_Axis1Placement(B,D);
//      Handle(AIS2D_Axis) TheAxis = new AIS2D_Axis(OrigineAndVect);
//      GetMapOfAIS2D().Bind(TheAxis,name);
//      TheAIS2DContext()->Display(TheAxis);
    }
  }

  return 0;
}

//==============================================================================
// Fonction        First click      Result
// 
// vpoint          vertex           AIS2D_Point=Vertex
//                 edge             AIS2D_Point=Middle of the edge
//==============================================================================

#if 0
//==============================================================================
//function : V2dPointBuilder
//purpose  : Build an AIS2D_Point from coordinates or with a selected vertex or edge
//Draw arg : v2dpoint PoinName [Xa] [Ya]
//==============================================================================
static int V2dPointBuilder (Draw_Interpretor& , Standard_Integer argc, char** argv) 
{
  // Declarations
  Standard_Boolean HasArg;
  TCollection_AsciiString name;
  Standard_Integer myCurrentIndex;
  
  // Verification
  if (argc < 2 || argc > 4 )
  {
    cout << " Syntaxe error" << endl;
    return 1;
  }
  if (argc == 4) HasArg = Standard_True;
  else HasArg = Standard_False;
  
  name = argv[1];
  // Fermeture des contextes
  TheAIS2DContext()->CloseAllContext();
  
  // Il y a des arguments: teste l'unique constructeur AIS2D_Pnt::AIS2D_Pnt(Point from Geom)
  if (HasArg)
  {
    Standard_Real thecoord[3];
    for (Standard_Integer i = 0; i <= 2; i++) thecoord[i] = atof(argv[2+i]);
    Handle(Geom2d_CartesianPoint) myGeomPoint = new Geom2d_CartesianPoint(thecoord[0],thecoord[1]);
//    Handle(AIS2D_Point) myAIS2DPoint = new AIS2D_Point(myGeomPoint);
//    GetMapOfAIS2D().Bind(myAIS2DPoint, name);
//    TheAIS2DContext()->Display(myAIS2DPoint);
  }
  else // Il n'a pas d'arguments
  {
    TheAIS2DContext()->OpenLocalContext();
    myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
    
//    TheAIS2DContext()->ActivateStandardMode (AIS2D_ProjShape::SelectionType(1));
//    TheAIS2DContext()->ActivateStandardMode (AIS2D_ProjShape::SelectionType(2));
    cout << " Select a vertex or an edge (build the middle)" << endl;
    TopoDS_Shape ShapeA = SelectShape2d(5);

    if (ShapeA.ShapeType() == TopAbs_VERTEX)
    {
      TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
      
      // Construction du point
      gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(ShapeA));
      Handle(Geom_CartesianPoint) myGeomPoint = new Geom_CartesianPoint(A);
//      Handle(AIS2D_Point) myAIS2DPoint = new AIS2D_Point(myGeomPoint);
//      GetMapOfAIS2D().Bind(myAIS2DPoint,name);
//      TheAIS2DContext()->Display(myAIS2DPoint);
    }
    else
    {
      // Un Edge a ete selectionne
      // Fermeture du context local
      TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
      
      // Construction du point milieu de l'edge
      TopoDS_Edge myEdge = TopoDS::Edge(ShapeA);
      TopoDS_Vertex myVertexA,myVertexB;
      TopExp::Vertices (myEdge, myVertexA, myVertexB);
      gp_Pnt A = BRep_Tool::Pnt(myVertexA);
      gp_Pnt B = BRep_Tool::Pnt(myVertexB);

      Handle(Geom_CartesianPoint) myGeomPointM =
        new Geom_CartesianPoint((A.X()+B.X())/2, (A.Y()+B.Y())/2, (A.Z()+B.Z())/2);
//      Handle(AIS2D_Point) myAIS2DPointM = new AIS2D_Point(myGeomPointM);
//      GetMapOfAIS2D().Bind(myAIS2DPointM, name);
//      TheAIS2DContext()->Display(myAIS2DPointM);
    }
  }

  return 0;
}
#endif

//==============================================================================
// Fonction        1st click   2de click  3de click   
// v2dplane          Vertex      Vertex     Vertex
//                   Vertex      Edge
//                   Edge        Vertex
//                   Face
// v2dplanepara      Face        Vertex
//                   Vertex      Face
// v2dplaneortho     Face        Edge
//                   Edge        Face
//==============================================================================

//==============================================================================
//function : V2dPlaneBuilder
//purpose  : Build an AIS2D_Plane from selected entities or Named AIs components
//Draw arg : v2dplane PlaneName [AxisName]  [PointName]
//                              [PointName] [PointName] [PointName]
//                              [PlaneName] [PointName]
//==============================================================================
/*#include <BRepExtrema_ExtPC.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Pln.hxx>
#include <GC_MakePlane.hxx>
static int V2dPlaneBuilder (Draw_Interpretor& , Standard_Integer argc, char** argv) 
{
  Standard_Boolean HasArg;
  TCollection_AsciiString name;
  Standard_Integer myCurrentIndex;
  
  if (argc < 2 || argc > 5)
  {
    cout << " Syntaxe error" << endl;
    return 1;
  }

  if (argc == 5 || argc == 4) HasArg = Standard_True;
  else HasArg = Standard_False;
  
  name = argv[1];
  TheAIS2DContext()->CloseAllContexts();
  
  if (HasArg)
  {
    if (!GetMapOfAIS2D().IsBound2(argv[2]))
    {
      cout << "v2dplane: error 1st name doesn't exist in the GetMapOfAIS2D()." << endl;
      return 1;
    }
    Handle(AIS2D_InteractiveObject) theShapeA = GetMapOfAIS2D().Find2(argv[2]);
    
    if (theShapeA->Type() == AIS2D_KOI_Datum && theShapeA->Signature() == 1)
    {
      if (argc < 5 || !GetMapOfAIS2D().IsBound2(argv[3]))
      {
        cout << "v2dplane: error 2de name doesn't exist in the GetMapOfAIS2D()." << endl;
        return 1;
      }
      Handle(AIS2D_InteractiveObject) theShapeB = GetMapOfAIS2D().Find2(argv[3]);
      if (!(theShapeB->Type() == AIS2D_KOI_Datum && theShapeB->Signature() == 1))
      {
        cout << "v2dplane: error 2de object is expected to be an AIS2D_Point. " << endl;
        return 1;
      }
      if (!GetMapOfAIS2D().IsBound2(argv[4]))
      {
        cout << "v2dplane: error 3de name doesn't exist in the GetMapOfAIS2D()." << endl;
        return 1;
      }
      Handle(AIS2D_InteractiveObject) theShapeC = GetMapOfAIS2D().Find2(argv[4]);
      if (!(theShapeC->Type() == AIS2D_KOI_Datum && theShapeC->Signature() == 1))
      {
        cout << "v2dplane: error 3de object is expected to be an AIS2D_Point. " << endl;
        return 1;
      }

      Handle(AIS2D_Point) theAIS2DPointA = *(Handle(AIS2D_Point)*)& theShapeA;
      Handle(AIS2D_Point) theAIS2DPointB = *(Handle(AIS2D_Point)*)& theShapeB;
      Handle(AIS2D_Point) theAIS2DPointC = *(Handle(AIS2D_Point)*)& theShapeC;
      
      Handle(Geom_Point) myGeomPointA = theAIS2DPointA->Component();
      Handle(Geom_CartesianPoint) myCartPointA = *((Handle(Geom_CartesianPoint)*)& myGeomPointA);

      Handle(Geom_Point) myGeomPointB = theAIS2DPointB->Component();
      Handle(Geom_CartesianPoint) myCartPointB= *((Handle(Geom_CartesianPoint)*)& theAIS2DPointB);

      Handle(Geom_Point) myGeomPointBC = theAIS2DPointC->Component();
      Handle(Geom_CartesianPoint) myCartPointC = *((Handle(Geom_CartesianPoint)*)& theAIS2DPointC);

      if (myCartPointB->X() == myCartPointA->X() &&
          myCartPointB->Y() == myCartPointA->Y() && myCartPointB->Z() == myCartPointA->Z())
      {
	// B=A
	cout << "v2dplane error: same points" << endl;
        return 1;
      }
      if (myCartPointC->X() == myCartPointA->X() &&
          myCartPointC->Y() == myCartPointA->Y() && myCartPointC->Z() == myCartPointA->Z())
      {
	// C=A
	cout << "v2dplane error: same points" << endl;
        return 1;
      }
      if (myCartPointC->X() == myCartPointB->X() &&
          myCartPointC->Y() == myCartPointB->Y() && myCartPointC->Z() == myCartPointB->Z())
      {
	// C=B
	cout << "v2dplane error: same points" << endl;
        return 1;
      }
      
      gp_Pnt A = myCartPointA->Pnt();
      gp_Pnt B = myCartPointB->Pnt();
      gp_Pnt C = myCartPointC->Pnt();
      
      // Construction de l'AIS2D_Plane
      GC_MakePlane MkPlane (A,B,C);
      Handle(Geom_Plane) myGeomPlane = MkPlane.Value();
      Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(myGeomPlane);
      GetMapOfAIS2D().Bind(myAIS2DPlane, name);
      TheAIS2DContext()->Display(myAIS2DPlane);
    }

    else if (theShapeA->Type() == AIS2D_KOI_Datum && theShapeA->Signature() == 2)
    {
      if (argc != 4 || !GetMapOfAIS2D().IsBound2(argv[3]))
      {
        cout << "v2dplane: error 2de name doesn't exist in the GetMapOfAIS2D()." << endl;
        return 1;
      }
      Handle(AIS2D_InteractiveObject) theShapeB = GetMapOfAIS2D().Find2(argv[3]);
      if (!(theShapeB->Type() == AIS2D_KOI_Datum && theShapeB->Signature() == 1))
      {
        cout << "v2dplane: error 2de object is expected to be an AIS2D_Point." << endl;
        return 1;
      }

      Handle(AIS2D_Axis) theAIS2DAxisA = *(Handle(AIS2D_Axis)*)& theShapeA;
      Handle(AIS2D_Point) theAIS2DPointB = *(Handle(AIS2D_Point)*)& theShapeB;
      
      Handle(Geom_Line) myGeomLineA = theAIS2DAxisA->Component();
      Handle(Geom_Point) myGeomPointB = theAIS2DPointB->Component();
      
      gp_Ax1 myAxis = myGeomLineA->Position();
      Handle(Geom_CartesianPoint) myCartPointB = *(Handle(Geom_CartesianPoint)*)& myGeomPointB;
      
      gp_Dir D = myAxis.Direction();
      gp_Pnt B = myCartPointB->Pnt();
      
      // Construction de l'AIS2D_Plane
      Handle(Geom_Plane) myGeomPlane = new Geom_Plane(B,D);
      Handle(AIS2D_Plane)  myAIS2DPlane = new AIS2D_Plane(myGeomPlane,B);
      GetMapOfAIS2D().Bind(myAIS2DPlane, name);
      TheAIS2DContext()->Display(myAIS2DPlane);
    }
    else if (theShapeA->Type() == AIS2D_KOI_Datum && theShapeA->Signature() == 7)
    {
      if (argc != 4 || !GetMapOfAIS2D().IsBound2(argv[3]))
      {
        cout << "vplane: error 2de name doesn't exist in the GetMapOfAIS2D()." << endl;
        return 1;
      }
      Handle(AIS2D_InteractiveObject) theShapeB = GetMapOfAIS2D().Find2(argv[3]);
      if (!(theShapeB->Type() == AIS2D_KOI_Datum && theShapeB->Signature() == 1))
      {
        cout << "vplane: error 2de object is expected to be an AIS2D_Point." << endl;
        return 1;
      }
      
      Handle(AIS2D_Plane) theAIS2DPlaneA = *(Handle(AIS2D_Plane)*)& theShapeA;
      Handle(AIS2D_Point) theAIS2DPointB = *(Handle(AIS2D_Point)*)& theShapeB;
      
      Handle(Geom_Plane) theNewGeomPlane = theAIS2DPlaneA->Component();
      Handle(Geom_Point) myGeomPointB = theAIS2DPointB->Component();
      
      Handle(Geom_CartesianPoint) myCartPointB = *(Handle(Geom_CartesianPoint)*)& myGeomPointB;
      gp_Pnt B = myCartPointB->Pnt();
      
      // Construction de l'AIS2D_Plane
      Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theNewGeomPlane,B);
      GetMapOfAIS2D().Bind(myAIS2DPlane, name);
      TheAIS2DContext()->Display(myAIS2DPlane);
    }
    else
    {
      cout << "v2dplane: error 1st object is not an AIS2D." << endl;
      return 1;
    }
  }
  else
  {
    if (!strcasecmp(argv[0] ,"v2dplane"))
    {
      TheAIS2DContext()->OpenLocalContext();
      myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
      
      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(1));
      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(2));
      cout << "Select a vertex, a face or an edge." << endl;
      ShapeA = SelectShape2d(5); // User-defined mode
      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(4));

      if (ShapeA.ShapeType() == TopAbs_VERTEX)
      {
	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(4));
	cout << " Select an edge or a different vertex." << endl;
        TopoDS_Shape ShapeB = SelectShape2d(5);

	if (ShapeB.ShapeType() == TopAbs_VERTEX)
        {
	  if (ShapeB.IsSame(ShapeA))
          {
            cout << " v2dplane: error, same points selected" << endl;
            return 1;
          }
	  TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(2));
	  cout<<" Select a different vertex."<<endl;
          TopoDS_Shape ShapeC = SelectShape2d(5);

	  if (ShapeC.IsSame(ShapeA) || ShapeC.IsSame(ShapeB))
          {
            cout << " v2dplane: error, same points selected" << endl;
            return 1;
          }
	  
	  TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	  
	  // Construction du plane 
	  gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(ShapeA));
	  gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
	  gp_Pnt C = BRep_Tool::Pnt(TopoDS::Vertex(ShapeC));
	  GC_MakePlane MkPlane (A,B,C);
	  Handle(Geom_Plane) theGeomPlane = MkPlane.Value();
	  Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane);
	  GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	  TheAIS2DContext()->Display(myAIS2DPlane);
	}
	else // ShapeB est un edge
        {
	  TopoDS_Edge EdgeB = TopoDS::Edge(ShapeB);
	  TopoDS_Vertex VertA = TopoDS::Vertex(ShapeA);
	  
	  BRepExtrema_ExtPC OrthoProj (VertA, EdgeB);
	  if (OrthoProj.Value(1) < 0.001)
          {
	    cout << " v2dplane: error point is on the edge." << endl;
            return 1;
	  }
	  else
          {
	    TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	    // Construction du plane 
	    gp_Pnt A = BRep_Tool::Pnt(VertA);
	    TopoDS_Vertex VBa, VBb;
	    TopExp::Vertices(EdgeB, VBa, VBb);
	    gp_Pnt Ba = BRep_Tool::Pnt(VBa);
	    gp_Pnt Bb = BRep_Tool::Pnt(VBb);
	    GC_MakePlane MkPlane (A,Ba,Bb);
	    Handle(Geom_Plane) theGeomPlane = MkPlane.Value();
	    Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane);
	    GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	    TheAIS2DContext()->Display(myAIS2DPlane);
	  }
	}
      }
      else if (ShapeA.ShapeType() == TopAbs_EDGE)
      {
	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(4));
	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(2));
	cout << " Select a vertex that don't belong to the edge." << endl;
        TopoDS_Shape ShapeB = SelectShape2d(5);

	TopoDS_Edge EdgeA = TopoDS::Edge(ShapeA);
	TopoDS_Vertex VertB = TopoDS::Vertex(ShapeB);
	  
	BRepExtrema_ExtPC OrthoProj (VertB,EdgeA);
	if (OrthoProj.Value(1) < 0.001)
        {
	  cout << " v2dplane: error point is on the edge." << endl;
          return 1;
	}
	else
        {
	  TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	  // Construction du plane 
	  gp_Pnt B = BRep_Tool::Pnt(VertB);
	  TopoDS_Vertex VAa, VAb;
	  TopExp::Vertices(EdgeA, VAa, VAb);
	  gp_Pnt Aa = BRep_Tool::Pnt(VAa);
	  gp_Pnt Ab = BRep_Tool::Pnt(VAb);
	  GC_MakePlane MkPlane (B,Aa,Ab);
	  Handle(Geom_Plane) theGeomPlane = MkPlane.Value();
	  Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane);
	  GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	  TheAIS2DContext()->Display(myAIS2DPlane);
	}
      }
      else // ShapeA est une Face
      {
	TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	// Construction du plane 
	TopoDS_Face myFace = TopoDS::Face(ShapeA);
	BRepAdaptor_Surface mySurface (myFace, Standard_False);
	if (mySurface.GetType() == GeomAbs_Plane)
        {
	  gp_Pln myPlane = mySurface.Plane();
	  Handle(Geom_Plane) theGeomPlane = new Geom_Plane(myPlane);
	  Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane);
	  GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	  TheAIS2DContext()->Display(myAIS2DPlane);
	}
	else
        {
	  cout << " v2dplane: error" << endl;
          return 1;
	}
      }
    }
    else if (!strcasecmp(argv[0], "v2dplanepara"))
    {
      TheAIS2DContext()->OpenLocalContext();
      myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
      
      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(1));
      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(4));
      cout << " Select a vertex or a face." << endl;
      TopoDS_Shape ShapeA = SelectShape2d(5);

      if (ShapeA.ShapeType() == TopAbs_VERTEX)
      {
	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(1));
	cout<<" Select a face."<<endl;
        TopoDS_Shape ShapeB = SelectShape2d(5);
	
	TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	
	// Construction du plane 
	gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(ShapeA));
	
	TopoDS_Face myFace = TopoDS::Face(ShapeB);
	BRepAdaptor_Surface mySurface (myFace, Standard_False);
	if (mySurface.GetType() == GeomAbs_Plane)
        {
	  gp_Pln myPlane = mySurface.Plane();
	  myPlane.SetLocation(A);
	  Handle(Geom_Plane) theGeomPlane = new Geom_Plane(myPlane);
	  Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane, A);
	  GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	  TheAIS2DContext()->Display(myAIS2DPlane);
	}
	else
        {
	  cout << " vplane: error" << endl;
          return 1;
	}
      }
      else
      {
	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(4));
	cout<<" Select a vertex."<<endl;
        TopoDS_Shape ShapeB = SelectShape2d(5);

	TheAIS2DContext()->CloseLocalContext(myCurrentIndex);

	// Construction du plane 
	gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB ) );

	TopoDS_Face myFace = TopoDS::Face(ShapeA);
	BRepAdaptor_Surface mySurface (myFace, Standard_False);
	if (mySurface.GetType() == GeomAbs_Plane)
        {
	  gp_Pln myPlane = mySurface.Plane();
	  myPlane.SetLocation(B);
	  Handle(Geom_Plane) theGeomPlane = new Geom_Plane(myPlane);
	  Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane, B);
	  GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	  TheAIS2DContext()->Display(myAIS2DPlane);
	}
	else
        {
	  cout << " v2dplane: error" << endl;
          return 1;
	}
      }
    }
    else // Fonction v2dplaneortho
    {
      TheAIS2DContext()->OpenLocalContext();
      myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
      
      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(2));
      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(4));
      cout << " Select a face and an edge coplanar." << endl;
      TopoDS_Shape ShapeA = SelectShape2d(5);
      
      if (ShapeA.ShapeType() == TopAbs_EDGE)
      {
	// ShapeA est un edge, on desactive le mode edge...
	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(2));
	cout<<" Select a face."<<endl;
        TopoDS_Shape ShapeB = SelectShape2d(5);
	
	TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	
	// Construction du plane 
	TopoDS_Edge EdgeA = TopoDS::Edge(ShapeA);
	TopoDS_Vertex VAa, VAb;

	TopExp::Vertices(EdgeA, VAa, VAb);
	gp_Pnt Aa = BRep_Tool::Pnt(VAa);
	gp_Pnt Ab = BRep_Tool::Pnt(VAb);
	gp_Vec ab (Aa,Ab);
	
	gp_Dir Dab (ab);
	// Creation de mon axe de rotation 
	gp_Ax1 myRotAxis (Aa,Dab); 
	
	TopoDS_Face myFace = TopoDS::Face(ShapeB);
	BRepExtrema_ExtPF myHauteurA (VAa, myFace);
	BRepExtrema_ExtPF myHauteurB (VAb, myFace);
	if (fabs(myHauteurA.Value(1) - myHauteurB.Value(1) ) > 0.1)
        {
	   cout << " v2dplaneOrtho error: l'edge n'est pas parallele a la face." << endl;
           return 1;
	}
	BRepAdaptor_Surface mySurface (myFace, Standard_False);
	if (mySurface.GetType() == GeomAbs_Plane)
        {
	  gp_Pln myPlane = mySurface.Plane();
	  myPlane.Rotate(myRotAxis, M_PI/2);
	  
	  Handle(Geom_Plane) theGeomPlane = new Geom_Plane(myPlane);
	  gp_Pnt theMiddle ((Aa.X()+Ab.X())/2, (Aa.Y()+Ab.Y())/2, (Aa.Z()+Ab.Z())/2);
	  Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane, theMiddle);
	  GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	  TheAIS2DContext()->Display(myAIS2DPlane);
	  
	}
	else
        {
	  cout << " v2dplaneOrtho: error" << endl;
          return 1;
	}
      }
      else
      {
	TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(4));
	cout << " Select an edge." << endl;
        TopoDS_Shape ShapeB = SelectShape2d(5);
	
	// Fermeture du context local
	TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	
	// Construction du plane 
	TopoDS_Edge EdgeB = TopoDS::Edge(ShapeB);
	TopoDS_Vertex VBa, VBb;
	TopExp::Vertices(EdgeB, VBa, VBb);
	gp_Pnt Ba = BRep_Tool::Pnt(VBa);
	gp_Pnt Bb = BRep_Tool::Pnt(VBb);
	gp_Vec ab  (Ba,Bb);
	gp_Dir Dab (ab);
	// Creation de mon axe de rotation 
	gp_Ax1 myRotAxis (Ba,Dab); 
	
	TopoDS_Face myFace = TopoDS::Face(ShapeA);
	BRepExtrema_ExtPF myHauteurA (VBa, myFace);
	BRepExtrema_ExtPF myHauteurB (VBb, myFace);

	if (fabs(myHauteurA.Value(1) - myHauteurB.Value(1) ) > 0.1)
        {
          cout << " v2dplaneOrtho error: l'edge n'est pas parallele a la face." << endl;
          return 1;
	}
	BRepAdaptor_Surface mySurface (myFace, Standard_False);
	if (mySurface.GetType() == GeomAbs_Plane)
        {
	  gp_Pln myPlane = mySurface.Plane();
	  myPlane.Rotate(myRotAxis, M_PI/2);
	  Handle(Geom_Plane) theGeomPlane = new Geom_Plane(myPlane);
	  gp_Pnt theMiddle ((Ba.X()+Bb.X())/2, (Ba.Y()+Bb.Y())/2, (Ba.Z()+Bb.Z())/2);
	  Handle(AIS2D_Plane) myAIS2DPlane = new AIS2D_Plane(theGeomPlane, theMiddle);
	  GetMapOfAIS2D().Bind(myAIS2DPlane, name);
	  TheAIS2DContext()->Display(myAIS2DPlane);
	}
	else
        {
	  cout << " v2dplaneOrtho: error" << endl;
          return 1;
	}
      }
    }
  }

  return 0;
}*/

//==============================================================================
// Fonction  vline
// ---------------  Uniquement par parametre. Pas de selection dans le viewer.
//==============================================================================

//==============================================================================
//function : V2dLineBuilder
//purpose  : Build a Graphic2d_Line
//Draw arg : v2dline LineName [AIS2D_PointName] [AIS2D_PointName]
//                            [Xa] [Ya] [Za]   [Xb] [Yb] [Zb]
//==============================================================================
/*static int V2dLineBuilder(Draw_Interpretor& , Standard_Integer argc, char** argv) 
{
  Standard_Integer myCurrentIndex;
  if (argc != 4 && argc != 8 && argc != 2)
  {
    cout << "vline error: number of arguments not correct " << endl;
    return 1;
  }
  TheAIS2DContext()->CloseAllContext();
  
  // On recupere les parametres
  Handle(AIS2D_InteractiveObject) theShapeA;
  Handle(AIS2D_InteractiveObject) theShapeB;
  
  // Parametres: AIS2D_Point AIS2D_Point
  if (argc == 4)
  {
    theShapeA = GetMapOfAIS2D().Find2(argv[2] );
    if (theShapeA->Type() == AIS2D_KOI_Datum && theShapeA->Signature() == 1)
    {
      theShapeB = GetMapOfAIS2D().Find2(argv[3]);
      if (!(theShapeB->Type() == AIS2D_KOI_Datum && theShapeB->Signature() == 1))
      {
	cout << "v2dline error: wrong type of 2de argument." << endl;
        return 1;
      }
    }
    else
    {
      cout << "v2dline error: wrong type of 1st argument." << endl;
      return 1;
    }
 
    Handle(AIS2D_Point) theAIS2DPointA = *(Handle(AIS2D_Point)*)& theShapeA;
    Handle(AIS2D_Point) theAIS2DPointB = *(Handle(AIS2D_Point)*)& theShapeB;
    
    Handle(Geom_Point) myGeomPointBA = theAIS2DPointA->Component();
    Handle(Geom_CartesianPoint) myCartPointA = *((Handle(Geom_CartesianPoint)*)& myGeomPointBA);

    Handle(Geom_Point) myGeomPointB = theAIS2DPointB->Component();
    Handle(Geom_CartesianPoint) myCartPointB = *((Handle(Geom_CartesianPoint)*)& myGeomPointB);
    
    if (myCartPointB->X() == myCartPointA->X() &&
        myCartPointB->Y() == myCartPointA->Y() && myCartPointB->Z() == myCartPointA->Z())
    {
      // B=A
      cout << "v2dline error: same points" << endl;
      return 1;
    }
    Handle(AIS2D_Line) theAIS2DLine = new AIS2D_Line(myCartPointA, myCartPointB);
    GetMapOfAIS2D().Bind(theAIS2DLine, argv[1]);
    TheAIS2DContext()->Display(theAIS2DLine);
  }
  else if (argc == 8) // Parametres 6 Reals
  {
    Standard_Real coord[6];
    for (Standard_Integer i = 0; i <= 2; i++)
    {
      coord[i] = atof(argv[2+i]);
      coord[i+3] = atof(argv[5+i]);
    }
    
    Handle(Geom_CartesianPoint) myCartPointA = new Geom_CartesianPoint(coord[0],coord[1],coord[2]);
    Handle(Geom_CartesianPoint) myCartPointB = new Geom_CartesianPoint(coord[3],coord[4],coord[5]);
    
    Handle(AIS2D_Line) theAIS2DLine = new AIS2D_Line(myCartPointA, myCartPointB);
    GetMapOfAIS2D().Bind(theAIS2DLine, argv[1]);
    TheAIS2DContext()->Display(theAIS2DLine);
  }
  else
  {
    TheAIS2DContext()->OpenLocalContext();
    myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
    
    // Active le mode Vertex.
    TopoDS_Shape ShapeA = SelectShape2d(1);
    
    if (ShapeA.ShapeType() == TopAbs_VERTEX)
    {
//      TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(1));
      cout << " Select a different vertex." << endl;
      
      TopoDS_Shape ShapeB;
      do
      {
        ShapeB = SelectShape2d(5);
      } while (ShapeB.IsSame(ShapeA));
      
      TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	
      // Construction de la line
      gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(ShapeA));
      gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
      
      Handle(Geom_CartesianPoint) myCartPointA = new Geom_CartesianPoint(A);
      Handle(Geom_CartesianPoint) myCartPointB = new Geom_CartesianPoint(B);
      
      Handle(AIS2D_Line) theAIS2DLine = new AIS2D_Line(myCartPointA, myCartPointB);
      GetMapOfAIS2D().Bind(theAIS2DLine, argv[1]);
      TheAIS2DContext()->Display(theAIS2DLine);
    }
    else
    {
      cout << "v2dline error." << endl;
    }
  }
  
  return 0;
}*/

//==============================================================================
//function : V2dCircleBuilder
//purpose  : Build an Graphic2d_Circle 
//Draw arg : v2dcircle CircleName X Y Radius [Color_index]
//==============================================================================
static int V2dCircleBuilder (Draw_Interpretor& , Standard_Integer argc, char** argv) 
{
  Standard_Integer myCurrentIndex;
  // verification of the arguments
  if (argc < 2)
  {
    cout << "Usage: v2dcircle CircleName X Y Radius [Color_index]" << endl;
    return 1;
  }
  TheAIS2DContext()->CloseAllContext();
  Handle(V2d_View) V = QAViewer2dTest::CurrentView();
  
  TCollection_AsciiString name = argv[1];

  if (argc >= 5)
  {
//    Handle(AIS2D_InteractiveObject) theShapeA;
//    Handle(AIS2D_InteractiveObject) theShapeB;
//    theShapeA=GetMapOfAIS2D().Find2(argv[2]);
//    theShapeB=GetMapOfAIS2D().Find2(argv[3]);
    
    // Arguments: AIS2D_Point AIS2D_Point AIS2D_Point
    // ========================================
/*    if (theShapeA->Type() == AIS2D_KOI_Datum && theShapeA->Signature() == 1)
    {
      if (theShapeB->Type() != AIS2D_KOI_Datum || theShapeB->Signature() != 1)
      {
	cout<<"vcircle error: 2de argument is unexpected to be a point."<<endl;return 1; 
      }
      Handle(AIS2D_InteractiveObject) theShapeC=GetMapOfAIS2D().Find2(argv[4]);
      if (theShapeC->Type()!=AIS2D_KOI_Datum || theShapeC->Signature()!=1 ) {
	cout<<"vcircle error: 3de argument is unexpected to be a point."<<endl;return 1; 
      }
      // tag
      Handle(AIS2D_Point) theAIS2DPointA = *(Handle(AIS2D_Point)*)& theShapeA;
      Handle(AIS2D_Point) theAIS2DPointB = *(Handle(AIS2D_Point)*)& theShapeB;
      Handle(AIS2D_Point) theAIS2DPointC = *(Handle(AIS2D_Point)*)& theShapeC;
      
      Handle(Geom_Point) myGeomPointA = theAIS2DPointA->Component();
      Handle(Geom_CartesianPoint) myCartPointA = *((Handle(Geom_CartesianPoint)*)& myGeomPointA);

      Handle(Geom_Point ) myGeomPointB =  theAIS2DPointB->Component();
      Handle(Geom_CartesianPoint ) myCartPointB= *((Handle(Geom_CartesianPoint)*)&  theAIS2DPointB);

      Handle(Geom_Point ) myGeomPointBC=  theAIS2DPointC->Component();
      Handle(Geom_CartesianPoint ) myCartPointC= *((Handle(Geom_CartesianPoint)*)&  theAIS2DPointC);
    
      // Test A=B
      if (myCartPointA->X()==myCartPointB->X() && myCartPointA->Y()==myCartPointB->Y() && myCartPointA->Z()==myCartPointB->Z()  ) {
	cout<<"vcircle error: Same points."<<endl;return 1; 
      }
      // Test A=C
      if (myCartPointA->X()==myCartPointC->X() && myCartPointA->Y()==myCartPointC->Y() && myCartPointA->Z()==myCartPointC->Z()  ) {
	cout<<"vcircle error: Same points."<<endl;return 1; 
      }
      // Test B=C
      if (myCartPointB->X()==myCartPointC->X() && myCartPointB->Y()==myCartPointC->Y() && myCartPointB->Z()==myCartPointC->Z()  ) {
	cout<<"vcircle error: Same points."<<endl;return 1; 
      }
      // Construction du cercle
      GC_MakeCircle Cir=GC_MakeCircle (myCartPointA->Pnt(),myCartPointB->Pnt(),myCartPointC->Pnt() );
      Handle (Geom_Circle) theGeomCircle=Cir.Value();
      Handle(AIS2D_Circle) theAIS2DCircle=new AIS2D_Circle(theGeomCircle );
      GetMapOfAIS2D().Bind(theAIS2DCircle,argv[1] );
      TheAIS2DContext()->Display(theAIS2DCircle );
    
    }
    // Arguments: ASI_Plane AIS2D_Point Real
    // ===================================
    else if (theShapeA->Type()==AIS2D_KOI_Datum && theShapeA->Signature()==7 ) {
      if (theShapeB->Type()!=AIS2D_KOI_Datum || theShapeB->Signature()!=1 ) {
	cout<<"vcircle error: 2de element is a unexpected to be a point."<<endl;return 1; 
      }
      // On verifie que le rayon est bien >=0
      if (atof(argv[4])<=0 ) {cout<<"vcircle error: the radius must be >=0."<<endl;return 1;  }
      
      // On recupere la normale au Plane.
      Handle(AIS2D_Plane) theAIS2DPlane= *(Handle(AIS2D_Plane)*)& theShapeA;
      Handle(AIS2D_Point) theAIS2DPointB= *(Handle(AIS2D_Point)*)& theShapeB;
      

//      Handle(Geom_Plane ) myGeomPlane= *(Handle(Geom_Plane)*)& (theAIS2DPlane->Component() );
      Handle(Geom_Plane ) myGeomPlane= theAIS2DPlane->Component();
       Handle(Geom_Point ) myGeomPointB =  theAIS2DPointB->Component();
      Handle(Geom_CartesianPoint ) myCartPointB= *((Handle(Geom_CartesianPoint)*)&  theAIS2DPointB);
//      Handle(Geom_CartesianPoint ) myCartPointB= *(Handle(Geom_CartesianPoint)*)& (theAIS2DPointB->Component() );
      
      //Handle(Geom_Axis2Placement) OrigineAndAxii=myGeomPlane->Axis();
      gp_Pln mygpPlane = myGeomPlane->Pln();
      gp_Ax1 thegpAxe = mygpPlane.Axis();
      gp_Dir theDir = thegpAxe.Direction();
      gp_Pnt theCenter=myCartPointB->Pnt();
      Standard_Real TheR = atof(argv[4]);
      GC_MakeCircle Cir=GC_MakeCircle (theCenter, theDir ,TheR);
      Handle (Geom_Circle) theGeomCircle=Cir.Value();
      Handle(AIS2D_Circle) theAIS2DCircle=new AIS2D_Circle(theGeomCircle );
      GetMapOfAIS2D().Bind(theAIS2DCircle,argv[1] );
      TheAIS2DContext()->Display(theAIS2DCircle );
    }
    else // Error
    {
      cout << "v2dcircle error: !st argument is a unexpected type." << endl;
      return 1;
    }
*/    
    //Draw arg : v2dcircle CircleName X Y Radius [Color_index]
    if (GetMapOfAIS2D().IsBound2(name))
    {
      cout << "There is already an object with name " << name << endl;
      return -1;
    }
    Handle(AIS2D_InteractiveObject) aisobj = new AIS2D_InteractiveObject();
    aisobj->SetView(V->View());
//    Handle(Graphic2d_GraphicObject) obj =
//      new Graphic2d_GraphicObject(V->View());

    Handle(Graphic2d_Circle) circle = new Graphic2d_Circle
      (aisobj, atof(argv[2]), atof(argv[3]), atof(argv[4]));

    if (argc > 5)
    {
      circle->SetTypeOfPolygonFilling(Graphic2d_TOPF_FILLED);
      circle->SetInteriorColorIndex(atoi(argv[5]));
    }

//    obj->Display();
      GetMapOfAIS2D().Bind(aisobj, name);
//      TheAIS2DContext()->Display(aisobj, Standard_False);
    aisobj->Display();
    
    V->Viewer()->Update();
  }
  else
  {
    TheAIS2DContext()->OpenLocalContext();
    myCurrentIndex = TheAIS2DContext()->IndexOfCurrentLocal();
    
    // Active le mode Vertex et face.
//    TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(1));
//    TheAIS2DContext()->ActivateStandardMode(AIS2D_ProjShape::SelectionType(4));
    cout << " Select a vertex or a face." << endl;
    TopoDS_Shape ShapeA = SelectShape2d(5);
    
    if (ShapeA.ShapeType() == TopAbs_VERTEX)
    {
//      TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(4));
      cout << " Select a different vertex." << endl;
      
      TopoDS_Shape ShapeB;
      do
      {
        ShapeB = SelectShape2d(5);
      } while (ShapeB.IsSame(ShapeA));
      
      // Selection de ShapeC
      cout << " Select the last vertex." << endl;
      TopoDS_Shape ShapeC;
      do
      {
        ShapeC = SelectShape2d(5);
      } while (ShapeC.IsSame(ShapeA) || ShapeC.IsSame(ShapeB));
      
      TheAIS2DContext()->CloseLocalContext(myCurrentIndex);
	
      // Construction du cercle 
/*
      gp_Pnt A = BRep_Tool::Pnt(TopoDS::Vertex(ShapeA));
      gp_Pnt B = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
      gp_Pnt C = BRep_Tool::Pnt(TopoDS::Vertex(ShapeC));

      GC_MakeCircle Cir = GC_MakeCircle(A,B,C);
      Handle(Geom_Circle)  theGeomCircle  = Cir.Value();
      Handle(AIS2D_Circle) theAIS2DCircle = new AIS2D_Circle(theGeomCircle);
      GetMapOfAIS2D().Bind(theAIS2DCircle,argv[1]);
      TheAIS2DContext()->Display(theAIS2DCircle);*/
    }
    // ShapeA est une face.
    else
    {
      cout << " Select a vertex (in your face)." << endl;
//      TheAIS2DContext()->DeactivateStandardMode(AIS2D_ProjShape::SelectionType(4));
      TopoDS_Shape ShapeB = SelectShape2d(5);

      Standard_Integer theRad;
      do
      {
	cout << " Enter the value of the radius:" << endl;
	cin >> theRad;
      } while (theRad <= 0);

      TheAIS2DContext()->CloseLocalContext(myCurrentIndex);

      // Construction du cercle.

      // On recupere la normale au Plane. tag
/*      TopoDS_Face myFace = TopoDS::Face(ShapeA);
      BRepAdaptor_Surface mySurface (myFace, Standard_False);
      gp_Pln myPlane = mySurface.Plane();
      Handle(Geom_Plane) theGeomPlane = new Geom_Plane (myPlane );
      gp_Pln mygpPlane = theGeomPlane->Pln();
      gp_Ax1 thegpAxe = mygpPlane.Axis();
      gp_Dir theDir = thegpAxe.Direction();
      
      // On recupere le centre.
      gp_Pnt theCenter = BRep_Tool::Pnt(TopoDS::Vertex(ShapeB));
      
      // On construit l'AIS2D_Circle
      GC_MakeCircle Cir = GC_MakeCircle(theCenter, theDir ,theRad);
      Handle(Geom_Circle)  theGeomCircle  = Cir.Value();
      Handle(AIS2D_Circle) theAIS2DCircle = new AIS2D_Circle(theGeomCircle);
      GetMapOfAIS2D().Bind(theAIS2DCircle,argv[1] ;
      TheAIS2DContext()->Display(theAIS2DCircle);*/
    }
  }
  
  return 0;
}

//=======================================================================
//function : ObjectCommands
//purpose  : 
//=======================================================================
void QAViewer2dTest::ObjectCommands (Draw_Interpretor& theCommands)
{
  char *group = "2D AIS Viewer - Primitives Creation";

//  theCommands.Add("v2dtrihedron",
//		  "v2dtrihedron name [Xo] [Yo] [Zo] [Zu] [Zv] [Zw] [Xu] [Xv] [Xw]",
//		  __FILE__,V2dTrihedron,group);

/*  theCommands.Add("v2dtri2d", 
		  "v2dtri2d Name (Selection in the viewer only)",
		  __FILE__, V2dTrihedron2D, group);

//  theCommands.Add("v2dplanetri",  
//		  "v2dplanetri Name (Selection in the viewer only)",
//		  __FILE__, V2dPlaneTrihedron, group);

  theCommands.Add("v2dsize",
		  "v2dsize [name(Default=Current)] [size(Default=100)] ",
		  __FILE__, V2dSize, group);
*/
  theCommands.Add("v2daxis",
		  "v2daxis nom [Xa] [Ya] [Xb] [Yb]",
		  __FILE__, V2dAxisBuilder, group);
  
/*  theCommands.Add("v2daxispara", 
		  "v2daxispara nom ",
		  __FILE__, V2dAxisBuilder, group);
  
  theCommands.Add("v2daxisortho",
		  "v2daxisotho nom ",
		  __FILE__, V2dAxisBuilder, group);
  
  theCommands.Add("v2dpoint",
		  "v2dpoint PointName [Xa] [Ya]",
		  __FILE__, V2dPointBuilder, group);
*/
/*  theCommands.Add("v2dplane",
		  "v2dplane  PlaneName [AxisName/PlaneName/PointName] [PointName/PointName/PointName] [Nothing/Nothing/PointName] ",
		  __FILE__,V2dPlaneBuilder,group);
  
  theCommands.Add("v2dplanepara",
		  "v2dplanepara  PlaneName  ",
		  __FILE__,V2dPlaneBuilder,group);

  theCommands.Add("v2dplaneortho",
		  "v2dplaneortho  PlaneName  ",
		  __FILE__,V2dPlaneBuilder,group);
*/
//  theCommands.Add("v2dline", 
//		  "v2dline LineName [Xa/PointName] [Ya/PointName] [Xb] [Yb]",
//		  __FILE__,V2dLineBuilder,group);
  
  theCommands.Add("v2dcircle",
		  "v2dcircle CircleName X Y Radius [color_index]",
		  __FILE__, V2dCircleBuilder, group);
}
#endif
