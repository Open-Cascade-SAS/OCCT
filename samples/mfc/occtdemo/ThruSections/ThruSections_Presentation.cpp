// ThruSections_Presentation.cpp: implementation of the ThruSections_Presentation class.
// Building shapes passed through sections.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThruSections_Presentation.h"

#include <Standard.hxx>
#include <Precision.hxx>

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

#include <gce_MakeCirc.hxx>
#include <gce_MakeElips.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_Plane.hxx>

#include <TColgp_Array1OfPnt.hxx>



// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new ThruSections_Presentation;

// Initialization of array of samples
const ThruSections_Presentation::PSampleFuncType ThruSections_Presentation::SampleFuncs[] =
{
  &ThruSections_Presentation::sample1,
  &ThruSections_Presentation::sample2
};

#ifdef WNT
 #define EOL "\r\n"
#else
 #define EOL "\n"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ThruSections_Presentation::ThruSections_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName("Lofting through sections");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void ThruSections_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// function: CreateCurve
// purpose:
//////////////////////////////////////////////////////////////////////

static Handle(Geom_BSplineCurve) CreateCurve(Standard_Real aCoords[][3],Standard_Integer nPoles)                     
{
  TColgp_Array1OfPnt aCurvePoint (1, nPoles);

  for (Standard_Integer i=0; i < nPoles; i++)
    aCurvePoint(i+1) = gp_Pnt (aCoords[i][0], aCoords[i][1], aCoords[i][2]);

  GeomAPI_PointsToBSpline aPTB (aCurvePoint);

  Handle (Geom_BSplineCurve) aCurve = aPTB.Curve();
  return aCurve;
}

//////////////////////////////////////////////////////////////////////
// function: CreateShape
// purpose:
//////////////////////////////////////////////////////////////////////

TopoDS_Shape ThruSections_Presentation::CreateShape(TopTools_SequenceOfShape& aWires,
                                                    Standard_Boolean IsSolid,
                                                    Standard_Boolean IsRuled,
                                                    Standard_Real aPrec)
{
  BRepOffsetAPI_ThruSections aTSec(IsSolid,IsRuled,aPrec);

  for(Standard_Integer i = 1 ; i < aWires.Length() + 1 ; i++)
    aTSec.AddWire(TopoDS::Wire(aWires(i)));

  aTSec.Build();

  TopoDS_Shape aShape = aTSec.Shape();
  return aShape;
}

//////////////////////////////////////////////////////////////////////
// function: sample1
// purpose:
//////////////////////////////////////////////////////////////////////

