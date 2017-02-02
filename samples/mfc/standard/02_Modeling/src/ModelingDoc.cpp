// ModelingDoc.cpp : implementation of the CModelingDoc class
//

#include "stdafx.h"

#include "ModelingDoc.h"

#include "ModelingApp.h"
#include "ResultDialog.h"
#include "State.h"

#include "ISession_Direction.h"
#include "..\res\resource.h"

#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <OCC_MainFrame.h>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom_Plane.hxx>
#include <BRepTools.hxx>

static Handle(AIS_Shape) AIS1;
static TopoDS_Face THE_F1, THE_F2;
static TopoDS_Edge THE_E1, THE_E2;

/////////////////////////////////////////////////////////////////////////////
// CModelingDoc

IMPLEMENT_DYNCREATE(CModelingDoc, CDocument)

BEGIN_MESSAGE_MAP(CModelingDoc, OCC_3dBaseDoc)
	//{{AFX_MSG_MAP(CModelingDoc)
	ON_COMMAND(ID_MIRROR, OnMirror)
	ON_COMMAND(ID_MIRRORAXIS, OnMirroraxis)
	ON_COMMAND(ID_ROTATE, OnRotate)
	ON_COMMAND(ID_SCALE, OnScale)
	ON_COMMAND(ID_TRANSLATION, OnTranslation)
	ON_COMMAND(ID_DISPLACEMENT, OnDisplacement)
	ON_COMMAND(ID_DEFORM, OnDeform)
	ON_COMMAND(ID_BOX, OnBox)
	ON_COMMAND(ID_Cylinder, OnCylinder)
	ON_COMMAND(ID_CONE, OnCone)
	ON_COMMAND(ID_SPHERE, OnSphere)
	ON_COMMAND(ID_TORUS, OnTorus)
	ON_COMMAND(ID_WEDGE, OnWedge)
	ON_COMMAND(ID_PRISM, OnPrism)
	ON_COMMAND(ID_REVOL, OnRevol)
	ON_COMMAND(ID_PIPE, OnPipe)
	ON_COMMAND(ID_THRU, OnThru)
	ON_COMMAND(ID_EVOLVED, OnEvolved)
	ON_COMMAND(ID_DRAFT, OnDraft)
	ON_COMMAND(ID_CUT, OnCut)
	ON_COMMAND(ID_FUSE, OnFuse)
	ON_COMMAND(ID_SECTION, OnSection)
	ON_COMMAND(ID_COMMON, OnCommon)
	ON_COMMAND(ID_PSECTION, OnPsection)
	ON_COMMAND(ID_BLEND, OnBlend)
	ON_COMMAND(ID_CHAMF, OnChamf)
	ON_COMMAND(ID_EVOLVEDBLEND, OnEvolvedblend)
	ON_COMMAND(ID_PRISM_LOCAL, OnPrismLocal)
	ON_COMMAND(ID_REVOL_LOCAL, OnRevolLocal)
	ON_COMMAND(ID_GLUE_LOCAL, OnGlueLocal)
 	ON_COMMAND(ID_DPRISM_LOCAL, OnDprismLocal)
 	ON_COMMAND(ID_Pipe_LOCAL, OnPipeLocal)
	ON_COMMAND(ID_LINEAR_LOCAL, OnLinearLocal)
	ON_COMMAND(ID_SPLIT_LOCAL, OnSplitLocal)
	ON_COMMAND(ID_THICK_LOCAL, OnThickLocal)
	ON_COMMAND(ID_OFFSET_LOCAL, OnOffsetLocal)
	ON_COMMAND(ID_VERTEX, OnVertex)
	ON_COMMAND(ID_EDGE, OnEdge)
	ON_COMMAND(ID_WIRE, OnWire)
	ON_COMMAND(ID_FACE, OnFace)
	ON_COMMAND(ID_SHELL, OnShell)
	ON_COMMAND(ID_COMPOUND, OnCompound)
	ON_COMMAND(ID_GEOMETRIE, OnGeometrie)
	ON_COMMAND(ID_SEWING, OnSewing)
	ON_COMMAND(ID_EXPLORER, OnExplorer)
	ON_COMMAND(ID_BUILDER, OnBuilder)
	ON_COMMAND(ID_VALID, OnValid)
	ON_COMMAND(ID_LINEAR, OnLinear)
	ON_COMMAND(ID_VOLUME, OnVolume)
	ON_COMMAND(ID_SURFACE, OnSurface)
	ON_COMMAND(ID_BUTTON_FILL, OnButtonFill)
	ON_COMMAND(ID_STOP_STOP, OnStopStop)
	ON_COMMAND(ID_FILLWITHTANG, OnFillwithtang)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelingDoc construction/destruction

CModelingDoc::CModelingDoc()
{
	myAISContext->SetDisplayMode(AIS_Shaded,Standard_False);
}

CModelingDoc::~CModelingDoc()
{
}
 
void CModelingDoc::OnMirror() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge (60.,100.,80.,20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Pnt PntCenterOfTheTransformation(110,60,60);
	Handle(AIS_Point) aispnt = new AIS_Point(new Geom_CartesianPoint(PntCenterOfTheTransformation));
	myAISContext->Display(aispnt,Standard_False);
	theTransformation.SetMirror(PntCenterOfTheTransformation);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Pnt PntCenterOfTheTransformation(110,60,60); \n\
theTransformation.SetMirror(PntCenterOfTheTransformation);\n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");
	PocessTextInDialog("Transform a Shape with Mirror and One point.", Message);
	
}

void CModelingDoc::OnMirroraxis() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Ax1 axe = gp_Ax1(gp_Pnt(110,60,60),gp_Dir(0.,1.,0.));
	Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
	myAISContext->Display(ax1,Standard_False);
	theTransformation.SetMirror(axe);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax1 Axis = gp_Ax1(gp_Pnt(110,60,60),gp_Dir(0.,1.,0.)); \n\
theTransformation.SetMirror(Axis);\n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Transform a Shape with Mirror and One axis.", Message);

}


void CModelingDoc::OnRotate() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Ax1 axe = gp_Ax1(gp_Pnt(200,60,60),gp_Dir(0.,1.,0.));
	Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
	myAISContext->Display(ax1,Standard_False);
	theTransformation.SetRotation(axe,30*M_PI/180);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax1 Axis = gp_Ax1(gp_Pnt(200,60,60),gp_Dir(0.,1.,0.)); \n\
theTransformation.SetRotation(Axis,30*PI/180); // Rotation of 30 degrees \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Transform a Shape with Rotation.", Message);
	
}

void CModelingDoc::OnScale() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Pnt theCenterOfScale(200,60,60);
	Handle(AIS_Point) aispnt = new AIS_Point(new Geom_CartesianPoint(theCenterOfScale));

	myAISContext->Display(aispnt,Standard_False);
	theTransformation.SetScale(theCenterOfScale,0.5);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Pnt theCenterOfScale(200,60,60); \n\
theTransformation.SetScale(theCenterOfScale,0.5); // Scale : value = 0.5 \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Scale a Shape with One point.", Message);
	

}



void CModelingDoc::OnTranslation() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	TopoDS_Shape S = BRepPrimAPI_MakeWedge(6.,10.,8.,2.).Shape(); 
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;
	gp_Vec theVectorOfTranslation(-6,-6,6);

	Handle(ISession_Direction) aDirection1 = new ISession_Direction(gp_Pnt(0,0,0),theVectorOfTranslation);
	myAISContext->Display(aDirection1,Standard_False);

	theTransformation.SetTranslation(theVectorOfTranslation);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);

	Fit();

    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(6.,10.,8.,2.); \n\
gp_Trsf theTransformation; \n\
gp_Vec theVectorOfTranslation(6,6,6); \n\
theTransformation.SetTranslation(theVectorOfTranslation); \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Translate a Shape with One vector.", Message);
	
}

void CModelingDoc::OnDisplacement() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	gp_Trsf theTransformation;

	gp_Ax3 ax3_1(gp_Pnt(0,0,0),gp_Dir(0,0,1));
	gp_Ax3 ax3_2(gp_Pnt(60,60,60),gp_Dir(1,1,1));

	theTransformation.SetDisplacement(ax3_1,ax3_2);
	BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
	TopoDS_Shape TransformedShape = myBRepTransformation.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(TransformedShape);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax3 ax3_1(gp_Pnt(0,0,0),gp_Dir(0,0,1)); \n\
gp_Ax3 ax3_2(gp_Pnt(60,60,60),gp_Dir(1,1,1)); \n\
theTransformation.SetDisplacement(ax3_1,ax3_2); \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n");

	PocessTextInDialog("Displace a Shape with Two coordinate systems.", Message);
	
	
}


void CModelingDoc::OnDeform() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);

	gp_GTrsf theTransformation;
	gp_Mat rot(1, 0, 0, 0, 0.5, 0, 0, 0, 1.5);
	theTransformation.SetVectorialPart(rot);
	theTransformation.SetTranslationPart(gp_XYZ(5,5,5));

	BRepBuilderAPI_GTransform myBRepTransformation(S,theTransformation);
	TopoDS_Shape S2 = myBRepTransformation.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_GTrsf theTransformation; \n\
gp_Mat rot(1, 0, 0, 0, 0.5, 0, 0, 0, 1.5); // scaling : 100% on X ; 50% on Y ; 150% on Z . \n\
theTransformation.SetVectorialPart(rot); \n\
theTransformation.SetTranslationPart(gp_XYZ(5,5,5)); \n\
BRepBuilderAPI_GTransform myBRepGTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepGTransformation.Shape();	\n");

	PocessTextInDialog("Deform a Shape with One matrix of deformation and One translation.", Message);
}

/* =================================================================================
   ====================   P R I M I T I V E S   ====================================
   ================================================================================= */

void CModelingDoc::OnBox() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape B1 = BRepPrimAPI_MakeBox(200., 150., 100.).Shape();
	Handle(AIS_Shape) aBox1 = new AIS_Shape(B1);
	myAISContext->SetMaterial(aBox1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aBox1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->Display(aBox1,Standard_False);
	TopoDS_Shape B2 = BRepPrimAPI_MakeBox (gp_Ax2(gp_Pnt(-200.,-80.,-70.),
                                         gp_Dir(1.,2.,1.)),
                                         80., 90., 120.).Shape();
	Handle(AIS_Shape) aBox2 = new AIS_Shape(B2);
	myAISContext->SetMaterial(aBox2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aBox2,Quantity_NOC_RED,Standard_False); 
	myAISContext->Display(aBox2,Standard_False);
	Fit();
    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape B1 = BRepPrimAPI_MakeBox (200.,150.,100.); \n\
TopoDS_Shape B2 = BRepPrimAPI_MakeBox (gp_Ax2(gp_Pnt(-200.,-80.,-70.), \n\
                                          gp_Dir(1.,2.,1.)), \n\
                                   80.,90.,120.); \n\
		\n");
	PocessTextInDialog("Make a topological box", Message);
}

void CModelingDoc::OnCylinder() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape C1 = BRepPrimAPI_MakeCylinder(50., 200.).Shape();
	Handle(AIS_Shape) aCyl1 = new AIS_Shape(C1);
	myAISContext->SetMaterial(aCyl1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aCyl1,Quantity_NOC_RED,Standard_False); 
	myAISContext->Display(aCyl1,Standard_False);
	TopoDS_Shape C2 = BRepPrimAPI_MakeCylinder (gp_Ax2(gp_Pnt(200.,200.,0.),
												                      gp_Dir(0.,0.,1.)),
                                              40., 110., 210.*M_PI / 180).Shape();
	Handle(AIS_Shape) aCyl2 = new AIS_Shape(C2);
	myAISContext->SetMaterial(aCyl2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(aCyl2,Quantity_NOC_MATRABLUE,Standard_False); 	
	myAISContext->Display(aCyl2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape C1 = BRepPrimAPI_MakeCylinder (50.,200.); \n\
TopoDS_Shape C2 = BRepPrimAPI_MakeCylinder (gp_Ax2(gp_Pnt(200.,200.,0.), \n\
                                        gp_Dir(0.,0.,1.)), \n\
                                        40.,110.,210.*PI180.); \n\
		\n");
	PocessTextInDialog("Make a cylinder", Message);
}


void CModelingDoc::OnCone() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape C1 = BRepPrimAPI_MakeCone(50., 25., 200.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(C1);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False); 		
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape C2 = BRepPrimAPI_MakeCone(gp_Ax2(gp_Pnt(100.,100.,0.),
												                 gp_Dir(0.,0.,1.)),
                                         60., 0., 150., 210.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(C2);
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape C1 = BRepPrimAPI_MakeCone (50.,25.,200.); \n\
TopoDS_Shape C2 = BRepPrimAPI_MakeCone(gp_Ax2(gp_Pnt(100.,100.,0.), \n\
                                          gp_Dir(0.,0.,1.)), \n\
                                   605.,0.,150.,210.*PI180); \n\
		\n");
	PocessTextInDialog("Make a cone", Message);
}

void CModelingDoc::OnSphere() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeSphere(gp_Pnt(-200., -250., 0.), 80.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais1,Standard_False);
  TopoDS_Shape S2 = BRepPrimAPI_MakeSphere(100., 120.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais2,Standard_False);
	TopoDS_Shape S3 = BRepPrimAPI_MakeSphere(gp_Pnt(200.,250.,0.),100.,
                                           -60.*M_PI / 180, 60.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais3 = new AIS_Shape(S3);
	myAISContext->SetColor(ais3,Quantity_NOC_RED,Standard_False); 
	myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais3,Standard_False);
	TopoDS_Shape S4 = BRepPrimAPI_MakeSphere(gp_Pnt(0.,0.,-300.),150.,
                                           -45.*M_PI / 180, 45.*M_PI / 180, 45.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais4 = new AIS_Shape(S4);
	myAISContext->SetColor(ais4,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais4,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeSphere(gp_Pnt(-200.,-250.,0.),80.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeSphere(100.,120.*PI180); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeSphere(gp_Pnt(200.,250.,0.),100., \n\
                                     -60.*PI180, 60.*PI180); \n\
TopoDS_Shape S4 = BRepPrimAPI_MakeSphere(gp_Pnt(0.,0.,-300.),150., \n\
                                     -45.*PI180, 45.*PI180, 45.*PI180); \n\
		\n");
	PocessTextInDialog("Make a sphere", Message);
}

void CModelingDoc::OnTorus() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeTorus(60., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(1.,1.,1.)),
                                          50., 20., 210.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais2,Standard_False);
	TopoDS_Shape S3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-200.,-150.,-100),gp_Dir(0.,1.,0.)),
                                          60., 20., -45.*M_PI / 180, 45.*M_PI / 180, 90.*M_PI / 180).Shape();
	Handle(AIS_Shape) ais3= new AIS_Shape(S3);
	myAISContext->SetColor(ais3,Quantity_NOC_CORAL,Standard_False); 
	myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais3,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeTorus(60.,20.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(1.,1.,1.)), \n\
                                    50.,20.,210.*PI180); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-200.,-150.,-100),gp_Dir(0.,1.,0.)), \n\
                                    60.,20.,-45.*PI180,45.*PI180,90.*PI180); \n\
		\n");
	PocessTextInDialog("Make a torus", Message);
}

void CModelingDoc::OnWedge() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakeWedge(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(0.,0.,1.)),
                                          60., 50., 80., 25., -10., 40., 70.).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_CORAL2,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais2,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeWedge(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(0.,0.,1.)), \n\
                                    60.,50.,80.,25.,-10.,40.,70.); \n\
		\n");
	PocessTextInDialog("Make a wedge", Message);
}

void CModelingDoc::OnPrism() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.));
	Handle(AIS_Shape) ais1 = new AIS_Shape(V1);
	myAISContext->Display(ais1,Standard_False);
	TopoDS_Shape S1 = BRepPrimAPI_MakePrism(V1,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais2 = new AIS_Shape(S1);
	myAISContext->Display(ais2,Standard_False);

	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-150.,-150,0.), gp_Pnt(-50.,-50,0.));
	Handle(AIS_Shape) ais3 = new AIS_Shape(E);
	myAISContext->Display(ais3,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakePrism(E,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais4 = new AIS_Shape(S2);
	myAISContext->SetColor(ais4,Quantity_NOC_CORAL2,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais4,Standard_False);

	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.));
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3);
	TopoDS_Shape S3 = BRepPrimAPI_MakePrism(W,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais5 = new AIS_Shape(W);
	myAISContext->Display(ais5,Standard_False);
	Handle(AIS_Shape) ais6 = new AIS_Shape(S3);
	myAISContext->SetColor(ais6,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais6,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais6,Standard_False);

	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc);
	Handle(AIS_Shape) ais7 = new AIS_Shape(F);
	myAISContext->Display(ais7,Standard_False);
	TopoDS_Shape S4 = BRepPrimAPI_MakePrism(F,gp_Vec(0.,0.,100.));
	Handle(AIS_Shape) ais8 = new AIS_Shape(S4);
	myAISContext->SetColor(ais8,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais8,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais8,Standard_False);
	Fit();

    TCollection_AsciiString Message ("\
		\n\
--- Prism a vertex -> result is an edge --- \n\
\n\
TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.)); \n\
TopoDS_Shape S1 = BRepBuilderAPI_MakePrism(V1,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism an edge -> result is a face --- \n\
\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-150.,-150,0.), gp_Pnt(-50.,-50,0.)); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakePrism(E,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism an wire -> result is a shell --- \n\
\n\
TopoDS_Edge E1 = BREpBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.)); \n\
TopoDS_Edge E2 = BREpBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.)); \n\
TopoDS_Edge E3 = BREpBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.)); \n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakePrism(W,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism a face or a shell -> result is a solid --- \n\
\n\
gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.gp_Dir(0.,0.,1.)), 80.); \n\
TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c); \n\
TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec); \n\
TopoDS_Face F = BRepBuilderAPI_MakeFace(gp::XOY(),Wc); \n\
TopoDS_Shape S4 = BRepBuilderAPI_MakePrism(F,gp_Vec(0.,0.,100.)); \n\
		\n");
	PocessTextInDialog("Make a prism", Message);
}

void CModelingDoc::OnRevol() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.));
	Handle(AIS_Shape) ais1 = new AIS_Shape(V1);
	myAISContext->Display(ais1,Standard_False);
	gp_Ax1 axe = gp_Ax1(gp_Pnt(-170.,-170.,0.),gp_Dir(0.,0.,1.));
	Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
	myAISContext->Display(ax1,Standard_False);
	TopoDS_Shape S1 = BRepPrimAPI_MakeRevol(V1,axe);
	Handle(AIS_Shape) ais2 = new AIS_Shape(S1);
	myAISContext->Display(ais2,Standard_False);

	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-120.,-120,0.), gp_Pnt(-120.,-120,100.));
	Handle(AIS_Shape) ais3 = new AIS_Shape(E);
	myAISContext->Display(ais3,Standard_False);
	axe = gp_Ax1(gp_Pnt(-100.,-100.,0.),gp_Dir(0.,0.,1.));
	Handle(Geom_Axis1Placement) Gax2 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax2 = new AIS_Axis(Gax2);
	myAISContext->Display(ax2,Standard_False);
	TopoDS_Shape S2 = BRepPrimAPI_MakeRevol(E,axe);
	Handle(AIS_Shape) ais4 = new AIS_Shape(S2);
	myAISContext->SetColor(ais4,Quantity_NOC_YELLOW,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais4,Standard_False);

	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.));
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.));
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.));
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3);
	axe = gp_Ax1(gp_Pnt(0.,0.,30.),gp_Dir(0.,1.,0.));
	Handle(Geom_Axis1Placement) Gax3 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax3 = new AIS_Axis(Gax3);
	myAISContext->Display(ax3,Standard_False);
	TopoDS_Shape S3 = BRepPrimAPI_MakeRevol(W,axe, 210.*M_PI/180);
	Handle(AIS_Shape) ais5 = new AIS_Shape(W);
	myAISContext->Display(ais5,Standard_False);
	Handle(AIS_Shape) ais6 = new AIS_Shape(S3);
	myAISContext->SetColor(ais6,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais6,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais6,Standard_False);

	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc);
	axe = gp_Ax1(gp_Pnt(290,290.,0.),gp_Dir(0.,1,0.));
	Handle(Geom_Axis1Placement) Gax4 = new Geom_Axis1Placement(axe);
	Handle (AIS_Axis) ax4 = new AIS_Axis(Gax4);
	myAISContext->Display(ax4,Standard_False);
	TopoDS_Shape S4 = BRepPrimAPI_MakeRevol(F,axe, 90.*M_PI/180);
	Handle(AIS_Shape) ais8 = new AIS_Shape(S4);
	myAISContext->SetColor(ais8,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais8,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(ais8,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
--- Revol of a vertex -> result is an edge --- \n\
\n\
TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.)); \n\
gp_Ax1 axe = gp_Ax1(gp_Pnt(-170.,-170.,0.),gp_Dir(0.,0.,1.)); \n\
TopoDS_Shape S1 = BRepPrimAPI_MakeRevol(V1,axe); \n\
\n\
--- Revol of an edge -> result is a face --- \n\
\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-120.,-120,0.), gp_Pnt(-120.,-120,100.)); \n\
axe = gp_Ax1(gp_Pnt(-100.,-100.,0.),gp_Dir(0.,0.,1.)); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeRevol(E,axe); \n\
\n\
--- Revol of a wire -> result is a shell --- \n\
\n\
TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.)); \n\
TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.)); \n\
TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.)); \n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3); \n\
axe = gp_Ax1(gp_Pnt(0.,0.,30.),gp_Dir(0.,1.,0.)); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeRevol(W,axe, 210.*PI180); \n\
\n\
--- Revol of a face -> result is a solid --- \n\
\n\
gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.); \n\
TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c); \n\
TopoDS_Wire Wc = BRepBuilderPI_MakeWire(Ec); \n\
TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc); \n\
axe = gp_Ax1(gp_Pnt(290,290.,0.),gp_Dir(0.,1,0.)); \n\
TopoDS_Shape S4 = BRepPrimAPI_MakeRevol(F,axe, 90.*PI180); \n\
		\n");
  PocessTextInDialog("Make a revol", Message);
}

