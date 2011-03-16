#include <SamplesTopologyPackage.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TCollection_AsciiString.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Circ2d.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <GCE2d_MakeLine.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepOffsetAPI_MakeEvolved.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepFeat_MakePrism.hxx>
#include <BRepFeat_MakeDPrism.hxx>
#include <BRepFeat_MakeRevol.hxx>
#include <BRepFeat_MakePipe.hxx>
#include <BRepFeat_MakeLinearForm.hxx>
#include <BRepFeat_Gluer.hxx>
#include <BRepFeat_SplitShape.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <BRepMesh.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopLoc_Location.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <LocOpe_FindEdges.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopAbs_Orientation.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Axis.hxx>
#include <AIS_Point.hxx>
#include <AIS_Line.hxx>
#include <AIS_Plane.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <ISession_Direction.hxx>
#include <Quantity_NameOfColor.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <Precision.hxx>
#include <OSD.hxx>

#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

#ifdef WNT
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#else
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#endif




//===============================================================
// Function name: CreateViewer
//===============================================================
 Handle(V3d_Viewer) SamplesTopologyPackage::CreateViewer (const Standard_ExtString aName) 
{
#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice) defaultDevice;
    
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_WNTGraphicDevice();
  return new V3d_Viewer(defaultDevice, aName);
#else
static Handle(Graphic3d_GraphicDevice) defaultDevice;
    
  if (defaultDevice.IsNull()) 
    defaultDevice = new Graphic3d_GraphicDevice("");
  return new V3d_Viewer(defaultDevice, aName);
#endif //WNT
}

//===============================================================
// Function name: SetWindow
//===============================================================
void SamplesTopologyPackage::SetWindow (const Handle(V3d_View)& aView,
					const Standard_Integer hiwin,
					const Standard_Integer lowin)
{
#ifdef WNT
  Handle(Graphic3d_WNTGraphicDevice) d = 
    Handle(Graphic3d_WNTGraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(WNT_Window) w = new WNT_Window(d,hiwin,lowin);
#else
  Handle(Graphic3d_GraphicDevice) d = 
    Handle(Graphic3d_GraphicDevice)::DownCast(aView->Viewer()->Device());
  Handle(Xw_Window) w = new Xw_Window(d,hiwin,lowin,Xw_WQ_3DQUALITY);
#endif
  aView->SetWindow(w);
}



//======================================================================
//=                                                                    =
//=                      Topology  Primitives                          =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: MakeBox
//===============================================================
 void SamplesTopologyPackage::MakeBox(const Handle(AIS_InteractiveContext)& aContext,
				      TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  TopoDS_Shape B1 = BRepPrimAPI_MakeBox (200.,150.,100.).Shape();
  Handle(AIS_Shape) aBox1 = new AIS_Shape(B1);
  aContext->SetMaterial(aBox1,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetColor(aBox1,Quantity_NOC_GREEN,Standard_False); 
  aContext->Display(aBox1);
  TopoDS_Shape B2 = BRepPrimAPI_MakeBox (gp_Ax2(gp_Pnt(-200.,-80.,-70.),gp_Dir(1.,2.,1.)),80.,90.,120.).Shape();
  Handle(AIS_Shape) aBox2 = new AIS_Shape(B2);
  aContext->SetMaterial(aBox2,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetColor(aBox2,Quantity_NOC_RED,Standard_False); 
  aContext->Display(aBox2);

  Message = "\
		\n\
TopoDS_Shape B1 = BRepPrimAPI_MakeBox (200.,150.,100.); \n\
TopoDS_Shape B2 = BRepPrimAPI_MakeBox (gp_Ax2(gp_Pnt(-200.,-80.,-70.), \n\
                                          gp_Dir(1.,2.,1.)), \n\
                                   80.,90.,120.); \n\
		\n";
}

//===============================================================
// Function name: MakeCylinder
//===============================================================
 void SamplesTopologyPackage::MakeCylinder(const Handle(AIS_InteractiveContext)& aContext,
					   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  TopoDS_Shape C1 = BRepPrimAPI_MakeCylinder (50.,200.).Shape();
  Handle(AIS_Shape) aCyl1 = new AIS_Shape(C1);
  aContext->SetMaterial(aCyl1,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetColor(aCyl1,Quantity_NOC_RED,Standard_False); 
  aContext->Display(aCyl1);
  TopoDS_Shape C2 = BRepPrimAPI_MakeCylinder (gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)),
					      40.,110.,210.*PI180).Shape();
  Handle(AIS_Shape) aCyl2 = new AIS_Shape(C2);
  aContext->SetMaterial(aCyl2,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetColor(aCyl2,Quantity_NOC_MATRABLUE,Standard_False); 	
  aContext->Display(aCyl2);

  Message = "\
		\n\
TopoDS_Shape C1 = BRepPrimAPI_MakeCylinder (50.,200.); \n\
TopoDS_Shape C2 = BRepPrimAPI_MakeCylinder (gp_Ax2(gp_Pnt(200.,200.,0.), \n\
                                        gp_Dir(0.,0.,1.)), \n\
                                        40.,110.,210.*PI180.); \n\
		\n";
}

//===============================================================
// Function name: MakeCone
//===============================================================
 void SamplesTopologyPackage::MakeCone(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape C1 = BRepPrimAPI_MakeCone (50.,25.,200.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(C1);
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False); 		
  aContext->Display(ais1);
  TopoDS_Shape C2 = BRepPrimAPI_MakeCone(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(0.,0.,1.)),
				     60.,0.,150.,210.*PI180).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(C2);
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
  aContext->Display(ais2);

  Message ="\
		\n\
TopoDS_Shape C1 = BRepPrimAPI_MakeCone (50.,25.,200.); \n\
TopoDS_Shape C2 = BRepPrimAPI_MakeCone(gp_Ax2(gp_Pnt(100.,100.,0.), \n\
                                          gp_Dir(0.,0.,1.)), \n\
                                   605.,0.,150.,210.*PI180); \n\
		\n";
}

//===============================================================
// Function name: MakeSphere
//===============================================================
 void SamplesTopologyPackage::MakeSphere(const Handle(AIS_InteractiveContext)& aContext,
					 TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S1 = BRepPrimAPI_MakeSphere(gp_Pnt(-200.,-250.,0.),80.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
  aContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais1);
  TopoDS_Shape S2 = BRepPrimAPI_MakeSphere(100.,120.*PI180).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais2);
  TopoDS_Shape S3 = BRepPrimAPI_MakeSphere(gp_Pnt(200.,250.,0.),100.,
				       -60.*PI180, 60.*PI180).Shape();
  Handle(AIS_Shape) ais3 = new AIS_Shape(S3);
  aContext->SetColor(ais3,Quantity_NOC_RED,Standard_False); 
  aContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais3);
  TopoDS_Shape S4 = BRepPrimAPI_MakeSphere(gp_Pnt(0.,0.,-300.),150.,
				       -45.*PI180, 45.*PI180, 45.*PI180).Shape();
  Handle(AIS_Shape) ais4 = new AIS_Shape(S4);
  aContext->SetColor(ais4,Quantity_NOC_MATRABLUE,Standard_False); 
  aContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais4);

  Message = "\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeSphere(gp_Pnt(-200.,-250.,0.),80.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeSphere(100.,120.*PI180); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeSphere(gp_Pnt(200.,250.,0.),100., \n\
                                     -60.*PI180, 60.*PI180); \n\
TopoDS_Shape S4 = BRepPrimAPI_MakeSphere(gp_Pnt(0.,0.,-300.),150., \n\
                                     -45.*PI180, 45.*PI180, 45.*PI180); \n\
		\n";
}

//===============================================================
// Function name: MakeTorus
//===============================================================
 void SamplesTopologyPackage::MakeTorus(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S1 = BRepPrimAPI_MakeTorus(60.,20.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
  aContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais1);
  TopoDS_Shape S2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(1.,1.,1.)),
				      50.,20.,210.*PI180).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais2);
  TopoDS_Shape S3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-200.,-150.,-100),gp_Dir(0.,1.,0.)),
				      60.,20.,-45.*PI180,45.*PI180,90.*PI180).Shape();
  Handle(AIS_Shape) ais3= new AIS_Shape(S3);
  aContext->SetColor(ais3,Quantity_NOC_CORAL,Standard_False); 
  aContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais3);

  Message = "\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeTorus(60.,20.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(1.,1.,1.)), \n\
                                    50.,20.,210.*PI180); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-200.,-150.,-100),gp_Dir(0.,1.,0.)), \n\
                                    60.,20.,-45.*PI180,45.*PI180,90.*PI180); \n\
		\n";
}

//===============================================================
// Function name: MakeWedge
//===============================================================
 void SamplesTopologyPackage::MakeWedge(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S1 = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
  aContext->SetColor(ais1,Quantity_NOC_AZURE,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais1);
  TopoDS_Shape S2 = BRepPrimAPI_MakeWedge(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(0.,0.,1.)),
				      60.,50.,80.,25.,-10.,40.,70.).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_CORAL2,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais2);
  
  Message = "\
		\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakeWedge(gp_Ax2(gp_Pnt(100.,100.,0.),gp_Dir(0.,0.,1.)), \n\
                                    60.,50.,80.,25.,-10.,40.,70.); \n\
		\n";
}

//===============================================================
// Function name: MakePrism
//===============================================================
 void SamplesTopologyPackage::MakePrism(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.));
  Handle(AIS_Shape) ais1 = new AIS_Shape(V1);
  aContext->Display(ais1);
  TopoDS_Shape S1 = BRepPrimAPI_MakePrism(V1,gp_Vec(0.,0.,100.));
  Handle(AIS_Shape) ais2 = new AIS_Shape(S1);
  aContext->Display(ais2);
  
  TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-150.,-150,0.), gp_Pnt(-50.,-50,0.));
  Handle(AIS_Shape) ais3 = new AIS_Shape(E);
  aContext->Display(ais3);
  TopoDS_Shape S2 = BRepPrimAPI_MakePrism(E,gp_Vec(0.,0.,100.));
  Handle(AIS_Shape) ais4 = new AIS_Shape(S2);
  aContext->SetColor(ais4,Quantity_NOC_CORAL2,Standard_False); 
  aContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais4);
  
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.)).Edge();
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.)).Edge();
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.)).Edge();
  TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3).Wire();
  TopoDS_Shape S3 = BRepPrimAPI_MakePrism(W,gp_Vec(0.,0.,100.)).Shape();
  Handle(AIS_Shape) ais5 = new AIS_Shape(W);
  aContext->Display(ais5);
  Handle(AIS_Shape) ais6 = new AIS_Shape(S3);
  aContext->SetColor(ais6,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais6,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais6);
  
  gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.);
  TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c).Edge();
  TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec).Wire();
  TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc).Face();
  Handle(AIS_Shape) ais7 = new AIS_Shape(F);
  aContext->Display(ais7);
  TopoDS_Shape S4 = BRepPrimAPI_MakePrism(F,gp_Vec(0.,0.,100.)).Shape();
  Handle(AIS_Shape) ais8 = new AIS_Shape(S4);
  aContext->SetColor(ais8,Quantity_NOC_MATRABLUE,Standard_False); 
  aContext->SetMaterial(ais8,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais8);
  
  Message = "\
		\n\
--- Prism a vertex -> result is an edge --- \n\
\n\
TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.)); \n\
TopoDS_Shape S1 = BRepPrimAPI_MakePrism(V1,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism an edge -> result is a face --- \n\
\n\
TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-150.,-150,0.), gp_Pnt(-50.,-50,0.)); \n\
TopoDS_Shape S2 = BRepPrimAPI_MakePrism(E,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism an wire -> result is a shell --- \n\
\n\
TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.)); \n\
TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.)); \n\
TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.)); \n\
TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3); \n\
TopoDS_Shape S3 = BRepPrimAPI_MakePrism(W,gp_Vec(0.,0.,100.)); \n\
\n\
--- Prism a face or a shell -> result is a solid --- \n\
\n\
gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.gp_Dir(0.,0.,1.)), 80.); \n\
TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c); \n\
TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec); \n\
TopoDS_Face F = BRepBuilderAPI_MakeFacePI_MakePrism(F,gp_Vec(0.,0.,100.)); \n\
		\n";
}