void ThruSections_Presentation::sample1()
{
  ResetView();
  SetViewCenter(-5.5147159194746e-007, 2.4494893207696);
  SetViewScale(75.788671414734); 

  setResultTitle("Lofting through closed sections");
  TCollection_AsciiString aText;
  aText = (
 
  "// this data used for building a shape through sections:" EOL
  "Standard_Boolean IsSolid,IsRuled;" EOL
  "Standard_Real aPrec;" EOL EOL

  "// creates wires:" EOL
  "TopoDS_Wire aWire1,aWire2,aWire3;" EOL EOL

  "// initializes these wires:" EOL
  "//aWire1 = ... ; aWire2 = ...; aWire3 = ...;" EOL EOL

  "// creates an algorithm for building a shell or " EOL
  "//solid passing through a set of wires:" EOL
  "BRepOffsetAPI_ThruSections aTSec;" EOL EOL

  "// adds wires to the set of wires:" EOL
  "aTSec.AddWire(aWire1);" EOL
  "aTSec.AddWire(aWire2);" EOL 
  "aTSec.AddWire(aWire3);" EOL EOL

  "// Initializes algorithm for building a shell " EOL
  "//passing through a set of wires with the faces are" EOL
  "//smoothed out by approximation:" EOL
  "aTSec.Init();" EOL
  "// builds the shell:" EOL
  "aTSec.Build();" EOL 
  "// takes this shape:" EOL
  "TopoDS_Shape aShape = aTSec.Shape();" EOL EOL

  "// Initializes algorithm for building a shell " EOL
  "//passing through a set of wires with the faces are ruled:" EOL
  "IsSolid = Standard_False;" EOL
  "IsRuled = Standard_True;" EOL
  "aTSec.Init(IsSolid,IsRuled);" EOL
  "// builds the shell:" EOL
  "aTSec.Build();" EOL 
  "// takes this shape:" EOL
  "TopoDS_Shape aShape = aTSec.Shape();" EOL EOL

  "// Initializes algorithm for building a solid" EOL
  "//passing through a set of wires with the faces are" EOL
  "//smoothed out by approximation:" EOL
  "IsSolid = Standard_True;" EOL
  "aTSec.Init(IsSolid);" EOL
  "// builds the solid:" EOL
  "aTSec.Build();" EOL 
  "// takes this shape:" EOL
  "TopoDS_Shape aShape = aTSec.Shape();" EOL EOL

  "// Initializes algorithm for building a solid" EOL
  "//passing through a set of wires with the faces are" EOL
  "//smoothed out by approximation with precision aPrec:" EOL
  "IsSolid = Standard_True;" EOL
  "IsRuled = Standard_False;" EOL
  "aPrec = 0.5;" EOL
  "aTSec.Init(IsSolid,IsRuled,aPrec);" EOL
  "// builds the solid:" EOL
  "aTSec.Build();" EOL 
  "// takes this shape:" EOL
  "TopoDS_Shape aShape = aTSec.Shape();" EOL EOL

    );

  setResultText(aText.ToCString());

//====================================================================

  Handle(AIS_InteractiveObject) aShow1,aShow2,aShowWire;

  // this data used for building a shape through sections:
  Standard_Boolean IsSolid,IsRuled;
  Standard_Real aPrec;

  // the number of points of wires:
  const Standard_Integer aNbOfPnt = 4;

  // creates arrays of coordinates of wires:
  Standard_Real aCoords [][aNbOfPnt][3] =
  {
    {{-4,0,0},{0,4,0},{4,0,0},{0,-4,0}},
    {{-2,-2,4},{-2,2,4},{2,2,4},{2,-2,4}}
  };


  // the number of wires:
  Standard_Integer aNbOfWire = (sizeof(aCoords)/(sizeof(Standard_Real)*3))/aNbOfPnt;

  TopTools_SequenceOfShape aWires;
  for( Standard_Integer i = 0 ; i < aNbOfWire ; i++)
  {
    BRepBuilderAPI_MakePolygon aPol;
    for( Standard_Integer j = 0 ; j < aNbOfPnt ; j++)
      aPol.Add(gp_Pnt(aCoords[i][j][0],aCoords[i][j][1],aCoords[i][j][2]));

    aPol.Close();

    TopoDS_Wire aW;
    aW = aPol.Wire();
    aWires.Append(aW);
  }


  // creates shell passing through a set of wires
  //with the faces are smoothed out by approximation:
  TopoDS_Shape aShape =  CreateShape(aWires);

  //draw this shell:
  getAISContext()->EraseAll();
  for( i =1 ; i < aWires.Length() + 1 ; i++)
    drawShape(TopoDS::Wire(aWires(i)));
  if(WAIT_A_LITTLE) return;
  aShow1 = drawShape(aShape);
  if(WAIT_A_SECOND) return;

  // creates a circle:
  gp_Pnt aCenter = gp_Pnt(0,0,6);
  gp_Dir aNorm = gp::DZ();
  Standard_Real aRad = 5;
  gce_MakeCirc aMC(aCenter,aNorm,aRad);
  gp_Circ aCirc = aMC.Value();

  // creates shell passing through a set of wires
  //with the faces are ruled:
  aWires.Append(BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aCirc)));
  IsSolid = Standard_False;
  IsRuled = Standard_True;
  aShape = CreateShape(aWires,IsSolid,IsRuled);
  
  // draw this shell:
  aShowWire = drawShape(aWires.Last());
  if(WAIT_A_SECOND) return;
  aShow2 = drawShape(aShape);
  getAISContext()->Erase(aShow1);

  // creates shell passing through a set of wires
  //with the faces are smoothed out by approximation:
  aShape = CreateShape(aWires);

  // draw this shell:
  if(WAIT_A_LITTLE) return;
  aShow1 = drawShape(aShape);
  getAISContext()->Erase(aShow2);

  // creates solid passing through a set of wires
  //with the faces are smoothed out by approximation:
  IsSolid = Standard_True;
  aShape = CreateShape(aWires,IsSolid);

  // draw this solid:
  if(WAIT_A_LITTLE) return;
  aShow2 = drawShape(aShape);
  getAISContext()->Erase(aShow1);

  if(WAIT_A_SECOND) return;
  BRepBuilderAPI_MakeEdge aME1(aCirc,0,PI/4);
  BRepBuilderAPI_MakeEdge aME2(aCirc,PI,5*PI/4);

  TopoDS_Edge aE1 = aME1.Edge();
  TopoDS_Edge aE2 = aME2.Edge();
  TopoDS_Edge aE3 = BRepBuilderAPI_MakeEdge(aME1.Vertex2(),aME2.Vertex1());
  TopoDS_Edge aE4 = BRepBuilderAPI_MakeEdge(aME2.Vertex2(),aME1.Vertex1());
  aWires.SetValue(aWires.Length(),BRepBuilderAPI_MakeWire(aE1,aE3,aE2,aE4));

  // creates solid passing through a set of wires
  //with the faces are smoothed out by approximation with precision aPrec:
  IsSolid = Standard_True;
  IsRuled = Standard_False;
  aPrec = 0.5;
  aShape = CreateShape(aWires,IsSolid,IsRuled,aPrec);

  // draw this solid:
  drawShape(TopoDS::Wire(aWires.Last()));
  getAISContext()->Erase(aShowWire);
  if(WAIT_A_LITTLE) return;
  drawShape(aShape);
  getAISContext()->Erase(aShow2);
}