void CModelingDoc::OnPipe() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TColgp_Array1OfPnt CurvePoles(1,4);
	gp_Pnt pt = gp_Pnt(0.,0.,0.);
	CurvePoles(1) = pt;
	pt = gp_Pnt(20.,50.,0.);
	CurvePoles(2) = pt;
	pt = gp_Pnt(60.,100.,0.);
	CurvePoles(3) = pt;
	pt = gp_Pnt(150.,0.,0.);
	CurvePoles(4) = pt;
	Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);
	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);
	Handle(AIS_Shape) ais1 = new AIS_Shape(W);
	myAISContext->Display(ais1,Standard_False);
	Fit();
	Sleep(500);
	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(0.,0.,0.),gp_Dir(0.,1.,0.)),10.);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	Handle(AIS_Shape) ais3 = new AIS_Shape(Wc);
	myAISContext->Display(ais3,Standard_False);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()),Wc);
	TopoDS_Shape S = BRepOffsetAPI_MakePipe(W,F);
	Handle(AIS_Shape) ais2 = new AIS_Shape(S);
	myAISContext->SetColor(ais2,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
TColgp_Array1OfPnt CurvePoles(1,6);\n\
gp_Pnt pt = gp_Pnt(0.,0.,0.);\n\
CurvePoles(1) = pt;\n\
pt = gp_Pnt(20.,50.,0.);\n\
CurvePoles(2) = pt;\n\
pt = gp_Pnt(60.,100.,0.);\n\
CurvePoles(3) = pt;\n\
pt = gp_Pnt(150.,0.,0.);\n\
CurvePoles(4) = pt;\n\
Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);\n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);\n\
gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(0.,0.,0.),gp_Dir(0.,1.,0.)),10.);\n\
TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);\n\
TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);\n\
TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()),Wc);\n\
TopoDS_Shape S = BRepBuilderAPI_MakePipe(W,F);\n\
		\n");
  PocessTextInDialog("Make a pipe", Message);

}

