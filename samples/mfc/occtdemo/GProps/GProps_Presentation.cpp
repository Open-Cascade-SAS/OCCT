// GProps_Presentation.cpp: implementation of the GProps_Presentation class.
// Global Properties of Shapes
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GProps_Presentation.h"

#include <BRepTools.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <GProp_PGProps.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <BRepGProp.hxx>
#include <GProp_PrincipalProps.hxx>
#include <Geom_BezierCurve.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <AIS_Point.hxx>
#include <TopoDS_Solid.hxx>
#include <ElCLib.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>


// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new GProps_Presentation;

// Initialization of array of samples
const GProps_Presentation::PSampleFuncType GProps_Presentation::SampleFuncs[] =
{
  &GProps_Presentation::samplePoints,
  &GProps_Presentation::sampleCurves,
  &GProps_Presentation::sampleSurfaces,
  &GProps_Presentation::sampleVolumes,
  &GProps_Presentation::sampleSystem
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif

#define CUSTOM_INERTIAL_ELEMS_COLOR Quantity_NOC_CYAN1
#define MAIN_ELEMS_COLOR Quantity_NOC_RED
#define SEC_AXIS_COLOR Quantity_NOC_GREEN

#define SCALE_FACTOR 15
#define SCALE_FACTOR_FOR_VECTORS SCALE_FACTOR*5

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GProps_Presentation::GProps_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Global Properties of Points, Lines, Surfaces and Volumes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void GProps_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

Standard_Boolean GProps_Presentation::GProps(GProp_GProps& aProps, 
                                             gp_Ax1& anAxisForInertialProp,
                                             TCollection_AsciiString& theText,
                                             TCollection_AsciiString theMassStr)
{
  theText += EOL
    "  gp_Pnt aCenterOfMass;// center of mass of the system" EOL
    "  Standard_Real aMass; // mass of the system" EOL
    "  " EOL
    "  Standard_Real aRadiusOfGyration;// radius of gyration around a given axis" EOL
    "  Standard_Real aMomentOfInertia; // moment of inertia around a given axis" EOL
    "  " EOL
    "  Standard_Real aPrincipalRadiusOfGyrationXX; // radiuses of gyration around " EOL
    "  Standard_Real aPrincipalRadiusOfGyrationYY; // the main inertial axes" EOL
    "  Standard_Real aPrincipalRadiusOfGyrationZZ;" EOL
    "" EOL
    "  Standard_Real aPrincipalMomentOfInertiaXX; // moments of inertia around " EOL
    "  Standard_Real aPrincipalMomentOfInertiaYY; // the main inertial axes" EOL
    "  Standard_Real aPrincipalMomentOfInertiaZZ;" EOL
    "" EOL
    "  gp_Ax1 anAxisForInertialProp; // axis to compute inertial properties" EOL
    "  // initializing anAxisForInertialProp" EOL
    "  // ..." EOL
    "" EOL
    "  GProp_PrincipalProps aPrincipalProps;   // the object that returns inertial properties" EOL
    "  " EOL
    "  gp_Vec aFInertiaAxVec;  // inertial axes as they are returned by aPrincipalProps" EOL
    "  gp_Vec aSInertiaAxVec;" EOL
    "  gp_Vec aTInertiaAxVec;" EOL
    "" EOL
    "  aCenterOfMass = aProps.CentreOfMass();" EOL
    "  aMass = aProps.Mass();" EOL
    "" EOL
    "  // axis for calculation gyration and inertia properties" EOL
    "  aPrincipalProps = aProps.PrincipalProperties();" EOL
    "" EOL
    "  if (!aPrincipalProps.HasSymmetryPoint())" EOL
    "  {" EOL
    "    aFInertiaAxVec = aPrincipalProps.FirstAxisOfInertia();" EOL
    "" EOL
    "    // computing a radiuses of gyration and moment of inertia " EOL
    "    // around the main axes of inertia" EOL
    "    aPrincipalProps.RadiusOfGyration(aPrincipalRadiusOfGyrationXX, " EOL
    "      aPrincipalRadiusOfGyrationYY, aPrincipalRadiusOfGyrationZZ);" EOL
    "    aPrincipalProps.Moments(aPrincipalMomentOfInertiaXX, " EOL
    "      aPrincipalMomentOfInertiaYY, aPrincipalMomentOfInertiaZZ);    " EOL
    "    " EOL
    "    if (!aPrincipalProps.HasSymmetryAxis())" EOL
    "    {" EOL
    "      aSInertiaAxVec = aPrincipalProps.SecondAxisOfInertia();" EOL
    "      aTInertiaAxVec = aPrincipalProps.ThirdAxisOfInertia();" EOL
    "    }" EOL
    "  }" EOL
    "  " EOL
    "  // computing a radius of gyration and moment of inertia around a given axis" EOL
    "  aRadiusOfGyration = aProps.RadiusOfGyration(anAxisForInertialProp);" EOL
    "  aMomentOfInertia = aProps.MomentOfInertia(anAxisForInertialProp);" EOL;
  setResultText(theText.ToCString());

  gp_Pnt aCenterOfMass;// center of mass of the system
  Standard_Real aMass; // mass of the system
  
  Standard_Real aRadiusOfGyration;// radius of gyration around a given axis
  Standard_Real aMomentOfInertia; // moment of inertia around a given axis
  
  Standard_Real aPrincipalRadiusOfGyrationXX; // radiuses of gyration around 
  Standard_Real aPrincipalRadiusOfGyrationYY; // the main inertial axes
  Standard_Real aPrincipalRadiusOfGyrationZZ;

  Standard_Real aPrincipalMomentOfInertiaXX; // moments of inertia around 
  Standard_Real aPrincipalMomentOfInertiaYY; // the main inertial axes
  Standard_Real aPrincipalMomentOfInertiaZZ;

  GProp_PrincipalProps aPrincipalProps;   // the object that returns inertial properties
  
  gp_Vec aFInertiaAxVec;  // inertial axes as they are returned by aPrincipalProps
  gp_Vec aSInertiaAxVec;
  gp_Vec aTInertiaAxVec;

  aCenterOfMass = aProps.CentreOfMass();
  aMass = aProps.Mass();

  theText = theText + EOL EOL
    "  //=================================================" EOL EOL

    "  Total " + theMassStr + " = " + TCollection_AsciiString(aMass) + EOL
    "  Center of mass: ["+TCollection_AsciiString(aCenterOfMass.X())+","
                         +TCollection_AsciiString(aCenterOfMass.Y())+","
                         +TCollection_AsciiString(aCenterOfMass.Z())+"]";

  // axis for calculation gyration and inertia properties
  aPrincipalProps = aProps.PrincipalProperties();

  if (!aPrincipalProps.HasSymmetryPoint())
  {
    aFInertiaAxVec = aPrincipalProps.FirstAxisOfInertia();

    // computing a radiuses of gyration and moment of inertia 
    // around the main axes of inertia
    aPrincipalProps.RadiusOfGyration(aPrincipalRadiusOfGyrationXX, 
      aPrincipalRadiusOfGyrationYY, aPrincipalRadiusOfGyrationZZ);
    aPrincipalProps.Moments(aPrincipalMomentOfInertiaXX, 
      aPrincipalMomentOfInertiaYY, aPrincipalMomentOfInertiaZZ);    

    theText = theText + EOL EOL
      "  Moment of inertia around the first axis of inertia = " + TCollection_AsciiString(aPrincipalMomentOfInertiaXX) + EOL
      "  Moment of inertia around the second axis of inertia = " + TCollection_AsciiString(aPrincipalMomentOfInertiaYY) + EOL
      "  Moment of inertia around the third axis of inertia = " + TCollection_AsciiString(aPrincipalMomentOfInertiaZZ) + EOL EOL

      "  Radius of gyration around the first axis of inertia = " + TCollection_AsciiString(aPrincipalRadiusOfGyrationXX) + EOL      
      "  Radius of gyration around the second axis of inertia = " + TCollection_AsciiString(aPrincipalRadiusOfGyrationYY) + EOL      
      "  Radius of gyration around the third axis of inertia = " + TCollection_AsciiString(aPrincipalRadiusOfGyrationZZ);    
    
    if (!aPrincipalProps.HasSymmetryAxis())
    {
      aSInertiaAxVec = aPrincipalProps.SecondAxisOfInertia();
      aTInertiaAxVec = aPrincipalProps.ThirdAxisOfInertia();
    }
  }
  
  // computing a radius of gyration and moment of inertia around a given axis
  aRadiusOfGyration = aProps.RadiusOfGyration(anAxisForInertialProp);
  aMomentOfInertia = aProps.MomentOfInertia(anAxisForInertialProp);

  theText = theText + EOL + EOL
      "  Radius of gyration around an axis = " + TCollection_AsciiString(aRadiusOfGyration) + EOL
      "  Moment of inertia around an axis = " + TCollection_AsciiString(aMomentOfInertia) + EOL;      
  setResultText(theText.ToCString());

  // ========= output =======================================
  // display the given axis
  gp_Lin aLin(anAxisForInertialProp.Location(), anAxisForInertialProp.Direction());
  // aParam = Parameter Of Projection Of CenterOfMass Onto AxisForInertialProp
  Standard_Real aParam = ElCLib::Parameter(aLin, aCenterOfMass);
  gp_Pnt aProjectedCenterOfMass = ElCLib::Value(aParam, aLin);
  
  gp_Ax2 anInertialCoordSys;
  anInertialCoordSys.SetDirection(anAxisForInertialProp.Direction());
  anInertialCoordSys.SetLocation(aProjectedCenterOfMass);

  // display a point on the axis around which the properties were calculated
  drawPoint(aProjectedCenterOfMass, CUSTOM_INERTIAL_ELEMS_COLOR);

  // display the axis around which the properties were calculated
  gp_Vec aVec(anAxisForInertialProp.Direction());
  aVec.SetXYZ(aVec.XYZ()*SCALE_FACTOR_FOR_VECTORS);
  drawVector(aProjectedCenterOfMass, aVec, CUSTOM_INERTIAL_ELEMS_COLOR);

  if (WAIT_A_SECOND) return Standard_False;

  Handle(AIS_Point) aPointIO = drawPoint(
    aCenterOfMass, MAIN_ELEMS_COLOR, Standard_False);
  aPointIO->UnsetMarker();
  aPointIO->SetMarker(Aspect_TOM_O);
  getAISContext()->Display(aPointIO);

  if (WAIT_A_SECOND) return Standard_False;

  // displaying inertial axes, main in red, other use default color
  if (!aPrincipalProps.HasSymmetryPoint())
  {
    aFInertiaAxVec.SetXYZ(aFInertiaAxVec.XYZ()*SCALE_FACTOR_FOR_VECTORS);
    drawVector(aCenterOfMass, aFInertiaAxVec, MAIN_ELEMS_COLOR);
    if (!aPrincipalProps.HasSymmetryAxis())
    {
      aSInertiaAxVec.SetXYZ(aSInertiaAxVec.XYZ()*SCALE_FACTOR_FOR_VECTORS);
      aTInertiaAxVec.SetXYZ(aTInertiaAxVec.XYZ()*SCALE_FACTOR_FOR_VECTORS);  
      drawVector(aCenterOfMass, aSInertiaAxVec, SEC_AXIS_COLOR);
      drawVector(aCenterOfMass, aTInertiaAxVec, SEC_AXIS_COLOR);
    }
    if (WAIT_A_SECOND) return Standard_False;
  }

  // display the cirle with radius = aRadiusOfGyration
  Handle(Geom_Circle) aCirc = new Geom_Circle(anInertialCoordSys, aRadiusOfGyration);
  drawCurve(aCirc, CUSTOM_INERTIAL_ELEMS_COLOR);

  return Standard_True;
}


//================================================================
// Function : GProps_Presentation::samplePoints
// Purpose  : 
//================================================================
void GProps_Presentation::samplePoints()
{
  ResetView();
  SetViewCenter(63.470922672300, 35.981561054950);
  SetViewScale(4.2572644504009); 

  setResultTitle("Global Properties of Points");
  TCollection_AsciiString aText(
    "  TColgp_Array1OfPnt aPoints (1, nPoints); // points to analyze" EOL
    "  TColStd_Array1OfReal aDens (1, nPoints); // dencities of points, weights" EOL
    "  // initializing arrays of points and densities" EOL
    "  // ..." EOL
    "" EOL
    "  GProp_PGProps aProps(aPoints, aDens);" EOL
    "" EOL
    "  gp_Pnt aBaryCentre;  // geometric centre of shape that consists all the points" EOL
    "  aBaryCentre = GProp_PGProps::Barycentre(aPoints);" EOL);

  // points to build the curves
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0.2},{1,2,0.2},{2,2,0.4},{2,1,0.4},{3,1,0.6},{3,0,0.6},
    {2,0,0.8},{2,-1,0},{3,-1,0},{3,-2,-0.5},{4,-2,1},{4,-1,1.2},{5,-1,1.2},
    {5,0,1.4},{6,0,1.4},{6,-1,1.6},{7,-1,1.6},{7,0,1.8},{8,0,1.8},{8,1,2}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aPoints (1, nPoints); // points to analyze
  TColStd_Array1OfReal aDens (1, nPoints); // dencities of points, weights
  Standard_Integer aOneColorSection = nPoints/5;
  Standard_Integer k = SCALE_FACTOR;
  for (Standard_Integer i=0; i < nPoints; i++)
  {
    aPoints(i+1) = gp_Pnt (aCoords[i][0]*k, aCoords[i][1]*k, aCoords[i][2]*k);
    aDens(i+1) = i*10+1; 
    
    // displaying points in different colors according to their weight
    Quantity_NameOfColor aColorName = Quantity_NOC_BLUE1;
    if      (i <= 1*aOneColorSection) aColorName = Quantity_NOC_WHITESMOKE;
    else if (i <= 2*aOneColorSection) aColorName = Quantity_NOC_ANTIQUEWHITE1;
    else if (i <= 3*aOneColorSection) aColorName = Quantity_NOC_YELLOW;
    else if (i <= 4*aOneColorSection) aColorName = Quantity_NOC_GREEN;

    drawPoint(aPoints(i+1), Quantity_Color(aColorName));
  }

  GProp_PGProps aProps(aPoints, aDens);

  gp_Pnt aBaryCentre;  // geometric centre of shape that consists all the points
  aBaryCentre = GProp_PGProps::Barycentre(aPoints);

  //if (WAIT_A_SECOND) return;

  Handle(AIS_Point) aPointIO = drawPoint(
    aBaryCentre, MAIN_ELEMS_COLOR, Standard_False);
  aPointIO->SetMarker(Aspect_TOM_STAR);
  getAISContext()->Display(aPointIO);

  // building a CUSTOM axis for calculation of inertial properties
  gp_Ax1 anAxisForInertialProp(gp_Pnt(5*k,5*k,5*k), gp_Dir(0.2,0.8,0));

  if (!GProps(aProps, anAxisForInertialProp, aText, "Mass")) return;
}