//===============================================================
// Function name: MakeRevol
//===============================================================
 void SamplesTopologyPackage::MakeRevol(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-200.,-200.,0.)).Vertex();
  Handle(AIS_Shape) ais1 = new AIS_Shape(V1);
  aContext->Display(ais1);
  gp_Ax1 axe = gp_Ax1(gp_Pnt(-170.,-170.,0.),gp_Dir(0.,0.,1.));
  Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
  Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
  aContext->Display(ax1);
  TopoDS_Shape S1 = BRepPrimAPI_MakeRevol(V1,axe).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S1);
  aContext->Display(ais2);
  
  TopoDS_Edge E = BRepBuilderAPI_MakeEdge(gp_Pnt(-120.,-120,0.), gp_Pnt(-120.,-120,100.)).Edge();
  Handle(AIS_Shape) ais3 = new AIS_Shape(E);
  aContext->Display(ais3);
  axe = gp_Ax1(gp_Pnt(-100.,-100.,0.),gp_Dir(0.,0.,1.));
  Handle(Geom_Axis1Placement) Gax2 = new Geom_Axis1Placement(axe);
  Handle (AIS_Axis) ax2 = new AIS_Axis(Gax2);
  aContext->Display(ax2);
  TopoDS_Shape S2 = BRepPrimAPI_MakeRevol(E,axe).Shape();
  Handle(AIS_Shape) ais4 = new AIS_Shape(S2);
  aContext->SetColor(ais4,Quantity_NOC_YELLOW,Standard_False); 
  aContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais4);

  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0.,0.,0.), gp_Pnt(50.,0.,0.)).Edge();
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,0.,0.), gp_Pnt(50.,50.,0.)).Edge();
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(gp_Pnt(50.,50.,0.), gp_Pnt(0.,0.,0.)).Edge();
  TopoDS_Wire W = BRepBuilderAPI_MakeWire(E1,E2,E3).Wire();
  axe = gp_Ax1(gp_Pnt(0.,0.,30.),gp_Dir(0.,1.,0.));
  Handle(Geom_Axis1Placement) Gax3 = new Geom_Axis1Placement(axe);
  Handle (AIS_Axis) ax3 = new AIS_Axis(Gax3);
  aContext->Display(ax3);
  TopoDS_Shape S3 = BRepPrimAPI_MakeRevol(W,axe, 210.*PI180).Shape();
  Handle(AIS_Shape) ais5 = new AIS_Shape(W);
  aContext->Display(ais5);
  Handle(AIS_Shape) ais6 = new AIS_Shape(S3);
  aContext->SetColor(ais6,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais6,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais6);
  
  gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(200.,200.,0.),gp_Dir(0.,0.,1.)), 80.);
  TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c).Edge();
  TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec).Wire();
  TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()),Wc).Face();
  axe = gp_Ax1(gp_Pnt(290,290.,0.),gp_Dir(0.,1,0.));
  Handle(Geom_Axis1Placement) Gax4 = new Geom_Axis1Placement(axe);
  Handle (AIS_Axis) ax4 = new AIS_Axis(Gax4);
  aContext->Display(ax4);
  TopoDS_Shape S4 = BRepPrimAPI_MakeRevol(F,axe, 90.*PI180).Shape();
  Handle(AIS_Shape) ais8 = new AIS_Shape(S4);
  aContext->SetColor(ais8,Quantity_NOC_MATRABLUE,Standard_False); 
  aContext->SetMaterial(ais8,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais8);

  Message = "\
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
TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec); \n\
TopoDS_Face F = BRepBuilderAPI_MakeFace0.,0.),gp_Dir(0.,1,0.)); \n\
TopoDS_Shape S4 = BRepPrimAPI_MakeRevol(F,axe, 90.*PI180); \n\
		\n";
}

//===============================================================
// Function name: MakePipe
//===============================================================
 void SamplesTopologyPackage::MakePipe(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

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
  TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve).Edge();
  TopoDS_Wire W = BRepBuilderAPI_MakeWire(E).Wire();
  Handle(AIS_Shape) ais1 = new AIS_Shape(W);
  aContext->Display(ais1);
  gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(0.,0.,0.),gp_Dir(0.,1.,0.)),10.);
  TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c).Edge();
  TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec).Wire();
  Handle(AIS_Shape) ais3 = new AIS_Shape(Wc);
  aContext->Display(ais3);
  TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()),Wc).Face();
  TopoDS_Shape S = BRepOffsetAPI_MakePipe(W,F).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S);
  aContext->SetColor(ais2,Quantity_NOC_MATRABLUE,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);

  Message = "\
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
TopoDS_Face F = BRepBuilderAPI_MakeFaceAPI_MakePipe(W,F);\n\
		\n";
}