void CModelingDoc::OnThru() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	gp_Circ c1 = gp_Circ(gp_Ax2(gp_Pnt(-100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(c1);
	TopoDS_Wire W1 = BRepBuilderAPI_MakeWire(E1);
	Handle(AIS_Shape) sec1 = new AIS_Shape(W1);
	myAISContext->Display(sec1,Standard_False);
	gp_Circ c2 = gp_Circ(gp_Ax2(gp_Pnt(-10.,0.,-0.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(c2);
	TopoDS_Wire W2 = BRepBuilderAPI_MakeWire(E2);
	Handle(AIS_Shape) sec2 = new AIS_Shape(W2);
	myAISContext->Display(sec2,Standard_False);	
	gp_Circ c3 = gp_Circ(gp_Ax2(gp_Pnt(-75.,0.,100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(c3);
	TopoDS_Wire W3 = BRepBuilderAPI_MakeWire(E3);
	Handle(AIS_Shape) sec3 = new AIS_Shape(W3);
	myAISContext->Display(sec3,Standard_False);
	gp_Circ c4= gp_Circ(gp_Ax2(gp_Pnt(0.,0.,200.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);
	TopoDS_Wire W4 = BRepBuilderAPI_MakeWire(E4);
	Handle(AIS_Shape) sec4 = new AIS_Shape(W4);
	myAISContext->Display(sec4,Standard_False);
	BRepOffsetAPI_ThruSections generator(Standard_False,Standard_True);
	generator.AddWire(W1);
	generator.AddWire(W2);
	generator.AddWire(W3);
	generator.AddWire(W4);
	generator.Build();
	TopoDS_Shape S1 = generator.Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);

	gp_Circ c1b = gp_Circ(gp_Ax2(gp_Pnt(100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E1b = BRepBuilderAPI_MakeEdge(c1b);
	TopoDS_Wire W1b = BRepBuilderAPI_MakeWire(E1b);
	Handle(AIS_Shape) sec1b = new AIS_Shape(W1b);
	myAISContext->Display(sec1b,Standard_False);
	gp_Circ c2b = gp_Circ(gp_Ax2(gp_Pnt(210.,0.,-0.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E2b = BRepBuilderAPI_MakeEdge(c2b);
	TopoDS_Wire W2b = BRepBuilderAPI_MakeWire(E2b);
	Handle(AIS_Shape) sec2b = new AIS_Shape(W2b);
	myAISContext->Display(sec2b,Standard_False);	
	gp_Circ c3b = gp_Circ(gp_Ax2(gp_Pnt(275.,0.,100.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E3b = BRepBuilderAPI_MakeEdge(c3b);
	TopoDS_Wire W3b = BRepBuilderAPI_MakeWire(E3b);
	Handle(AIS_Shape) sec3b = new AIS_Shape(W3b);
	myAISContext->Display(sec3b,Standard_False);
	gp_Circ c4b= gp_Circ(gp_Ax2(gp_Pnt(200.,0.,200.),gp_Dir(0.,0.,1.)),40.);
	TopoDS_Edge E4b = BRepBuilderAPI_MakeEdge(c4b);
	TopoDS_Wire W4b = BRepBuilderAPI_MakeWire(E4b);
	Handle(AIS_Shape) sec4b = new AIS_Shape(W4b);
	myAISContext->Display(sec4b,Standard_False);
	BRepOffsetAPI_ThruSections generatorb(Standard_True,Standard_False);
	generatorb.AddWire(W1b);
	generatorb.AddWire(W2b);
	generatorb.AddWire(W3b);
	generatorb.AddWire(W4b);
	generatorb.Build();
	TopoDS_Shape S2 = generatorb.Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
	myAISContext->SetColor(ais2,Quantity_NOC_ALICEBLUE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
---------- ruled -------------- \n\
\n\
gp_Circ c1 = gp_Circ(gp_Ax2(gp_Pnt(-100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(c1);\n\
TopoDS_Wire W1 = BRepBuilderAPI_MakeWire(E1);\n\
gp_Circ c2 = gp_Circ(gp_Ax2(gp_Pnt(-10.,0.,-0.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(c2);\n\
TopoDS_Wire W2 = BRepBuilderAPI_MakeWire(E2);\n\
gp_Circ c3 = gp_Circ(gp_Ax2(gp_Pnt(-75.,0.,100.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(c3);\n\
TopoDS_Wire W3 = BRepBuilderAPI_MakeWire(E3);\n\
gp_Circ c4= gp_Circ(gp_Ax2(gp_Pnt(0.,0.,200.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E4 = BRep>BuilderAPI_MakeEdge(c4);\n\
TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);\n\
TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);\n\
TopoDS_Wire W4 = BRepBuilderAPI_MakeWire(E4);\n\
BRepOffsetAPI_ThruSections generator(Standard_False,Standard_True);\n\
generator.AddWire(W1);\n\
generator.AddWire(W2);\n\
generator.AddWire(W3);\n\
generator.AddWire(W4);\n\
generator.Build();\n\
TopoDS_Shape S1 = generator.Shape();\n\
\n\
---------- smooth -------------- \n\
\n\
gp_Circ c1b = gp_Circ(gp_Ax2(gp_Pnt(100.,0.,-100.),gp_Dir(0.,0.,1.)),40.); \n\
TopoDS_Edge E1b = BRepBuilderAPI_MakeEdge(c1b); \n\
TopoDS_Wire W1b = BRepBuilderAPI_MakeWire(E1b); \n\
gp_Circ c2b = gp_Circ(gp_Ax2(gp_Pnt(210.,0.,-0.),gp_Dir(0.,0.,1.)),40.); \n\
TopoDS_Edge E2b = BRepBuilderAPI_MakeEdge(c2b);\n\
TopoDS_Wire W2b = BRepBuilderAPI_MakeWire(E2b); \n\
gp_Circ c3b = gp_Circ(gp_Ax2(gp_Pnt(275.,0.,100.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E3b = BRepBuilderAPI_MakeEdge(c3b);\n\
TopoDS_Wire W3b = BRepBuilderAPI_MakeWire(E3b);\n\
gp_Circ c4b= gp_Circ(gp_Ax2(gp_Pnt(200.,0.,200.),gp_Dir(0.,0.,1.)),40.);\n\
TopoDS_Edge E4b = BRepBuilderAPI_MakeEdge(c4b);\n\
TopoDS_Wire W4b = BRepBuilderAPI_MakeWire(E4b);\n\
BRepOffsetAPI_ThruSections generatorb(Standard_True,Standard_False);\n\
generatorb.AddWire(W1b);\n\
generatorb.AddWire(W2b);\n\
generatorb.AddWire(W3b);\n\
generatorb.AddWire(W4b);\n\
generatorb.Build();\n\
TopoDS_Shape S2 = generatorb.Shape();\n\
		\n");
  PocessTextInDialog("Make a Thru sections", Message);

}

void CModelingDoc::OnEvolved() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	BRepBuilderAPI_MakePolygon P;
	P.Add(gp_Pnt(0.,0.,0.));
	P.Add(gp_Pnt(200.,0.,0.));
	P.Add(gp_Pnt(200.,200.,0.));
	P.Add(gp_Pnt(0.,200.,0.));
	P.Add(gp_Pnt(0.,0.,0.));
	TopoDS_Wire W = P.Wire();
	
	Handle(AIS_Shape) ais1 = new AIS_Shape(W);
	myAISContext->Display(ais1,Standard_False);
	
	TopoDS_Wire wprof = BRepBuilderAPI_MakePolygon(gp_Pnt(0.,0.,0.),gp_Pnt(-60.,-60.,-200.));
	
	Handle(AIS_Shape) ais3 = new AIS_Shape(wprof);
	myAISContext->Display(ais3,Standard_False);
	Fit();
	Sleep(500);
	TopoDS_Shape S = BRepOffsetAPI_MakeEvolved(W,wprof,GeomAbs_Arc,Standard_True,Standard_False,Standard_True,0.0001);
	
	Handle(AIS_Shape) ais2 = new AIS_Shape(S);
	myAISContext->SetColor(ais2,Quantity_NOC_MATRABLUE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	Fit();
	
	TCollection_AsciiString Message ("\
		\n\
---------- Evolved shape -------------- \n\
\n\
BRepBuilderAPI_MakePolygon P;\n\
P.Add(gp_Pnt(0.,0.,0.));\n\
P.Add(gp_Pnt(200.,0.,0.));\n\
P.Add(gp_Pnt(200.,200.,0.));\n\
P.Add(gp_Pnt(0.,200.,0.));\n\
P.Add(gp_Pnt(0.,0.,0.));\n\
TopoDS_Wire W = P.Wire();\n\
TopoDS_Wire wprof = BRepBuilderAPI_MakePolygon(gp_Pnt(0.,0.,0.),gp_Pnt(-60.,-60.,-200.));\n\
TopoDS_Shape S = BRepBuilderAPI_MakeEvolved(W,wprof,GeomAbs_Arc,Standard_True,Standard_False,Standard_True,0.0001);\n\
		\n");
  PocessTextInDialog("Make an evolved shape", Message);

}

void CModelingDoc::OnDraft() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(200., 300., 150.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();
	Sleep(500);
	BRepOffsetAPI_DraftAngle adraft(S);
	TopExp_Explorer Ex;
	for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {
		TopoDS_Face F = TopoDS::Face(Ex.Current());
		Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));
		gp_Pln apln = surf->Pln();
		gp_Dir dirF = apln.Axis().Direction();
		if (dirF.IsNormal(gp_Dir(0.,0.,1.),Precision::Angular()))
			adraft.Add(F, gp_Dir(0.,0.,1.), 15.*M_PI/180, gp_Pln(gp::XOY()));
	}
	ais1->Set(adraft.Shape());
	myAISContext->Redisplay(ais1,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
---------- Tapered shape -------------- \n\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(200.,300.,150.);\n\
BRepOffsetAPI_DraftAngle adraft(S);\n\
TopExp_Explorer Ex;\n\
for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {\n\
	TopoDS_Face F = TopoDS::Face(Ex.Current());\n\
	Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));\n\
	gp_Pln apln = surf->Pln();\n\
	gp_Dir dirF = apln.Axis().Direction();\n\
	if (dirF.IsNormal(gp_Dir(0.,0.,1.),Precision::Angular()))\n\
		adraft.Add(F, gp_Dir(0.,0.,1.), 15.*PI180, gp_Pln(gp::XOY()));\n\
}\n\
		\n");
  PocessTextInDialog("Make a tapered shape", Message);

}

/* =================================================================================
   ====================   O P E R A T I O N S   ====================================
   ================================================================================= */

void CModelingDoc::OnCut() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 60, 60).Shape();

Handle (AIS_Shape)	ais1 = new AIS_Shape(theBox);
myAISContext->SetDisplayMode(ais1,1,Standard_False);
myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False);
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(1000);


TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100, 20, 20), 80).Shape();
Handle (AIS_Shape)	ais2 = new AIS_Shape(theSphere);
myAISContext->SetDisplayMode(ais2,1,Standard_False);
myAISContext->SetColor(ais2,Quantity_NOC_YELLOW,Standard_False);
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();
Sleep(1000);

TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theSphere,theBox);

myAISContext->Erase(ais1,Standard_False);
myAISContext->Erase(ais2,Standard_False);

Handle (AIS_Shape)	aSection = new AIS_Shape(ShapeCut);
myAISContext->SetDisplayMode(aSection,1,Standard_False);
myAISContext->SetColor(aSection,Quantity_NOC_RED,Standard_False);
myAISContext->SetMaterial(aSection,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(aSection,Standard_False);
const Handle(AIS_InteractiveObject)& anIOSection = aSection;
myAISContext->SetSelected (anIOSection, Standard_False);
Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,40,40); \n\
 \n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80); \n\
 \n\
TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theSphere,theBox); \n\
 \n");
	PocessTextInDialog("Cut the sphere with a box", Message);


}










void CModelingDoc::OnFuse() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

gp_Pnt P(-5,5,-5);
TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(60, 200, 70).Shape();
Handle (AIS_Shape)	ais1 = new AIS_Shape(theBox1);
myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False);
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(1000);

TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P, 20, 150, 110).Shape();
Handle (AIS_Shape)	ais2 = new AIS_Shape(theBox2);
myAISContext->SetColor(ais2,Quantity_NOC_YELLOW,Standard_False);
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();
Sleep(1000);

TopoDS_Shape FusedShape = BRepAlgoAPI_Fuse(theBox1,theBox2);

myAISContext->Erase(ais1,Standard_True);
myAISContext->Erase(ais2,Standard_True);

Handle (AIS_Shape)	aFusion = new AIS_Shape(FusedShape);
myAISContext->SetDisplayMode(aFusion,1,Standard_False);
myAISContext->SetColor(aFusion,Quantity_NOC_RED,Standard_False);
myAISContext->SetMaterial(aFusion,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->Display(aFusion,Standard_False);
const Handle(AIS_InteractiveObject)& anIOFusion = aFusion;
myAISContext->SetSelected (anIOFusion, Standard_False);
Fit();

    TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(50,200,70); \n\
 \n\
TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(-30,150,70); \n\
 \n\
TopoDS_Shape FusedShape = BRepAlgoAPI_Fuse(theBox1,theBox2); \n");
	PocessTextInDialog("Fuse the boxes", Message);

}

void CModelingDoc::OnCommon() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

gp_Ax2 axe(gp_Pnt(10,10,10),gp_Dir(1,2,1));
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(axe, 60, 80, 100).Shape();

Handle(AIS_Shape) aboxshape=new AIS_Shape(theBox);
myAISContext->SetColor(aboxshape,Quantity_NOC_YELLOW,Standard_False);
myAISContext->SetMaterial(aboxshape,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->SetDisplayMode(aboxshape,1,Standard_False);
myAISContext->SetTransparency(aboxshape,0.2,Standard_False);
myAISContext->Display(aboxshape,Standard_False);
const Handle(AIS_InteractiveObject)& anIOBoxShape = aboxshape;
myAISContext->SetSelected (anIOBoxShape, Standard_False);
Fit();
Sleep(500);

TopoDS_Shape theWedge = BRepPrimAPI_MakeWedge(60., 100., 80., 20.).Shape();

Handle(AIS_Shape) awedge = new AIS_Shape(theWedge);
myAISContext->SetColor(awedge,Quantity_NOC_RED,Standard_False);
myAISContext->SetMaterial(awedge,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->SetTransparency(awedge,0.0,Standard_False);
myAISContext->Display(awedge,Standard_False);
const Handle(AIS_InteractiveObject)& anIOWedge = awedge;
myAISContext->SetSelected (anIOWedge, Standard_False);
Fit();
Sleep(500);

TopoDS_Shape theCommonSurface = BRepAlgoAPI_Common(theBox,theWedge);

myAISContext->Erase(aboxshape,Standard_True);
myAISContext->Erase(awedge,Standard_True);

Handle(AIS_Shape) acommon = new AIS_Shape(theCommonSurface);
myAISContext->SetColor(acommon,Quantity_NOC_GREEN,Standard_False); 
myAISContext->SetMaterial(acommon,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(acommon,Standard_False);
const Handle(AIS_InteractiveObject)& anIOCommon = acommon;
myAISContext->SetSelected (anIOCommon, Standard_False);

   TCollection_AsciiString Message ("\
		\n\
gp_Ax2 axe(gp_Pnt(10,10,10),gp_Dir(1,2,1)); \n\
 \n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(axe,60,80,100); \n\
 \n\
TopoDS_Shape theWedge = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
 \n\
TopoDS_Shape theCommonSurface = BRepAlgoAPI_Common(theBox,theWedge); \n\
 \n");

	PocessTextInDialog("Compute the common surface ", Message);

}

void CModelingDoc::OnSection() 
{

	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape atorus = BRepPrimAPI_MakeTorus(120, 20).Shape();

    Handle(AIS_Shape) ashape=new AIS_Shape(atorus);
    myAISContext->SetColor(ashape,Quantity_NOC_RED,Standard_False);
    myAISContext->SetMaterial(ashape,Graphic3d_NOM_PLASTIC,Standard_False);    
    myAISContext->SetDisplayMode(ashape,1,Standard_False);
	myAISContext->SetTransparency(ashape,0.1,Standard_False);
    myAISContext->Display(ashape,Standard_False);

gp_Vec V1(1,1,1);
Standard_Real radius = 120;
Standard_Integer i=-3;

for(i;i<=3;i++) {
    TopoDS_Shape asphere = BRepPrimAPI_MakeSphere(gp_Pnt(26 * 3 * i, 0, 0), radius).Shape();

    Handle (AIS_Shape) theShape=new AIS_Shape (asphere);
    myAISContext->SetTransparency(theShape,0.1,Standard_False);
    myAISContext->SetColor(theShape,Quantity_NOC_WHITE,Standard_False);
    myAISContext->SetDisplayMode(theShape,1,Standard_False);
    myAISContext->Display(theShape,Standard_False);
	Fit();

    Standard_Boolean PerformNow=Standard_False; 

    BRepAlgoAPI_Section section(atorus,asphere,PerformNow);
    section.ComputePCurveOn1(Standard_True);
    section.Approximation(TopOpeBRepTool_APPROX);
    section.Build();

    Handle(AIS_Shape) asection=new AIS_Shape(section.Shape());
    myAISContext->SetDisplayMode (asection, 0, Standard_False);
    myAISContext->SetColor (asection, Quantity_NOC_WHITE, Standard_False);
    myAISContext->Display (asection, Standard_False);
    if(i<3) {
    myAISContext->Remove (theShape, Standard_False);
	}
}
  myAISContext->UpdateCurrentViewer();
   TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape atorus = BRepPrimAPI_MakeTorus(120,20); \n\
gp_Vec V1(1,1,1); \n\
Standard_Real radius = 120; \n\
Standard_Integer i=-3; \n\
for(i;i<=3;i++) { \n\
    TopoDS_Shape asphere = BRepPrimAPI_MakeSphere(gp_Pnt(78*i,0,0),radius); \n\
    Standard_Boolean PerformNow=Standard_False; \n\
    BRepAlgoAPI_Section section(atorus,asphere,PerformNow); \n\
    section.ComputePCurveOn1(Standard_True); \n\
    section.Approximation(TopOpeBRepTool_APPROX); \n\
    section.Build(); \n\
	\n");

	PocessTextInDialog("Compute the sections ", Message);

}

void CModelingDoc::OnPsection() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theTorus = BRepPrimAPI_MakeTorus(35, 8).Shape();
Handle(AIS_Shape) atorus = new AIS_Shape(theTorus);
myAISContext->SetColor(atorus,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(atorus,Graphic3d_NOM_PLASTIC,Standard_False);
myAISContext->SetTransparency(atorus,0.1,Standard_False);
myAISContext->Display(atorus,Standard_False);
const Handle(AIS_InteractiveObject)& anIOTorus = atorus;
myAISContext->SetSelected (anIOTorus, Standard_False);
Fit();
Sleep(500);

gp_Pln aplane(1,0.25,3,4);
Handle (Geom_Plane) thePlane = new Geom_Plane(aplane);
Handle (AIS_Plane) ais1 = new AIS_Plane(thePlane);
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(300);

BRepAlgoAPI_Section section(theTorus,thePlane,Standard_False);
section.ComputePCurveOn1(Standard_True);
section.Approximation(TopOpeBRepTool_APPROX);
section.Build();

Handle(AIS_Shape) asection=new AIS_Shape(section.Shape());
myAISContext->SetDisplayMode(asection ,0,Standard_False);
myAISContext->SetColor(asection,Quantity_NOC_WHITE,Standard_False); 
myAISContext->Display(asection,Standard_False);
Fit();

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theTorus = BRepPrimAPI_MakeTorus(60.,20.); \n\
 \n\
gp_Pln P(1,2,1,-15); \n\
 \n\
TopoDS_Shape Psection = BRepAlgoAPI_Section(theTorus,P);  \n\
\n");

	PocessTextInDialog("Compute the plane section ", Message);

}

void CModelingDoc::OnBlend() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape Box = BRepPrimAPI_MakeBox(gp_Pnt(-400,0,0),200,230,180).Shape();
Handle(AIS_Shape) ais1 = new AIS_Shape(Box);
myAISContext->SetColor(ais1,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False); 
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeFillet fillet(Box);

for (TopExp_Explorer ex(Box,TopAbs_EDGE); ex.More(); ex.Next()) {
	TopoDS_Edge Edge =TopoDS::Edge(ex.Current());
	fillet.Add(20,Edge);
}

myAISContext->Remove(ais1,Standard_False);

TopoDS_Shape blendedBox = fillet.Shape();
Handle(AIS_Shape) aBlendbox = new AIS_Shape(blendedBox);
myAISContext->SetColor(aBlendbox,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(aBlendbox,Graphic3d_NOM_PLASTIC,Standard_False); 
myAISContext->Display(aBlendbox,Standard_False);
const Handle(AIS_InteractiveObject)& anIOBlendBox = aBlendbox;
myAISContext->SetSelected (anIOBlendBox, Standard_False);
Fit();
Sleep(500);


gp_Pnt P1(250,150,75);
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(300, 200, 200).Shape();
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(P1, 120, 180, 70).Shape();

TopoDS_Shape fusedShape = BRepAlgoAPI_Fuse(S1,S2);
Handle(AIS_Shape) ais2 = new AIS_Shape(fusedShape);
myAISContext->SetColor(ais2,Quantity_NOC_RED,Standard_False); 
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);  
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();

BRepFilletAPI_MakeFillet fill(fusedShape);

for (TopExp_Explorer ex1(fusedShape,TopAbs_EDGE); ex1.More(); ex1.Next()) {
	TopoDS_Edge E =TopoDS::Edge(ex1.Current());
	fill.Add(E);
}

for (Standard_Integer i = 1;i<=fill.NbContours();i++) {
	Standard_Real longueur(fill.Length(i));
	Standard_Real Rad(0.15*longueur);
	fill.SetRadius(Rad,i, 1);
}

TopoDS_Shape blendedFusedSolids = fill.Shape();

Handle(AIS_Shape) aBlend = new AIS_Shape(blendedFusedSolids);
myAISContext->SetColor(aBlend,Quantity_NOC_RED,Standard_False); 
myAISContext->SetMaterial(aBlend,Graphic3d_NOM_PLASTIC,Standard_False);  
myAISContext->Display(aBlend,Standard_False);

myAISContext->Remove(ais2,Standard_False);
Fit();


 TCollection_AsciiString Message ("\
		\n\
//THE YELLOW BOX\n\
TopoDS_Shape Box = BRepPrimAPI_MakeBox(gp_Pnt(-400,0,0),200,230,180);\n\
\n\
BRepPrimAPI_MakeFillet fillet(Box);\n\
\n\
for (TopExp_Explorer ex(Box,TopAbs_EDGE); ex.More(); ex.Next()) {\n\
	TopoDS_Edge Edge =TopoDS::Edge(ex.Current());\n\
	fillet.Add(20,Edge);\n\
}\n\
TopoDS_Shape blendedBox = fillet.Shape();\n\
\n\
////////////////////////////////////////////////////////////////////////\n\
\n\
//THE RED SOLID\n\
 \n\
//Warning : On the acute angles of the boxes a fillet is created. \n\
On the angles of fusion a blend is created. \n\
\n\
gp_Pnt P1(150,150,75);\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(300,200,200);\n\
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(P1,100,200,70);\n\
\n\
TopoDS_Shape fusedShape = BRepAlgoAPI_Fuse(S1,S2);\n\
BRepPrimAPI_MakeFillet fill(fusedShape);\n\
\n\
for (TopExp_Explorer ex1(fusedShape,TopAbs_EDGE); ex1.More(); ex1.Next()) {\n\
	TopoDS_Edge E =TopoDS::Edge(ex1.Current());\n\
	fill.Add(E);\n\
}\n\
\n\
for (Standard_Integer i = 1;i<=fill.NbContours();i++) {\n\
	Standard_Real longueur(fill.Length(i));\n\
	Standard_Real Rad(0.2*longueur);\n\
	fill.SetRadius(Rad,i);\n\
}\n\
\n\
TopoDS_Shape blendedFusedSolids = fill.Shape(); \n");

	PocessTextInDialog("Compute the blend on the edges ", Message);
}

void CModelingDoc::OnEvolvedblend() 
{

	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 200, 200).Shape();
Handle(AIS_Shape) ais1 = new AIS_Shape(theBox);
myAISContext->SetColor(ais1,Quantity_NOC_BROWN,Standard_False); 
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False); 
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeFillet Rake(theBox);

TopExp_Explorer ex(theBox,TopAbs_EDGE);
ex.Next();
ex.Next();
ex.Next();
ex.Next();
Rake.Add(8,50,TopoDS::Edge(ex.Current()));
Rake.Build();
if (Rake.IsDone() ){
	TopoDS_Shape evolvedBox = Rake.Shape();
	ais1->Set(evolvedBox);
	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected(anIO1, Standard_False);
	Fit();
	Sleep(500);

}

TopoDS_Shape theCylinder = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(-300, 0, 0), gp::DZ()), 100, 200).Shape();
Handle(AIS_Shape) ais3 = new AIS_Shape(theCylinder);
myAISContext->SetColor(ais3,Quantity_NOC_GREEN,Standard_False); 
myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(ais3,Standard_False);
const Handle(AIS_InteractiveObject)& anIO3 = ais3;
myAISContext->SetSelected (anIO3, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeFillet fillet(theCylinder);

TColgp_Array1OfPnt2d TabPoint2(1,20);

for (Standard_Integer i=0; i<=19; i++) {
	gp_Pnt2d Point2d(i*2*M_PI/19,60*cos(i*M_PI/19-M_PI/2)+10);
	TabPoint2.SetValue(i+1,Point2d);
}

TopExp_Explorer exp2(theCylinder,TopAbs_EDGE);
fillet.Add(TabPoint2,TopoDS::Edge(exp2.Current()));
fillet.Build();
if (fillet.IsDone() ){
	TopoDS_Shape LawEvolvedCylinder = fillet.Shape();
	ais3->Set(LawEvolvedCylinder);
	myAISContext->Redisplay(ais3,Standard_False);
	myAISContext->SetSelected(anIO3,Standard_False);
	Fit();
	Sleep(500);
}

gp_Pnt P(350,0,0);
TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P, 200, 200, 200).Shape();
Handle(AIS_Shape) ais2 = new AIS_Shape(theBox2);
myAISContext->SetColor(ais2,Quantity_NOC_RED,Standard_False); 
myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(ais2,Standard_False);
const Handle(AIS_InteractiveObject)& anIO2 = ais2;
myAISContext->SetSelected (anIO2, Standard_False);
Fit();
Sleep(500);


BRepFilletAPI_MakeFillet afillet(theBox2);

TColgp_Array1OfPnt2d TabPoint(1,6);

gp_Pnt2d P1(0.,8.);
gp_Pnt2d P2(0.2,16.);
gp_Pnt2d P3(0.4,25.);
gp_Pnt2d P4(0.6,55.);
gp_Pnt2d P5(0.8,28.);
gp_Pnt2d P6(1.,20.);
TabPoint.SetValue(1,P1);
TabPoint.SetValue(2,P2);
TabPoint.SetValue(3,P3);
TabPoint.SetValue(4,P4);
TabPoint.SetValue(5,P5);
TabPoint.SetValue(6,P6);

TopExp_Explorer exp(theBox2,TopAbs_EDGE);
exp.Next();
exp.Next();
exp.Next();
exp.Next();

afillet.Add(TabPoint, TopoDS::Edge(exp.Current()));

afillet.Build();
if (afillet.IsDone() ){
	TopoDS_Shape LawevolvedBox = afillet.Shape();
	ais2->Set(LawevolvedBox);
	myAISContext->Redisplay(ais2,Standard_False);
	myAISContext->SetSelected(anIO2,Standard_False);
	Fit();
	
}

   TCollection_AsciiString Message ("\
		\n\
//THE BROWN BOX \n\
\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,200,200);	\n\
	\n\
BRepPrimAPI_MakeFillet Rake(theBox);	\n\
ChFi3d_FilletShape FSh = ChFi3d_Rational;	\n\
Rake.SetFilletShape(FSh);	\n\
	\n\
TopExp_Explorer ex(theBox,TopAbs_EDGE);	\n\
ex.Next();   //in order to recover the front edge	\n\
ex.Next();	\n\
ex.Next();	\n\
ex.Next();	\n\
Rake.Add(8,50,TopoDS::Edge(ex.Current()));	\n\
 	\n\
Rake.Build();  \n\
if (Rake.IsDone() )  \n\
	TopoDS_Shape theBrownBox = Rake.Shape();	\n\
	\n\
//////////////////////////////////////////////////////////	\n\
	\n\
//THE GREEN CYLINDER	\n\
	\n\
TopoDS_Shape theCylinder = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(-300,0,0),gp_Dir(0,0,1)),100,200);	\n\
	\n\
BRepPrimAPI_MakeFillet fillet(theCylinder);	\n\
	\n\
TColgp_Array1OfPnt2d TabPoint2(1,20);	\n\
	\n\
for (Standard_Integer i=0; i<=19; i++) {	\n\
	gp_Pnt2d Point2d(i*2*PI/19,60*cos(i*PI/19-PI/2)+10);	\n\
	TabPoint2.SetValue(i+1,Point2d);	\n\
}	\n\
	\n\
TopExp_Explorer exp2(theCylinder,TopAbs_EDGE);	\n\
fillet.Add(TabPoint2,TopoDS::Edge(exp2.Current()));	\n\
 	\n\
fillet.Build();  \n\
if (fillet.IsDone() )  \n\
	TopoDS_Shape LawEvolvedCylinder = fillet.Shape();	\n\
	\n\
////////////////////////////////////////////////////////////	\n\
	\n\
	//THE RED BOX \n\
 \n\
gp_Pnt P(350,0,0);	\n\
TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P,200,200,200);	\n\
	\n\
BRepPrimAPI_MakeFillet fill(theBox2);	\n\
	\n\
TColgp_Array1OfPnt2d TabPoint(1,6);	\n\
gp_Pnt2d P1(0,8);	\n\
gp_Pnt2d P2(0.2,16);	\n\
gp_Pnt2d P3(0.4,25);	\n\
gp_Pnt2d P4(0.6,55);	\n\
gp_Pnt2d P5(0.8,28);	\n\
gp_Pnt2d P6(1,20);	\n\
TabPoint.SetValue(1,P1);	\n\
TabPoint.SetValue(2,P2);	\n\
TabPoint.SetValue(3,P3);	\n\
TabPoint.SetValue(4,P4);	\n\
TabPoint.SetValue(5,P5);	\n\
TabPoint.SetValue(6,P6);	\n\
	\n\
TopExp_Explorer exp(theBox2,TopAbs_EDGE);	\n\
exp.Next();  //in order to trcover the front edge	\n\
exp.Next();	\n\
exp.Next();	\n\
exp.Next();	\n\
fill.Add(TabPoint,TopoDS::Edge(exp.Current()));	\n\
 	\n\
fill.Build();   \n\
if (fillet.IsDone() )  \n\
	TopoDS_Shape theRedBox = fill.Shape();	\n\
\n");

	PocessTextInDialog("Compute evolutiv blend on an edge ", Message);

}

void CModelingDoc::OnChamf() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

TopoDS_Shape theBox = BRepPrimAPI_MakeBox(60,200,70).Shape();
Handle(AIS_Shape) ais1 = new AIS_Shape(theBox);
myAISContext->SetColor(ais1,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(ais1,Standard_False);
const Handle(AIS_InteractiveObject)& anIO1 = ais1;
myAISContext->SetSelected (anIO1, Standard_False);
Fit();
Sleep(500);

BRepFilletAPI_MakeChamfer MC(theBox);
// add all the edges to chamfer
TopTools_IndexedDataMapOfShapeListOfShape M;
TopExp::MapShapesAndAncestors(theBox,TopAbs_EDGE,TopAbs_FACE,M);
for (Standard_Integer i = 1;i<=M.Extent();i++) {
	TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
	TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First());
	MC.Add(5,5,E,F);
	}

TopoDS_Shape ChanfrenedBox = MC.Shape();
Handle(AIS_Shape) aBlendedBox = new AIS_Shape(ChanfrenedBox);
myAISContext->SetColor(aBlendedBox,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->SetMaterial(aBlendedBox,Graphic3d_NOM_PLASTIC,Standard_False);    
myAISContext->Display(aBlendedBox,Standard_False);
const Handle(AIS_InteractiveObject)& anIOBlendedBox = aBlendedBox;
myAISContext->SetSelected (anIOBlendedBox, Standard_False);
Fit();
Sleep(500);

myAISContext->Erase(ais1,Standard_True);

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(130,200,170); \n\
BRepFilletAPI_MakeChamfer MC(theBox); \n\
TopTools_IndexedDataMapOfShapeListOfShape M; \n\
TopExp::MapShapesAndAncestors(theBox,TopAbs_EDGE,TopAbs_FACE,M); \n\
for (Standar1d_Integer i;i<M.Extent();i++) { \n\
	TopoDS_Edge E = TopoDS::Edge(M.FindKey(i)); \n\
	TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First()); \n\
	MC.Add(15,15,E,F); \n\
	} \n\
TopoDS_Shape ChanfrenedBox = MC.Shape();  \n");

	PocessTextInDialog("Compute the chamfers on all the edges ", Message);
}

/* =================================================================================
   ====================   L O C A L   O P E R A T I O N S   ========================
   ================================================================================= */

void CModelingDoc::OnPrismLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);

	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	TopoDS_Face F = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
	Handle(Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);
	gp_Dir D = Pl->Pln().Axis().Direction();
// new in 2.0 ..use the trigonometric orientation to make the extrusion.
	D.Reverse();
	gp_Pnt2d p1,p2;
	Handle(Geom2d_Curve) aline;
	BRepBuilderAPI_MakeWire MW;
	p1 = gp_Pnt2d(200.,-100.);
	p2 = gp_Pnt2d(100.,-100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,-200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(200.,-200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(200.,-100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF;
	MKF.Init(surf,Standard_False, Precision::Confusion());
	MKF.Add(MW.Wire());
	TopoDS_Shape FP = MKF.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakePrism MKP(S,FP,F,D,0,Standard_True);
	MKP.Perform(200.);
	TopoDS_Shape res1 = MKP.Shape();
	ais1->Set(res1);

	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected(anIO1,Standard_False);
	Fit();
	Sleep(500);

	Ex.Next();
	TopoDS_Face F2 = TopoDS::Face(Ex.Current());
	surf = BRep_Tool::Surface(F2);
	Pl = Handle(Geom_Plane)::DownCast(surf);
	D = Pl->Pln().Axis().Direction();
	D.Reverse();
	BRepBuilderAPI_MakeWire MW2;
	p1 = gp_Pnt2d(100.,100.);
	p2 = gp_Pnt2d(200.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(150.,200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF2;
	MKF2.Init(surf,Standard_False, Precision::Confusion());
	MKF2.Add(MW2.Wire());
	FP = MKF2.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakePrism MKP2(res1,FP,F2,D,1,Standard_True);
	MKP2.Perform(100.);
	TopoDS_Shape res2 = MKP2.Shape();
	ais1->Set(res2);

	myAISContext->Redisplay (ais1, Standard_False);
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
	\n\
--- Extrusion ---\n\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
TopoDS_Face F = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F);\n\
Handle(Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);\n\
gp_Dir D = Pl->Pln().Axis().Direction();\n\
D.Reverse();\n\
gp_Pnt2d p1,p2;\n\
Handle(Geom2d_Curve) aline;\n\
BRepBuilderAPI_MakeWire MW;\n\
p1 = gp_Pnt2d(200.,-100.);\n\
p2 = gp_Pnt2d(100.,-100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,-200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(200.,-200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(200.,-100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF;\n\
MKF.Init(surf,Standard_False);\n\
MKF.Add(MW.Wire());\n\
TopoDS_Shape FP = MKF.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakePrism MKP(S,FP,F,D,0,Standard_True);\n\
MKP.Perform(200);\n\
TopoDS_Shape res1 = MKP.Shape();\n\
	\n");
		Message += "\n\
--- Protrusion --- \n\
\n\
Ex.Next();\n\
TopoDS_Face F2 = TopoDS::Face(Ex.Current());\n\
surf = BRep_Tool::Surface(F2);\n\
Pl = Handle(Geom_Plane)::DownCast(surf);\n\
D = Pl->Pln().Axis().Direction();\n\
D.Reverse();\n\
BRepBuilderAPI_MakeWire MW2;\n\
p1 = gp_Pnt2d(100.,100.);\n\
p2 = gp_Pnt2d(200.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(150.,200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW2.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF2;\n\
MKF2.Init(surf,Standard_False);\n\
MKF2.Add(MW2.Wire());\n\
FP = MKF2.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakePrism MKP2(res1,FP,F2,D,1,Standard_True);\n\
MKP2.Perform(100.);\n\
TopoDS_Shape res2 = MKP2.Shape();\n\
	\n";
	PocessTextInDialog("Make an extrusion or a protrusion", Message);
}

//
// BRepFeat_MakeDPrism
//
void CModelingDoc::OnDprismLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);

	myAISContext->SetColor(ais1,Quantity_NOC_RED,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	
	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	Ex.Next();
	Ex.Next();
	Ex.Next();
	Ex.Next();
	TopoDS_Face F = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
	gp_Circ2d c(gp_Ax2d(gp_Pnt2d(200.,130.),gp_Dir2d(1.,0.)),50.);
	BRepBuilderAPI_MakeWire MW;
	Handle(Geom2d_Curve) aline = new Geom2d_Circle(c);
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,M_PI));
	MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,M_PI,2.*M_PI));
	BRepBuilderAPI_MakeFace MKF;
	MKF.Init(surf,Standard_False, Precision::Confusion());
	MKF.Add(MW.Wire());
	TopoDS_Face FP = MKF.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakeDPrism MKDP(S,FP,F,10*M_PI/180,1,Standard_True);
	MKDP.Perform(200);
	TopoDS_Shape res1 = MKDP.Shape();

	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	ais1->Set(res1);
	
	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected(anIO1,Standard_False);

	Fit();

	TCollection_AsciiString Message ("\
	\n\
--- Protrusion with draft angle --- \n\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
Ex.Next();\n\
Ex.Next();\n\
Ex.Next();\n\
Ex.Next();\n\
TopoDS_Face F = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F);\n\
gp_Circ2d c(gp_Ax2d(gp_Pnt2d(200.,130.),gp_Dir2d(1.,0.)),50.);\n\
BRepBuilderAPI_MakeWire MW;\n\
Handle(Geom2d_Curve) aline = new Geom2d_Circle(c);\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,PI));\n\
MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,PI,2.*PI));\n\
BRepBuilderAPI_MakeFace MKF;\n\
MKF.Init(surf,Standard_False);\n\
MKF.Add(MW.Wire());\n\
TopoDS_Face FP = MKF.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakeDPrism MKDP(S,FP,F,10*PI180,1,Standard_True);\n\
MKDP.Perform(200);\n\
TopoDS_Shape res1 = MKDP.Shape();\n\
	\n");
	PocessTextInDialog("Make an extrusion or a protrusion with a draft angle", Message);
}

void CModelingDoc::OnRevolLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	
	myAISContext->SetColor(ais1,Quantity_NOC_CORAL,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	Ex.Next();
	TopoDS_Face F1 = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);
	Handle (Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);
	gp_Ax1 D = gp::OX();
	BRepBuilderAPI_MakeWire MW1;
	gp_Pnt2d p1,p2;
	p1 = gp_Pnt2d(100.,100.);
	p2 = gp_Pnt2d(200.,100.);
	Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(150.,200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF1;
	MKF1.Init(surf,Standard_False, Precision::Confusion());
	MKF1.Add(MW1.Wire());
	TopoDS_Face FP = MKF1.Face();
	BRepLib::BuildCurves3d(FP);
	BRepFeat_MakeRevol MKrev(S,FP,F1,D,1,Standard_True);
	Ex.Next();
	Ex.Next();
	TopoDS_Face F2 = TopoDS::Face(Ex.Current());
	MKrev.Perform(F2);
	TopoDS_Shape res1 = MKrev.Shape();


	myAISContext->Remove (ais1, Standard_False);
	Handle(AIS_Shape) ais2 = new AIS_Shape(res1);
	myAISContext->Display(ais2,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO2 = ais2;
	myAISContext->SetSelected (anIO2, Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
Ex.Next();\n\
TopoDS_Face F1 = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);\n\
Handle (Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);\n\
gp_Ax1 D = gp::OX();\n\
BRepBuilderAPI_MakeWire MW1;\n\
gp_Pnt2d p1,p2;\n\
p1 = gp_Pnt2d(100.,100.);\n\
p2 = gp_Pnt2d(200.,100.);\n\
Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(150.,200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF1;\n\
MKF1.Init(surf,Standard_False);\n\
MKF1.Add(MW1.Wire());\n\
TopoDS_Face FP = MKF1.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
BRepFeat_MakeRevol MKrev(S,FP,F1,D,1,Standard_True);\n\
Ex.Next();\n\
TopoDS_Face F2 = TopoDS::Face(Ex.Current());\n\
MKrev.Perform(F2);\n\
TopoDS_Shape res1 = MKrev.Shape();\n\
	\n");
  PocessTextInDialog("Make a local revolution", Message);
}

void CModelingDoc::OnGlueLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(gp_Pnt(-500., -500., 0.), gp_Pnt(-100., -250., 300.)).Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
	myAISContext->SetColor(ais1,Quantity_NOC_ORANGE,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(1000);
	
	TopExp_Explorer Ex1;
	Ex1.Init(S1,TopAbs_FACE);
	Ex1.Next();
	Ex1.Next();
	Ex1.Next();
	Ex1.Next();
	Ex1.Next();
	TopoDS_Face F1 = TopoDS::Face(Ex1.Current());
  TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(-400., -400., 300.), gp_Pnt(-200., -300., 500.)).Shape();
	Handle(AIS_Shape) ais2 = new AIS_Shape(S2);

	myAISContext->SetColor(ais2,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais2,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO2 = ais2;
	myAISContext->SetSelected (anIO2, Standard_False);
	Fit();
	Sleep(1000);

	TopExp_Explorer Ex2;
	Ex2.Init(S2,TopAbs_FACE);
	Ex2.Next();
	Ex2.Next();
	Ex2.Next();
	Ex2.Next();
	TopoDS_Face F2 = TopoDS::Face(Ex2.Current());
	BRepFeat_Gluer glue(S2,S1);
	glue.Bind(F2,F1);
	TopoDS_Shape res1 = glue.Shape();
	myAISContext->Erase(ais2,Standard_False);
	
	ais1->Set(res1);

	myAISContext->Redisplay (ais1, Standard_False);
	myAISContext->SetSelected(anIO1,Standard_False);
	Fit();
	Sleep(1000);

  TopoDS_Shape S3 = BRepPrimAPI_MakeBox(500., 400., 300.).Shape();

	Handle(AIS_Shape) ais3 = new AIS_Shape(S3);
	myAISContext->SetColor(ais3,Quantity_NOC_ORANGE,Standard_False); 
	myAISContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais3,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO3 = ais3;
	myAISContext->SetSelected (anIO3, Standard_False);
	Fit();
	Sleep(1000);

	TopExp_Explorer Ex3;
	Ex3.Init(S3,TopAbs_FACE);
	Ex3.Next();
	Ex3.Next();
	Ex3.Next();
	Ex3.Next();
	Ex3.Next();
	TopoDS_Face F3 = TopoDS::Face(Ex3.Current());
  TopoDS_Shape S4 = BRepPrimAPI_MakeBox(gp_Pnt(0., 0., 300.), gp_Pnt(200., 200., 500.)).Shape();

	Handle(AIS_Shape) ais4 = new AIS_Shape(S4);
	myAISContext->SetColor(ais4,Quantity_NOC_AZURE,Standard_False); 
	myAISContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais4,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO4 = ais4;
	myAISContext->SetSelected (anIO4, Standard_False);
	Fit();
	Sleep(1000);

	TopExp_Explorer Ex4;
	Ex4.Init(S4,TopAbs_FACE);
	Ex4.Next();
	Ex4.Next();
	Ex4.Next();
	Ex4.Next();
	TopoDS_Face F4 = TopoDS::Face(Ex4.Current());
	BRepFeat_Gluer glue2(S4,S3);
	glue2.Bind(F4,F3);
	LocOpe_FindEdges CommonEdges(F4,F3);
	for (CommonEdges.InitIterator(); CommonEdges.More(); CommonEdges.Next()) 
		glue2.Bind(CommonEdges.EdgeFrom(),CommonEdges.EdgeTo());
	TopoDS_Shape res2 = glue2.Shape();
	myAISContext->Erase(ais3,Standard_False);
	
	ais4->Set(res2);

	myAISContext->Redisplay(ais4,Standard_False);
	myAISContext->SetSelected(anIO4,Standard_False);
	Fit();
	Sleep(1000);

	TCollection_AsciiString Message ("\
	\n\
--- Without common edges ---\n\
	\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(gp_Pnt(-500.,-500.,0.),gp_Pnt(-100.,-250.,300.));\n\
TopExp_Explorer Ex1;\n\
Ex1.Init(S1,TopAbs_FACE);\n\
Ex1.Next();\n\
Ex1.Next();\n\
Ex1.Next();\n\
Ex1.Next();\n\
Ex1.Next();\n\
TopoDS_Face F1 = TopoDS::Face(Ex1.Current());\n\
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(-400.,-400.,300.),gp_Pnt(-200.,-300.,500.));\n\
TopExp_Explorer Ex2;\n\
Ex2.Init(S2,TopAbs_FACE);\n\
Ex2.Next();\n\
Ex2.Next();\n\
Ex2.Next();\n\
Ex2.Next();\n\
TopoDS_Face F2 = TopoDS::Face(Ex2.Current());\n\
BRepFeat_Gluer glue(S2,S1);\n\
glue.Bind(F2,F1);\n\
TopoDS_Shape res1 = glue.Shape();\n\
\n\
--- With common edges ---\n\
\n\
TopoDS_Shape S3 = BRepPrimAPI_MakeBox(500.,400.,300.);\n\
TopExp_Explorer Ex3;\n\
Ex3.Init(S3,TopAbs_FACE);\n\
Ex3.Next();\n\
Ex3.Next();\n\
Ex3.Next();\n\
Ex3.Next();\n\
Ex3.Next();\n\
TopoDS_Face F3 = TopoDS::Face(Ex3.Current());\n\
TopoDS_Shape S4 = BRepPrimAPI_MakeBox(gp_Pnt(0.,0.,300.),gp_Pnt(200.,200.,500.));\n\
TopExp_Explorer Ex4;\n\
Ex4.Init(S4,TopAbs_FACE);\n\
Ex4.Next();\n\
Ex4.Next();\n\
Ex4.Next();\n\
Ex4.Next();\n\
TopoDS_Face F4 = TopoDS::Face(Ex4.Current());\n\
BRepFeat_Gluer glue2(S4,S3);\n\
glue2.Bind(F4,F3);\n\
LocOpe_FindEdges CommonEdges(F4,F3);\n\
for (CommonEdges.InitIterator(); CommonEdges.More(); CommonEdges.Next()) \n\
	glue2.Bind(CommonEdges.EdgeFrom(),CommonEdges.EdgeTo());\n\
TopoDS_Shape res2 = glue2.Shape();\n\
	\n");
  PocessTextInDialog("Glue two solids", Message);
}


void CModelingDoc::OnPipeLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
  TopoDS_Shape S = BRepPrimAPI_MakeBox(400., 250., 300.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);

	myAISContext->SetColor(ais1,Quantity_NOC_CORAL,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopExp_Explorer Ex;
	Ex.Init(S,TopAbs_FACE);
	Ex.Next();
	Ex.Next();
	TopoDS_Face F1 = TopoDS::Face(Ex.Current());
	Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);
	BRepBuilderAPI_MakeWire MW1;
	gp_Pnt2d p1,p2;
	p1 = gp_Pnt2d(100.,100.);
	p2 = gp_Pnt2d(200.,100.);
	Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(150.,200.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	p1 = p2;
	p2 = gp_Pnt2d(100.,100.);
	aline = GCE2d_MakeLine(p1,p2).Value();
	MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));
	BRepBuilderAPI_MakeFace MKF1;
	MKF1.Init(surf,Standard_False, Precision::Confusion());
	MKF1.Add(MW1.Wire());
	TopoDS_Face FP = MKF1.Face();
	BRepLib::BuildCurves3d(FP);
	TColgp_Array1OfPnt CurvePoles(1,3);
	gp_Pnt pt = gp_Pnt(150.,0.,150.);
	CurvePoles(1) = pt;
	pt = gp_Pnt(200.,-100.,150.);
	CurvePoles(2) = pt;
	pt = gp_Pnt(150.,-200.,150.);
	CurvePoles(3) = pt;
	Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);
	TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);
	BRepFeat_MakePipe MKPipe(S,FP,F1,W,1,Standard_True);
	MKPipe.Perform();
	TopoDS_Shape res1 = MKPipe.Shape();
	ais1->Set(res1);

	myAISContext->Redisplay(ais1,Standard_False); 
	myAISContext->SetSelected(anIO1,Standard_False);
	Fit();
	
	TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.);\n\
TopExp_Explorer Ex;\n\
Ex.Init(S,TopAbs_FACE);\n\
Ex.Next();\n\
Ex.Next();\n\
TopoDS_Face F1 = TopoDS::Face(Ex.Current());\n\
Handle(Geom_Surface) surf = BRep_Tool::Surface(F1);\n\
BRepBuilderAPI_MakeWire MW1;\n\
gp_Pnt2d p1,p2;\n\
p1 = gp_Pnt2d(100.,100.);\n\
p2 = gp_Pnt2d(200.,100.);\n\
Handle(Geom2d_Line) aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(150.,200.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
p1 = p2;\n\
p2 = gp_Pnt2d(100.,100.);\n\
aline = GCE2d_MakeLine(p1,p2).Value();\n\
MW1.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,p1.Distance(p2)));\n\
BRepBuilderAPI_MakeFace MKF1;\n\
MKF1.Init(surf,Standard_False);\n\
TopoDS_Face FP = MKF1.Face();\n\
BRepLib::BuildCurves3d(FP);\n\
TColgp_Array1OfPnt CurvePoles(1,3);\n\
gp_Pnt pt = gp_Pnt(150.,0.,150.);\n\
CurvePoles(1) = pt;\n\
pt = gp_Pnt(200.,-100.,150.);\n\
CurvePoles(2) = pt;\n\
pt = gp_Pnt(150.,-200.,150.);\n\
CurvePoles(3) = pt;\n\
Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(CurvePoles);\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);\n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E);\n\
BRepFeat_MakePipe MKPipe(S,FP,F1,W,1,Standard_True);\n\
MKPipe.Perform();\n\
TopoDS_Shape res1 = MKPipe.Shape();\n\
	\n");
  PocessTextInDialog("Make a local pipe", Message);
}


void CModelingDoc::OnLinearLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	BRepBuilderAPI_MakeWire mkw;
	gp_Pnt p1 = gp_Pnt(0.,0.,0.);
	gp_Pnt p2 = gp_Pnt(200.,0.,0.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(200.,0.,50.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(50.,0.,50.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(50.,0.,200.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	p2 = gp_Pnt(0.,0.,200.);
	mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));
	p1 = p2;
	mkw.Add(BRepBuilderAPI_MakeEdge(p2,gp_Pnt(0.,0.,0.)));
	
	TopoDS_Shape S = BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(mkw.Wire()), 
									   gp_Vec(gp_Pnt(0.,0.,0.),gp_Pnt(0.,100.,0.)));

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_CYAN2,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	TopoDS_Wire W = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(gp_Pnt(50.,45.,100.),
													  gp_Pnt(100.,45.,50.)));	
	Handle(Geom_Plane) aplane = new Geom_Plane(0.,1.,0.,-45.);
	BRepFeat_MakeLinearForm aform(S, W, aplane, gp_Vec(0.,10.,0.), gp_Vec(0.,0.,0.),
								  1, Standard_True);
	aform.Perform(/*10.*/); // new in 2.0

	TopoDS_Shape res = aform.Shape();
	ais1->Set(res);
	myAISContext->Redisplay(ais1,Standard_False);
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
	\n\
BRepBuilderAPI_MakeWire mkw;\n\
gp_Pnt p1 = gp_Pnt(0.,0.,0.);\n\
gp_Pnt p2 = gp_Pnt(200.,0.,0.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(200.,0.,50.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(50.,0.,50.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(50.,0.,200.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
p2 = gp_Pnt(0.,0.,200.);\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p1,p2));\n\
p1 = p2;\n\
mkw.Add(BRepBuilderAPI_MakeEdge(p2,gp_Pnt(0.,0.,0.)));\n\
TopoDS_Shape S = BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(mkw.Wire()), \n\
								gp_Vec(gp_Pnt(0.,0.,0.),gp_Pnt(0.,100.,0.)));\n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(gp_Pnt(50.,45.,100.),\n\
												gp_Pnt(100.,45.,50.)));\n\
Handle(Geom_Plane) aplane = new Geom_Plane(0.,1.,0.,-45.);\n\
BRepFeat_MakeLinearForm aform(S, W, aplane, gp_Dir(0.,10.,0.), gp_Dir(0.,0.,0.),\n\
							1, Standard_True);\n\
aform.Perform(10.);\n\
TopoDS_Shape res = aform.Shape();\n\
	\n");
  PocessTextInDialog("Make a rib", Message);
}


void CModelingDoc::OnSplitLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(gp_Pnt(-100, -60, -80), 150, 200, 170).Shape();
	
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(500);

	BRepAlgoAPI_Section asect(S, gp_Pln(1,2,1,-15),Standard_False);
	asect.ComputePCurveOn1(Standard_True);
	asect.Approximation(Standard_True);
	asect.Build();
	TopoDS_Shape R = asect.Shape();

	BRepFeat_SplitShape asplit(S);
	
	for (TopExp_Explorer Ex(R,TopAbs_EDGE); Ex.More(); Ex.Next()) {
		TopoDS_Shape anEdge = Ex.Current();
		TopoDS_Shape aFace;
		if (asect.HasAncestorFaceOn1(anEdge,aFace)) {
			TopoDS_Face F = TopoDS::Face(aFace);
			TopoDS_Edge E = TopoDS::Edge(anEdge);
			asplit.Add(E,F);
		}
	}

	asplit.Build();

	//Sleep(1000);
	myAISContext->Erase(ais1,Standard_False);
	//Fit();

	TopoDS_Shape Result = asplit.Shape();
	
	Handle(AIS_Shape) ais2 = new AIS_Shape(Result);

	myAISContext->SetColor(ais2,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->SetDisplayMode(ais2,1,Standard_False);
	myAISContext->Display(ais2,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO2 = ais2;
	myAISContext->SetSelected (anIO2, Standard_False);
	Fit();
		TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(gp_Pnt(-100,-60,-80),150,200,170); 	\n\
		\n\
BRepBuilderAPI_Section asect(S, gp_Pln(1,2,1,-15),Standard_False);	\n\
asect.ComputePCurveOn1(Standard_True);	\n\
asect.Approximation(Standard_True);	\n\
asect.Build();	\n\
TopoDS_Shape R = asect.Shape();	\n\
	\n\
BRepFeat_SplitShape asplit(S);	\n\
	\n\
for (TopExp_Explorer Ex(R,TopAbs_EDGE); Ex.More(); Ex.Next()) {	\n\
TopoDS_Shape anEdge = Ex.Current();	\n\
	TopoDS_Shape aFace;	\n\
	if (asect.HasAncestorFaceOn1(anEdge,aFace)) {	\n\
		TopoDS_Face F = TopoDS::Face(aFace);	\n\
		TopoDS_Edge E = TopoDS::Edge(anEdge);	\n\
		asplit.Add(E,F);	\n\
	}	\n\
}	\n\
	\n\
asplit.Build();	\n\
	\n\
TopoDS_Shape Result = asplit.Shape();	\n\
	\n\
\n");

PocessTextInDialog("Split a shape", Message);
}



void CModelingDoc::OnThickLocal() 
{
	AIS_ListOfInteractive L;
	myAISContext->DisplayedObjects(L);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(L);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150, 200, 110).Shape();
	
	Handle(AIS_Shape) abox1 = new AIS_Shape(S1);
	myAISContext->SetColor (abox1, Quantity_NOC_WHITE, Standard_False);
	myAISContext->SetMaterial(abox1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(abox1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIOBox1 = abox1;
	myAISContext->SetSelected (anIOBox1, Standard_False);
	Fit();
	Sleep(1000);

	TopTools_ListOfShape aList;
	TopExp_Explorer Ex(S1,TopAbs_FACE);
	Ex.Next();	//this is the front face
	TopoDS_Shape aFace = Ex.Current();
	aList.Append(aFace);
	
    BRepOffsetAPI_MakeThickSolid aSolidMaker;
    aSolidMaker.MakeThickSolidByJoin(S1,aList,10,0.01);
	TopoDS_Shape aThickSolid = aSolidMaker.Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(aThickSolid);
	myAISContext->SetColor(ais1,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	const Handle(AIS_InteractiveObject)& anIO1 = ais1;
	myAISContext->SetSelected (anIO1, Standard_False);
	Fit();
	Sleep(1000);
	
	myAISContext->Erase(abox1,Standard_True);
	Fit();


		TCollection_AsciiString Message ("\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(150,200,110);	\n\
	\n\
TopTools_ListOfShape aList;	\n\
TopExp_Explorer Ex(S,TopAbs_FACE);	\n\
Ex.Next();	//in order to recover the front face	\n\
TopoDS_Shape aFace = Ex.Current();	\n\
aList.Append(aFace);	\n\
			\n\
TopoDS_Shape aThickSolid = BRepPrimAPI_MakeThickSolid(S,aList,15,0.01);	\n\
	\n\
\n");
PocessTextInDialog("Make a thick solid", Message);
}

void CModelingDoc::OnOffsetLocal() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150, 200, 110).Shape();

	Handle(AIS_Shape) aisBox1 = new AIS_Shape(S1);
	myAISContext->SetColor(aisBox1,Quantity_NOC_BROWN,Standard_False);
	myAISContext->SetMaterial(aisBox1,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->Display(aisBox1,Standard_False);
	Fit();
	Sleep(500);

    BRepOffsetAPI_MakeOffsetShape aShapeMaker1;
    aShapeMaker1.PerformByJoin(S1,60,0.01);
	TopoDS_Shape anOffsetShape1 = aShapeMaker1.Shape();

	Handle(AIS_Shape) ais1 = new AIS_Shape(anOffsetShape1);
	myAISContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False);
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->SetTransparency(ais1,0.5,Standard_False);
	myAISContext->Display(ais1,Standard_False);
	Fit();
	Sleep(500);

  TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(500, 0, 0), 220, 140, 180).Shape();
	
	Handle(AIS_Shape) aisBox2 = new AIS_Shape(S2);
	myAISContext->SetColor(aisBox2,Quantity_NOC_WHITE,Standard_False);
	myAISContext->SetMaterial(aisBox2,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->SetTransparency(aisBox2,0.5,Standard_False);
	myAISContext->Display(aisBox2,Standard_False);
	Fit();
	Sleep(500);

    BRepOffsetAPI_MakeOffsetShape aShapeMaker2;
    aShapeMaker2.PerformByJoin(S2,-40,0.01,
      BRepOffset_Skin,Standard_False,Standard_False,GeomAbs_Arc);
	TopoDS_Shape anOffsetShape2 = aShapeMaker2.Shape();

	Handle(AIS_Shape) ais2 = new AIS_Shape(anOffsetShape2);
	myAISContext->SetColor (ais2, Quantity_NOC_MATRABLUE, Standard_False);
	myAISContext->SetMaterial(ais2,Graphic3d_NOM_GOLD,Standard_False);
	myAISContext->Display (ais2, Standard_False);
	Fit();
	
	TCollection_AsciiString Message ("\
	\n\
\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150,200,110);	\n\
\n\
TopoDS_Shape anOffsetShape1 = BRepPrimAPI_MakeOffsetShape(S1,60,0.01);	\n\
\n\
//The white box	\n\
	\n\
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(300,0,0),220,140,180);	\n\
\n\
TopoDS_Shape anOffsetShape2 = BRepPrimAPI_MakeOffsetShape(S2,-20,0.01,	\n\
	BRepOffset_Skin,Standard_False,Standard_False,GeomAbs_Arc);	\n\
	\n\
\n\
\n");
PocessTextInDialog("Make an offset shape", Message);

}

/* =================================================================================
   ====================   B U I L D I N G   ========================================
   ================================================================================= */


void CModelingDoc::OnVertex() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Vertex V1,V2,V3;
	
	V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0,0,0));
	V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,25));
	
	gp_Pnt P(-12,8,-4);
	BRepBuilderAPI_MakeVertex MV(P);
	V3 = MV.Vertex();

	Handle(AIS_Shape) Point1 = new AIS_Shape(V1);
	myAISContext->Display(Point1,Standard_False);
	Handle(AIS_Shape) Point2 = new AIS_Shape(V2);
	myAISContext->Display(Point2,Standard_False);
	Handle(AIS_Shape) Point3 = new AIS_Shape(V3);
	myAISContext->Display(Point3,Standard_False);

	Fit();

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Vertex V1,V2,V3;	\n\
		\n\
V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0,0,0));	\n\
	\n\
V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,25));	\n\
		\n\