//================================================================
// Function : GProps_Presentation::sampleCurves
// Purpose  : 
//================================================================
void GProps_Presentation::sampleCurves()
{ 
  ResetView();
  SetViewCenter(61.308121161914, 17.674575084056);
  SetViewScale(5.3585487153415); 

  setResultTitle("Global Linear Properties of Curves");
  TCollection_AsciiString aText(
    "  TopoDS_Wire aWire;" EOL
    "  // initializing aWire" EOL
    "  // ..." EOL
    "  GProp_GProps aProps; // Global properties object" EOL
    "" EOL
    "  // initializing Global properties object with linear properties of aWire" EOL
    "  BRepGProp::LinearProperties(aWire, aProps);" EOL);

  // constructing a wire made of several edges
  // initializing array of points mainly used for construction of anEdge2
  Standard_Real aCoords[][3] = {
    {0,0,0},{0,1,0},{1,1,0.2},{1,2,0.2},{2,2,0.4},{2,1,0.4},{3,1,0.6},{3,0,0.6},
    {2,0,0.8},{2,-1,0},{3,-1,0},{3,-2,-0.5},{4,-2,1},{4,-1,1.2},{5,-1,1.2},
    {5,0,1.4},{6,0,1.4},{6,-1,1.6},{7,-1,1.6},{7,0,1.8},{8,0,1.8},{8,1,2}
  };
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aPoints (1, nPoints); 
  Standard_Integer k = SCALE_FACTOR;
  for (Standard_Integer i=0; i < nPoints; i++)
    aPoints(i+1) = gp_Pnt (aCoords[i][0]*k, aCoords[i][1]*k, aCoords[i][2]*k);

  // edge 2 is a bezier curve that starts in edge1's end point
  Handle(Geom_BezierCurve) aCurve = new Geom_BezierCurve(aPoints);
  TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(aCurve);

  // edge 1 is a linear edge between 2 points
  TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(gp_Pnt(-k, -k, 0), aCurve->StartPoint());

  // edge 3 is a linear section as well
  //TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aCurve->EndPoint(), gp_Pnt(1000, 300, 400));

  // creating a wire that consists of the edges defined above
  TopoDS_Wire aWire = BRepBuilderAPI_MakeWire (anEdge1, anEdge2);

  drawShape(aWire);

  GProp_GProps aProps; // Global properties object

  // initializing Global properties object with linear properties of aWire
  BRepGProp::LinearProperties(aWire, aProps);

  // building a CUSTOM axis for calculation of inertial properties
  gp_Ax1 anAxisForInertialProp(gp_Pnt(5*k,3*k,3*k), gp_Dir(0.1,0.2,0.7));

  if (!GProps(aProps, anAxisForInertialProp, aText, "Length")) return;
}