//===============================================================
// Function name: MakeThru
//===============================================================
 void SamplesTopologyPackage::MakeThru(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  gp_Circ c1 = gp_Circ(gp_Ax2(gp_Pnt(-100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(c1).Edge();
  TopoDS_Wire W1 = BRepBuilderAPI_MakeWire(E1).Wire();
  Handle(AIS_Shape) sec1 = new AIS_Shape(W1);
  aContext->Display(sec1,Standard_False);
  gp_Circ c2 = gp_Circ(gp_Ax2(gp_Pnt(-10.,0.,-0.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(c2);
  TopoDS_Wire W2 = BRepBuilderAPI_MakeWire(E2);
  Handle(AIS_Shape) sec2 = new AIS_Shape(W2);
  aContext->Display(sec2,Standard_False);	
  gp_Circ c3 = gp_Circ(gp_Ax2(gp_Pnt(-75.,0.,100.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E3 = BRepBuilderAPI_MakeEdge(c3);
  TopoDS_Wire W3 = BRepBuilderAPI_MakeWire(E3);
  Handle(AIS_Shape) sec3 = new AIS_Shape(W3);
  aContext->Display(sec3,Standard_False);
  gp_Circ c4= gp_Circ(gp_Ax2(gp_Pnt(0.,0.,200.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);
  TopoDS_Wire W4 = BRepBuilderAPI_MakeWire(E4);
  Handle(AIS_Shape) sec4 = new AIS_Shape(W4);
  aContext->Display(sec4);
  BRepOffsetAPI_ThruSections generator(Standard_False,Standard_True);
  generator.AddWire(W1);
  generator.AddWire(W2);
  generator.AddWire(W3);
  generator.AddWire(W4);
  generator.Build();
  TopoDS_Shape S1 = generator.Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
  aContext->SetColor(ais1,Quantity_NOC_MATRABLUE,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
  
  gp_Circ c1b = gp_Circ(gp_Ax2(gp_Pnt(100.,0.,-100.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E1b = BRepBuilderAPI_MakeEdge(c1b).Edge();
  TopoDS_Wire W1b = BRepBuilderAPI_MakeWire(E1b).Wire();
  Handle(AIS_Shape) sec1b = new AIS_Shape(W1b);
  aContext->Display(sec1b,Standard_False);
  gp_Circ c2b = gp_Circ(gp_Ax2(gp_Pnt(210.,0.,-0.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E2b = BRepBuilderAPI_MakeEdge(c2b).Edge();
  TopoDS_Wire W2b = BRepBuilderAPI_MakeWire(E2b).Wire();
  Handle(AIS_Shape) sec2b = new AIS_Shape(W2b);
  aContext->Display(sec2b,Standard_False);	
  gp_Circ c3b = gp_Circ(gp_Ax2(gp_Pnt(275.,0.,100.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E3b = BRepBuilderAPI_MakeEdge(c3b).Edge();
  TopoDS_Wire W3b = BRepBuilderAPI_MakeWire(E3b).Wire();
  Handle(AIS_Shape) sec3b = new AIS_Shape(W3b);
  aContext->Display(sec3b,Standard_False);
  gp_Circ c4b= gp_Circ(gp_Ax2(gp_Pnt(200.,0.,200.),gp_Dir(0.,0.,1.)),40.);
  TopoDS_Edge E4b = BRepBuilderAPI_MakeEdge(c4b).Edge();
  TopoDS_Wire W4b = BRepBuilderAPI_MakeWire(E4b).Wire();
  Handle(AIS_Shape) sec4b = new AIS_Shape(W4b);
  aContext->Display(sec4b);
  BRepOffsetAPI_ThruSections generatorb(Standard_True,Standard_False);
  generatorb.AddWire(W1b);
  generatorb.AddWire(W2b);
  generatorb.AddWire(W3b);
  generatorb.AddWire(W4b);
  generatorb.Build();
  TopoDS_Shape S2 = generatorb.Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_ALICEBLUE,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);
  
  Message = "\
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
TopoDS_Edge E4 = BRepBuilderAPI_MakeEdge(c4);\n\
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
		\n";
}

//===============================================================
// Function name: MakeEvolved
//===============================================================
 void SamplesTopologyPackage::MakeEvolved(const Handle(AIS_InteractiveContext)& aContext,
					  TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  BRepBuilderAPI_MakePolygon P;
  P.Add(gp_Pnt(0.,0.,0.));
  P.Add(gp_Pnt(200.,0.,0.));
  P.Add(gp_Pnt(200.,200.,0.));
  P.Add(gp_Pnt(0.,200.,0.));
  P.Add(gp_Pnt(0.,0.,0.));
  TopoDS_Wire W = P.Wire();
  Handle(AIS_Shape) ais1 = new AIS_Shape(W);
  aContext->Display(ais1);
  TopoDS_Wire wprof = BRepBuilderAPI_MakePolygon(gp_Pnt(0.,0.,0.),gp_Pnt(-60.,-60.,-200.)).Wire();
  Handle(AIS_Shape) ais3 = new AIS_Shape(wprof);
  aContext->Display(ais3);
  TopoDS_Shape S = BRepOffsetAPI_MakeEvolved(W,wprof,GeomAbs_Arc,Standard_True,Standard_False,Standard_True,0.0001);
  Handle(AIS_Shape) ais2 = new AIS_Shape(S);
  aContext->SetColor(ais2,Quantity_NOC_MATRABLUE,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);
  
  Message = "\
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
TopoDS_Shape S = BRepOffsetAPI_MakeEvolved(W,wprof,GeomAbs_Arc,Standard_True,Standard_False,Standard_True,0.0001);\n\
		\n";
}

//===============================================================
// Function name: MakeDraft
//===============================================================
 void SamplesTopologyPackage::MakeDraft(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  TopoDS_Shape S = BRepPrimAPI_MakeBox(200.,300.,150.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
  BRepOffsetAPI_DraftAngle adraft(S);
  TopExp_Explorer Ex;
  for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {
    TopoDS_Face F = TopoDS::Face(Ex.Current());
    Handle(Geom_Plane) surf = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(F));
    gp_Pln apln = surf->Pln();
    gp_Dir dirF = apln.Axis().Direction();
    if (dirF.IsNormal(gp_Dir(0.,0.,1.),Precision::Angular()))
      adraft.Add(F, gp_Dir(0.,0.,1.), 15.*PI180, gp_Pln(gp::XOY()));
  }
  ais1->Set(adraft.Shape());
  aContext->Redisplay(ais1);

  Message = "\
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
		\n";
}


//======================================================================
//=                                                                    =
//=                       Topological  Operations                      =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: Cut
//===============================================================
 void SamplesTopologyPackage::Cut(const Handle(AIS_InteractiveContext)& aContext,
				  TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60).Shape();
  Handle (AIS_Shape) ais1 = new AIS_Shape(theBox);
  aContext->SetDisplayMode(ais1,1);
  aContext->SetColor(ais1,Quantity_NOC_GREEN);
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC);
  aContext->Display(ais1);
  
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80).Shape();
  Handle (AIS_Shape) ais2 = new AIS_Shape(theSphere);
  aContext->SetDisplayMode(ais2,1);
  aContext->SetColor(ais2,Quantity_NOC_YELLOW);
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC);
  aContext->Display(ais2);
  
  TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theSphere,theBox);
  
  OSD::MilliSecSleep(1000);
  aContext->Erase(ais1,Standard_True,Standard_False);
  aContext->Erase(ais2,Standard_True,Standard_False);
  
  Handle (AIS_Shape) aSection = new AIS_Shape(ShapeCut);
  aContext->SetDisplayMode(aSection,1);
  aContext->SetColor(aSection,Quantity_NOC_RED);
  aContext->SetMaterial(aSection,Graphic3d_NOM_PLASTIC);
  aContext->Display(aSection);
  
  Message = "\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,40,40); \n\
 \n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80); \n\
 \n\
TopoDS_Shape ShapeCut = BRepAlgoAPI_Cut(theSphere,theBox); \n\
 \n";
}

//===============================================================
// Function name: Fuse
//===============================================================
 void SamplesTopologyPackage::Fuse(const Handle(AIS_InteractiveContext)& aContext,
				   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  gp_Pnt P(-5,5,-5);
  TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(60,200,70).Shape();
  Handle (AIS_Shape) ais1 = new AIS_Shape(theBox1);
  aContext->SetColor(ais1,Quantity_NOC_GREEN);
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC);
  aContext->Display(ais1);
  
  TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P,20,150,110).Shape();
  Handle (AIS_Shape) ais2 = new AIS_Shape(theBox2);
  aContext->SetColor(ais2,Quantity_NOC_YELLOW);
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC);
  aContext->Display(ais2);
  
  TopoDS_Shape FusedShape = BRepAlgoAPI_Fuse(theBox1,theBox2);
  
  OSD::MilliSecSleep(1000);
  aContext->Erase(ais1,Standard_True,Standard_False);
  aContext->Erase(ais2,Standard_True,Standard_False);
  
  Handle (AIS_Shape) aFusion = new AIS_Shape(FusedShape);
  aContext->SetDisplayMode(aFusion,1);
  aContext->SetColor(aFusion,Quantity_NOC_RED);
  aContext->SetMaterial(aFusion,Graphic3d_NOM_PLASTIC);
  aContext->Display(aFusion);

  Message = "\
		\n\
TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(50,200,70); \n\
 \n\
TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(-30,150,70); \n\
 \n\
TopoDS_Shape FusedShape = BRepAlgoAPI_Fuse(theBox1,theBox2); \n";
}

//===============================================================
// Function name: Common
//===============================================================
 void SamplesTopologyPackage::Common(const Handle(AIS_InteractiveContext)& aContext,
				     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  gp_Ax2 axe(gp_Pnt(10,10,10),gp_Dir(1,2,1));
  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(axe,60,80,100).Shape();
  
  Handle(AIS_Shape) aboxshape=new AIS_Shape(theBox);
  aContext->SetColor(aboxshape,Quantity_NOC_YELLOW);
  aContext->SetMaterial(aboxshape,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetDisplayMode(aboxshape,1);
  aContext->SetTransparency(aboxshape,0.7);
  aContext->Display(aboxshape);
  
  TopoDS_Shape theWedge = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape();
  
  Handle(AIS_Shape) awedge = new AIS_Shape(theWedge);
  aContext->SetColor(awedge,Quantity_NOC_RED,Standard_False);
  aContext->SetMaterial(awedge,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetTransparency(awedge,0.5);
  aContext->Display(awedge);
  
  TopoDS_Shape theCommonSurface = BRepAlgoAPI_Common(theBox,theWedge);

  OSD::MilliSecSleep(1000);
  aContext->Erase(aboxshape,Standard_True,Standard_False);
  aContext->Erase(awedge,Standard_True,Standard_False);

  Handle(AIS_Shape) acommon = new AIS_Shape(theCommonSurface);
  aContext->SetColor(acommon,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(acommon,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(acommon);

  Message = "\
		\n\
gp_Ax2 axe(gp_Pnt(10,10,10),gp_Dir(1,2,1)); \n\
 \n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(axe,60,80,100); \n\
 \n\
TopoDS_Shape theWedge = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
 \n\
TopoDS_Shape theCommonSurface = BRepAlgoAPI_Common(theBox,theWedge); \n\
 \n";
}

//===============================================================
// Function name: Section
//===============================================================
 void SamplesTopologyPackage::Section(const Handle(AIS_InteractiveContext)& aContext,
				      TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape atorus = BRepPrimAPI_MakeTorus(120,20).Shape();

  Handle(AIS_Shape) ashape=new AIS_Shape(atorus);
  aContext->SetColor(ashape,Quantity_NOC_RED);
  aContext->SetMaterial(ashape,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetDisplayMode(ashape,1);
  aContext->SetTransparency(ashape,0.1);
  aContext->Display(ashape);
  
  //  gp_Vec V1(1,1,1);
  Standard_Real radius = 120;
  Standard_Integer i;

  for (i=-3;i<=3;i++) {
    TopoDS_Shape asphere = BRepPrimAPI_MakeSphere(gp_Pnt(26*3*i,0,0),radius).Shape();
    
    Handle (AIS_Shape) theShape=new AIS_Shape (asphere);
    aContext->SetTransparency(theShape,0.1);
    aContext->SetColor(theShape,Quantity_NOC_WHITE);
    aContext->SetDisplayMode(theShape,1);
    aContext->Display(theShape);

    Standard_Boolean PerformNow=Standard_False; 

    BRepAlgoAPI_Section section(atorus,asphere,PerformNow);
    section.ComputePCurveOn1(Standard_True);
    section.Approximation(TopOpeBRepTool_APPROX);
    section.Build();

    Handle(AIS_Shape) asection=new AIS_Shape(section.Shape());
    aContext->SetDisplayMode(asection,0);
    aContext->SetColor(asection,Quantity_NOC_WHITE);
    aContext->Display(asection);
    if(i<3) {
      aContext->Erase(theShape,Standard_True,Standard_False);
    }
  }

  Message = "\
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
	\n";
}

//===============================================================
// Function name: PSection
//===============================================================
 void SamplesTopologyPackage::PSection(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape theTorus = BRepPrimAPI_MakeTorus(35,8).Shape();
  Handle(AIS_Shape) atorus = new AIS_Shape(theTorus);
  aContext->SetColor(atorus,Quantity_NOC_YELLOW,Standard_False); 
  aContext->SetMaterial(atorus,Graphic3d_NOM_PLASTIC,Standard_False);
  aContext->SetTransparency(atorus,0.1);
  aContext->Display(atorus);
  
  gp_Pln aplane(1,0.25,3,4);
  Handle (Geom_Plane) thePlane = new Geom_Plane(aplane);
  Handle (AIS_Plane) ais1 = new AIS_Plane(thePlane);
  aContext->Display(ais1);

  BRepAlgoAPI_Section section(theTorus,thePlane,Standard_False);
  section.ComputePCurveOn1(Standard_True);
  section.Approximation(TopOpeBRepTool_APPROX);
  section.Build();
  Handle(AIS_Shape) asection=new AIS_Shape(section.Shape());
  aContext->SetDisplayMode(asection , 0);
  aContext->SetColor(asection,Quantity_NOC_WHITE); 
  aContext->Display(asection);
  
  Message = "\
		\n\
TopoDS_Shape theTorus = BRepPrimAPI_MakeTorus(60.,20.); \n\
 \n\
gp_Pln P(1,2,1,-15); \n\
 \n\
TopoDS_Shape Psection = BRepAlgoAPI_Section(theTorus,P);  \n\
\n";
}

//===============================================================
// Function name: Blend
//===============================================================
 void SamplesTopologyPackage::Blend(const Handle(AIS_InteractiveContext)& aContext,
				    TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape Box = BRepPrimAPI_MakeBox(gp_Pnt(-400,0,0),200,230,180).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(Box);
  aContext->SetColor(ais1,Quantity_NOC_YELLOW,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False); 
  aContext->Display(ais1);
  
  BRepFilletAPI_MakeFillet fillet(Box);

  for (TopExp_Explorer ex(Box,TopAbs_EDGE); ex.More(); ex.Next()) {
    TopoDS_Edge Edge =TopoDS::Edge(ex.Current());
    fillet.Add(20,Edge);
  }

  OSD::MilliSecSleep(1000);
  aContext->Erase(ais1,Standard_True,Standard_False);

  TopoDS_Shape blendedBox = fillet.Shape();
  Handle(AIS_Shape) aBlendbox = new AIS_Shape(blendedBox);
  aContext->SetColor(aBlendbox,Quantity_NOC_YELLOW,Standard_False); 
  aContext->SetMaterial(aBlendbox,Graphic3d_NOM_PLASTIC,Standard_False); 
  aContext->Display(aBlendbox);



  gp_Pnt P1(250,150,75);
  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(300,200,200).Shape();
  TopoDS_Shape S2 = BRepPrimAPI_MakeBox(P1,120,180,70).Shape();

  TopoDS_Shape fusedShape = BRepAlgoAPI_Fuse(S1,S2);
  Handle(AIS_Shape) ais2 = new AIS_Shape(fusedShape);
  aContext->SetColor(ais2,Quantity_NOC_RED,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);  
  aContext->Display(ais2);

  BRepFilletAPI_MakeFillet fill(fusedShape);

  for (TopExp_Explorer ex1(fusedShape,TopAbs_EDGE); ex1.More(); ex1.Next()) {
    TopoDS_Edge E =TopoDS::Edge(ex1.Current());
    fill.Add(E);
  }

  for (Standard_Integer i = 1;i<=fill.NbContours();i++) {
    Standard_Real longueur(fill.Length(i));
    Standard_Real Rad(0.15*longueur);
    for (Standard_Integer j = 1; j <=fill.NbEdges(i);j++)
      fill.SetRadius(Rad,i,j);
  }

  TopoDS_Shape blendedFusedSolids = fill.Shape();

  Handle(AIS_Shape) aBlend = new AIS_Shape(blendedFusedSolids);
  aContext->SetColor(aBlend,Quantity_NOC_RED,Standard_False); 
  aContext->SetMaterial(aBlend,Graphic3d_NOM_PLASTIC,Standard_False);  
  aContext->Display(aBlend);

  aContext->Erase(ais2,Standard_True,Standard_False);


  Message = "\
		\n\
//THE YELLOW BOX\n\
TopoDS_Shape Box = BRepPrimAPI_MakeBox(gp_Pnt(-400,0,0),200,230,180);\n\
\n\
BRepFilletAPI_MakeFillet fillet(Box);\n\
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
BRepFilletAPI_MakeFillet fill(fusedShape);\n\
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
TopoDS_Shape blendedFusedSolids = fill.Shape(); \n";
}

//===============================================================
// Function name: EvolvedBlend
//===============================================================
 void SamplesTopologyPackage::EvolvedBlend(const Handle(AIS_InteractiveContext)& aContext,
					   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,200,200).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(theBox);
  aContext->SetColor(ais1,Quantity_NOC_BROWN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False); 
  aContext->Display(ais1);
  
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
    aContext->Redisplay(ais1);
  }
  
  OSD::MilliSecSleep(1000);

/////////////////////////////////////////////////////////////////////


  
  TopoDS_Shape theCylinder = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(-300,0,0),gp_Dir(0,0,1)),100,200).Shape();
  Handle(AIS_Shape) ais3 = new AIS_Shape(theCylinder);
  aContext->SetColor(ais3,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais3);
  
  BRepFilletAPI_MakeFillet fillet(theCylinder);
  
  TColgp_Array1OfPnt2d TabPoint2(1,20);
  
  for (Standard_Integer i=0; i<=19; i++) {
    gp_Pnt2d Point2d(i*2*PI/19,60*cos(i*PI/19-PI/2)+10);
    TabPoint2.SetValue(i+1,Point2d);
  }
  
  TopExp_Explorer exp2(theCylinder,TopAbs_EDGE);
  fillet.Add(TabPoint2,TopoDS::Edge(exp2.Current()));
  fillet.Build();
  if (fillet.IsDone() ){
    TopoDS_Shape LawEvolvedCylinder = fillet.Shape();
    ais3->Set(LawEvolvedCylinder);
    aContext->Redisplay(ais3);
  }
  
  gp_Pnt P(350,0,0);
  TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P,200,200,200).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(theBox2);
  aContext->SetColor(ais2,Quantity_NOC_RED,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais2);
  
  
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
    aContext->Redisplay(ais2);
  }


  Message = "\
		\n\
//THE BROWN BOX \n\
\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,200,200);	\n\
	\n\
BRepFilletAPI_MakeFillet Rake(theBox);	\n\
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
BRepFilletAPI_MakeFillet fillet(theCylinder);	\n\
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
BRepFilletAPI_MakeFillet fill(theBox2);	\n\
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
\n";
}

//===============================================================
// Function name: Chamfer
//===============================================================
 void SamplesTopologyPackage::Chamfer(const Handle(AIS_InteractiveContext)& aContext,
				      TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(60,200,70).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(theBox);
  aContext->SetColor(ais1,Quantity_NOC_YELLOW,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(ais1);

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
  aContext->SetColor(aBlendedBox,Quantity_NOC_YELLOW,Standard_False); 
  aContext->SetMaterial(aBlendedBox,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(aBlendedBox);
  
  OSD::MilliSecSleep(1000);
  aContext->Erase(ais1,Standard_True,Standard_False);
  
  Message = "\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(130,200,170); \n\
BRepFilletAPI_MakeChamfer MC(theBox); \n\
TopTools_IndexedDataMapOfShapeListOfShape M; \n\
TopExp::MapShapesAndAncestors(theBox,TopAbs_EDGE,TopAbs_FACE,M); \n\
for (Standard_Integer i;i<M.Extent();i++) { \n\
	TopoDS_Edge E = TopoDS::Edge(M.FindKey(i)); \n\
	TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First()); \n\
	MC.Add(15,15,E,F); \n\
	} \n\
TopoDS_Shape ChanfrenedBox = MC.Shape();  \n";
}


//======================================================================
//=                                                                    =
//=                      Topology  Building                            =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: Vertex
//===============================================================
 void SamplesTopologyPackage::Vertex(const Handle(AIS_InteractiveContext)& aContext,
				     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Vertex V1,V2,V3;
  
  V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(0,0,0)).Vertex();
  V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,25)).Vertex();
	
  gp_Pnt P(-12,8,-4);
  BRepBuilderAPI_MakeVertex MV(P);
  V3 = MV.Vertex();
  
  Handle(AIS_Shape) Point1 = new AIS_Shape(V1);
  aContext->Display(Point1);
  Handle(AIS_Shape) Point2 = new AIS_Shape(V2);
  aContext->Display(Point2);
  Handle(AIS_Shape) Point3 = new AIS_Shape(V3);
  aContext->Display(Point3);
  
  
  Message = "\
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
\n";
}

//===============================================================
// Function name: Edge
//===============================================================
 void SamplesTopologyPackage::Edge(const Handle(AIS_InteractiveContext)& aContext,
				   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Edge BlueEdge,YellowEdge,WhiteEdge,RedEdge,GreenEdge;
  TopoDS_Vertex V1,V2,V3,V4;
	
/////////////The blue edge

  BlueEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(-80,-50,-20),gp_Pnt(-30,-60,-60)).Edge();

/////////////The yellow edge
	
  V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30)).Vertex();
  V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(10,7,-25)).Vertex();
  YellowEdge = BRepBuilderAPI_MakeEdge(V1,V2).Edge();

/////////////The white edge
	
  gp_Ax1 aLocalAx1(gp_Pnt(10,10,10),gp_Dir(1,0,0));
  gp_Lin line(aLocalAx1);
  //  gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));
  WhiteEdge = BRepBuilderAPI_MakeEdge(line,-20,10).Edge();

//////////////The red edge

  gp_Ax2 aLocalAx2(gp_Pnt(10,0,0),gp_Dir(1,1,1));
  gp_Elips Elips(aLocalAx2,60,30);
  //  gp_Elips Elips(gp_Ax2(gp_Pnt(10,0,0),gp_Dir(1,1,1)),60,30);
  RedEdge = BRepBuilderAPI_MakeEdge(Elips,0,PI/2).Edge();

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
  aContext->SetColor(blue,Quantity_NOC_MATRABLUE,Standard_False); 
  aContext->Display(blue);
  
  Handle(AIS_Shape) yellow = new AIS_Shape(YellowEdge);
  aContext->SetColor(yellow,Quantity_NOC_YELLOW,Standard_False); 
  aContext->Display(yellow);
  
  Handle(AIS_Shape) white = new AIS_Shape(WhiteEdge);
  aContext->SetColor(white,Quantity_NOC_WHITE,Standard_False); 
  aContext->Display(white);
  
  Handle(AIS_Shape) red = new AIS_Shape(RedEdge);
  aContext->SetColor(red,Quantity_NOC_RED,Standard_False); 
  aContext->Display(red);
  
  Handle(AIS_Shape) green = new AIS_Shape(GreenEdge);
  aContext->SetColor(green,Quantity_NOC_GREEN,Standard_False); 
  aContext->Display(green);

  Handle(AIS_Shape) Point1 = new AIS_Shape(V3);
  aContext->Display(Point1);
  Handle(AIS_Shape) Point2 = new AIS_Shape(V4);
  aContext->Display(Point2);


  Message = "\
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
\n";
}

//===============================================================
// Function name: Wire
//===============================================================
 void SamplesTopologyPackage::Wire(const Handle(AIS_InteractiveContext)& aContext,
				   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Wire RedWire,YellowWire,WhiteWire,
  ExistingWire, ExistingWire2;

  TopoDS_Edge Edge1,Edge2,Edge3,Edge4,Edge5,Edge6,Edge7,LastEdge;
  TopoDS_Vertex LastVertex;

////////////The red wire is build from a single edge

  gp_Ax2   alocalAx2(gp_Pnt(250,0,0),gp_Dir(1,1,1));
  gp_Elips Elips(alocalAx2,160,90);
    //  gp_Elips Elips(gp_Ax2(gp_Pnt(250,0,0),gp_Dir(1,1,1)),160,90);
  Edge1 = BRepBuilderAPI_MakeEdge(Elips,0,PI/2).Edge();

  RedWire = BRepBuilderAPI_MakeWire(Edge1);

///////////the yellow wire is build from an existing wire and an edge
	
  alocalAx2 = gp_Ax2(gp_Pnt(-300,0,0),gp_Dir(1,0,0));
  gp_Circ circle(alocalAx2,80);
  //  gp_Circ circle(gp_Ax2(gp_Pnt(-300,0,0),gp_Dir(1,0,0)),80);
  Edge2 = BRepBuilderAPI_MakeEdge(circle,0,PI).Edge();

  ExistingWire = BRepBuilderAPI_MakeWire(Edge2);

  Edge3 = BRepBuilderAPI_MakeEdge(gp_Pnt(-300,0,-80),gp_Pnt(-90,20,-30)).Edge();

  BRepBuilderAPI_MakeWire MW1(ExistingWire,Edge3);
  if (MW1.IsDone()) {
    YellowWire = MW1;
  }


//////////the white wire is built with an existing wire and 3 edges.
//////////we use the methods Add, Edge and Vertex from BRepBuilderAPI_MakeWire.
  
  gp_Ax2 aLocalAx2(gp_Pnt(0,0,0),gp_Dir(0,1,0));
  gp_Circ circle2(aLocalAx2,200);
  //  gp_Circ circle2(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,1,0)),200);
  Edge4 = BRepBuilderAPI_MakeEdge(circle2,0,PI);

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
  aContext->SetColor(red,Quantity_NOC_RED); 
  aContext->Display(red);

  Handle(AIS_Shape) yellow = new AIS_Shape(YellowWire);
  aContext->SetColor(yellow,Quantity_NOC_YELLOW); 
  aContext->Display(yellow);

  Handle(AIS_Shape) white = new AIS_Shape(WhiteWire);
  aContext->SetColor(white,Quantity_NOC_WHITE); 
  aContext->Display(white);

  Handle(AIS_Shape) lastE = new AIS_Shape(LastEdge);
  aContext->SetWidth(lastE,3);
  aContext->SetColor(lastE,Quantity_NOC_RED); 
  aContext->Display(lastE);

  Handle(AIS_Shape) lastV = new AIS_Shape(LastVertex);
  aContext->Display(lastV);


  Message = "\
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
///we use the methods Add, Edge and Vertex from BRepBuilderAPI_MakeWire 	\n\
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
\n";
}

//===============================================================
// Function name: Face
//===============================================================
 void SamplesTopologyPackage::Face(const Handle(AIS_InteractiveContext)& aContext,
				   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Face WhiteFace, BrownFace, RedFace, PinkFace;
  TopoDS_Edge Edge1, Edge2, Edge3, Edge4, Edge5, Edge6, Edge7;
  TopoDS_Wire Wire1;
  gp_Pnt P1, P2, P3, P4, P5, P6;



  gp_Ax3 aLocalAx3(gp_Pnt(0,0,0),gp_Dir(1,0,0));
  gp_Sphere sphere (aLocalAx3,150);
  //  gp_Sphere sphere (gp_Ax3(gp_Pnt(0,0,0),gp_Dir(1,0,0)),150);

  WhiteFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9).Face();

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

  RedFace = BRepBuilderAPI_MakeFace(curve);

////////////////////

  gp_Ax2 aLocalAx2(gp_Pnt(0,0,0),gp_Dir(1,0,0));
  gp_Circ circle(aLocalAx2,80);
  //  gp_Circ circle(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(1,0,0)),80);
  Edge1 = BRepBuilderAPI_MakeEdge(circle,0,PI).Edge();

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
  
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(BSplineSurf);

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
  
  BRepTools::Write(PinkFace,"PinkFace.rle");


/////////////Display
  Handle(AIS_Shape) white = new AIS_Shape(WhiteFace);
  aContext->SetColor(white,Quantity_NOC_WHITE);
  aContext->SetMaterial(white,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(white);
  
  Handle(AIS_Shape) red = new AIS_Shape(RedFace);
  aContext->SetColor(red,Quantity_NOC_RED);
  aContext->SetMaterial(red,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(red);
  
  Handle(AIS_Shape) brown = new AIS_Shape(BrownFace);
  aContext->SetColor(brown,Quantity_NOC_BROWN);
  aContext->SetMaterial(brown,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(brown);
  
  Handle(AIS_Shape) pink = new AIS_Shape(PinkFace);
  aContext->SetColor(pink,Quantity_NOC_HOTPINK);
  aContext->SetMaterial(pink,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(pink);


  Message = "\
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
\n";
Message += "\
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
\n";
}

//===============================================================
// Function name: Shell
//===============================================================
 void SamplesTopologyPackage::Shell(const Handle(AIS_InteractiveContext)& aContext,
				    TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

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
  
  TopoDS_Face WhiteFace = BRepBuilderAPI_MakeFace(BSpline);
  
  Handle(AIS_Shape) white = new AIS_Shape(WhiteFace);
  aContext->SetColor(white,Quantity_NOC_WHITE);
  aContext->SetMaterial(white,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->SetTransparency(white,0.7);
  aContext->Display(white);
  
  TopoDS_Shell aShell = BRepBuilderAPI_MakeShell(BSpline);
  Handle(AIS_Shape) anAISShell = new AIS_Shape(aShell);
  aContext->SetDisplayMode(anAISShell,0);
  aContext->Display(anAISShell);
  

  
  Message = "\
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
\n";
}

//===============================================================
// Function name: Compound
//===============================================================
 void SamplesTopologyPackage::Compound(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  BRep_Builder builder;
  TopoDS_Compound Comp;
  builder.MakeCompound(Comp);
  
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(-20,10,-30)).Vertex();
  builder.Add(Comp,aVertex);
  
  gp_Ax1 aLocalAx1(gp_Pnt(10,10,10),gp_Dir(1,0,0));
  gp_Lin line(aLocalAx1);
  //  gp_Lin line(gp_Ax1(gp_Pnt(10,10,10),gp_Dir(1,0,0)));
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(line,-20,10).Edge();
  builder.Add(Comp,anEdge);
  
  gp_Ax3 aLocalAx3(gp_Pnt(-80,0,0),gp_Dir(1,0,0));
  gp_Sphere sphere (aLocalAx3,150);
  //  gp_Sphere sphere (gp_Ax3(gp_Pnt(-80,0,0),gp_Dir(1,0,0)),150);
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(sphere,0.1,0.7,0.2,0.9).Face();
  builder.Add(Comp,aFace);
  
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(-60,0,0),30,60,40).Shape();
  builder.Add(Comp,aBox);
  
  Handle(AIS_Shape) white = new AIS_Shape(Comp);
  aContext->SetDisplayMode(white,0);
  aContext->Display(white);


  
  Message = "\
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
\n";
}

//===============================================================
// Function name: Sewing
//===============================================================
 void SamplesTopologyPackage::Sewing(const Handle(AIS_InteractiveContext)& aContext,
				     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  gp_Pnt P(0,0,0);
  gp_Vec V(0,0,1);
  Handle(Geom_Plane) Pi=new Geom_Plane(P,V);
  Handle(Geom_RectangularTrimmedSurface) GeometricSurface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);
  TopoDS_Shape FirstShape = BRepBuilderAPI_MakeFace(GeometricSurface);
  
  Handle(AIS_Shape) white1 = new AIS_Shape(FirstShape);
  aContext->SetColor(white1,Quantity_NOC_RED);
  aContext->SetMaterial(white1,Graphic3d_NOM_PLASTIC,Standard_False); 
  aContext->SetTransparency(white1,0.4);
  aContext->Display(white1);

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
  TopoDS_Shape SecondShape = BRepBuilderAPI_MakeFace(aSurf);
  
  Handle(AIS_Shape) white2 = new AIS_Shape(SecondShape);
  aContext->SetColor(white2,Quantity_NOC_YELLOW);
  aContext->SetMaterial(white2,Graphic3d_NOM_PLASTIC,Standard_False);  
  aContext->SetTransparency(white2,0.4);
  aContext->Display(white2);

  
  BRepOffsetAPI_Sewing aMethod;
  aMethod.Add(FirstShape);	
  aMethod.Add(SecondShape);
  
  aMethod.Perform();
  
  TopoDS_Shape sewedShape = aMethod.SewedShape();

  Handle(AIS_Shape) result = new AIS_Shape(sewedShape);
  aContext->SetDisplayMode(result,0);
  aContext->Display(result);


  Message = "\
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
\n";
}

//===============================================================
// Function name: Builder
//===============================================================
 void SamplesTopologyPackage::Builder(const Handle(AIS_InteractiveContext)& aContext,
				      TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

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
  //  Standard_Real R = 100;
  
  //Edge EX01
  C = new Geom_Circle(gp_Ax2(gp_Pnt(100,0,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
  B.MakeEdge(EX01,C,precision);
  V001.Orientation(TopAbs_FORWARD);
  V101.Orientation(TopAbs_REVERSED);
  B.Add(EX01,V001);
  B.Add(EX01,V101);
  //Parameters
  B.UpdateVertex(V001,0,EX01,precision);
  B.UpdateVertex(V101,PI,EX01,precision);

  //Edge EX11
  C = new Geom_Circle(gp_Ax2(gp_Pnt(100,150,100),gp_Dir(0,1,0),gp_Dir(-1,0,0)),100);
  B.MakeEdge(EX11,C,precision);
  V011.Orientation(TopAbs_FORWARD);
  V111.Orientation(TopAbs_REVERSED);
  B.Add(EX11,V011);
  B.Add(EX11,V111);
  //Parameters
  B.UpdateVertex(V011,0,EX11,precision);
  B.UpdateVertex(V111,PI,EX11,precision);
  
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
  
  BRepTools::Write(FXMAX,"f1.rle");
  
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
  B.UpdateVertex(V101,PI,EX01,FYMAX,precision);

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
  B.UpdateVertex(V111,PI,EX11,FYMIN,precision);

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
  B.UpdateVertex(V101,PI,EX01,FZMIN,precision);
  
  EY11.Orientation(TopAbs_FORWARD);
  B.Add(W,EY11);
  //pcurve
  L2d = new Geom2d_Line(gp_Pnt2d(PI,0),gp_Dir2d(0,1));
  B.UpdateEdge(EY11,L2d,FZMIN,precision);

  EX11.Orientation(TopAbs_REVERSED);
  B.Add(W,EX11);
  //pcurve
  L2d = new Geom2d_Line(gp_Ax2d(gp_Pnt2d(0,150),gp_Dir2d(1,0)));
  B.UpdateEdge(EX11,L2d,FZMIN,precision);
  B.UpdateVertex(V111,PI,EX11,FZMIN,precision);
  B.UpdateVertex(V011,0,EX11,FZMIN,precision);
  
  EY01.Orientation(TopAbs_REVERSED);
  B.Add(W,EY01);
  //pcurve
  L2d = new Geom2d_Line(gp_Pnt2d(0,0),gp_Dir2d(0,1));
  B.UpdateEdge(EY01,L2d,FZMIN,precision);
  
  B.Add(FZMIN,W);
  
  FYMAX.Orientation(TopAbs_REVERSED);

  BRepTools::Write(FZMIN,"f3.rle");
  BRepTools::Write(FYMAX,"f2.rle");

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

  BRepTools::Write(Sol,"solid.rle");
  Handle(AIS_Shape) borne = new AIS_Shape(Sol);
  aContext->SetDisplayMode(borne,1);
  aContext->SetColor(borne,Quantity_NOC_RED);
  aContext->SetMaterial(borne,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(borne);

  Message = "\
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
B.MakeWire (W);	\n";
Message += "\
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
\n";
}

//===============================================================
// Function name: Geometry
//===============================================================
 void SamplesTopologyPackage::Geometry(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  //geometrie of a vertex
  TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0,120,70));
  //  gp_Pnt GeometricPoint = 
  BRep_Tool::Pnt(aVertex);

  Handle(AIS_Shape) vert = new AIS_Shape(aVertex);
  aContext->Display(vert);

  //geometrie of an edge
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(100,50,250),gp_Pnt(-30,-100,-50));
  Handle(AIS_Shape) yellow = new AIS_Shape(anEdge);
  aContext->SetWidth(yellow,2);
  aContext->Display(yellow);

  TopLoc_Location location;
  Standard_Real first, last;
  Handle (Geom_Curve) aCurve = BRep_Tool::Curve(anEdge,location,first,last);
  TopoDS_Edge anEdgeDS = BRepBuilderAPI_MakeEdge(aCurve);

  Handle (Geom_Line) aLine = Handle (Geom_Line)::DownCast(aCurve);
  if (!aLine.IsNull()) {
    Handle (AIS_Line) DispLine = new AIS_Line(aLine);
    aContext->Display(DispLine);
  }
  
  //geometrie of a face
  gp_Pnt P(-20,-20,-20);
  gp_Vec V(0,0,1);
  Handle(Geom_Plane) Pi=new Geom_Plane(P,V);
  Handle(Geom_RectangularTrimmedSurface) Surface=new Geom_RectangularTrimmedSurface(Pi,0.,100.,0.,100.);
  TopoDS_Face RedFace = BRepBuilderAPI_MakeFace(Surface);
  
  Handle(AIS_Shape) red = new AIS_Shape(RedFace);
  aContext->SetColor(red,Quantity_NOC_RED);
  aContext->SetMaterial(red,Graphic3d_NOM_PLASTIC,Standard_False);    
  aContext->Display(red);
  
  TopLoc_Location location2;
  Handle (Geom_Surface) aGeometricSurface = BRep_Tool::Surface(RedFace,location2);
  
  Handle (Geom_Plane) aPlane = Handle (Geom_Plane)::DownCast(aGeometricSurface);
  if (!aPlane.IsNull()) {
    Handle (AIS_Plane) DispPlane = new AIS_Plane(aPlane);
    aContext->Display(DispPlane);
  }



  Message = "\
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
\n";
}

//===============================================================
// Function name: Explorer
//===============================================================
 void SamplesTopologyPackage::Explorer(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
	
  TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100,100,100).Shape();
  Standard_Integer j(8);
  Handle(AIS_Shape) theBox = new AIS_Shape(aBox);
  aContext->SetColor(theBox,Quantity_NOC_RED);
  aContext->SetMaterial(theBox,Graphic3d_NOM_PLASTIC,Standard_False);  
  aContext->Display(theBox);

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
    aContext->SetColor(theMovingFace,aCurrentColor);
    aContext->SetMaterial(theMovingFace,Graphic3d_NOM_PLASTIC,Standard_False);  
    //Find the normal vector of each face
    gp_Pln agpPlane = aPlane->Pln();
    gp_Ax1 norm = agpPlane.Axis();
    gp_Dir dir = norm.Direction();
    gp_Vec move(dir);

    //Connect
    TopLoc_Location aLocation;
    Handle (AIS_ConnectedInteractive) theTransformedDisplay = new AIS_ConnectedInteractive();
    theTransformedDisplay->Connect(theMovingFace, aLocation);


    Handle (Geom_Transformation) theMove = new Geom_Transformation(aLocation.Transformation());
    aContext->Display(theTransformedDisplay);

    for (Standard_Integer i=1;i<=30;i++) {
		  
      //Build a transformation on the display
      theMove->SetTranslation(move*i);
      if (orient==TopAbs_FORWARD) aContext->SetLocation(theTransformedDisplay,TopLoc_Location(theMove->Trsf()));
      else aContext->SetLocation(theTransformedDisplay,TopLoc_Location(theMove->Inverted()->Trsf()));
      
      aContext->Redisplay(theTransformedDisplay,Standard_False);
    }
    j+=15;
  }
  aContext->Erase(theBox,Standard_True,Standard_False);	


  Message = "\
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
\n";
}


//======================================================================
//=                                                                    =
//=                      Topology  Analysis                            =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: Validity
//===============================================================
 void SamplesTopologyPackage::Validity(const Handle(AIS_InteractiveContext)& aContext,
				       TCollection_AsciiString& Message,TCollection_AsciiString& Result) 
{
  aContext->EraseAll(Standard_False);
  TopoDS_Shape S = BRepPrimAPI_MakeBox(200.,300.,150.).Shape();
	
	
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);


  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);


  Message = "\
		\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(200.,300.,150.);\n\
Standard_Boolean theShapeIsValid = BRepAlgo::IsValid(S);\n\
if ( theShapeIsValid )\n\
{\n\
   Result = \"The Shape Is Valid !!\";\n\
}\n\
else\n\
{\n\
   Result = \"The Shape Is NOT Valid !!\";\n\
}\n\
\n";


	Standard_Boolean theShapeIsValid = BRepAlgo::IsValid(S);
	if ( theShapeIsValid )
	{
		Result = "The Shape Is Valid !! ";
	}
	else
	{
		Result = "The Shape Is NOT Valid !! ";
	}
}

//===============================================================
// Function name: LinearProp
//===============================================================
 void SamplesTopologyPackage::LinearProp(const Handle(AIS_InteractiveContext)& aContext,
                                         TCollection_AsciiString& Message,
                                         TCollection_AsciiString& Result) 
{
  aContext->EraseAll(Standard_False);


  TColgp_Array1OfPnt Points1(1,4);
  Points1.SetValue(1,gp_Pnt(0,0,0));
  Points1.SetValue(2,gp_Pnt(2,1,0));
  Points1.SetValue(3,gp_Pnt(4,0,0));
  Points1.SetValue(4,gp_Pnt(6,2,0));
  GeomAPI_PointsToBSpline PTBS1(Points1);
  Handle(Geom_BSplineCurve) BSC1 = PTBS1.Curve();
  TopoDS_Edge S = BRepBuilderAPI_MakeEdge(BSC1).Edge();

  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);


  GProp_GProps System;
  BRepGProp::LinearProperties(S,System);
  gp_Pnt G = System.CentreOfMass ();
  Standard_Real Length = System.Mass();
  gp_Mat I = System.MatrixOfInertia();

  Result = "Length Of all the Edges =";
  Result += TCollection_AsciiString(Length);
  Result += "\nCenterOfMass : \n   X=";
  Result += TCollection_AsciiString(G.X());
  Result += " Y=";
  Result += TCollection_AsciiString(G.Y());
  Result += " Z=";
  Result += TCollection_AsciiString(G.Z());
  Result +="\n";
  
  Result += "Matrix of Inertia :\n     ";
  Result += TCollection_AsciiString(I(1,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(1,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(1,3));
  Result += "\n     " ;
  Result += TCollection_AsciiString(I(2,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(2,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(2,3));
  Result += "\n     " ;
  Result += TCollection_AsciiString(I(3,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(3,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(3,3));
  Result += "\n" ;


  Message = "\
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
\n";
}

//===============================================================
// Function name: SurfaceProp
//===============================================================
 void SamplesTopologyPackage::SurfaceProp(const Handle(AIS_InteractiveContext)& aContext,
					  TCollection_AsciiString& Message,
					  TCollection_AsciiString& Result) 
{
  aContext->EraseAll(Standard_False);

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
  
  TopoDS_Shape S = BRepBuilderAPI_MakeFace(BSS).Face();
  
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);


  GProp_GProps System;
  BRepGProp::SurfaceProperties(S,System);
  gp_Pnt G = System.CentreOfMass ();
  Standard_Real Area = System.Mass();
  gp_Mat I = System.MatrixOfInertia();

  Result = "Area Of the Face =";
  Result += Area;
  Result += "\nCenterOfMass : \n   X=";
  Result += TCollection_AsciiString(G.X());
  Result += " Y=";
  Result += TCollection_AsciiString(G.Y());
  Result += " Z=";
  Result += TCollection_AsciiString(G.Z());
  Result +="\n";
  
  Result += "Matrix of Inertia :\n     ";
  Result += TCollection_AsciiString(I(1,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(1,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(1,3));
  Result += "\n     " ;
  Result += TCollection_AsciiString(I(2,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(2,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(2,3));
  Result += "\n     " ;
  Result += TCollection_AsciiString(I(3,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(3,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(3,3));
  Result += "\n" ;


  Message = "\
		\n\
TopoDS_Shape S = BRepBuilderAPI_MakeFace(BSplineSurf).Face();\n\
GProp_GProps System;\n\
BRepGProp::SurfaceProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Area = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n";
}

//===============================================================
// Function name: VolumeProp
//===============================================================
 void SamplesTopologyPackage::VolumeProp(const Handle(AIS_InteractiveContext)& aContext,
					 TCollection_AsciiString& Message,
					 TCollection_AsciiString& Result) 
{
  aContext->EraseAll(Standard_False);
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);


  GProp_GProps System;
  BRepGProp::VolumeProperties(S,System);
  gp_Pnt G = System.CentreOfMass ();
  Standard_Real Volume = System.Mass();
  gp_Mat I = System.MatrixOfInertia();
  
  Result = "Volume Of all the Shape =";
  Result += Volume;
  Result += "\nCenterOfMass : \n   X=";
  Result += TCollection_AsciiString(G.X());
  Result += " Y=";
  Result += TCollection_AsciiString(G.Y());
  Result += " Z=";
  Result += TCollection_AsciiString(G.Z());
  Result +="\n";
  
  Result += "Matrix of Inertia :\n     ";
  Result += TCollection_AsciiString(I(1,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(1,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(1,3));
  Result += "\n     " ;
  Result += TCollection_AsciiString(I(2,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(2,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(2,3));
  Result += "\n     " ;
  Result += TCollection_AsciiString(I(3,1));
  Result += " " ;
  Result += TCollection_AsciiString(I(3,2));
  Result += " " ;
  Result += TCollection_AsciiString(I(3,3));
  Result += "\n" ;
  

  Message = "\
		\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.);;\n\
GProp_GProps System;\n\
BRepGProp::VolumeProperties(S,System);\n\
gp_Pnt G = System.CentreOfMass ();\n\
Standard_Real Volume = System.Mass();\n\
gp_Mat I = System.MatrixOfInertia();\n\
\n";
}


//======================================================================
//=                                                                    =
//=                      Topology  Transformations                     =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: Mirror
//===============================================================
 void SamplesTopologyPackage::Mirror(const Handle(AIS_InteractiveContext)& aContext,
				     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1,Standard_False);
  gp_Trsf theTransformation;
  gp_Pnt PntCenterOfTheTransformation(110,60,60);
  Handle(AIS_Point) aispnt = new AIS_Point(new Geom_CartesianPoint(PntCenterOfTheTransformation));
  aContext->Display(aispnt);
  theTransformation.SetMirror(PntCenterOfTheTransformation);
  BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
  TopoDS_Shape S2 = myBRepTransformation.Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);

  Message = "\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Pnt PntCenterOfTheTransformation(110,60,60); \n\
theTransformation.SetMirror(PntCenterOfTheTransformation);\n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n";
}

//===============================================================
// Function name: Mirroraxis
//===============================================================
 void SamplesTopologyPackage::Mirroraxis(const Handle(AIS_InteractiveContext)& aContext,
					 TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1,Standard_False);
  gp_Trsf theTransformation;
  gp_Ax1 axe = gp_Ax1(gp_Pnt(110,60,60),gp_Dir(0.,1.,0.));
  Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
  Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
  aContext->Display(ax1);
  theTransformation.SetMirror(axe);
  BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
  TopoDS_Shape S2 = myBRepTransformation.Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);

  Message = "\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax1 Axis = gp_Ax1(gp_Pnt(110,60,60),gp_Dir(0.,1.,0.)); \n\
theTransformation.SetMirror(Axis);\n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n";
}

//===============================================================
// Function name: Rotate
//===============================================================
 void SamplesTopologyPackage::Rotate(const Handle(AIS_InteractiveContext)& aContext,
				     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1,Standard_False);
  gp_Trsf theTransformation;
  gp_Ax1 axe = gp_Ax1(gp_Pnt(200,60,60),gp_Dir(0.,1.,0.));
  Handle(Geom_Axis1Placement) Gax1 = new Geom_Axis1Placement(axe);
  Handle (AIS_Axis) ax1 = new AIS_Axis(Gax1);
  aContext->Display(ax1);
  theTransformation.SetRotation(axe,30*PI/180);
  BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
  TopoDS_Shape S2 = myBRepTransformation.Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);
  
  Message = "\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax1 Axis = gp_Ax1(gp_Pnt(200,60,60),gp_Dir(0.,1.,0.)); \n\
theTransformation.SetRotation(Axis,30*PI/180); // Rotation of 30 degrees \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n";
}

//===============================================================
// Function name: Scale
//===============================================================
 void SamplesTopologyPackage::Scale(const Handle(AIS_InteractiveContext)& aContext,
				    TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);
  
  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1,Standard_False);
  gp_Trsf theTransformation;
  gp_Pnt theCenterOfScale(200,60,60);
  Handle(AIS_Point) aispnt = new AIS_Point(new Geom_CartesianPoint(theCenterOfScale));

  aContext->Display(aispnt);
  theTransformation.SetScale(theCenterOfScale,0.5);
  BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
  TopoDS_Shape S2 = myBRepTransformation.Shape();

  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);

  Message = "\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Pnt theCenterOfScale(200,60,60); \n\
theTransformation.SetScale(theCenterOfScale,0.5); // Scale : value = 0.5 \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n";
}

//===============================================================
// Function name: Translation
//===============================================================
 void SamplesTopologyPackage::Translation(const Handle(AIS_InteractiveContext)& aContext,
					  TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeWedge(6.,10.,8.,2.).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
  aContext->Display(ais1);
  gp_Trsf theTransformation;
  gp_Vec theVectorOfTranslation(-6,-6,6);
 
  Handle (ISession_Direction) aDirection1 = new ISession_Direction(gp_Pnt(0,0,0),theVectorOfTranslation);
  aContext->Display(aDirection1);

  theTransformation.SetTranslation(theVectorOfTranslation);
  BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
  TopoDS_Shape S2 = myBRepTransformation.Shape();

  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);


  Message = "\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(6.,10.,8.,2.); \n\
gp_Trsf theTransformation; \n\
gp_Vec theVectorOfTranslation(6,6,6); \n\
theTransformation.SetTranslation(theVectorOfTranslation); \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n";
}

//===============================================================
// Function name: Displacement
//===============================================================
 void SamplesTopologyPackage::Displacement(const Handle(AIS_InteractiveContext)& aContext,
					   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
  gp_Trsf theTransformation;
  
  gp_Ax3 ax3_1(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  gp_Ax3 ax3_2(gp_Pnt(60,60,60),gp_Dir(1,1,1));

  theTransformation.SetDisplacement(ax3_1,ax3_2);
  BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);
  TopoDS_Shape TransformedShape = myBRepTransformation.Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(TransformedShape);
  aContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);

  Message = "\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_Trsf theTransformation; \n\
gp_Ax3 ax3_1(gp_Pnt(0,0,0),gp_Dir(0,0,1)); \n\
gp_Ax3 ax3_2(gp_Pnt(60,60,60),gp_Dir(1,1,1)); \n\
theTransformation.SetDisplacement(ax3_1,ax3_2); \n\
BRepBuilderAPI_Transform myBRepTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepTransformation.Shape();	\n";
}

//===============================================================
// Function name: Deform
//===============================================================
 void SamplesTopologyPackage::Deform(const Handle(AIS_InteractiveContext)& aContext,
				     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);

  gp_GTrsf theTransformation;
  gp_Mat rot(1, 0, 0, 0, 0.5, 0, 0, 0, 1.5);
  theTransformation.SetVectorialPart(rot);
  theTransformation.SetTranslationPart(gp_XYZ(5,5,5));

  BRepBuilderAPI_GTransform myBRepTransformation(S,theTransformation);
  TopoDS_Shape S2 = myBRepTransformation.Shape();

  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_BLUE1,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);
  
  Message = "\
\n\
TopoDS_Shape S = BRepPrimAPI_MakeWedge(60.,100.,80.,20.); \n\
gp_GTrsf theTransformation; \n\
gp_Mat rot(1, 0, 0, 0, 0.5, 0, 0, 0, 1.5); // scaling : 100% on X ; 50% on Y ; 150% on Z . \n\
theTransformation.SetVectorialPart(rot); \n\
theTransformation.SetTranslationPart(gp_XYZ(5,5,5)); \n\
BRepBuilderAPI_GTransform myBRepGTransformation(S,theTransformation);\n\
TopoDS_Shape TransformedShape = myBRepGTransformation.Shape();	\n";
}


//======================================================================
//=                                                                    =
//=                      Local  Operations                             =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: LocalPrism
//===============================================================
 void SamplesTopologyPackage::LocalPrism(const Handle(AIS_InteractiveContext)& aContext,
					 TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_GREEN,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
  
  TopExp_Explorer Ex;
  Ex.Init(S,TopAbs_FACE);
  Ex.Next();
  TopoDS_Face F = TopoDS::Face(Ex.Current());
  Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
  Handle(Geom_Plane) Pl = Handle(Geom_Plane)::DownCast(surf);
  gp_Dir D = Pl->Pln().Axis().Direction();

  // use the trigonometric orientation to make the extrusion.
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
  MKF.Init(surf,Standard_False);
  MKF.Add(MW.Wire());
  TopoDS_Shape FP = MKF.Face();
  BRepLib::BuildCurves3d(FP);
  BRepFeat_MakePrism MKP(S,FP,F,D,0,Standard_True);
  MKP.Perform(200.);
  TopoDS_Shape res1 = MKP.Shape();
  ais1->Set(res1);
  aContext->Redisplay(ais1); 

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
  MKF2.Init(surf,Standard_False);
  MKF2.Add(MW2.Wire());
  FP = MKF2.Face();
  BRepLib::BuildCurves3d(FP);
  BRepFeat_MakePrism MKP2(res1,FP,F2,D,1,Standard_True);
  MKP2.Perform(100.);
  TopoDS_Shape res2 = MKP2.Shape();
  ais1->Set(res2);
  aContext->Redisplay(ais1);

  Message = "\
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
	\n";
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
}

//===============================================================
// Function name: LocalDPrism
//===============================================================
 void SamplesTopologyPackage::LocalDPrism(const Handle(AIS_InteractiveContext)& aContext,
					  TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_RED,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
  TopExp_Explorer Ex;
  Ex.Init(S,TopAbs_FACE);
  Ex.Next();
  Ex.Next();
  Ex.Next();
  Ex.Next();
  Ex.Next();
  TopoDS_Face F = TopoDS::Face(Ex.Current());
  Handle(Geom_Surface) surf = BRep_Tool::Surface(F);
  gp_Ax2d   aLocalAx2d(gp_Pnt2d(200.,130.),gp_Dir2d(1.,0.));
  gp_Circ2d c(aLocalAx2d,50.);
  //  gp_Circ2d c(gp_Ax2d(gp_Pnt2d(200.,130.),gp_Dir2d(1.,0.)),50.);
  BRepBuilderAPI_MakeWire MW;
  Handle(Geom2d_Curve) aline = new Geom2d_Circle(c);
  MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,0.,PI));
  MW.Add(BRepBuilderAPI_MakeEdge(aline,surf,PI,2.*PI));
  BRepBuilderAPI_MakeFace MKF;
  MKF.Init(surf,Standard_False);
  MKF.Add(MW.Wire());
  TopoDS_Face FP = MKF.Face();
  BRepLib::BuildCurves3d(FP);
  BRepFeat_MakeDPrism MKDP(S,FP,F,10*PI180,1,Standard_True);
  MKDP.Perform(200);
  TopoDS_Shape res1 = MKDP.Shape();
  ais1->Set(res1);
  aContext->Redisplay(ais1);

  Message = "\
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
	\n";
}

//===============================================================
// Function name: LocalRevol
//===============================================================
 void SamplesTopologyPackage::LocalRevol(const Handle(AIS_InteractiveContext)& aContext,
					 TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_CORAL,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
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
  MKF1.Init(surf,Standard_False);
  MKF1.Add(MW1.Wire());
  TopoDS_Face FP = MKF1.Face();
  BRepLib::BuildCurves3d(FP);
  BRepFeat_MakeRevol MKrev(S,FP,F1,D,1,Standard_True);
  Ex.Next();
  Ex.Next();
  TopoDS_Face F2 = TopoDS::Face(Ex.Current());
  MKrev.Perform(F2);
  TopoDS_Shape res1 = MKrev.Shape();
  ais1->Set(res1);
  aContext->Redisplay(ais1); 

  Message = "\
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
	\n";
}

//===============================================================
// Function name: LocalPipe
//===============================================================
 void SamplesTopologyPackage::LocalPipe(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeBox(400.,250.,300.).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_CORAL,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
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
  MKF1.Init(surf,Standard_False);
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
  aContext->Redisplay(ais1); 
	
  Message = "\
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
	\n";
}

//===============================================================
// Function name: Rib
//===============================================================
 void SamplesTopologyPackage::Rib(const Handle(AIS_InteractiveContext)& aContext,
				  TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

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
				     gp_Vec(gp_Pnt(0.,0.,0.),gp_Pnt(0.,100.,0.))).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_CYAN2,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
  TopoDS_Wire W = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(gp_Pnt(50.,45.,100.),
						    gp_Pnt(100.,45.,50.)));	
  Handle(Geom_Plane) aplane = new Geom_Plane(0.,1.,0.,-45.);
  BRepFeat_MakeLinearForm aform(S, W, aplane, gp_Vec(0.,10.,0.), gp_Vec(0.,0.,0.),
				1, Standard_True);
  aform.Perform();
  
  TopoDS_Shape res = aform.Shape();
  ais1->Set(res);
  aContext->Redisplay(ais1);
  
  Message = "\
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
	\n";
}