gp_Pnt P(-12,8,-4);	\n\
BRepBuilderAPI_MakeVertex MV(P);	\n\
V3 = MV.Vertex();	\n\
	\n\
\n");

	PocessTextInDialog("Make vertex from point ", Message);

}

void CModelingDoc::OnEdge() 
{
	
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	
	
	TopoDS_Edge BlueEdge,YellowEdge,WhiteEdge,RedEdge,GreenEdge;
	TopoDS_Vertex V1,V2,V3,V4;
	
/////////////The blue edge

	BlueEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(-80,-50,-20),gp_Pnt(-30,-60,-60));

/////////////The yellow edge
	
	V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));
	V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,-25));
	YellowEdge = BRepBuilderAPI_MakeEdge(V1,V2);

/////////////The white edge
	
	gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));
	WhiteEdge = BRepBuilderAPI_MakeEdge(line,-20,10);

//////////////The red edge

	gp_Elips Elips(gp_Ax2(gp_Pnt(10,0,0),gp_Dir(1,1,1)),60,30);
	RedEdge = BRepBuilderAPI_MakeEdge(Elips,0,M_PI/2);

/////////////The green edge and the both extreme vertex

	gp_Pnt P1(-15,200,10);
	gp_Pnt P2(5,204,0);
	gp_Pnt P3(15,200,0);
	gp_Pnt P4(-15,20,15);
	gp_Pnt P5(-5,20,0);
	gp_Pnt P6(15,20,0);
	gp_Pnt P7(24,120,0);
	gp_Pnt P8(-24,120,12.5);
	TColgp_Array1OfPnt array(1,8);
	array.SetValue(1,P1);
	array.SetValue(2,P2);
	array.SetValue(3,P3); 
	array.SetValue(4,P4); 
	array.SetValue(5,P5); 
	array.SetValue(6,P6); 
	array.SetValue(7,P7); 
	array.SetValue(8,P8); 
	Handle (Geom_BezierCurve) curve = new Geom_BezierCurve(array);
	
	BRepBuilderAPI_MakeEdge ME (curve);
	GreenEdge = ME;
	V3 = ME.Vertex1();
	V4 = ME.Vertex2();