//================================================================
// Function : GProps_Presentation::sampleSurfaces
// Purpose  : 
//================================================================
void GProps_Presentation::sampleSurfaces()
{
  ResetView();
  SetViewCenter(7.3480331410807, 5.9413176110886);
  SetViewScale(6.0684301060407); 

  setResultTitle("Global Surface Properties of Shapes");
  TCollection_AsciiString aText(
    "  TopoDS_Shell aShell;" EOL
    "  // initializing initial surface" EOL
    "  // ..." EOL
    "" EOL
    "  GProp_GProps aProps; // Global properties object" EOL
    "" EOL
    "  // initializing Global properties object with surface properties of aShell" EOL
    "  BRepGProp::SurfaceProperties(aShell, aProps);" EOL);

  // constructing initial surface
  Standard_Real aZCoords [] = {{-1},{0},{0},{1},{0},{-1},{-1},{0},{0},{-1.5},{-2.5},{0},{1},{-2},{-3},{0}}; 
  
  TColStd_Array2OfReal aZPoints(1,4,1,4);
  TColgp_Array2OfPnt aPnt(1,4,1,4);
   
  Standard_Integer aColLength = aZPoints.ColLength();
  Standard_Integer aRowLength = aZPoints.RowLength();
  Standard_Integer aIndex = 0;
  Standard_Integer k = SCALE_FACTOR;
  
  for(Standard_Integer i = 0 ; i < aRowLength ; i++)
    for(Standard_Integer j = 0; j < aColLength ; j++)
      aZPoints(i+1,j+1) = (aZCoords[aIndex++] * k);

  Standard_Real aXStep = 1.75*k, aYStep = 1.75*k;
  Standard_Real aXBound = -3*k, aYBound = -2*k;
  
  GeomAPI_PointsToBSplineSurface aPTBS(
    aZPoints,aXBound,aXStep,aYBound,aYStep,3,8,GeomAbs_C3,k);

  TopoDS_Shell aShell = BRepBuilderAPI_MakeShell(aPTBS.Surface());

  drawShape(aShell);

  GProp_GProps aProps; // Global properties object

  // initializing Global properties object with surface properties of aShell
  BRepGProp::SurfaceProperties(aShell, aProps);

  // building a CUSTOM axis for calculation of inertial properties
  gp_Ax1 anAxisForInertialProp(gp_Pnt(-3*k,-3*k,-3*k), gp_Dir(0.33,0.33,0.33));

  if (!GProps(aProps, anAxisForInertialProp, aText, "Area")) return;
}