//===============================================================
// Function name: Glue
//===============================================================
 void SamplesTopologyPackage::Glue(const Handle(AIS_InteractiveContext)& aContext,
				   TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(gp_Pnt(-500.,-500.,0.),gp_Pnt(-100.,-250.,300.)).Shape();
  Handle(AIS_Shape) ais1 = new AIS_Shape(S1);
  aContext->SetColor(ais1,Quantity_NOC_ORANGE,Standard_False); 
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais1);
  TopExp_Explorer Ex1;
  Ex1.Init(S1,TopAbs_FACE);
  Ex1.Next();
  Ex1.Next();
  Ex1.Next();
  Ex1.Next();
  Ex1.Next();
  TopoDS_Face F1 = TopoDS::Face(Ex1.Current());
  TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(-400.,-400.,300.),gp_Pnt(-200.,-300.,500.)).Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(S2);
  aContext->SetColor(ais2,Quantity_NOC_AZURE,Standard_False); 
  aContext->SetMaterial(ais2,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais2);
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
  aContext->Erase(ais2,Standard_False,Standard_False);
  ais1->Set(res1);
  aContext->Redisplay(ais1);	

  TopoDS_Shape S3 = BRepPrimAPI_MakeBox(500.,400.,300.).Shape();
  Handle(AIS_Shape) ais3 = new AIS_Shape(S3);
  aContext->SetColor(ais3,Quantity_NOC_ORANGE,Standard_False); 
  aContext->SetMaterial(ais3,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais3);
  TopExp_Explorer Ex3;
  Ex3.Init(S3,TopAbs_FACE);
  Ex3.Next();
  Ex3.Next();
  Ex3.Next();
  Ex3.Next();
  Ex3.Next();
  TopoDS_Face F3 = TopoDS::Face(Ex3.Current());
  TopoDS_Shape S4 = BRepPrimAPI_MakeBox(gp_Pnt(0.,0.,300.),gp_Pnt(200.,200.,500.)).Shape();
  Handle(AIS_Shape) ais4 = new AIS_Shape(S4);
  aContext->SetColor(ais4,Quantity_NOC_AZURE,Standard_False); 
  aContext->SetMaterial(ais4,Graphic3d_NOM_PLASTIC,Standard_False);   
  aContext->Display(ais4);
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
  aContext->Erase(ais3,Standard_False,Standard_False);
  ais4->Set(res2);
  aContext->Redisplay(ais4);
  
  Message = "\
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
	\n";
}