//////////////Display
Handle(AIS_Shape) blue = new AIS_Shape(BlueEdge);
myAISContext->SetColor(blue,Quantity_NOC_MATRABLUE,Standard_False); 
myAISContext->Display(blue,Standard_False);

Handle(AIS_Shape) yellow = new AIS_Shape(YellowEdge);
myAISContext->SetColor(yellow,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->Display(yellow,Standard_False);

Handle(AIS_Shape) white = new AIS_Shape(WhiteEdge);
myAISContext->SetColor(white,Quantity_NOC_WHITE,Standard_False); 
myAISContext->Display(white,Standard_False);

Handle(AIS_Shape) red = new AIS_Shape(RedEdge);
myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False); 
myAISContext->Display(red,Standard_False);

Handle(AIS_Shape) green = new AIS_Shape(GreenEdge);
myAISContext->SetColor(green,Quantity_NOC_GREEN,Standard_False); 
myAISContext->Display(green,Standard_False);

Handle(AIS_Shape) Point1 = new AIS_Shape(V3);
myAISContext->Display(Point1,Standard_False);
Handle(AIS_Shape) Point2 = new AIS_Shape(V4);
myAISContext->Display(Point2,Standard_False);

Fit();

   TCollection_AsciiString Message ("\
		\n\
TopoDS_Edge BlueEdge, YellowEdge, WhiteEdge, RedEdge, GreenEdge;	\n\
TopoDS_Vertex V1,V2,V3,V4;	\n\
		\n\
/////////////The blue edge	\n\
	\n\
BlueEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(-80,-50,-20),gp_Pnt(-30,-60,-60));	\n\
	\n\
/////////////The yellow edge	\n\
		\n\
V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));	\n\
V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,-25));	\n\
YellowEdge = BRepBuilderAPI_MakeEdge(V1,V2);	\n\
	\n\
/////////////The white edge	\n\
		\n\
gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));	\n\
WhiteEdge = BRepBuilderAPI_MakeEdge(line,-20,10);	\n\
	\n\
//////////////The red edge	\n\
	\n\
gp_Elips Elips(gp_Ax2(gp_Pnt(10,0,0),gp_Dir(1,1,1)),60,30);	\n\
RedEdge = BRepBuilderAPI_MakeEdge(Elips,0,PI/2);	\n\
	\n\
/////////////The green edge and the both extreme vertex	\n\
	\n\
gp_Pnt P1(-15,200,10);	\n\
gp_Pnt P2(5,204,0);	\n\
gp_Pnt P3(15,200,0);	\n\
gp_Pnt P4(-15,20,15);	\n\
gp_Pnt P5(-5,20,0);	\n\
gp_Pnt P6(15,20,0);	\n\
gp_Pnt P7(24,120,0);	\n\
gp_Pnt P8(-24,120,12.5);	\n\
TColgp_Array1OfPnt array(1,8);	\n\
array.SetValue(1,P1);	\n\
array.SetValue(2,P2);	\n\
array.SetValue(3,P3); 	\n\
array.SetValue(4,P4); 	\n\
array.SetValue(5,P5); 	\n\
array.SetValue(6,P6); 	\n\
array.SetValue(7,P7); 	\n\
array.SetValue(8,P8); 	\n\
Handle (Geom_BezierCurve) curve = new Geom_BezierCurve(array);	\n\
	\n\
BRepBuilderAPI_MakeEdge ME (curve);	\n\
GreenEdge = ME;	\n\
V3 = ME.Vertex1();	\n\
V4 = ME.Vertex2();	\n\
	\n\
\n");

	PocessTextInDialog("Make edge", Message);
	
}

void CModelingDoc::OnWire() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TopoDS_Wire RedWire,YellowWire,WhiteWire,
		ExistingWire, ExistingWire2;

	TopoDS_Edge Edge1,Edge2,Edge3,Edge4,Edge5,Edge6,Edge7,LastEdge;
	TopoDS_Vertex LastVertex;

////////////The red wire is build from a single edge

	gp_Elips Elips(gp_Ax2(gp_Pnt(250,0,0),gp_Dir(1,1,1)),160,90);
	Edge1 = BRepBuilderAPI_MakeEdge(Elips,0,M_PI/2);

	RedWire = BRepBuilderAPI_MakeWire(Edge1);

///////////the yellow wire is build from an existing wire and an edge
	
	gp_Circ circle(gp_Ax2(gp_Pnt(-300,0,0),gp_Dir(1,0,0)),80);
	Edge2 = BRepBuilderAPI_MakeEdge(circle,0,M_PI);

	ExistingWire = BRepBuilderAPI_MakeWire(Edge2);

	Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-300,0,-80),gp_Pnt(-90,20,-30));

	BRepBuilderAPI_MakeWire MW1(ExistingWire,Edge3);
	if (MW1.IsDone()) {
			YellowWire = MW1;
	}


//////////the white wire is built with an existing wire and 3 edges.
//////////we use the methods Add, Edge and Vertex from BRepBuilderAPI_MakeWire.

	gp_Circ circle2(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,1,0)),200);
	Edge4 = BRepBuilderAPI_MakeEdge(circle2,0,M_PI);

	ExistingWire2 = BRepBuilderAPI_MakeWire(Edge4);

	gp_Pnt P1(0,0,-200);
	gp_Pnt P2(5,204,0);
	Edge5 = BRepBuilderAPI_MakeEdge(P1,P2);

	gp_Pnt P3(-15,20,15);
	Edge6 = BRepBuilderAPI_MakeEdge(P2,P3);
	gp_Pnt P4(15,20,0);	
	Edge7 = BRepBuilderAPI_MakeEdge(P3,P4);

	BRepBuilderAPI_MakeWire MW;
	MW.Add(ExistingWire2);
	MW.Add(Edge5);
	MW.Add(Edge6);
	MW.Add(Edge7);

	if (MW.IsDone()) {
		WhiteWire = MW.Wire();
		LastEdge = MW.Edge();
		LastVertex = MW.Vertex();
	}


Handle(AIS_Shape) red = new AIS_Shape(RedWire);
myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False); 
myAISContext->Display(red,Standard_False);

Handle(AIS_Shape) yellow = new AIS_Shape(YellowWire);
myAISContext->SetColor(yellow,Quantity_NOC_YELLOW,Standard_False); 
myAISContext->Display(yellow,Standard_False);

Handle(AIS_Shape) white = new AIS_Shape(WhiteWire);
myAISContext->SetColor(white,Quantity_NOC_WHITE,Standard_False); 
myAISContext->Display(white,Standard_False);

Handle(AIS_Shape) lastE = new AIS_Shape(LastEdge);
myAISContext->SetWidth(lastE,3,Standard_False);
myAISContext->SetColor(lastE,Quantity_NOC_RED,Standard_False); 
myAISContext->Display(lastE,Standard_False);

Handle(AIS_Shape) lastV = new AIS_Shape(LastVertex);
myAISContext->Display(lastV,Standard_False);

Fit();

   TCollection_AsciiString Message ("\
	\n\
TopoDS_Wire RedWire,YellowWire,WhiteWire,	\n\
ExistingWire, ExistingWire2;	\n\
	\n\
TopoDS_Edge Edge1,Edge2,Edge3,Edge4,Edge5,Edge6,Edge7,LastEdge;	\n\
TopoDS_Vertex LastVertex;	\n\
	\n\
////////////The red wire is build from a single edge	\n\
	\n\
gp_Elips Elips(gp_Ax2(gp_Pnt(10,0,0),gp_Dir(1,1,1)),160,90);	\n\
Edge1 = BRepBuilderAPI_MakeEdge(Elips,0,PI/2);	\n\
	\n\
RedWire = BRepBuilderAPI_MakeWire(Edge1);	\n\
	\n\
///////////the yellow wire is build from an existing wire and an edge	\n\
		\n\
gp_Circ circle(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(1,0,0)),80);	\n\
Edge2 = BRepBuilderAPI_MakeEdge(circle,0,PI);	\n\
	\n\
ExistingWire = BRepBuilderAPI_MakeWire(Edge2);	\n\
	\n\
Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,-80),gp_Pnt(90,20,30));	\n\
	\n\
BRepBuilderAPI_MakeWire MW1(ExistingWire,Edge3);	\n\
if (MW1.IsDone()) {	\n\
		YellowWire = MW1;	\n\
}	\n\
	\n\
///the white wire is built with an existing wire and 3 edges.	\n\
///we use the methods Add, Edge and Vertex from BRepBuilderAPI_MakeWire	\n\
///in order to display the last edge and the last vertices we	\n\
///add to the wire. 	\n\
	\n\
gp_Circ circle2(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,1,0)),200);	\n\
Edge4 = BRepBuilderAPI_MakeEdge(circle2,0,PI);	\n\
	\n\
ExistingWire2 = BRepBuilderAPI_MakeWire(Edge4);	\n\
	\n\
gp_Pnt P1(0,0,-200);	\n\
gp_Pnt P2(5,204,0);	\n\
Edge5 = BRepBuilderAPI_MakeEdge(P1,P2);	\n\
	\n\
gp_Pnt P3(-15,20,15);	\n\
Edge6 = BRepBuilderAPI_MakeEdge(P2,P3);	\n\
gp_Pnt P4(15,20,0);		\n\
Edge7 = BRepBuilderAPI_MakeEdge(P3,P4);	\n\
	\n\
BRepBuilderAPI_MakeWire MW;	\n\
MW.Add(ExistingWire2);	\n\
MW.Add(Edge5);	\n\
MW.Add(Edge6);	\n\
MW.Add(Edge7);	\n\
	\n\
if (MW.IsDone()) {	\n\
	WhiteWire = MW.Wire();	\n\
	LastEdge = MW.Edge();	\n\
	LastVertex = MW.Vertex();	\n\
}	\n\
	\n\
\n");

	PocessTextInDialog("Make wire ", Message);
}

void CModelingDoc::OnFace() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}



	TopoDS_Face WhiteFace, BrownFace, RedFace, PinkFace;
	TopoDS_Edge Edge1, Edge2, Edge3, Edge4, Edge5, Edge6, Edge7;
	TopoDS_Wire Wire1;
	gp_Pnt P1, P2, P3, P4, P5, P6, P7;

	gp_Sphere sphere (gp_Ax3(gp_Pnt(0,0,0),gp_Dir(1,0,0)),150);

	WhiteFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);

//////////////////////////////////

	P1.SetCoord(-15,200,10);
	P2.SetCoord(5,204,0);
	P3.SetCoord(15,200,0);
	P4.SetCoord(-15,20,15);
	P5.SetCoord(-5,20,0);
	P6.SetCoord(15,20,35);
	TColgp_Array2OfPnt array(1,3,1,2);
	array.SetValue(1,1,P1);
	array.SetValue(2,1,P2);
	array.SetValue(3,1,P3); 
	array.SetValue(1,2,P4); 
	array.SetValue(2,2,P5); 
	array.SetValue(3,2,P6);
	Handle (Geom_BSplineSurface) curve = GeomAPI_PointsToBSplineSurface(array,3,8,GeomAbs_C2,0.001);

	RedFace = BRepBuilderAPI_MakeFace(curve, Precision::Confusion());

////////////////////

	gp_Circ circle(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(1,0,0)),80);
	Edge1 = BRepBuilderAPI_MakeEdge(circle,0,M_PI);

	Edge2 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,-80),gp_Pnt(0,-10,40));
	Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,-10,40),gp_Pnt(0,0,80));

	TopoDS_Wire YellowWire;
	BRepBuilderAPI_MakeWire MW1(Edge1,Edge2,Edge3);
	if (MW1.IsDone()) {
			YellowWire = MW1;
	}

	BrownFace = BRepBuilderAPI_MakeFace(YellowWire);


/////////////

	P1.SetCoord(35,-200,40);
	P2.SetCoord(50,-204,30);
	P3.SetCoord(65,-200,30);
	P4.SetCoord(35,-20,45);
	P5.SetCoord(45,-20,30);
	P6.SetCoord(65,-20,65);
	TColgp_Array2OfPnt array2(1,3,1,2);
	array2.SetValue(1,1,P1);
	array2.SetValue(2,1,P2);
	array2.SetValue(3,1,P3); 
	array2.SetValue(1,2,P4); 
	array2.SetValue(2,2,P5); 
	array2.SetValue(3,2,P6);
	
	Handle (Geom_BSplineSurface) BSplineSurf = GeomAPI_PointsToBSplineSurface(array2,3,8,GeomAbs_C2,0.001);
	
	TopoDS_Face aFace = BRepBuilderAPI_MakeFace(BSplineSurf, Precision::Confusion());

	//2d lines
	gp_Pnt2d P12d(0.9,0.1);
	gp_Pnt2d P22d(0.2,0.7);
	gp_Pnt2d P32d(0.02,0.1);

	Handle (Geom2d_Line) line1 = new Geom2d_Line(P12d,gp_Dir2d((0.2-0.9),(0.7-0.1)));
	Handle (Geom2d_Line) line2 = new Geom2d_Line(P22d,gp_Dir2d((0.02-0.2),(0.1-0.7)));
	Handle (Geom2d_Line) line3 = new Geom2d_Line(P32d,gp_Dir2d((0.9-0.02),(0.1-0.1)));


	//Edges are on the BSpline surface
	Edge1 = BRepBuilderAPI_MakeEdge(line1,BSplineSurf,0,P12d.Distance(P22d));
	Edge2 = BRepBuilderAPI_MakeEdge(line2,BSplineSurf,0,P22d.Distance(P32d));
	Edge3 = BRepBuilderAPI_MakeEdge(line3,BSplineSurf,0,P32d.Distance(P12d));

	Wire1 = BRepBuilderAPI_MakeWire(Edge1,Edge2,Edge3);
	Wire1.Reverse();
	PinkFace = BRepBuilderAPI_MakeFace(aFace,Wire1);
	BRepLib::BuildCurves3d(PinkFace);

/////////////Display
	Handle(AIS_Shape) white = new AIS_Shape(WhiteFace);
	myAISContext->SetColor(white,Quantity_NOC_WHITE,Standard_False);
	myAISContext->SetMaterial(white,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(white,Standard_False);
	
	Handle(AIS_Shape) red = new AIS_Shape(RedFace);
	myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(red,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(red,Standard_False);
	
	Handle(AIS_Shape) brown = new AIS_Shape(BrownFace);
	myAISContext->SetColor(brown,Quantity_NOC_BROWN,Standard_False);
	myAISContext->SetMaterial(brown,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(brown,Standard_False);
	
	Handle(AIS_Shape) pink = new AIS_Shape(PinkFace);
	myAISContext->SetColor(pink,Quantity_NOC_HOTPINK,Standard_False);
	myAISContext->SetMaterial(pink,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(pink,Standard_False);

	Fit();


  TCollection_AsciiString Message ("\
 	\n\
TopoDS_Face WhiteFace, BrownFace, RedFace, PinkFace;	\n\
TopoDS_Edge Edge1, Edge2, Edge3, Edge4, Edge5, Edge6, Edge7;	\n\
TopoDS_Wire Wire1;	\n\
gp_Pnt P1, P2, P3, P4, P5, P6, P7;	\n\
\n\
////////The white Face \n\
\n\
gp_Sphere sphere (gp_Ax3(gp_Pnt(0,0,0),gp_Dir(1,0,0)),150);	\n\
\n\
WhiteFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);	\n\
\n\
////////The red face	\n\
\n\
P1.SetCoord(-15,200,10);	\n\
P2.SetCoord(5,204,0);	\n\
P3.SetCoord(15,200,0);	\n\
P4.SetCoord(-15,20,15);	\n\
P5.SetCoord(-5,20,0);	\n\
P6.SetCoord(15,20,35);	\n\
TColgp_Array2OfPnt array(1,3,1,2);	\n\
array.SetValue(1,1,P1);	\n\
array.SetValue(2,1,P2);	\n\
array.SetValue(3,1,P3); 	\n\
array.SetValue(1,2,P4); 	\n\
array.SetValue(2,2,P5); 	\n\
array.SetValue(3,2,P6);	\n\
Handle (Geom_BSplineSurface) curve = GeomAPI_PointsToBSplineSurface(array,3,8,GeomAbs_C2,0.001);	\n\
\n\
RedFace = BRepBuilderAPI_MakeFace(curve);	\n\
\n\
////////The brown face	\n\
\n\
gp_Circ circle(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(1,0,0)),80);	\n\
Edge1 = BRepBuilderAPI_MakeEdge(circle,0,PI);	\n\
\n\
Edge2 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,0,-80),gp_Pnt(0,-10,40));	\n\
Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,-10,40),gp_Pnt(0,0,80));	\n\
\n\
TopoDS_Wire YellowWire;	\n\
BRepBuilderAPI_MakeWire MW1(Edge1,Edge2,Edge3);	\n\
if (MW1.IsDone()) {	\n\
		YellowWire = MW1;	\n\
}	\n\
\n\
BrownFace = BRepBuilderAPI_MakeFace(YellowWire);	\n\
\n");
Message +=("\
////////The pink face	\n\
\n\
P1.SetCoord(35,-200,40);	\n\
P2.SetCoord(50,-204,30);	\n\
P3.SetCoord(65,-200,30);	\n\
P4.SetCoord(35,-20,45);	\n\
P5.SetCoord(45,-20,30);	\n\
P6.SetCoord(65,-20,65);	\n\
TColgp_Array2OfPnt array2(1,3,1,2);	\n\
array2.SetValue(1,1,P1);	\n\
array2.SetValue(2,1,P2);	\n\
array2.SetValue(3,1,P3); 	\n\
array2.SetValue(1,2,P4); 	\n\
array2.SetValue(2,2,P5); 	\n\
array2.SetValue(3,2,P6);	\n\
	\n\
Handle (Geom_BSplineSurface) BSplineSurf = GeomAPI_PointsToBSplineSurface(array2,3,8,GeomAbs_C2,0.001);	\n\
	\n\
TopoDS_Face aFace = BRepBuilderAPI_MakeFace(BSplineSurf);	\n\
\n\
//2d lines	\n\
gp_Pnt2d P12d(0.9,0.1);	\n\
gp_Pnt2d P22d(0.2,0.7);	\n\
gp_Pnt2d P32d(0.02,0.1);	\n\
\n\
Handle (Geom2d_Line) line1=		\n\
	new Geom2d_Line(P12d,gp_Dir2d((0.2-0.9),(0.7-0.1)));	\n\
Handle (Geom2d_Line) line2=		\n\
	new Geom2d_Line(P22d,gp_Dir2d((0.02-0.2),(0.1-0.7)));   \n\
Handle (Geom2d_Line) line3=		\n\
	new Geom2d_Line(P32d,gp_Dir2d((0.9-0.02),(0.1-0.1)));	\n\
		\n\
//Edges are on the BSpline surface	\n\
Edge1 = BRepBuilderAPI_MakeEdge(line1,BSplineSurf,0,P12d.Distance(P22d));	\n\
Edge2 = BRepBuilderAPI_MakeEdge(line2,BSplineSurf,0,P22d.Distance(P32d));	\n\
Edge3 = BRepBuilderAPI_MakeEdge(line3,BSplineSurf,0,P32d.Distance(P12d));	\n\
\n\
Wire1 = BRepBuilderAPI_MakeWire(Edge1,Edge2,Edge3);	\n\
Wire1.Reverse();	\n\
PinkFace = BRepBuilderAPI_MakeFace(aFace,Wire1);	\n\
BRepLib::BuildCurves3d(PinkFace);	\n\
\n\
\n");

PocessTextInDialog("Make face ", Message);
}

void CModelingDoc::OnShell() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TColgp_Array2OfPnt Poles(1,2,1,4);
	Poles.SetValue(1,1,gp_Pnt(0,0,0));
	Poles.SetValue(1,2,gp_Pnt(0,10,2));
	Poles.SetValue(1,3,gp_Pnt(0,20,10)); 
	Poles.SetValue(1,4,gp_Pnt(0,30,0)); 
	Poles.SetValue(2,1,gp_Pnt(10,0,5));
	Poles.SetValue(2,2,gp_Pnt(10,10,3));
	Poles.SetValue(2,3,gp_Pnt(10,20,20));
	Poles.SetValue(2,4,gp_Pnt(10,30,0));

	TColStd_Array1OfReal UKnots(1,2);
	UKnots.SetValue(1,0);
	UKnots.SetValue(2,1);

	TColStd_Array1OfInteger UMults(1,2);
	UMults.SetValue(1,2);
	UMults.SetValue(2,2);

	TColStd_Array1OfReal VKnots(1,3);
	VKnots.SetValue(1,0);
	VKnots.SetValue(2,1);
	VKnots.SetValue(3,2);

	TColStd_Array1OfInteger VMults(1,3);
	VMults.SetValue(1,3);
	VMults.SetValue(2,1);
	VMults.SetValue(3,3);

	Standard_Integer UDegree(1);
	Standard_Integer VDegree(2);
	
	Handle (Geom_BSplineSurface) BSpline = new Geom_BSplineSurface(Poles,UKnots,VKnots,UMults,VMults,UDegree,VDegree);
	
	TopoDS_Face WhiteFace = BRepBuilderAPI_MakeFace(BSpline, Precision::Confusion());

	
	Handle(AIS_Shape) white = new AIS_Shape(WhiteFace);
	myAISContext->SetColor (white, Quantity_NOC_WHITE, Standard_False);
	myAISContext->SetMaterial(white,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->SetTransparency (white, 0.7, Standard_False);
	myAISContext->Display(white,Standard_False);
	

	TopoDS_Shell aShell = BRepBuilderAPI_MakeShell(BSpline);
	Handle(AIS_Shape) anAISShell = new AIS_Shape(aShell);
	myAISContext->SetDisplayMode (anAISShell, 0, Standard_False);
	myAISContext->Display(anAISShell,Standard_False);
	//myAISContext->SetSelected(anAISShell);

	Fit();
  
	TCollection_AsciiString Message ("\
	\n\
TColgp_Array2OfPnt Poles(1,2,1,4);	\n\
Poles.SetValue(1,1,gp_Pnt(0,0,0));	\n\
Poles.SetValue(1,2,gp_Pnt(0,10,2));	\n\
Poles.SetValue(1,3,gp_Pnt(0,20,10)); 	\n\
Poles.SetValue(1,4,gp_Pnt(0,30,0)); 	\n\
Poles.SetValue(2,1,gp_Pnt(10,0,5));	\n\
Poles.SetValue(2,2,gp_Pnt(10,10,3));	\n\
Poles.SetValue(2,3,gp_Pnt(10,20,20));	\n\
Poles.SetValue(2,4,gp_Pnt(10,30,0));	\n\
\n\
TColStd_Array1OfReal UKnots(1,2);	\n\
UKnots.SetValue(1,0);	\n\
UKnots.SetValue(2,1);	\n\
\n\
TColStd_Array1OfInteger UMults(1,2);	\n\
UMults.SetValue(1,2);	\n\
UMults.SetValue(2,2);	\n\
\n\
TColStd_Array1OfReal VKnots(1,3);	\n\
VKnots.SetValue(1,0);	\n\
VKnots.SetValue(2,1);	\n\
VKnots.SetValue(3,2);	\n\
\n\
TColStd_Array1OfInteger VMults(1,3);	\n\
VMults.SetValue(1,3);	\n\
VMults.SetValue(2,1);	\n\
VMults.SetValue(3,3);	\n\
\n\
Standard_Integer UDegree(1);	\n\
Standard_Integer VDegree(2);	\n\
	\n\
Handle (Geom_BSplineSurface) BSpline = new Geom_BSplineSurface(Poles,UKnots,VKnots,UMults,VMults,UDegree,VDegree);	\n\
	\n\
TopoDS_Shell aShell = BRepBuilderAPI_MakeShell(BSpline);	\n\
\n\
\n");

  PocessTextInDialog("Make shell", Message);
	
}

void CModelingDoc::OnCompound() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	BRep_Builder builder;
	TopoDS_Compound Comp;
	builder.MakeCompound(Comp);

	TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));
	builder.Add(Comp,aVertex);
	
	gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));
	TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(line,-20,10);
	builder.Add(Comp,anEdge);

	gp_Sphere sphere (gp_Ax3(gp_Pnt(-80,0,0),gp_Dir(1,0,0)),150);
	TopoDS_Face aFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);
	builder.Add(Comp,aFace);

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(-60, 0, 0), 30, 60, 40).Shape();
	builder.Add(Comp,aBox);

	Handle(AIS_Shape) white = new AIS_Shape(Comp);
	myAISContext->SetDisplayMode (white, 0, Standard_False);
	myAISContext->Display(white,Standard_False);

	Fit();


   TCollection_AsciiString Message ("\
		\n\
BRep_Builder builder;	\n\
TopoDS_Compound Comp;	\n\
builder.MakeCompound(Comp);	\n\
\n\
TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30));	\n\
builder.Add(Comp,aVertex);	\n\
	\n\
gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));	\n\
TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(line,-20,10);	\n\
builder.Add(Comp,anEdge);	\n\
	\n\
gp_Sphere sphere (gp_Ax3(gp_Pnt(-80,0,0),gp_Dir(1,0,0)),150);	\n\
TopoDS_Face aFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9);	\n\
builder.Add(Comp,aFace);	\n\
	\n\
TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(-60,0,0),30,60,40);	\n\
builder.Add(Comp,aBox);	\n\
	\n\
\n");

	PocessTextInDialog("Make compound ", Message);

}