//================================================================
// Function : GProps_Presentation::sampleVolumes
// Purpose  : 
//================================================================
void GProps_Presentation::sampleVolumes()
{ 
  ResetView();
  SetViewCenter(83.572045344035, 38.188854653904);
  SetViewScale(5.2570164985648); 

  setResultTitle("Global Volume Properties of Solids");
  TCollection_AsciiString aText(
    "  TopoDS_Solid aSolid;" EOL
    "  // initializing the solid" EOL
    "  // ..." EOL
    "" EOL
    "  GProp_GProps aProps;   // Global properties object" EOL
    "" EOL
    "  // initializing Global properties object with volume properties of aSolid" EOL
    "  BRepGProp::VolumeProperties(aSolid, aProps);" EOL);

  // constructing initial surface
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

  TopoDS_Solid aSolid;
  TopExp_Explorer anExp(aShape, TopAbs_SOLID);
  if (anExp.More()) 
    aSolid = TopoDS::Solid(anExp.Current());

  drawShape(aSolid);

  GProp_GProps aProps;   // Global properties object

  // initializing Global properties object with volume properties of aSolid
  BRepGProp::VolumeProperties(aSolid, aProps);

  // building a CUSTOM axis for calculation of inertial properties
  Standard_Integer k = SCALE_FACTOR;
  gp_Ax1 anAxisForInertialProp(gp_Pnt(2.5*k,2.5*k,2.5*k), gp_Dir(0.2,0.8,0));

  if (!GProps(aProps, anAxisForInertialProp, aText, "Volume")) return;
}