//===============================================================
// Function name: Split
//===============================================================
 void SamplesTopologyPackage::Split(const Handle(AIS_InteractiveContext)& aContext,
				    TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S = BRepPrimAPI_MakeBox(gp_Pnt(-100,-60,-80),150,200,170).Shape(); 
  Handle(AIS_Shape) ais1 = new AIS_Shape(S);
  aContext->SetColor(ais1,Quantity_NOC_RED);
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
  aContext->Display(ais1);

  BRepAlgoAPI_Section asect(S, gp_Pln(1,2,1,-15),Standard_False);
  asect.ComputePCurveOn1(Standard_True);
  asect.Approximation(Standard_True);
  asect.Build();
  TopoDS_Shape R = asect.Shape();
  BRepTools::Write(R,"R.rle");

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

  OSD::MilliSecSleep(1000);
  aContext->Erase(ais1,Standard_True,Standard_False);
  
  TopoDS_Shape Result = asplit.Shape();
  Handle(AIS_Shape) ais2 = new AIS_Shape(Result);
  aContext->SetDisplayMode(ais2,0);
  aContext->Display(ais2);


  Message = "\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(gp_Pnt(-100,-60,-80),150,200,170); 	\n\
		\n\
BRepAlgoAPI_Section asect(S, gp_Pln(1,2,1,-15),Standard_False);	\n\
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
\n";
}

