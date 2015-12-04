// HLR_Presentation.cpp: implementation of the HLR_Presentation class.
// Hidden lines removal
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HLR_Presentation.h"

#include <Geom_ToroidalSurface.hxx>
#include <HLRBRep_Algo.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <HLRAlgo_Projector.hxx>
#include <Prs3d_Projector.hxx>
#include <TopoDS_Shell.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <BRepTools.hxx>
#include <BRepMesh.hxx>
#include <BRep_Builder.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <TopoDS_Solid.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new HLR_Presentation;

// Initialization of array of samples
const HLR_Presentation::PSampleFuncType HLR_Presentation::SampleFuncs[] =
{
  &HLR_Presentation::sampleCylinder,
  &HLR_Presentation::sampleTorus,
  &HLR_Presentation::sampleBrepShape2,
  &HLR_Presentation::sampleBrepShape1
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define SharpEdge_COLOR   Quantity_NOC_CYAN1
#define SmoothEdge_COLOR  Quantity_NOC_RED
#define SewnEdge_COLOR    Quantity_NOC_ORANGE1
#define OutLine_COLOR     Quantity_NOC_MAGENTA1
#define IsoLine_COLOR     Quantity_NOC_YELLOW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HLR_Presentation::HLR_Presentation()
{
  FitMode = true;
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Hidden Lines Removal");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void HLR_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  setResultTitle("Hidden Lines Removal");
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}


//================================================================
// Function : Projector
// Purpose  : returns Prs3d_Projector object created corresponding 
//            to the current view parameters
//================================================================
Handle(Prs3d_Projector) HLR_Presentation::Projector(gp_Trsf& theTrsf)
{
  // retrieve view parameters for creation of Prs3d_Projector object
  getViewer()->InitActiveViews();
  Handle(V3d_View) aView = getViewer()->ActiveView();
  
  Standard_Real aProjVecX, aProjVecY, aProjVecZ;
  aView->Proj(aProjVecX, aProjVecY, aProjVecZ);
  
  Standard_Real aUpX, aUpY, aUpZ;
  aView->Up(aUpX, aUpY, aUpZ);

  Standard_Real aPntX = 0.0, aPntY = 0.0, aPntZ = 0.0;

  // create a projector object
  Handle(Prs3d_Projector) aProjector = new Prs3d_Projector(Standard_False, 0.0, 
    aProjVecX, aProjVecY, aProjVecZ, aPntX, aPntY, aPntZ, aUpX, aUpY, aUpZ);

  gp_Pnt At (aPntX,aPntY,aPntZ);
  gp_Dir Zpers (aProjVecX,aProjVecY,aProjVecZ);
  gp_Dir Ypers (aUpX,aUpY,aUpZ);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 NewCoordSystem (At, Zpers, Xpers);
  gp_Ax3 CurrentCoordSystem(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  gp_Trsf Trsf;
  Trsf.SetDisplacement(CurrentCoordSystem, NewCoordSystem);

  theTrsf = Trsf;

  return aProjector;
}


//================================================================
// Function : HLR_Presentation::HLR
// Purpose  : creates and displays shape representation by HLRBRep_Algo class
//================================================================
Standard_Boolean HLR_Presentation::HLR(const TopoDS_Shape& aShape, const Standard_Integer nbIso)
{
  setResultText(
    "  TopoDS_Shape aShape;" EOL
    "  // initializing aShape ..." EOL
    "" EOL
    "  // Build The algorithm object" EOL
    "  Handle(HLRBRep_Algo) myAlgo = new HLRBRep_Algo();" EOL
    "  " EOL
    "  // Add Shapes into the algorithm" EOL
    "  Standard_Real nbIso = 5; // number of isolines to display" EOL
    "  myAlgo->Add(aShape, nbIso);" EOL
    "" EOL
    "  // create a projector object" EOL
    "  Handle(V3d_View) aView;" EOL
    "  // initializing V3d_View ..." EOL
    "  " EOL
    "  Standard_Real aProjVecX, aProjVecY, aProjVecZ;" EOL
    "  aView->Proj(aProjVecX, aProjVecY, aProjVecZ);" EOL
    "  " EOL
    "  Standard_Real aUpX, aUpY, aUpZ;" EOL
    "  aView->Up(aUpX, aUpY, aUpZ);" EOL
    "" EOL
    "  Standard_Real aPntX = 0.0, aPntY = 0.0, aPntZ = 0.0;" EOL
    "" EOL
    "  // create a projector object" EOL
    "  Handle(Prs3d_Projector) aProjector = new Prs3d_Projector(Standard_False, 0.0, " EOL
    "    aProjVecX, aProjVecY, aProjVecZ, aPntX, aPntY, aPntZ, aUpX, aUpY, aUpZ);" EOL
    "  " EOL
    "  // Set The Projector" EOL
    "  myAlgo->Projector(aProjector->Projector());" EOL
    "  " EOL
    "  // Launches calculation of outlines of the shape " EOL
    "  myAlgo->Update();" EOL
    "" EOL
    "  // Computes the visible and hidden lines of the shape " EOL
    "  myAlgo->Hide();" EOL
    "  " EOL
    "  // Build the extraction object :" EOL
    "  HLRBRep_HLRToShape aHLRToShape(myAlgo);" EOL
    "  " EOL
    "  // extract the results of visible parts of the shape:" EOL
    "  TopoDS_Shape VCompound = aHLRToShape.VCompound();" EOL
    "  //Visible sharp edges, Sharp edges present a C0 continuity" EOL
    "  " EOL
    "  TopoDS_Shape Rg1LineVCompound = aHLRToShape.Rg1LineVCompound();" EOL
    "  //Visible smooth edges, Smooth edges present a G1 continuity " EOL
    "" EOL
    "  TopoDS_Shape RgNLineVCompound = aHLRToShape.RgNLineVCompound();" EOL
    "  //Visible sewn edges, Sewn edges present a C2 continuity" EOL
    "" EOL
    "  TopoDS_Shape OutLineVCompound = aHLRToShape.OutLineVCompound();" EOL
    "  //Visible outline edges" EOL
    "  " EOL
    "  TopoDS_Shape IsoLineVCompound = aHLRToShape.IsoLineVCompound();" EOL
    "  //Visible isoparameters" EOL
    "  " EOL
    "  // extract the results of hidden parts of the shape:" EOL
    "  TopoDS_Shape HCompound = aHLRToShape.HCompound();" EOL
    "  TopoDS_Shape Rg1LineHCompound = aHLRToShape.Rg1LineHCompound();" EOL
    "  TopoDS_Shape RgNLineHCompound = aHLRToShape.RgNLineHCompound();" EOL
    "  TopoDS_Shape OutLineHCompound = aHLRToShape.OutLineHCompound();" EOL
    "  TopoDS_Shape IsoLineHCompound = aHLRToShape.IsoLineHCompound();" EOL
    "  " EOL
    "  " EOL
    "  //===============================" EOL
    "  Colors of results:" EOL
    "  Outlines     - MAGENTA" EOL
    "  Sharp Edges  - CYAN" EOL
    "  Smooth Edges - RED" EOL
    "  Sewn Edges   - ORANGE" EOL
    "  Isolines     - YELLOW" EOL);

  // Build The algorithm object
  Handle(HLRBRep_Algo) myAlgo = new HLRBRep_Algo();
  
  // Add Shapes into the algorithm
  myAlgo->Add(aShape, nbIso);

  // create a projector and transformation object
  gp_Trsf Trsf;
  HLRAlgo_Projector aProjector = Projector(Trsf)->Projector();
  TopLoc_Location aLoc(Trsf);
  
  // Set The Projector
  myAlgo->Projector(aProjector);
  
  // Launches calculation of outlines of the shape 
  myAlgo->Update();

  // Computes the visible and hidden lines of the shape 
  myAlgo->Hide();
  
  // Build the extraction object :
  HLRBRep_HLRToShape aHLRToShape(myAlgo);
  
  // extract the results of visible parts of the shape:
  TopoDS_Shape VCompound = aHLRToShape.VCompound();
  //Visible sharp edges, Sharp edges present a C0 continuity
  
  TopoDS_Shape Rg1LineVCompound = aHLRToShape.Rg1LineVCompound();
  //Visible smooth edges, Smooth edges present a G1 continuity 

  TopoDS_Shape RgNLineVCompound = aHLRToShape.RgNLineVCompound();
  //Visible sewn edges, Sewn edges present a C2 continuity

  TopoDS_Shape OutLineVCompound = aHLRToShape.OutLineVCompound();
  //Visible outline edges
  
  TopoDS_Shape IsoLineVCompound = aHLRToShape.IsoLineVCompound();
  //Visible isoparameters
  
  // extract the results of hidden parts of the shape:
  TopoDS_Shape HCompound = aHLRToShape.HCompound();
  TopoDS_Shape Rg1LineHCompound = aHLRToShape.Rg1LineHCompound();
  TopoDS_Shape RgNLineHCompound = aHLRToShape.RgNLineHCompound();
  TopoDS_Shape OutLineHCompound = aHLRToShape.OutLineHCompound();
  TopoDS_Shape IsoLineHCompound = aHLRToShape.IsoLineHCompound();

  Handle(AIS_Shape) aShapeIO = drawShape(aShape);

  if (WAIT_A_SECOND) return Standard_False;

  getAISContext()->Erase(aShapeIO, Standard_False);

  // Draw visible and hidden parts of shape.  
  if (!drawHLR(OutLineVCompound, OutLineHCompound, aLoc, OutLine_COLOR)) 
    return Standard_False;
  if (!drawHLR(VCompound, HCompound, aLoc, SharpEdge_COLOR)) 
    return Standard_False;
  if (!drawHLR(Rg1LineVCompound, Rg1LineHCompound, aLoc, SmoothEdge_COLOR)) 
    return Standard_False;
  if (!drawHLR(RgNLineVCompound, RgNLineHCompound, aLoc, SewnEdge_COLOR)) 
    return Standard_False;
  if (!drawHLR(IsoLineVCompound, IsoLineHCompound, aLoc, IsoLine_COLOR)) 
    return Standard_False;

  return Standard_True;
}


//================================================================
// Function : HLR_Presentation::PolyHLR
// Purpose  : creates and displays shape representation by HLRBRep_PolyAlgo class
//================================================================
Standard_Boolean HLR_Presentation::PolyHLR(const TopoDS_Shape& aShape)
{
  setResultText(
    "  TopoDS_Shape aShape;" EOL
    "  // initializing aShape ..." EOL
    "  // triangulating aShape with BRepMesh::Mesh" EOL
    "" EOL
    "  // Build The algorithm object" EOL
    "  Handle(HLRBRep_PolyAlgo) myPolyAlgo = new HLRBRep_PolyAlgo();" EOL
    "  " EOL
    "  // Add Shapes into the algorithm" EOL
    "  myPolyAlgo->Load(aShape);" EOL
    "" EOL
    "  // create a projector object" EOL
    "  Handle(V3d_View) aView;" EOL
    "  // initializing V3d_View ..." EOL
    "  " EOL
    "  Standard_Real aProjVecX, aProjVecY, aProjVecZ;" EOL
    "  aView->Proj(aProjVecX, aProjVecY, aProjVecZ);" EOL
    "  " EOL
    "  Standard_Real aUpX, aUpY, aUpZ;" EOL
    "  aView->Up(aUpX, aUpY, aUpZ);" EOL
    "" EOL
    "  Standard_Real aPntX = 0.0, aPntY = 0.0, aPntZ = 0.0;" EOL
    "" EOL
    "  // create a projector object" EOL
    "  Handle(Prs3d_Projector) aProjector = new Prs3d_Projector(Standard_False, 0.0, " EOL
    "    aProjVecX, aProjVecY, aProjVecZ, aPntX, aPntY, aPntZ, aUpX, aUpY, aUpZ);" EOL
    "  " EOL
    "  // Set The Projector" EOL
    "  myPolyAlgo->Projector(aProjector);" EOL
    "  " EOL
    "  // Launches calculation of outlines of the shape " EOL
    "  myPolyAlgo->Update();" EOL
    "" EOL
    "  // Build the extraction object :" EOL
    "  HLRBRep_PolyHLRToShape aPolyHLRToShape;" EOL
    "  aPolyHLRToShape.Update(myPolyAlgo);" EOL
    "  " EOL
    "  // extract the results of visible parts of the shape:" EOL
    "  TopoDS_Shape VCompound = aPolyHLRToShape.VCompound();" EOL
    "  //Visible sharp edges, Sharp edges present a C0 continuity" EOL
    "  " EOL
    "  TopoDS_Shape Rg1LineVCompound = aPolyHLRToShape.Rg1LineVCompound();" EOL
    "  //Visible smooth edges, Smooth edges present a G1 continuity " EOL
    "" EOL
    "  TopoDS_Shape RgNLineVCompound = aPolyHLRToShape.RgNLineVCompound();" EOL
    "  //Visible sewn edges, Sewn edges present a C2 continuity" EOL
    "" EOL
    "  TopoDS_Shape OutLineVCompound = aPolyHLRToShape.OutLineVCompound();" EOL
    "  //Visible outline edges" EOL
    "  " EOL
    "  // extract the results of hidden parts of the shape:" EOL
    "  TopoDS_Shape HCompound = aPolyHLRToShape.HCompound();" EOL
    "  TopoDS_Shape Rg1LineHCompound = aPolyHLRToShape.Rg1LineHCompound();" EOL
    "  TopoDS_Shape RgNLineHCompound = aPolyHLRToShape.RgNLineHCompound();" EOL
    "  TopoDS_Shape OutLineHCompound = aPolyHLRToShape.OutLineHCompound();" EOL
    "  " EOL
    "  " EOL
    "  //===============================" EOL
    "  Colors of results:" EOL
    "  Outlines     - MAGENTA" EOL
    "  Sharp Edges  - CYAN" EOL
    "  Smooth Edges - RED" EOL
    "  Sewn Edges   - ORANGE" EOL
    "  Isolines     - YELLOW" EOL);

  // Build The algorithm object
  Handle(HLRBRep_PolyAlgo) myPolyAlgo = new HLRBRep_PolyAlgo();
  
  // Add Shapes into the algorithm
  myPolyAlgo->Load(aShape);

  // create a projector and transformation object
  gp_Trsf Trsf;
  HLRAlgo_Projector aProjector = Projector(Trsf)->Projector();
  TopLoc_Location aLoc(Trsf);
  
  // Set The Projector
  myPolyAlgo->Projector(aProjector);
  
  // Launches calculation of outlines of the shape 
  myPolyAlgo->Update();

  // Build the extraction object :
  HLRBRep_PolyHLRToShape aPolyHLRToShape;
  aPolyHLRToShape.Update(myPolyAlgo);
  
  // extract the results of visible parts of the shape:
  TopoDS_Shape VCompound = aPolyHLRToShape.VCompound();
  //Visible sharp edges, Sharp edges present a C0 continuity
  
  TopoDS_Shape Rg1LineVCompound = aPolyHLRToShape.Rg1LineVCompound();
  //Visible smooth edges, Smooth edges present a G1 continuity 

  TopoDS_Shape RgNLineVCompound = aPolyHLRToShape.RgNLineVCompound();
  //Visible sewn edges, Sewn edges present a C2 continuity

  TopoDS_Shape OutLineVCompound = aPolyHLRToShape.OutLineVCompound();
  //Visible outline edges
  
  // extract the results of hidden parts of the shape:
  TopoDS_Shape HCompound = aPolyHLRToShape.HCompound();
  TopoDS_Shape Rg1LineHCompound = aPolyHLRToShape.Rg1LineHCompound();
  TopoDS_Shape RgNLineHCompound = aPolyHLRToShape.RgNLineHCompound();
  TopoDS_Shape OutLineHCompound = aPolyHLRToShape.OutLineHCompound();

  Handle(AIS_Shape) aShapeIO = drawShape(aShape);

  if (WAIT_A_SECOND) return Standard_False;

  getAISContext()->Erase(aShapeIO, Standard_False);

  // Draw visible and hidden parts of shape.  
  if (!drawHLR(OutLineVCompound, OutLineHCompound, aLoc, OutLine_COLOR)) 
    return Standard_False;
  if (!drawHLR(VCompound, HCompound, aLoc, SharpEdge_COLOR)) 
    return Standard_False;
  if (!drawHLR(Rg1LineVCompound, Rg1LineHCompound, aLoc, SmoothEdge_COLOR)) 
    return Standard_False;
  if (!drawHLR(RgNLineVCompound, RgNLineHCompound, aLoc, SewnEdge_COLOR)) 
    return Standard_False;

  return Standard_True;
}


//================================================================
// Function : HLR_Presentation::drawHLR
// Purpose  : moves a given shape to the given location, displays it.
//            for visible shape using Aspect_TOL_SOLID line type
//            for hidden shape using Aspect_TOL_DASH line type
//================================================================
Standard_Boolean HLR_Presentation::drawHLR(TopoDS_Shape& aVShape,
                                           TopoDS_Shape& aHShape,
                                           const TopLoc_Location& aLoc, 
                                           const enum Quantity_NameOfColor aColor)
{
  Standard_Boolean VShapeIsNull = aVShape.IsNull();
  Standard_Boolean HShapeIsNull = aHShape.IsNull();

  if (VShapeIsNull && HShapeIsNull) return Standard_True;

  if (!VShapeIsNull)
  {
    // move the shape to an absolute location
    aVShape.Location(aLoc);

    // create a look for a line according to aColor and aLineType
    Handle(AIS_Shape) anIO = drawShape(aVShape, aColor, Standard_False);
    anIO->Attributes()->WireAspect()->SetTypeOfLine(Aspect_TOL_SOLID);
    
    getAISContext()->Display(anIO, HShapeIsNull);
  }
  if (!HShapeIsNull)
  {
    // move the shape to an absolute location
    aHShape.Location(aLoc);

    // create a look for a line according to aColor and aLineType
    Handle(AIS_Shape) anIO = drawShape(aHShape, aColor, Standard_False);
    anIO->Attributes()->WireAspect()->SetTypeOfLine(Aspect_TOL_DASH);

    getAISContext()->Display(anIO);
  }

  return !WAIT_A_SECOND;
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

//================================================================
// Function : HLR_Presentation::sampleCylinder
// Purpose  : 
//================================================================
void HLR_Presentation::sampleCylinder()
{
  // define cylinder's radius and height
  Standard_Real aRadius = 20;
  Standard_Real aHeight = 50;

  // make cylinder
  TopoDS_Solid aShape = BRepPrimAPI_MakeCylinder(aRadius, aHeight);

  Standard_Integer nbIso = 3;
  if (!HLR(aShape, nbIso)) return;
}

//================================================================
// Function : HLR_Presentation::sampleTorus
// Purpose  : 
//================================================================
void HLR_Presentation::sampleTorus()
{
  //define toroidal surface's axis
  gp_Ax3 anAx3(gp_Pnt(0,0,0), gp_Dir(1,0,0));

  //define two radiuses
  Standard_Real MajorRadius = 20;
  Standard_Real MinorRadius = 10;
  
  //make torus
  Handle(Geom_ToroidalSurface) aTorSurface = new Geom_ToroidalSurface(anAx3, MajorRadius, MinorRadius);
  TopoDS_Shell aShape = BRepBuilderAPI_MakeShell(aTorSurface);

  Standard_Integer nbIso = 3;
  if (!HLR(aShape, nbIso)) return;
}


//================================================================
// Function : HLR_Presentation::sampleBox
// Purpose  : 
//================================================================
void HLR_Presentation::sampleBrepShape2()
{
  // read a shape from shape2.brep file in DATA dir
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  TCollection_AsciiString aFileName(GetDataDir());
  aFileName += "\\shape2.brep";
  Standard_Boolean Result = BRepTools::Read(aShape, aFileName.ToCString(), aBuilder);

  if (!Result)
  {
    aFileName += " was not found.  The sample can not be shown.";
    setResultText(aFileName.ToCString());
    return;
  }

  Standard_Integer nbIso = 0;
  if (!HLR(aShape, nbIso)) return;
}


//================================================================
// Function : HLR_Presentation::sampleShape1
// Purpose  : 
//================================================================
void HLR_Presentation::sampleBrepShape1()
{
  // read a shape from shape1.brep file in DATA dir
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;
  TCollection_AsciiString aFileName(GetDataDir());
  aFileName += "\\shape1.brep";
  Standard_Boolean Result = BRepTools::Read(aShape, aFileName.ToCString(), aBuilder);

  if (!Result)
  {
    aFileName += " was not found.  The sample can not be shown.";
    setResultText(aFileName.ToCString());
    return;
  }

  // triangulating the shape
  Standard_Real Deflection = 50;
  BRepMesh::Mesh(aShape, Deflection);

  if (!PolyHLR(aShape)) return;
}