void CModelingDoc::OnSewing() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	gp_Pnt P(0,0,0);
	gp_Vec V(0,0,1);
	Handle(Geom_Plane) Pi=new Geom_Plane(P,V);
	Handle(Geom_RectangularTrimmedSurface) GeometricSurface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);
	TopoDS_Shape FirstShape = BRepBuilderAPI_MakeFace(GeometricSurface, Precision::Confusion());
	
	Handle(AIS_Shape) white1 = new AIS_Shape(FirstShape);
	
	myAISContext->SetColor(white1,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(white1,Graphic3d_NOM_PLASTIC,Standard_False); 
	myAISContext->SetTransparency(white1,0.4,Standard_False);
	myAISContext->Display(white1,Standard_False);
	//Sleep(1000);
	
	gp_Pnt P1(0,0,0);
	gp_Pnt P2(50,0,0);
	gp_Pnt P3(100,0,0);
	gp_Pnt P4(25,12,85);
	gp_Pnt P5(100,0,80);
	gp_Pnt P6(135,-12,85);

	TColgp_Array2OfPnt Array(1,3,1,2);
	Array.SetValue(1,1,P1);
	Array.SetValue(2,1,P2);
	Array.SetValue(3,1,P3);
	Array.SetValue(1,2,P4);
	Array.SetValue(2,2,P5);
	Array.SetValue(3,2,P6);

	Handle (Geom_BSplineSurface) aSurf = GeomAPI_PointsToBSplineSurface(Array,3,8,GeomAbs_C2,0.00001);
	TopoDS_Shape SecondShape = BRepBuilderAPI_MakeFace(aSurf, Precision::Confusion());
	
	Handle(AIS_Shape) white2 = new AIS_Shape(SecondShape);
	
	myAISContext->SetColor(white2,Quantity_NOC_YELLOW,Standard_False);
	myAISContext->SetMaterial(white2,Graphic3d_NOM_PLASTIC,Standard_False);  
	myAISContext->SetTransparency(white2,0.4,Standard_False);
	myAISContext->Display(white2,Standard_False);

	//Sleep(1000);
	
	BRepOffsetAPI_Sewing aMethod;
	aMethod.Add(FirstShape);	
	aMethod.Add(SecondShape);

	aMethod.Perform();

	TopoDS_Shape sewedShape = aMethod.SewedShape();

	Handle(AIS_Shape) result = new AIS_Shape(sewedShape);
	myAISContext->SetDisplayMode(result,0,Standard_False);
	myAISContext->Display(result,Standard_False);

	Fit();

   TCollection_AsciiString Message ("\
	\n\
///////The first shape \n\
 \n\
gp_Pnt P(0,0,0);	\n\
gp_Vec V(0,0,1);	\n\
Handle(Geom_Plane) Pi=new Geom_Plane(P,V);	\n\
Handle(Geom_RectangularTrimmedSurface) GeometricSurface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);	\n\
TopoDS_Shape FirstShape = BRepBuilderAPI_MakeFace(GeometricSurface);	\n\
	\n\
///////The second shape \n\
 \n\
gp_Pnt P1(0,0,0);	\n\
gp_Pnt P2(50,0,0);	\n\
gp_Pnt P3(100,0,0);	\n\
gp_Pnt P4(25,12,85);	\n\
gp_Pnt P5(100,0,80);	\n\
gp_Pnt P6(135,-12,85);	\n\
\n\
TColgp_Array2OfPnt Array(1,3,1,2);	\n\
Array.SetValue(1,1,P1);	\n\
Array.SetValue(2,1,P2);	\n\
Array.SetValue(3,1,P3);	\n\
Array.SetValue(1,2,P4);	\n\
Array.SetValue(2,2,P5);	\n\
Array.SetValue(3,2,P6);	\n\
\n\
Handle (Geom_BSplineSurface) aSurf = GeomAPI_PointsToBSplineSurface(Array,3,8,GeomAbs_C2,0.00001);	\n\
TopoDS_Shape SecondShape = BRepBuilderAPI_MakeFace(aSurf);	\n\
	\n\
BRepOffsetAPI_Sewing aMethod;	\n\
aMethod.Add(FirstShape);		\n\
aMethod.Add(SecondShape);	\n\
\n\
aMethod.Perform();	\n\
\n\
TopoDS_Shape sewedShape = aMethod.SewedShape();	\n\
	\n\
\n");

	PocessTextInDialog("Sew faces ", Message);

}







void CModelingDoc::OnBuilder() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	//The tolerance is the tolerance of confusion
	Standard_Real precision = Precision::Confusion();

	//The builder
	BRep_Builder B;

	//Build the vertices
	TopoDS_Vertex V000, V001, V010, V011, V100, V101, V110, V111;
	B.MakeVertex(V000,gp_Pnt(0,0,0),precision);
	B.MakeVertex(V001,gp_Pnt(0,0,100),precision);
	B.MakeVertex(V010,gp_Pnt(0,150,0),precision);
	B.MakeVertex(V011,gp_Pnt(0,150,100),precision);
	B.MakeVertex(V100,gp_Pnt(200,0,0),precision);
	B.MakeVertex(V101,gp_Pnt(200,0,100),precision);
	B.MakeVertex(V110,gp_Pnt(200,150,0),precision);
	B.MakeVertex(V111,gp_Pnt(200,150,100),precision);

	//Build the edges
	//the edges are oriented as the axis X,Y,Z
	TopoDS_Edge EX00, EX01, EX10, EX11;
	TopoDS_Edge EY00, EY01, EY10, EY11;
	TopoDS_Edge EZ00, EZ01, EZ10, EZ11;
	Handle (Geom_Line) L;

	//Edge X00
	L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(1,0,0));
	B.MakeEdge(EX00,L,precision);
	V000.Orientation(TopAbs_FORWARD);
	V100.Orientation(TopAbs_REVERSED);
	B.Add(EX00,V000);
	B.Add(EX00,V100);
	//Parameters
	B.UpdateVertex(V000,0,EX00,precision);
	B.UpdateVertex(V100,200,EX00,precision);

	//Edge X10
	L = new Geom_Line(gp_Pnt(0,150,0),gp_Dir(1,0,0));
	B.MakeEdge(EX10,L,precision);
	V010.Orientation(TopAbs_FORWARD);
	V110.Orientation(TopAbs_REVERSED);
	B.Add(EX10,V010);
	B.Add(EX10,V110);
	//Parameters
	B.UpdateVertex(V010,0,EX10,precision);
	B.UpdateVertex(V110,200,EX10,precision);

	//Edge Y00
	L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(0,1,0));
	B.MakeEdge(EY00,L,precision);
	V000.Orientation(TopAbs_FORWARD);
	V010.Orientation(TopAbs_REVERSED);
	B.Add(EY00,V000);
	B.Add(EY00,V010);
	//Parameters
	B.UpdateVertex(V000,0,EY00,precision);
	B.UpdateVertex(V010,150,EY00,precision);

	//Edge Y10
	L = new Geom_Line(gp_Pnt(200,0,0),gp_Dir(0,1,0));
	B.MakeEdge(EY10,L,precision);
	V100.Orientation(TopAbs_FORWARD);
	V110.Orientation(TopAbs_REVERSED);
	B.Add(EY10,V100);
	B.Add(EY10,V110);
	//Parameters
	B.UpdateVertex(V100,0,EY10,precision);
	B.UpdateVertex(V110,150,EY10,precision);

	//Edge Y01
	L = new Geom_Line(gp_Pnt(0,0,100),gp_Dir(0,1,0));
	B.MakeEdge(EY01,L,precision);
	V001.Orientation(TopAbs_FORWARD);
	V011.Orientation(TopAbs_REVERSED);
	B.Add(EY01,V001);
	B.Add(EY01,V011);
	//Parameters
	B.UpdateVertex(V001,0,EY01,precision);
	B.UpdateVertex(V011,150,EY01,precision);

	//Edge Y11
	L = new Geom_Line(gp_Pnt(200,0,100),gp_Dir(0,1,0));
	B.MakeEdge(EY11,L,precision);
	V101.Orientation(TopAbs_FORWARD);
	V111.Orientation(TopAbs_REVERSED);
	B.Add(EY11,V101);
	B.Add(EY11,V111);
	//Parameters
	B.UpdateVertex(V101,0,EY11,precision);
	B.UpdateVertex(V111,150,EY11,precision);

	//Edge Z00
	L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ00,L,precision);
	V000.Orientation(TopAbs_FORWARD);
	V001.Orientation(TopAbs_REVERSED);
	B.Add(EZ00,V000);
	B.Add(EZ00,V001);
	//Parameters
	B.UpdateVertex(V000,0,EZ00,precision);
	B.UpdateVertex(V001,100,EZ00,precision);

	//Edge Z01
	L = new Geom_Line(gp_Pnt(0,150,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ01,L,precision);
	V010.Orientation(TopAbs_FORWARD);
	V011.Orientation(TopAbs_REVERSED);
	B.Add(EZ01,V010);
	B.Add(EZ01,V011);
	//Parameters
	B.UpdateVertex(V010,0,EZ01,precision);
	B.UpdateVertex(V011,100,EZ01,precision);

	//Edge Z10
	L = new Geom_Line(gp_Pnt(200,0,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ10,L,precision);
	V100.Orientation(TopAbs_FORWARD);
	V101.Orientation(TopAbs_REVERSED);
	B.Add(EZ10,V100);
	B.Add(EZ10,V101);
	//Parameters
	B.UpdateVertex(V100,0,EZ10,precision);
	B.UpdateVertex(V101,100,EZ10,precision);

	//Edge Z11
	L = new Geom_Line(gp_Pnt(200,150,0),gp_Dir(0,0,1));
	B.MakeEdge(EZ11,L,precision);
	V110.Orientation(TopAbs_FORWARD);
	V111.Orientation(TopAbs_REVERSED);
	B.Add(EZ11,V110);
	B.Add(EZ11,V111);
	//Parameters
	B.UpdateVertex(V110,0,EZ11,precision);
	B.UpdateVertex(V111,100,EZ11,precision);


	//Circular Edges
	Handle (Geom_Circle) C;
	//Standard_Real R = 100;

	//Edge EX01
	C = new Geom_Circle(gp_Ax2(gp_Pnt(100,0,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
	B.MakeEdge(EX01,C,precision);
	V001.Orientation(TopAbs_FORWARD);
	V101.Orientation(TopAbs_REVERSED);
	B.Add(EX01,V001);
	B.Add(EX01,V101);
	//Parameters
	B.UpdateVertex(V001,0,EX01,precision);
	B.UpdateVertex(V101,M_PI,EX01,precision);

	//Edge EX11
	C = new Geom_Circle(gp_Ax2(gp_Pnt(100,150,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
	B.MakeEdge(EX11,C,precision);
	V011.Orientation(TopAbs_FORWARD);
	V111.Orientation(TopAbs_REVERSED);
	B.Add(EX11,V011);
	B.Add(EX11,V111);
	//Parameters
	B.UpdateVertex(V011,0,EX11,precision);
	B.UpdateVertex(V111,M_PI,EX11,precision);

	//Build wire and faces
	//Faces normals are along the axis X,Y,Z
	TopoDS_Face FXMIN, FXMAX, FYMIN, FYMAX, FZMIN, FZMAX;
	TopoDS_Wire W;
	Handle (Geom_Plane) P;
	Handle (Geom2d_Line) L2d;
	Handle (Geom2d_Circle) C2d;
	Handle (Geom_CylindricalSurface) S;

	//Face FXMAX
	P = new Geom_Plane(gp_Ax2(gp_Pnt(200,0,0),gp_Dir(1,0,0),gp_Dir(0,1,0)));
	B.MakeFace(FXMAX,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EY10.Orientation(TopAbs_FORWARD);
	B.Add(W,EY10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EY10,L2d,FXMAX,precision);

	EZ11.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(150,0),gp_Dir2d(0,1));
	B.UpdateEdge(EZ11,L2d,FXMAX,precision);

	EY11.Orientation(TopAbs_REVERSED);
	B.Add(W,EY11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,100),gp_Dir2d(1,0));
	B.UpdateEdge(EY11,L2d,FXMAX,precision);

	EZ10.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EZ10,L2d,FXMAX,precision);

	B.Add(FXMAX,W);

	//Face FXMIN
	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(-1,0,0),gp_Dir(0,0,1)));
	B.MakeFace(FXMIN,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EZ00.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EZ00,L2d,FXMIN,precision);

	EY01.Orientation(TopAbs_FORWARD);
	B.Add(W,EY01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(100,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY01,L2d,FXMIN,precision);

	EZ01.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,150),gp_Dir2d(1,0));
	B.UpdateEdge(EZ01,L2d,FXMIN,precision);

	EY00.Orientation(TopAbs_REVERSED);
	B.Add(W,EY00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY00,L2d,FXMIN,precision);


	B.Add(FXMIN,W);
	
	//Face FYMAX

	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,1,0),gp_Dir(0,0,1)));
	B.MakeFace(FYMAX,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EZ00.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EZ00,L2d,FYMAX,precision);

	EX01.Orientation(TopAbs_FORWARD);
	B.Add(W,EX01);
	//pcurve
	C2d = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(100,100),gp_Dir2d(0,-1)),100);
	B.UpdateEdge(EX01,C2d,FYMAX,precision);
	B.UpdateVertex(V001,0,EX01,FYMAX,precision);
	B.UpdateVertex(V101,M_PI,EX01,FYMAX,precision);

	EZ10.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,200),gp_Dir2d(1,0));
	B.UpdateEdge(EZ10,L2d,FYMAX,precision);

	EX00.Orientation(TopAbs_REVERSED);
	B.Add(W,EX00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX00,L2d,FYMAX,precision);


	B.Add(FYMAX,W);



	//Face FYMIN
	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,150,0),gp_Dir(0,1,0),gp_Dir(0,0,1)));
	B.MakeFace(FYMIN,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EZ01.Orientation(TopAbs_FORWARD);
	B.Add(W,EZ01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EZ01,L2d,FYMIN,precision);

	EX11.Orientation(TopAbs_FORWARD);
	B.Add(W,EX11);
	//pcurve
	C2d = new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(100,100),gp_Dir2d(0,-1)),100);
	B.UpdateEdge(EX11,C2d,FYMIN,precision);
	B.UpdateVertex(V011,0,EX11,FYMIN,precision);
	B.UpdateVertex(V111,M_PI,EX11,FYMIN,precision);

	EZ11.Orientation(TopAbs_REVERSED);
	B.Add(W,EZ11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,200),gp_Dir2d(1,0));
	B.UpdateEdge(EZ11,L2d,FYMIN,precision);

	EX10.Orientation(TopAbs_REVERSED);
	B.Add(W,EX10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX10,L2d,FYMIN,precision);

	B.Add(FYMIN,W);

	//Face FZMAX
	P = new Geom_Plane(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,-1),gp_Dir(0,1,0)));
	B.MakeFace(FZMAX,P,precision);
	//the wire and the edges
	B.MakeWire (W);

	EY00.Orientation(TopAbs_FORWARD);
	B.Add(W,EY00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));
	B.UpdateEdge(EY00,L2d,FZMAX,precision);

	EX10.Orientation(TopAbs_FORWARD);
	B.Add(W,EX10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(150,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX10,L2d,FZMAX,precision);

	EY10.Orientation(TopAbs_REVERSED);
	B.Add(W,EY10);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,200),gp_Dir2d(1,0));
	B.UpdateEdge(EY10,L2d,FZMAX,precision);

	EX00.Orientation(TopAbs_REVERSED);
	B.Add(W,EX00);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EX00,L2d,FZMAX,precision);


	B.Add(FZMAX,W);
		
	//Face FZMIN
	S = new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(100,0,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
	B.MakeFace(FZMIN,S,precision);

	//the wire and the edges
	B.MakeWire (W);

	EX01.Orientation(TopAbs_FORWARD);
	B.Add(W,EX01);
	//pcurve
	L2d = new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0,0),gp_Dir2d(1,0)));
	B.UpdateEdge(EX01,L2d,FZMIN,precision);
	B.UpdateVertex(V001,0,EX01,FZMIN,precision);
	B.UpdateVertex(V101,M_PI,EX01,FZMIN,precision);

	EY11.Orientation(TopAbs_FORWARD);
	B.Add(W,EY11);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(M_PI,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY11,L2d,FZMIN,precision);

	EX11.Orientation(TopAbs_REVERSED);
	B.Add(W,EX11);
	//pcurve
	L2d = new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0,150),gp_Dir2d(1,0)));
	B.UpdateEdge(EX11,L2d,FZMIN,precision);
	B.UpdateVertex(V111,M_PI,EX11,FZMIN,precision);
	B.UpdateVertex(V011,0,EX11,FZMIN,precision);

	EY01.Orientation(TopAbs_REVERSED);
	B.Add(W,EY01);
	//pcurve
	L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
	B.UpdateEdge(EY01,L2d,FZMIN,precision);

	B.Add(FZMIN,W);

	FYMAX.Orientation(TopAbs_REVERSED);

	//Shell
	TopoDS_Shell Sh;
	B.MakeShell(Sh);
	B.Add(Sh,FXMAX);
	B.Add(Sh,FXMIN);
	B.Add(Sh,FYMAX);
	B.Add(Sh,FYMIN);
	B.Add(Sh,FZMAX);
	B.Add(Sh,FZMIN);

	// Solid
	TopoDS_Solid Sol;
	B.MakeSolid(Sol);
	B.Add(Sol,Sh);

	Handle(AIS_Shape) borne = new AIS_Shape(Sol);
	myAISContext->SetDisplayMode (borne, 1, Standard_False);
	myAISContext->SetColor (borne, Quantity_NOC_RED, Standard_False);
	myAISContext->SetMaterial(borne,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(borne,Standard_False);


	Fit();
   TCollection_AsciiString Message ("\
		\n\
//The tolerance is 0.01 	\n\
Standard_Real precision(0.01);	\n\
\n\
//The builder	\n\
BRep_Builder B;	\n\
\n\
//Build the vertices	\n\
TopoDS_Vertex V000, V001, V010, V011, V100, V101, V110, V111;	\n\
B.MakeVertex(V000,gp_Pnt(0,0,0),precision);	\n\
B.MakeVertex(V001,gp_Pnt(0,0,100),precision);	\n\
B.MakeVertex(V010,gp_Pnt(0,150,0),precision);	\n\
B.MakeVertex(V011,gp_Pnt(0,150,100),precision);	\n\
B.MakeVertex(V100,gp_Pnt(200,0,0),precision);	\n\
B.MakeVertex(V101,gp_Pnt(200,0,100),precision);	\n\
B.MakeVertex(V110,gp_Pnt(200,150,0),precision);	\n\
B.MakeVertex(V111,gp_Pnt(200,150,100),precision);	\n\
\n\
//Build the edges	\n\
//the edges are oriented as the axis X,Y,Z	\n\
TopoDS_Edge EX00, EX01, EX10, EX11;	\n\
TopoDS_Edge EY00, EY01, EY10, EY11;	\n\
TopoDS_Edge EZ00, EZ01, EZ10, EZ11;	\n\
Handle (Geom_Line) L;	\n\
\n\
//Edge X00	\n\
L = new Geom_Line(gp_Pnt(0,0,0),gp_Dir(1,0,0));	\n\
B.MakeEdge(EX00,L,precision);	\n\
V000.Orientation(TopAbs_FORWARD);	\n\
V100.Orientation(TopAbs_REVERSED);	\n\
B.Add(EX00,V000);	\n\
B.Add(EX00,V100);	\n\
//Parameters	\n\
B.UpdateVertex(V000,0,EX00,precision);	\n\
B.UpdateVertex(V100,200,EX00,precision);	\n\
\n\
//Idem for all the linear edges...	\n\
\n\
//Circular Edges	\n\
Handle (Geom_Circle) C;	\n\
Standard_Real R = 100;	\n\
\n\
//Edge EX01	\n\
C = new Geom_Circle(gp_Ax2(gp_Pnt(100,0,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);	\n\
B.MakeEdge(EX01,C,precision);	\n\
V001.Orientation(TopAbs_FORWARD);	\n\
V101.Orientation(TopAbs_REVERSED);	\n\
B.Add(EX01,V001);	\n\
B.Add(EX01,V101);	\n\
//Parameters	\n\
B.UpdateVertex(V001,0,EX01,precision);	\n\
B.UpdateVertex(V101,PI,EX01,precision);	\n\
\n\
//Idem for EX11	\n\
\n\
//Build wire and faces	\n\
//Faces normals are along the axis X,Y,Z	\n\
TopoDS_Face FXMIN, FXMAX, FYMIN, FYMAX, FZMIN, FZMAX;	\n\
TopoDS_Wire W;	\n\
Handle (Geom_Plane) P;	\n\
Handle (Geom2d_Line) L2d;	\n\
Handle (Geom2d_Circle) C2d;	\n\
Handle (Geom_CylindricalSurface) S;	\n\
\n\
//Face FXMAX	\n\
P = new Geom_Plane(gp_Ax2(gp_Pnt(200,0,0),gp_Dir(1,0,0),gp_Dir(0,1,0)));	\n\
B.MakeFace(FXMAX,P,precision);	\n\
//the wire and the edges	\n\
B.MakeWire (W);	\n");
Message += ("\
\n\
EY10.Orientation(TopAbs_FORWARD);	\n\
B.Add(W,EY10);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(1,0));	\n\
B.UpdateEdge(EY10,L2d,FXMAX,precision);	\n\
	\n\
EZ11.Orientation(TopAbs_FORWARD);	\n\
B.Add(W,EZ11);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(150,0),gp_Dir2d(0,1));	\n\
B.UpdateEdge(EZ11,L2d,FXMAX,precision);	\n\
	\n\
EY11.Orientation(TopAbs_REVERSED);	\n\
B.Add(W,EY11);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(0,100),gp_Dir2d(1,0));	\n\
B.UpdateEdge(EY11,L2d,FXMAX,precision);	\n\
	\n\
EZ10.Orientation(TopAbs_REVERSED);	\n\
B.Add(W,EZ10);	\n\
//pcurve	\n\
L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));	\n\
B.UpdateEdge(EZ10,L2d,FXMAX,precision);	\n\
\n\
B.Add(FXMAX,W);	\n\
\n\
//Idem for other faces...	\n\
\n\
//Shell	\n\
TopoDS_Shell Sh;	\n\
B.MakeShell(Sh);	\n\
B.Add(Sh,FXMAX);	\n\
B.Add(Sh,FXMIN);	\n\
B.Add(Sh,FYMAX);	\n\
B.Add(Sh,FYMIN);	\n\
B.Add(Sh,FZMAX);	\n\
B.Add(Sh,FZMIN);	\n\
\n\
// Solid	\n\
TopoDS_Solid Sol;	\n\
B.MakeSolid(Sol);	\n\
B.Add(Sol,Sh);	\n\
\n\
\n");

PocessTextInDialog("Make a shape with a builder", Message);

}