//===============================================================
// Function name: Thick
//===============================================================
 void SamplesTopologyPackage::Thick(const Handle(AIS_InteractiveContext)& aContext,
				    TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150,200,110).Shape();
  Handle(AIS_Shape) abox1 = new AIS_Shape(S1);
  aContext->SetColor(abox1,Quantity_NOC_WHITE);
  aContext->SetMaterial(abox1,Graphic3d_NOM_PLASTIC,Standard_False);
  aContext->Display(abox1);

  TopTools_ListOfShape aList;
  TopExp_Explorer Ex(S1,TopAbs_FACE);
  Ex.Next();	//this is the front face
  TopoDS_Shape aFace = Ex.Current();
  aList.Append(aFace);
		
  TopoDS_Shape aThickSolid = BRepOffsetAPI_MakeThickSolid(S1,aList,10,0.01);

  Handle(AIS_Shape) ais1 = new AIS_Shape(aThickSolid);
  aContext->SetColor(ais1,Quantity_NOC_RED);
  aContext->SetMaterial(ais1,Graphic3d_NOM_PLASTIC,Standard_False);
  aContext->Display(ais1);


	
  OSD::MilliSecSleep(1000);
  aContext->Erase(abox1,Standard_True,Standard_False);

  Message = "\
	\n\
TopoDS_Shape S = BRepPrimAPI_MakeBox(150,200,110);	\n\
	\n\
TopTools_ListOfShape aList;	\n\
TopExp_Explorer Ex(S,TopAbs_FACE);	\n\
Ex.Next();	//in order to recover the front face	\n\
TopoDS_Shape aFace = Ex.Current();	\n\
aList.Append(aFace);	\n\
			\n\
TopoDS_Shape aThickSolid = BRepOffsetAPI_MakeThickSolid(S,aList,15,0.01);	\n\
	\n\
\n";
}