//////////////////////////////////////////////////////////////////////
// function: sample2
// purpose:
//////////////////////////////////////////////////////////////////////

void ThruSections_Presentation::sample2()
{
  ResetView();
  SetViewCenter(2.4529999187450e-007, 4.0824822167758);
  SetViewScale(63.822115234655); 

 
  setResultTitle("Lofting through unclosed sections");
  TCollection_AsciiString aText;
  aText = (
 
  "// this data used for building a shape through sections:" EOL
  "const Standard_Boolean IsSolid = Standard_False;" EOL
  "Standard_Boolean IsRuled;" EOL
  "Standard_Real aPrec;" EOL EOL

  "// creates wires:" EOL
  "TopoDS_Wire aWire1,aWire2;" EOL EOL

  "// initializes these wires:" EOL
  "//aWire1 = ... ; aWire2 = ...;" EOL EOL

  "// creates an algorithm for building a shell or " EOL
  "//solid passing through a set of wires:" EOL
  "BRepOffsetAPI_ThruSections aTSec;" EOL EOL

  "// adds wires to the set of wires:" EOL
  "aTSec.AddWire(aWire1);" EOL
  "aTSec.AddWire(aWire2);" EOL EOL

  "// Initializes algorithm for building a shell " EOL
  "//passing through a set of wires with the faces are" EOL
  "//smoothed out by approximation:" EOL
  "aTSec.Init();" EOL
  "// builds the shell:" EOL
  "aTSec.Build();" EOL 
  "// takes this shape:" EOL
  "TopoDS_Shape aShape = aTSec.Shape();" EOL EOL

  "// Initializes algorithm for building a shell" EOL
  "//passing through a set of wires with the faces are" EOL
  "//smoothed out by approximation with precision aPrec:" EOL
  "IsRuled = Standard_False;" EOL
  "aPrec = 0.5;" EOL
  "aTSec.Init(IsSolid,IsRuled,aPrec);" EOL
  "// builds the solid:" EOL
  "aTSec.Build();" EOL 
  "// takes this shape:" EOL
  "TopoDS_Shape aShape = aTSec.Shape();" EOL EOL

  "// Initializes algorithm for building a shell " EOL
  "//passing through a set of wires with the faces are ruled:" EOL
  "IsRuled = Standard_True;" EOL
  "aTSec.Init(IsSolid,IsRuled);" EOL
  "// builds the shell:" EOL
  "aTSec.Build();" EOL 
  "// takes this shape:" EOL
  "TopoDS_Shape aShape = aTSec.Shape();" EOL EOL
  
  );

  setResultText(aText.ToCString());

//====================================================================

  Handle(AIS_InteractiveObject) aShow1,aShow2,aShowWire1,aShowWire2,aShowWire3;

  // this data used for building a shape through sections:
  const Standard_Boolean IsSolid = Standard_False;
  Standard_Boolean IsRuled;
  Standard_Real aPrec;

  // creates arrays of coordinates of wires:
  Standard_Real aCoords1[][3] = 
  {
    {-5,0,0},{-2,3,0},{3,-2.5,0},{5,0,0}
  };

  Standard_Real aCoords2[][3] = 
  {
    {-5,0,7},{-2,3,7},{3,-2.5,7},{5,0,7}
  };

  Standard_Real aCoords3 [][3] =
  {
    {-4,0,10},{0,4,10},{4,0,10},{0,-4,10}
  };

  // numbers of points for wires:
  Standard_Integer aSize1 = sizeof(aCoords1)/(sizeof(Standard_Real)*3);
  Standard_Integer aSize2 = sizeof(aCoords2)/(sizeof(Standard_Real)*3);
  Standard_Integer aSize3 = sizeof(aCoords3)/(sizeof(Standard_Real)*3);

  // creates curves:
  Handle(Geom_BSplineCurve) aCurve1 = CreateCurve(aCoords1,aSize1);
  Handle(Geom_BSplineCurve) aCurve2 = CreateCurve(aCoords2,aSize2);

  // creates wires based on the curves:
  TopoDS_Wire aW1 = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aCurve1));
  TopoDS_Wire aW2 = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aCurve2));

  // the sequence of wires:
  TopTools_SequenceOfShape aWires;

  // adds the wires to the sequence:
  aWires.Append(aW1);
  aWires.Append(aW2);

  // creates shell passing through a set of wires
  //with the faces are smoothed out by approximation:
  TopoDS_Shape aShape = CreateShape(aWires);
  
  // draw this shell and wires:
  aShowWire1 = drawShape(aW1);
  aShowWire2 = drawShape(aW2);
  if(WAIT_A_LITTLE) return;
  aShow1 = drawShape(aShape);
  if(WAIT_A_SECOND) return;

  // changes coordinates of second array:
  for( Standard_Integer i = 0 ; i < aSize2 ; i++)
  {
    aCoords2[i][0] = 0.5*aCoords2[i][0]; 
    aCoords2[i][1] = -aCoords2[i][1];
  }

  // create curve:
  aCurve2 = CreateCurve(aCoords2,aSize2);

  // create wire:
  aW2 = BRepBuilderAPI_MakeWire(BRepBuilderAPI_MakeEdge(aCurve2));

  // changes value of the last element of sequence:
  aWires.SetValue(aWires.Length(),aW2);

  // creates shell with new wire:
  aPrec = 0.5;
  IsRuled = Standard_False;
  aShape = CreateShape(aWires,IsSolid,IsRuled,aPrec);

  // draw this shell and new wire with precision aPrec:
  aShowWire3 = drawShape(aW2);
  getAISContext()->Erase(aShowWire2);
  if(WAIT_A_LITTLE) return;
  aShow2 = drawShape(aShape);
  getAISContext()->Erase(aShow1);
  if(WAIT_A_SECOND) return;


  // creates elipses:
  gce_MakeElips aMEl(gp::XOY(),5,3.5);
  gce_MakeElips aME2(gp_Pnt(-2.5,0,7),gp_Pnt(0,2,7),gp_Pnt(0,0,7));
  gp_Elips aElips1 = aMEl.Value();
  gp_Elips aElips2 = aME2.Value();

  // create edges based on pathes of elipses:
  TopoDS_Edge aE1 = BRepBuilderAPI_MakeEdge(aElips1,-PI/4,5*PI/4);
  TopoDS_Edge aE2 = BRepBuilderAPI_MakeEdge(aElips2,-PI/4,5*PI/4);

  // creates wires:
  aW1 = BRepBuilderAPI_MakeWire(aE1);
  aW2 = BRepBuilderAPI_MakeWire(aE2);

  // sequence clear: 
  aWires.Clear();

  // adds wires to sequence:
  aWires.Append(aW1);
  aWires.Append(aW2);

  // creates shell based on the new wires:
  aShape = CreateShape(aWires);

  // draw this shell and new wires:
  drawShape(aW1);
  drawShape(aW2);
  getAISContext()->Erase(aShowWire1);
  getAISContext()->Erase(aShowWire3);
  if(WAIT_A_LITTLE) return;
  aShow1 = drawShape(aShape);
  getAISContext()->Erase(aShow2);
  if(WAIT_A_SECOND) return;

  // creates polygon:
  BRepBuilderAPI_MakePolygon aPol;
  for( i = 0 ; i < aSize3; i++)
    aPol.Add(gp_Pnt(aCoords3[i][0],aCoords3[i][1],aCoords3[i][2]));

  // takes the wire:
  aW2 = aPol.Wire();

  // adds the polygon wire to sequence:
  aWires.Append(aW2);

  // creates shell passing through a set of wires
  //with the faces are ruled:
  IsRuled = Standard_True;
  aShape = CreateShape(aWires,IsSolid,IsRuled);
  
  drawShape(aW2);
  if(WAIT_A_LITTLE) return;
  aShow2 = drawShape(aShape);
  getAISContext()->Erase(aShow1);
}