void CModelingDoc::OnGeometrie() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	//geometrie of a vertex
	TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0,120,70));
	gp_Pnt GeometricPoint = BRep_Tool::Pnt(aVertex);

	Handle(AIS_Shape) vert = new AIS_Shape(aVertex);
	myAISContext->Display(vert,Standard_False);
	Fit();
	Sleep (500);

	//geometrie of an edge
	TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(100,50,250),gp_Pnt(-30,-100,-50));
	Handle(AIS_Shape) yellow = new AIS_Shape(anEdge);
	myAISContext->SetWidth(yellow,2,Standard_False);
	myAISContext->Display(yellow,Standard_False);
	Fit();
	Sleep (500);

	TopLoc_Location location;
	Standard_Real first, last;
	Handle (Geom_Curve) aCurve = BRep_Tool::Curve(anEdge,location,first,last);
	TopoDS_Edge anEdgeDS = BRepBuilderAPI_MakeEdge(aCurve);

	Handle (Geom_Line) aLine = Handle (Geom_Line)::DownCast(aCurve);
	if (!aLine.IsNull()) {
		Handle (AIS_Line) DispLine = new AIS_Line(aLine);
		myAISContext->Display(DispLine,Standard_False);
		Fit();
		Sleep (500);
	}
		
	//geometrie of a face
	gp_Pnt P(-20,-20,-20);
	gp_Vec V(0,0,1);
	Handle(Geom_Plane) Pi=new Geom_Plane(P,V);
	Handle(Geom_RectangularTrimmedSurface) Surface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);
	TopoDS_Face RedFace = BRepBuilderAPI_MakeFace(Surface, Precision::Confusion());

	Handle(AIS_Shape) red = new AIS_Shape(RedFace);
	myAISContext->SetColor(red,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(red,Graphic3d_NOM_PLASTIC,Standard_False);    
	myAISContext->Display(red,Standard_False);
	Fit();
	Sleep (500);

	TopLoc_Location location2;
	Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(RedFace,location2);

	Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);
	if (!aPlane.IsNull()) {
		Handle (AIS_Plane) DispPlane = new AIS_Plane(aPlane);
		myAISContext->Display(DispPlane,Standard_False);

	}


	Fit();
	Sleep (500);


   TCollection_AsciiString Message ("\
	\n\
///////geometrie of a vertex	\n\
TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0,120,70));	\n\
gp_Pnt GeometricPoint = BRep_Tool::Pnt(aVertex);	\n\
\n\
///////geometrie of an edge	\n\
TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(100,50,250),gp_Pnt(-30,-100,-50));	\n\
\n\
TopLoc_Location location;	\n\
Standard_Real first, last;	\n\
Handle (Geom_Curve) aCurve = BRep_Tool::Curve(anEdge,location,first,last);	\n\
TopoDS_Edge anEdgeDS = BRepBuilderAPI_MakeEdge(aCurve);	\n\
\n\
Handle (Geom_Line) aLine = Handle (Geom_Line)::DownCast(aCurve);	\n\
if (!aLine.IsNull()) {	\n\
	Handle (AIS_Line) DispLine = new AIS_Line(aLine);	\n\
}	\n\
		\n\
///////geometrie of a face	\n\
gp_Pnt P(-20,-20,-20);	\n\
gp_Vec V(0,0,1);	\n\
Handle(Geom_Plane) Pi=new Geom_Plane(P,V);	\n\
Handle(Geom_RectangularTrimmedSurface) Surface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);	\n\
TopoDS_Face RedFace = BRepBuilderAPI_MakeFace(Surface);	\n\
\n\
TopLoc_Location location2;	\n\
Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(RedFace,location2);	\n\
\n\
Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);	\n\
if (!aPlane.IsNull()) {	\n\
	Handle (AIS_Plane) DispPlane = new AIS_Plane(aPlane);	\n\
}	\n\
\n\
\n");

	PocessTextInDialog("Recover the geometrie of vertex, edge and face ", Message);


}

void CModelingDoc::OnExplorer() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100, 100, 100).Shape();
	Standard_Integer j(8);
	Handle(AIS_Shape) theBox = new AIS_Shape(aBox);
	myAISContext->SetColor(theBox,Quantity_NOC_RED,Standard_False);
	myAISContext->SetMaterial(theBox,Graphic3d_NOM_PLASTIC,Standard_False);  
	myAISContext->Display(theBox,Standard_False);
	Fit();
	Sleep(500);

	for (TopExp_Explorer exp (aBox,TopAbs_FACE);exp.More();exp.Next()) {
		TopoDS_Face aCurrentFace = TopoDS::Face(exp.Current());

		//test the orientation of the current face
		TopAbs_Orientation orient = aCurrentFace.Orientation();

		//Recover the geometric plane
		TopLoc_Location location;
		Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(aCurrentFace,location);

		Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);

		//Build an AIS_Shape with a new color
		Handle(AIS_Shape) theMovingFace = new AIS_Shape(aCurrentFace);
		Quantity_NameOfColor aCurrentColor = (Quantity_NameOfColor)j;
		myAISContext->SetColor(theMovingFace,aCurrentColor,Standard_False);
		myAISContext->SetMaterial(theMovingFace,Graphic3d_NOM_PLASTIC,Standard_False);  
		//Find the normal vector of each face
		gp_Pln agpPlane = aPlane->Pln();
		gp_Ax1 norm = agpPlane.Axis();
		gp_Dir dir = norm.Direction();
		gp_Vec move(dir);
		//Connect
// new in 2.0 ... AIS_ConnectedInteractive wants a TopLoc_Location instead of a Geom_Transformation
//		TopLoc_Location aLocation;
//		Handle (AIS_ConnectedInteractive) theTransformedDisplay = new AIS_ConnectedInteractive();
//		theTransformedDisplay->Connect(theMovingFace,theMove);
//		theTransformedDisplay->Connect(theMovingFace, aLocation);
//		Handle (Geom_Transformation) theMove = new Geom_Transformation(aLocation->Transformation());
// new in 2.0
//		myAISContext->Display(theTransformedDisplay);

		TopLoc_Location aLocation;
		Handle (AIS_ConnectedInteractive) theTransformedDisplay = new AIS_ConnectedInteractive();
		theTransformedDisplay->Connect(theMovingFace, aLocation);


		// = myAISContext->Location(theMovingFace);
		Handle (Geom_Transformation) theMove = new Geom_Transformation(aLocation.Transformation());
        myAISContext->Display(theTransformedDisplay,Standard_False);
		Fit();
		Sleep (500);

		for (Standard_Integer i=1;i<=30;i++) {

		//Build a transformation on the display
//			theMove->SetTranslation(move*i);

//			if (orient==TopAbs_FORWARD) theTransformedDisplay->SetTransformation(theMove);
//			else theTransformedDisplay->SetTransformation(theMove->Inverted());

//			myAISContext->Redisplay(theTransformedDisplay);

// new in 2.0			
			theMove->SetTranslation(move*i);
			if (orient==TopAbs_FORWARD) myAISContext->SetLocation(theTransformedDisplay,TopLoc_Location(theMove->Trsf()));
			else myAISContext->SetLocation(theTransformedDisplay,TopLoc_Location(theMove->Inverted()->Trsf()));

			myAISContext->Redisplay(theTransformedDisplay,Standard_False);
		}
		j+=15;
	}
	//myAISContext->Erase(theBox,Standard_True,Standard_False);	
	myAISContext->Remove(theBox, Standard_False);
	Fit();
	Sleep (500);

	   TCollection_AsciiString Message ("\
\n\
TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100,100,100);	\n\
\n\
for (TopExp_Explorer exp (aBox,TopAbs_FACE);exp.More();exp.Next()) {	\n\
	TopoDS_Face aCurrentFace = TopoDS::Face(exp.Current());	\n\
\n\
	//Recover the geometric plane	\n\
	TopLoc_Location location;	\n\
	Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(aCurrentFace,location);	\n\
\n\
	Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);	\n\
	\n\
\n");
	PocessTextInDialog("Explode a shape in faces ", Message);

}

/* =================================================================================
   ====================   A N A L Y S I S   ========================================
   ================================================================================= */

void CModelingDoc::OnValid() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

  TopoDS_Shape S = BRepPrimAPI_MakeBox(200., 300., 150.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();

	TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S = BRepPrimI_MakeBox(200.,300.,150.);\n\
Standard_Boolean theShapeIsValid = BRepAlgo::IsValid(S);\n\
if ( theShapeIsValid )\n\
{\n\
  MessageBox(\"The Shape Is Valid !! \",\"Checking Shape\");\n\
}\n\
else\n\
{\n\
  MessageBox(\"The Shape Is NOT Valid !! \",\"Checking Shape\");\n\
}\n\
\n");
	PocessTextInDialog("Check a shape", Message);

	Standard_Boolean theShapeIsValid = BRepAlgo::IsValid(S);
  MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, theShapeIsValid ? L"The Shape Is Valid !! " : L"The Shape Is NOT Valid !! ", L"Checking Shape", MB_OK);
}


void CModelingDoc::OnLinear() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}


	TColgp_Array1OfPnt Points1(1,4);
	Points1.SetValue(1,gp_Pnt(0,0,0));
	Points1.SetValue(2,gp_Pnt(2,1,0));
	Points1.SetValue(3,gp_Pnt(4,0,0));
	Points1.SetValue(4,gp_Pnt(6,2,0));
	GeomAPI_PointsToBSpline PTBS1(Points1);
	Handle(Geom_BSplineCurve) BSC1 = PTBS1.Curve();
	TopoDS_Edge S = BRepBuilderAPI_MakeEdge(BSC1).Edge();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();


	GProp_GProps System;
	BRepGProp::LinearProperties(S,System);
	gp_Pnt G = System.CentreOfMass ();
	Standard_Real Length = System.Mass();
	gp_Mat I = System.MatrixOfInertia();

	TCollection_ExtendedString string("Length Of all the Edges =");
  TCollection_ExtendedString string1(Length);
	
	string += string1;
	string += "\nCenterOfMass : \n   X=";
	string1 = G.X();
	string += string1;
	string += " Y=";
	string1 = G.Y();
	string += string1;
	string += " Z=";
	string1 = G.Z();
	string += string1;
	string +="\n";

	string += "Matrix of Inertia :\n     ";
	string1 = I(1,1);
	string += string1;
	string += " " ;
	string1 = I(1,2);
	string += string1;
	string += " " ;
	string1 = I(1,3);
	string += string1;
	string += "\n     " ;
	string1 = I(2,1);
	string += string1;
	string += " " ;
	string1 = I(2,2);
	string += string1;
	string += " " ;
	string1 = I(2,3);
	string += string1;
	string += "\n     " ;
	string1 = I(3,1);
	string += string1;
	string += " " ;
	string1 = I(3,2);
	string += string1;
	string += " " ;
	string1 = I(3,3);
	string += string1;
	string += "\n" ;

  TCollection_AsciiString Message ("\
		\n\
TColgp_Array1OfPnt Points1(1,4);\n\
Points1.SetValue(1,gp_Pnt(0,0,0));\n\
Points1.SetValue(2,gp_Pnt(2,1,0));\n\
Points1.SetValue(3,gp_Pnt(4,0,0));\n\
Points1.SetValue(4,gp_Pnt(6,2,0));\n\
GeomAPI_PointsToBSpline PTBS1(Points1);\n\
Handle(Geom_BSplineCurve) BSC1 = PTBS1.Curve();\n\
TopoDS_Shape S = BRepBuilderAPI_MakeEdge(BSC1).Edge();\n\
GProp_GProps System;\n\
BRepGProp::LinearProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Length = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n");
	PocessTextInDialog("Linear Properties", Message);
	MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, string.ToWideString(), L"Linear Properties", MB_OK);
}

void CModelingDoc::OnSurface() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}

	TColgp_Array1OfPnt Pnts1(1,3);
	TColgp_Array1OfPnt Pnts2(1,3);
	TColgp_Array1OfPnt Pnts3(1,3);
	TColgp_Array1OfPnt Pnts4(1,3);

	Pnts1(1) = gp_Pnt(0,0,0);
	Pnts1(2) = gp_Pnt(5,0,0);
	Pnts1(3) = gp_Pnt(10,10,0);

	Pnts2(1) = gp_Pnt(10,10,0);
	Pnts2(2) = gp_Pnt(5,12,4);
	Pnts2(3) = gp_Pnt(0,15,10);

	Pnts3(1) = gp_Pnt(0,15,10);
	Pnts3(2) = gp_Pnt(-12,10,11);
	Pnts3(3) = gp_Pnt(-10,5,13);

	Pnts4(1) = gp_Pnt(-10,5,13);
	Pnts4(2) = gp_Pnt(-2,-2,2);
	Pnts4(3) = gp_Pnt(0,0,0);
	
	GeomAPI_PointsToBSpline PTBS1(Pnts1);
	GeomAPI_PointsToBSpline PTBS2(Pnts2);
	GeomAPI_PointsToBSpline PTBS3(Pnts3);
	GeomAPI_PointsToBSpline PTBS4(Pnts4);
	Handle(Geom_BSplineCurve) C1 = PTBS1.Curve();
	Handle(Geom_BSplineCurve) C2 = PTBS2.Curve();
	Handle(Geom_BSplineCurve) C3 = PTBS3.Curve();
	Handle(Geom_BSplineCurve) C4 = PTBS4.Curve();

	GeomFill_BSplineCurves fill; 
	fill.Init(C1,C2,C3,C4,GeomFill_CoonsStyle);
	Handle(Geom_BSplineSurface) BSS = fill.Surface();

	TopoDS_Shape S = BRepBuilderAPI_MakeFace(BSS, Precision::Confusion()).Face();

	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();


	GProp_GProps System;
	BRepGProp::SurfaceProperties(S,System);
	gp_Pnt G = System.CentreOfMass ();
	Standard_Real Area = System.Mass();
	gp_Mat I = System.MatrixOfInertia();

	TCollection_ExtendedString string("Area Of the Face =");
  TCollection_ExtendedString string1(Area);
	
	string += string1;
	string += "\nCenterOfMass : \n   X=";
	string1 = G.X();
	string += string1;
	string += " Y=";
	string1 = G.Y();
	string += string1;
	string += " Z=";
	string1 = G.Z();
	string += string1;
	string +="\n";

	string += "Matrix of Inertia :\n     ";
	string1 = I(1,1);
	string += string1;
	string += " " ;
	string1 = I(1,2);
	string += string1;
	string += " " ;
	string1 = I(1,3);
	string += string1;
	string += "\n     " ;
	string1 = I(2,1);
	string += string1;
	string += " " ;
	string1 = I(2,2);
	string += string1;
	string += " " ;
	string1 = I(2,3);
	string += string1;
	string += "\n     " ;
	string1 = I(3,1);
	string += string1;
	string += " " ;
	string1 = I(3,2);
	string += string1;
	string += " " ;
	string1 = I(3,3);
	string += string1;
	string += "\n" ;

  TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S = BRepBuilderAPI_MakeFace(BSplineSurf).Face();\n\
GProp_GProps System;\n\
BRepGProp::SurfaceProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Area = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n");
  PocessTextInDialog("Surface Properties", Message);
	MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, string.ToWideString(), L"Surface Properties", MB_OK);

}