//===============================================================
// Function name: Offset
//===============================================================
 void SamplesTopologyPackage::Offset(const Handle(AIS_InteractiveContext)& aContext,
				     TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150,200,110).Shape();

  Handle(AIS_Shape) aisBox1 = new AIS_Shape(S1);
  aContext->SetColor(aisBox1,Quantity_NOC_BROWN);
  aContext->SetMaterial(aisBox1,Graphic3d_NOM_GOLD,Standard_False);
  aContext->Display(aisBox1);

  TopoDS_Shape anOffsetShape1 = BRepOffsetAPI_MakeOffsetShape(S1,60,0.01);

  Handle(AIS_Shape) ais1 = new AIS_Shape(anOffsetShape1);
  aContext->SetColor(ais1,Quantity_NOC_MATRABLUE);
  aContext->SetMaterial(ais1,Graphic3d_NOM_GOLD,Standard_False);
  aContext->SetTransparency(ais1,0.5);
  aContext->Display(ais1);

  OSD::MilliSecSleep(1000);

  TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(500,0,0),220,140,180).Shape();
	
  Handle(AIS_Shape) aisBox2 = new AIS_Shape(S2);
  aContext->SetColor(aisBox2,Quantity_NOC_WHITE);
  aContext->SetMaterial(aisBox2,Graphic3d_NOM_GOLD,Standard_False);
  aContext->SetTransparency(aisBox2,0.5);
  aContext->Display(aisBox2);

  TopoDS_Shape anOffsetShape2 = BRepOffsetAPI_MakeOffsetShape(S2,-40,0.01,
		   BRepOffset_Skin,Standard_False,Standard_False,GeomAbs_Arc);

  Handle(AIS_Shape) ais2 = new AIS_Shape(anOffsetShape2);
  aContext->SetColor(ais2,Quantity_NOC_MATRABLUE);
  aContext->SetMaterial(ais2,Graphic3d_NOM_GOLD,Standard_False);
  aContext->Display(ais2);

	
  Message = "\
	\n\
//The red box	\n\
\n\
TopoDS_Shape S1 = BRepPrimAPI_MakeBox(150,200,110);	\n\
\n\
TopoDS_Shape anOffsetShape1 = BRepOffsetAPI_MakeOffsetShape(S1,60,0.01);	\n\
\n\
//The white box	\n\
	\n\
TopoDS_Shape S2 = BRepPrimAPI_MakeBox(gp_Pnt(300,0,0),220,140,180);	\n\
\n\
TopoDS_Shape anOffsetShape2 = BRepOffsetAPI_MakeOffsetShape(S2,-20,0.01,	\n\
	BRepOffset_Skin,Standard_False,Standard_False,GeomAbs_Arc);	\n\
	\n\
//Warning : the offset shapes are blue.	\n\
\n\
\n";
}