//================================================================
// Function : GProps_Presentation::sampleSystem
// Purpose  : 
//================================================================
void GProps_Presentation::sampleSystem()
{  
  ResetView();
  SetViewCenter(107.10015835676, 76.510352154100);
  SetViewScale(2.3110667631188); 

  setResultTitle("Global Properties of System of Shapes");
  TCollection_AsciiString aText(
    "  TopoDS_Compound aShape;" EOL
    "  // initializing the shape" EOL
    "  // ..." EOL
    "" EOL
    "  GProp_GProps aProps;" EOL
    "  GProp_GProps aLinearProps;" EOL
    "  GProp_GProps aSurfaceProps;" EOL
    "  GProp_GProps aVolumeProps;" EOL
    "" EOL
    "  // creating a linear system, system that consists of all edges" EOL
    "  TopExp_Explorer anExp(aShape, TopAbs_EDGE, TopAbs_FACE);" EOL
    "  while (anExp.More())" EOL
    "  {" EOL
    "    GProp_GProps lp;" EOL
    "    BRepGProp::LinearProperties(anExp.Current(), lp);" EOL
    "    aLinearProps.Add(lp);" EOL
    "    anExp.Next();" EOL
    "  }" EOL
    "" EOL
    "  // creating a surface system, system that consists of all faces" EOL
    "  anExp.Init(aShape, TopAbs_FACE, TopAbs_SHELL);" EOL
    "  while (anExp.More())" EOL
    "  {" EOL
    "    GProp_GProps sp;" EOL
    "    BRepGProp::SurfaceProperties(anExp.Current(), sp);" EOL
    "    aSurfaceProps.Add(sp);" EOL
    "    anExp.Next();" EOL
    "  }" EOL
    "" EOL
    "  // creating a volume system, system that consists of all solids" EOL
    "  anExp.Init(aShape, TopAbs_SOLID);" EOL
    "  while (anExp.More())" EOL
    "  {" EOL
    "    GProp_GProps vp;" EOL
    "    BRepGProp::VolumeProperties(anExp.Current(), vp);" EOL
    "    aVolumeProps.Add(vp);" EOL
    "    anExp.Next();" EOL
    "  }" EOL
    "" EOL
    "  Standard_Real aSurfaceDensity = 2;" EOL
    "  Standard_Real aVolumeDensity = 3;" EOL
    "  aProps.Add (aLinearProps); // density = 1.0" EOL
    "  aProps.Add (aSurfaceProps, aSurfaceDensity);" EOL
    "  aProps.Add (aVolumeProps, aVolumeDensity);" EOL);

  // building a shape "space ship" that consists of a solid, 2 faces, and 2 edges
  Standard_Real aCoords[][3] = {{3,-1,0},{1,0,0},{0,1,0},{0,7,0},{1,8,0},{3,9,0}};
  Standard_Integer nPoints = sizeof(aCoords)/(sizeof(Standard_Real)*3);
  TColgp_Array1OfPnt aPoints (1, nPoints);
  Standard_Integer k = 15;
  for (Standard_Integer i=0; i < nPoints; i++)
    aPoints(i+1) = gp_Pnt (aCoords[i][0]*k, aCoords[i][1]*k, aCoords[i][2]*k);

  Handle(Geom_BezierCurve) aCurve = new Geom_BezierCurve(aPoints);
  gp_Ax1 anAxis(aPoints(1), gp_Dir(0,1,0));
  Handle(Geom_SurfaceOfRevolution) aSurface = new Geom_SurfaceOfRevolution(aCurve, anAxis);
  TopoDS_Solid aSolid = BRepBuilderAPI_MakeSolid(BRepBuilderAPI_MakeShell(aSurface));

  TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(gp_Pnt(5*k, 2*k, 0), gp_Pnt(12*k, 7*k, 0));
  TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(gp_Pnt(12*k, 7*k, 0), gp_Pnt(12*k, 11*k, 0));
  TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(gp_Pnt(12*k, 11*k, 0), gp_Pnt(5*k, 5*k, 0));
  TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(gp_Pnt(5*k, 5*k, 0), gp_Pnt(5*k, 2*k, 0));
  TopoDS_Face aWing1 = BRepBuilderAPI_MakeFace(BRepBuilderAPI_MakeWire(e1, e2, e3, e4));

  gp_Ax2d aMirrorAx(gp_Pnt2d(3*k, 0), gp_Dir2d(0,1));
  gp_Trsf2d aTrsf2d;
  aTrsf2d.SetMirror(aMirrorAx);
  gp_Trsf aTrsf(aTrsf2d);
  BRepBuilderAPI_Transform aBuilder(aWing1, aTrsf, Standard_True);

  aBuilder.Perform(aWing1, Standard_True);
  TopoDS_Face aWing2 = TopoDS::Face(aBuilder.Shape());

  Handle(Geom_Circle) aCirc = new Geom_Circle(gp_Ax2(gp_Pnt(3*k, 11*k, 0), gp_Dir(0,-0.2,0.8)), (12-3)*k);
  Handle(Geom_TrimmedCurve) aTrimCirc = new Geom_TrimmedCurve(aCirc, -PI/2, PI/2);
  TopoDS_Edge aSpoiler = BRepBuilderAPI_MakeEdge(aTrimCirc);

  TopoDS_Edge anArrow = BRepBuilderAPI_MakeEdge(gp_Pnt(3*k, 0, 0), gp_Pnt(3*k, -5*k, 0));
  
  TopoDS_Compound aShape;
  BRep_Builder aCompoundBuilder;
  aCompoundBuilder.MakeCompound(aShape);
  aCompoundBuilder.Add(aShape,aSolid);
  aCompoundBuilder.Add(aShape,aWing1);
  aCompoundBuilder.Add(aShape,aWing2);
  aCompoundBuilder.Add(aShape,anArrow);
  aCompoundBuilder.Add(aShape,aSpoiler);

  drawShape(aSolid);
  drawShape(aWing1);
  drawShape(aWing2);
  drawShape(anArrow, Quantity_NOC_GOLD);
  drawShape(aSpoiler, Quantity_NOC_GOLD);

  GProp_GProps aProps;
  GProp_GProps aLinearProps;
  GProp_GProps aSurfaceProps;
  GProp_GProps aVolumeProps;

  GProp_PrincipalProps aPrincipalProps;

  // creating a linear system, system that consists of all edges
  TopExp_Explorer anExp(aShape, TopAbs_EDGE, TopAbs_FACE);
  while (anExp.More())
  {
    GProp_GProps lp;
    BRepGProp::LinearProperties(anExp.Current(), lp);
    aLinearProps.Add(lp);
    anExp.Next();
  }

  // creating a surface system, system that consists of all faces
  anExp.Init(aShape, TopAbs_FACE, TopAbs_SHELL);
  while (anExp.More())
  {
    GProp_GProps sp;
    BRepGProp::SurfaceProperties(anExp.Current(), sp);
    aSurfaceProps.Add(sp);
    anExp.Next();
  }

  // creating a volume system, system that consists of all solids
  anExp.Init(aShape, TopAbs_SOLID);
  while (anExp.More())
  {
    GProp_GProps vp;
    BRepGProp::VolumeProperties(anExp.Current(), vp);
    aVolumeProps.Add(vp);
    anExp.Next();
  }

  Standard_Real aSurfaceDensity = 2;
  Standard_Real aVolumeDensity = 3;
  aProps.Add (aLinearProps); // density = 1.0
  aProps.Add (aSurfaceProps, aSurfaceDensity);
  aProps.Add (aVolumeProps, aVolumeDensity);

  // Length of free edges
  Standard_Real aEdgesLength = aLinearProps.Mass();
  Standard_Real aFacesArea = aSurfaceProps.Mass();
  Standard_Real aSolidsVolume = aVolumeProps.Mass();

  // building a CUSTOM axis for calculation of inertial properties
  gp_Ax1 anAxisForInertialProp(gp_Pnt(3.5*k,2*k,7*k), gp_Dir(-0.33,-0.33,-0.33));

  if (!GProps(aProps, anAxisForInertialProp, aText, "Mass")) return;

  aText = aText + EOL
    "  Length of free edges = " + TCollection_AsciiString(aEdgesLength) + EOL
    "  Area of free faces = " + TCollection_AsciiString(aFacesArea) + EOL
    "  Volume of solids = " + TCollection_AsciiString(aSolidsVolume);
  setResultText(aText.ToCString());
}