void CModelingDoc::OnVolume() 
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
		myAISContext->Remove (aListIterator.Value(), Standard_False);
	}
	

	TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape();
	Handle(AIS_Shape) ais1 = new AIS_Shape(S);
	myAISContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
	myAISContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
	myAISContext->Display(ais1,Standard_False);
	Fit();


	GProp_GProps System;
	BRepGProp::VolumeProperties(S,System);
	gp_Pnt G = System.CentreOfMass ();
	Standard_Real Volume = System.Mass();
	gp_Mat I = System.MatrixOfInertia();

	TCollection_ExtendedString string("Volume Of all the Shape =");
  TCollection_ExtendedString string1(Volume);
	
	string += string1;
	string += "\nCenterOfMass : \n   X=";
	string1 = G.X();
	string += string1;
	string += " Y=";
	string1 = G.Y();
	string += string1;
	string += " Z=";
	string1 = G.Z();
	string += string1;
	string +="\n";

	string += "Matrix of Inertia :\n     ";
	string1 = I(1,1);
	string += string1;
	string += " " ;
	string1 = I(1,2);
	string += string1;
	string += " " ;
	string1 = I(1,3);
	string += string1;
	string += "\n     " ;
	string1 = I(2,1);
	string += string1;
	string += " " ;
	string1 = I(2,2);
	string += string1;
	string += " " ;
	string1 = I(2,3);
	string += string1;
	string += "\n     " ;
	string1 = I(3,1);
	string += string1;
	string += " " ;
	string1 = I(3,2);
	string += string1;
	string += " " ;
	string1 = I(3,3);
	string += string1;
	string += "\n" ;

	TCollection_AsciiString Message ("\
		\n\
TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60.,100.,80.,20.);;\n\
GProp_GProps System;\n\
BRepGProp::VolumeProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Volume = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n");
  PocessTextInDialog("Volume Properties", Message);
	MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, string.ToWideString(), L"Volume Properties", MB_OK);
}


void CModelingDoc::OnButtonFill() 
{
	// TODO: Add your command handler code here
	myAISContext->InitSelected();
	if (myAISContext->MoreSelected()) {
		AIS1 = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
		myAISContext->Unhilight (AIS1, Standard_True);
		myAISContext->Activate(AIS1,2);
		myState = SELECT_EDGE_PLATE;
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select hole contour edges and then press right mouse button");	}
	else {
		AIS_ListOfInteractive LI;
		myAISContext->DisplayedObjects(LI);
		if(LI.IsEmpty()){
			if(OnFileImportBrep_WithInitDir (L"HoleFilling") == 1)
				return;
		myAISContext->DisplayedObjects(LI);
		myAISContext->SetSelected(LI.First(), Standard_True);
			OnButtonFill();
			return;
		}
	AfxMessageBox (L"Select a shape before!");
	}
}

void CModelingDoc::OnStopStop() 
{
	// TODO: Add your command handler code here
	// Stop selection
	if (myState == SELECT_EDGE_PLATE ) {
		Standard_Integer nbedges = 0;
		for (myAISContext->InitSelected(); myAISContext->MoreSelected();
		     myAISContext->NextSelected()) {
			nbedges++;
			
		}
                Handle(GeomPlate_HArray1OfHCurve) Fronts =
                        new GeomPlate_HArray1OfHCurve(1, nbedges);
		Handle(TColStd_HArray1OfInteger) Tang = 
			new TColStd_HArray1OfInteger(1,nbedges);
		Handle(TColStd_HArray1OfInteger) NbPtsCur = 
			new TColStd_HArray1OfInteger(1,nbedges);
		Standard_Integer i = 0;
		TopoDS_Shape S1 = AIS1->Shape();
		TopTools_IndexedDataMapOfShapeListOfShape M;
		TopExp::MapShapesAndAncestors(S1, TopAbs_EDGE, TopAbs_FACE, M);

		for (myAISContext->InitSelected(); myAISContext->MoreSelected();myAISContext->NextSelected()) {
			i++;
			Tang->SetValue(i,1);
			NbPtsCur->SetValue(i,10);
			TopoDS_Edge E = TopoDS::Edge(myAISContext->SelectedShape());
			TopoDS_Face F = TopoDS::Face(M.FindFromKey(E).First());

			BRepAdaptor_Surface S(F);
			GeomAdaptor_Surface aGAS = S.Surface();
			Handle(GeomAdaptor_HSurface) aHGAS = new GeomAdaptor_HSurface(aGAS);

			Handle(BRepAdaptor_HCurve2d) C = new BRepAdaptor_HCurve2d();
			C->ChangeCurve2d().Initialize(E,F);

			Adaptor3d_CurveOnSurface ConS(C,aHGAS);

			Handle (Adaptor3d_HCurveOnSurface) HConS = new Adaptor3d_HCurveOnSurface(ConS);
			Fronts->SetValue(i,HConS);
		}
		GeomPlate_BuildPlateSurface abuildplate(NbPtsCur,Fronts,Tang,3);
		abuildplate.Perform();
		if (!abuildplate.IsDone()){ // New in 2.0
			MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Build plate not valid!", L"CasCade Error", MB_ICONERROR);
			return;
		}
		Handle(GeomPlate_Surface) aplate = abuildplate.Surface();

		GeomPlate_MakeApprox aMKS(aplate, Precision::Approximation(), 4, 7, 0.001, 1);
		Handle(Geom_BSplineSurface) support = aMKS.Surface();
		BRepBuilderAPI_MakeWire MW;
		TopTools_Array1OfShape tab(1,nbedges);
		for (i=1 ; i<=nbedges ; i++) {
			if (abuildplate.Sense()->Value(abuildplate.Order()->Value(i))==1) {
				BRepBuilderAPI_MakeEdge ME(abuildplate.Curves2d()->Value(abuildplate.Order()->Value(i)),
									support,
									Fronts->Value(abuildplate.Order()->Value(i))->LastParameter(),
									Fronts->Value(abuildplate.Order()->Value(i))->FirstParameter());
				TopoDS_Edge E = ME.Edge();
				BRepLib::BuildCurves3d(E);
				tab(abuildplate.Order()->Value(i)) = E;
				//MW.Add(E);
			}
			else {
				BRepBuilderAPI_MakeEdge ME(abuildplate.Curves2d()->Value(abuildplate.Order()->Value(i)),
									support,
									Fronts->Value(abuildplate.Order()->Value(i))->FirstParameter(),
									Fronts->Value(abuildplate.Order()->Value(i))->LastParameter());
				TopoDS_Edge E = ME.Edge();
				BRepLib::BuildCurves3d(E);
				tab(abuildplate.Order()->Value(i)) = E;
			}	
		}
		for (i=1 ; i<=nbedges ; i++) 
			MW.Add(TopoDS::Edge(tab(i)));
		TopoDS_Wire W;
		try{
		W=MW.Wire();
		}
		
		catch(StdFail_NotDone)
		{
			AfxMessageBox (L"Can't build wire!");
			return;
		}

		if (!(W.Closed())){
			AfxMessageBox (L"Wire is not closed!");
			return;
			//throw Standard_Failure("Wire is not closed");
			
		}
		BRepBuilderAPI_MakeFace MF(support,W,Standard_True);
		TopoDS_Face aface;
		aface = MF.Face();
		BRepTopAdaptor_FClass2d clas2d(aface,Precision::Confusion());
		if (clas2d.PerformInfinitePoint() == TopAbs_IN) {
			W.Reverse();
			BRepBuilderAPI_MakeFace MF1(support,W,Standard_True);
			aface = MF1.Face();
		}
		if (!BRepAlgo::IsValid(aface))
			MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : The plate face is not valid!", L"CasCade Error", MB_ICONERROR);
		myState = -1;
		Handle(AIS_Shape) anAISShape = new AIS_Shape(aface);
		myAISContext->SetColor (anAISShape,Quantity_NOC_AZURE, Standard_False);
		myAISContext->SetMaterial (anAISShape, Graphic3d_NOM_SILVER, Standard_False);
    myAISContext->SetDisplayMode (anAISShape, 1, Standard_False);
		myAISContext->Display (anAISShape, Standard_True);
	}
}

void CModelingDoc::OnFillwithtang() 
{
	static BOOL flag = 0;
	if (flag == 1){
		flag = 0;
		Handle(AIS_InteractiveObject) aObject;
		myAISContext ->InitSelected();
		if(myAISContext->MoreSelected())
			aObject = myAISContext->SelectedInteractive();
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select a file with second face");
		if(OnFileImportBrep_WithInitDir (L"TangentSurface") == 1){
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");
				AIS_ListOfInteractive aList;
				myAISContext->DisplayedObjects(aList);
				AIS_ListIteratorOfListOfInteractive aListIterator;
				for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
					myAISContext->Remove (aListIterator.Value(), Standard_False);
				}
        myAISContext->UpdateCurrentViewer();
				return;
			}
		myAISContext->SetSelected(aObject, Standard_True);
	}

	myAISContext->InitSelected();
	if (myAISContext->MoreSelected()) {
		Handle(AIS_Shape) ashape = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
		try {
                  THE_F1 = TopoDS::Face(ashape->Shape());
		}
	    catch(Standard_Failure){}
		if (THE_F1.IsNull())
                {
                    AfxMessageBox (L"Current object is not a face!\n\
Please, select a face to continue\nthe creation of a tangent surface.");
                    return;
                }
		myAISContext->Activate(ashape,2);
		myState = SELECT_EDGE_PLATE_TGTES_1;
		
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select an edge on the first face");
	}
	else {
		AIS_ListOfInteractive LI;
		myAISContext->DisplayedObjects(LI);
		if(LI.IsEmpty()){
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select a file with first face");
			if(OnFileImportBrep_WithInitDir (L"TangentSurface") == 1){
				((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");
				AIS_ListOfInteractive aList;
				myAISContext->DisplayedObjects(aList);
				AIS_ListIteratorOfListOfInteractive aListIterator;
				for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
					myAISContext->Remove(aListIterator.Value(), Standard_False);
				}
        myAISContext->UpdateCurrentViewer();
				return;
			}
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");
			myAISContext->DisplayedObjects(LI);
			myAISContext->SetSelected (LI.First(), Standard_True);
			Sleep(700);
			flag = 1;
			OnFillwithtang();
			return;
		}
		AfxMessageBox (L"Select a face before");
	}
}

void CModelingDoc::InputEvent(const Standard_Integer /*x*/,
                              const Standard_Integer /*y*/,
                              const Handle(V3d_View)& /*aView*/)
{
    myAISContext->Select (Standard_True);
	if (myState == SELECT_EDGE_PLATE_TGTES_1) {
		myAISContext->InitSelected();
 		if (myAISContext->MoreSelected()) {
 			THE_E1 = TopoDS::Edge(myAISContext->SelectedShape());
 			myState = SELECT_EDGE_PLATE_TGTES_2;
 			
			AIS_ListOfInteractive aLI;
			myAISContext->DisplayedObjects(aLI);
			if(aLI.Extent() == 2){
				myState = SELECT_EDGE_PLATE_TGTES_2;
				if (myAISContext->IsSelected(aLI.First()))
					myAISContext->SetSelected (aLI.Last(), Standard_True);
				else
					myAISContext->SetSelected (aLI.First(), Standard_True);
				myAISContext->InitSelected();
				Handle(AIS_Shape) ashape = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
 				THE_F2 = TopoDS::Face(ashape->Shape());
				myAISContext->Activate(ashape,2);
				myState = SELECT_EDGE_PLATE_TGTES_3;
				((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select an edge on the second face");
				return;
			}
			
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select second face");
			AIS_ListOfInteractive LI;
			myAISContext->DisplayedObjects(LI);
			if(LI.Extent() == 1){
				if(OnFileImportBrep_WithInitDir (L"TangentSurface") == 1)
				return;
			}
 		}
 		else
 			AfxMessageBox (L"Select an edge on the face!");
 
 	}
 	else if (myState == SELECT_EDGE_PLATE_TGTES_2) {
		myAISContext->InitSelected();
		if (myAISContext->MoreSelected()) {
			Handle(AIS_Shape) ashape = Handle(AIS_Shape)::DownCast(myAISContext->SelectedInteractive());
			THE_F2 = TopoDS::Face(ashape->Shape());
			myAISContext->Activate(ashape,2);
			myState = SELECT_EDGE_PLATE_TGTES_3;
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select an edge on the second face");
		}
		else 
			AfxMessageBox (L"Select the second face!");
	}
	else if (myState == SELECT_EDGE_PLATE_TGTES_3) {
		myAISContext->InitSelected();
		if (myAISContext->MoreSelected()) {
			THE_E2 = TopoDS::Edge(myAISContext->SelectedShape());

			Standard_Integer i, nbPntsOnFaces=10;
			Standard_Real u,First, Last, Delta, Tol=0.001, TolProj;
			Plate_Plate aPlate;
			gp_Vec V1,V2,W1,W2;
			gp_Pnt2d P2d;
			gp_Pnt P, PP;

			//get the pcurve, curve and surface
			BRepAdaptor_Curve   Curve3d1(THE_E1), Curve3d2(THE_E2);
			BRepAdaptor_Curve2d Curve2d1(THE_E1,THE_F1), Curve2d2(THE_E2,THE_F2);
			BRepAdaptor_Surface Surf1(THE_F1), Surf2(THE_F2);

			//compute the average plane : initial surface
			Handle(TColgp_HArray1OfPnt) theTanPoints = new
				TColgp_HArray1OfPnt (1,2*nbPntsOnFaces );

			Delta = (Curve3d1.LastParameter()-Curve3d1.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d1.FirstParameter(),i=1;i<=nbPntsOnFaces; i++,u+=Delta)
				theTanPoints->SetValue(i,Curve3d1.Value(u));

			Delta = (Curve3d2.LastParameter()-Curve3d2.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d2.FirstParameter(),i=1;i<=nbPntsOnFaces; i++,u+=Delta)
				theTanPoints->SetValue(nbPntsOnFaces+i,Curve3d2.Value(u));

			//Building an initial plane
			GeomPlate_BuildAveragePlane aMkPlane (theTanPoints,int(Tol),1,1,1);
			Handle(Geom_Plane) aPlane = aMkPlane.Plane();
			gp_Pln aPln = aPlane->Pln();
			gp_XYZ aNormale = aPln.Axis().Direction().XYZ();
			gp_Trsf aTrsf; // to compute the U and V of the points
			aTrsf.SetTransformation(aPln.Position());

			aPlane->D1(0,0,P,W1,W2); // extract plane DU & DV

			// 1st surface tangencies constraints
			Delta = (Curve3d1.LastParameter()-Curve3d1.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d1.FirstParameter(),i=1; i<=nbPntsOnFaces; i++,u+=Delta) {
				P = Curve3d1.Value(u).Transformed(aTrsf);
				gp_XY UV(P.X(),P.Y());
				aPlate.Load(Plate_PinpointConstraint(UV,aNormale*P.Z()));
				Curve2d1.D0(u,P2d);
				Surf1.D1(P2d.X(),P2d.Y(),P,V1,V2);  // extract surface UV of the point
				aPlate.Load(Plate_GtoCConstraint(UV,
												 Plate_D1(W1.XYZ(),W2.XYZ()),
												 Plate_D1(V1.XYZ(),V2.XYZ())));
			}
			// 2nd surface 
			Delta = (Curve3d2.LastParameter()-Curve3d2.FirstParameter())/(nbPntsOnFaces-1);
			for (u=Curve3d2.FirstParameter(),i=1; i<=nbPntsOnFaces; i++,u+=Delta) {
				P = Curve3d2.Value(u).Transformed(aTrsf);
				gp_XY UV(P.X(),P.Y());
				aPlate.Load(Plate_PinpointConstraint(UV,aNormale*P.Z()));

				Curve2d2.D0(u,P2d);
				Surf2.D1(P2d.X(),P2d.Y(),P,V1,V2); 
				aPlate.Load(Plate_GtoCConstraint(UV,
												 Plate_D1(W1.XYZ(),W2.XYZ()),
												 Plate_D1(V1.XYZ()*-1,V2.XYZ()*-1)));
			}

			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Select a file with passing points");
			//Some passing points
			CFileDialog dlg(TRUE,
					NULL,
					NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Points Files (*.pass)|*.pass; |All Files (*.*)|*.*||", 
					NULL);

			CString initdir;
			initdir.GetEnvironmentVariable (L"CSF_OCCTDataPath");
			initdir += L"\\occ";

			dlg.m_ofn.lpstrInitialDir = initdir;

			if (dlg.DoModal() == IDOK) 
			{
				((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Building the tangent surface...");
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
				CString filename = dlg.GetPathName();
				filebuf fic;
				istream in(&fic);  
				if (!fic.open(filename, ios::in))
					MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Unable to open file", L"CasCade Error", MB_ICONERROR);
				Standard_Real x,y,z;
				BRep_Builder B;
				TopoDS_Compound C;
				B.MakeCompound(C);
				while (!in.fail()|| !in.eof()){
					if (in >> x && in >> y && in >> z){
						PP = gp_Pnt(x, y, z);
						P = PP.Transformed(aTrsf);
						aPlate.Load(Plate_PinpointConstraint(gp_XY(P.X(),P.Y()),
															aNormale*P.Z()));
						BRepBuilderAPI_MakeVertex V(PP);
						B.Add(C,V.Vertex());	    
					}
				}
				fic.close();
 				Handle(AIS_Shape) anAISCompound = new AIS_Shape(C);
				myAISContext->Display(anAISCompound, Standard_False);
				Fit();
				Sleep(500);
			}
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
			((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("Building the tangent surface...");
			//Solving ... 
			Standard_Integer Order = 3; // constraints continuity + 2
			aPlate.SolveTI(Order,1.);
			if (!aPlate.IsDone()){
				MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : Build plate not valid!", L"CasCade Error", MB_ICONERROR);
				return;
			}
			//Plate Surface creation 
			Handle(GeomPlate_Surface) aPlateSurface = new GeomPlate_Surface(aPlane,aPlate);
			//BSplineSurface approximation 
			GeomPlate_MakeApprox aMkSurf(aPlateSurface,Tol,1,8,0.1,1);
			Handle(Geom_Surface) theSurface =aMkSurf.Surface();

			// Face building ... 
			Handle(Geom2d_Curve)C1,C2,C3,C4;
			Handle(Geom_Curve)C;
			C = BRep_Tool::Curve(THE_E1,First,Last);
			TolProj = 0.01;
			C1 = GeomProjLib::Curve2d(C,First,Last,theSurface,TolProj);
			TopoDS_Edge Ed1 = BRepBuilderAPI_MakeEdge(C1,theSurface).Edge();

			C = BRep_Tool::Curve(THE_E2,First,Last);
			TolProj = 0.01;
			C3 = GeomProjLib::Curve2d(C,First,Last,theSurface,TolProj);
			TopoDS_Edge Ed3 = BRepBuilderAPI_MakeEdge(C3,theSurface).Edge();
			
			C2 = GCE2d_MakeSegment(C1->Value(C1->FirstParameter()),
								C3->Value(C3->FirstParameter())).Value();
			TopoDS_Edge Ed2 = BRepBuilderAPI_MakeEdge(C2,theSurface).Edge();
			C4 = GCE2d_MakeSegment(C1->Value(C1->LastParameter()),
								C3->Value(C3->LastParameter())).Value();
			TopoDS_Edge Ed4 = BRepBuilderAPI_MakeEdge(C4,theSurface).Edge();
			Ed2.Reverse();
			Ed3.Reverse();
			TopoDS_Wire theWire = BRepBuilderAPI_MakeWire(Ed1,Ed2,Ed3,Ed4);
			TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theWire);
			BRepLib::BuildCurves3d(theFace);
			if (!BRepAlgo::IsValid(theFace)){
				C2 = GCE2d_MakeSegment(C1->Value(C1->LastParameter()),
										C3->Value(C3->FirstParameter())).Value();
				Ed2 = BRepBuilderAPI_MakeEdge(C2,theSurface).Edge();
				C4 = GCE2d_MakeSegment(C3->Value(C3->LastParameter()),
								C1->Value(C1->FirstParameter())).Value();
				Ed4 = BRepBuilderAPI_MakeEdge(C4,theSurface).Edge();
				Ed3.Reverse();
				theWire = BRepBuilderAPI_MakeWire(Ed1,Ed2,Ed3,Ed4);
				theFace = BRepBuilderAPI_MakeFace(theWire);
				BRepLib::BuildCurves3d(theFace);
				if (!BRepAlgo::IsValid(theFace))
					MessageBoxW (AfxGetApp()->m_pMainWnd->m_hWnd, L"Error : The plate surface is not valid!!!", L"CasCade Error", MB_ICONERROR);
			}

			Handle(AIS_Shape) anAISShape=new AIS_Shape(theFace);
			myAISContext->SetColor (anAISShape, Quantity_NOC_BLUE1, Standard_False);
			myAISContext->SetMaterial (anAISShape, Graphic3d_NOM_SILVER, Standard_False);
			myAISContext->SetDisplayMode (anAISShape, 1, Standard_False);
			myAISContext->Display (anAISShape, Standard_False);
			myState = -1;
		}
		else
			AfxMessageBox (L"Select an edge on the second face!");
		((OCC_MainFrame*)AfxGetMainWnd())->SetStatusMessage("");

	}
}

void CModelingDoc::Popup(const Standard_Integer  x,
					     const Standard_Integer  y ,
                         const Handle(V3d_View)& aView)
{
  Standard_Integer PopupMenuNumber=0;
  myAISContext->InitSelected();
  if (myState == SELECT_EDGE_PLATE) 
    PopupMenuNumber=2;
  else if (myAISContext->MoreSelected())
    PopupMenuNumber=1;

  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_Popup3D));
  CMenu* pPopup = menu.GetSubMenu(PopupMenuNumber);
  ASSERT(pPopup != NULL);

  if (PopupMenuNumber == 1) // more than 1 object.
  {
    bool OneOrMoreInShading = false;
	for (myAISContext->InitSelected();myAISContext->MoreSelected ();myAISContext->NextSelected ())
    if (myAISContext->IsDisplayed(myAISContext->SelectedInteractive(),1)) OneOrMoreInShading=true;
	if(!OneOrMoreInShading)
   	pPopup->EnableMenuItem(5, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
   }

 
  POINT winCoord = { x , y };
  Handle(WNT_Window) aWNTWindow=
  Handle(WNT_Window)::DownCast(aView->Window());
  ClientToScreen ( (HWND)(aWNTWindow->HWindow()),&winCoord);
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , winCoord.x, winCoord.y , 
                         AfxGetMainWnd());
}