//======================================================================
//=                                                                    =
//=                      Triangulations                                =
//=                                                                    =
//======================================================================

//===============================================================
// Function name: BuildMesh
//===============================================================
 void SamplesTopologyPackage::BuildMesh(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60).Shape();
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80).Shape();
  TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);
  BRepMesh::Mesh(ShapeFused,1);

  Handle (AIS_Shape) aSection = new AIS_Shape(ShapeFused);
  aContext->SetDisplayMode(aSection,1);
  aContext->SetColor(aSection,Quantity_NOC_RED);
  aContext->SetMaterial(aSection,Graphic3d_NOM_GOLD);
  aContext->Display(aSection);

  Standard_Integer result(0);

  for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {
    TopoDS_Face F =TopoDS::Face(ex.Current());
    TopLoc_Location L;
    Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);
    result = result + facing->NbTriangles();
  }

  Message = "\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60);	\n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80);	\n\
	\n\
TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);	\n\
	\n\
BRepMesh::Mesh(ShapeFused,1);	\n\
	\n\
Standard_Integer result(0);	\n\
	\n\
for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {	\n\
	TopoDS_Face F =TopoDS::Face(ex.Current());	\n\
	TopLoc_Location L;	\n\
	Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);	\n\
	result = result + facing->NbTriangles();	\n\
} 	\n\
\n\
--- Number of created triangles ---\n";

  TCollection_AsciiString nombre(result);
  Message += nombre;
  Message += "\
				  \n\
				\n";
}

//===============================================================
// Function name: DisplayMesh
//===============================================================
 void SamplesTopologyPackage::DisplayMesh(const Handle(AIS_InteractiveContext)& aContext,
					  TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60).Shape();
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80).Shape();
  TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);
  BRepMesh::Mesh(ShapeFused,1);
  
  Handle (AIS_Shape) aSection = new AIS_Shape(ShapeFused);
  aContext->SetDisplayMode(aSection,1);
  aContext->SetColor(aSection,Quantity_NOC_RED);
  aContext->SetMaterial(aSection,Graphic3d_NOM_GOLD);
  aContext->SetTransparency(aSection,0.1);
  aContext->Display(aSection);

  BRep_Builder builder;
  TopoDS_Compound Comp;
  builder.MakeCompound(Comp);

  for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {
  		
    TopoDS_Face F =TopoDS::Face(ex.Current());
    TopLoc_Location L;
    Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);
    TColgp_Array1OfPnt tab(1,(facing->NbNodes()));
    tab = facing->Nodes();
    Poly_Array1OfTriangle tri(1,facing->NbTriangles());
    tri = facing->Triangles();
    
    for (Standard_Integer i=1;i<=(facing->NbTriangles());i++) {
      Poly_Triangle trian = tri.Value(i);
      Standard_Integer index1,index2,index3,M=0,N=0;
      trian.Get(index1,index2,index3);
	
      for (Standard_Integer j=1;j<=3;j++) {
	switch (j) {
	case 1 :
	  M = index1;
	  N = index2;
	  break;
	case 2 :
	  N = index3;
	  break;
	case 3 :
	  M = index2;
	}
	
	BRepBuilderAPI_MakeEdge ME(tab.Value(M),tab.Value(N));
	if (ME.IsDone()) {
	  builder.Add(Comp,ME.Edge());
	}
      }
    }
  }
  Handle (AIS_Shape) atriangulation = new AIS_Shape(Comp);
  aContext->SetDisplayMode(atriangulation,0);
  aContext->SetColor(atriangulation,Quantity_NOC_WHITE);
  aContext->Display(atriangulation);

  
  Message = "\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60);	\n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80);	\n\
TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);	\n\
BRepMesh::Mesh(ShapeFused,1);	\n\
	\n\
BRep_Builder builder;	\n\
TopoDS_Compound Comp;	\n\
builder.MakeCompound(Comp);	\n\
	\n\
for (TopExp_Explorer ex(ShapeFused,TopAbs_FACE) ; ex.More(); ex.Next()) {	\n\
  			\n\
	TopoDS_Face F =TopoDS::Face(ex.Current());	\n\
	TopLoc_Location L;	\n\
	Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(F,L);	\n\
	TColgp_Array1OfPnt tab(1,(facing->NbNodes()));	\n\
	tab = facing->Nodes();	\n\
	Poly_Array1OfTriangle tri(1,facing->NbTriangles());	\n\
	tri = facing->Triangles();	\n\
	\n\
	for (Standard_Integer i=1;i<=(facing->NbTriangles());i++) {	\n\
		Poly_Triangle trian = tri.Value(i);	\n\
		Standard_Integer index1,index2,index3,M,N;	\n\
		trian.Get(index1,index2,index3);	\n\
		\n\
		for (Standard_Integer j=1;j<=3;j++) {	\n\
			switch (j) {	\n\
			case 1 :	\n\
				M = index1;	\n\
				N = index2;	\n\
			break;	\n\
			case 2 :	\n\
				N = index3;	\n\
			break;	\n\
			case 3 :	\n\
				M = index2;	\n\
			}	\n\
				\n\
			BRepBuilderAPI_MakeEdge ME(tab.Value(M),tab.Value(N));	\n\
			if (ME.IsDone()) {	\n\
				builder.Add(Comp,ME.Edge());	\n\
			}	\n\
		}	\n\
	}	\n\
}	\n\
	\n\
Warning : The visualisation of the mesh is not optimised.\n\
The shared edges between triangles are dispayed twice.\n\
The purpose here is only to show how to decode the data structure of triangulation.\n\
	\n";
}

//===============================================================
// Function name: ClearMesh
//===============================================================
 void SamplesTopologyPackage::ClearMesh(const Handle(AIS_InteractiveContext)& aContext,
					TCollection_AsciiString& Message) 
{
  aContext->EraseAll(Standard_False);

  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60).Shape();
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80).Shape();
  TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);
  BRepMesh::Mesh(ShapeFused,1);


  Handle (AIS_Shape) aSection = new AIS_Shape(ShapeFused);
  aContext->SetDisplayMode(aSection,1);
  aContext->SetColor(aSection,Quantity_NOC_RED);
  aContext->SetMaterial(aSection,Graphic3d_NOM_GOLD);
  aContext->Display(aSection);

  BRepTools::Clean(ShapeFused);

  TCollection_AsciiString test;
  if (!BRepTools::Triangulation(ShapeFused,1)) {
    test = "In fact the triangulation has been removed\n";
  }

  Message = "\
		\n\
TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200,60,60);	\n\
TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100,20,20),80);	\n\
TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere,theBox);	\n\
BRepMesh::Mesh(ShapeFused,1);	\n\
	\n\
BRepTools::Clean(ShapeFused);	\n\
	\n\
if (!BRepTools::Triangulation(ShapeFused,1)) {	\n\
	TCollection_AsciiString test(<In fact the triangulation has been removed>);	\n\
}	\n\
	\n\
	--- Result ---\n";

  Message += test;
  Message += "\
				  \n\
				\n";
}

