// Transform_Presentation.cpp: implementation of the Transform_Presentation class.
// Transform shapes: apply gp_Trsf or gp_GTrsf transformation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Transform_Presentation.h" 
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <AIS_Axis.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <gp_GTrsf.hxx>
#include <BRepBuilderAPI_GTransform.hxx>


#ifdef WNT
 #define EOL "\r\n"
 #define DELIM "\\"
#else
 #define EOL "\n"
 #define DELIM "/"
#endif

// Initialization of global variable with an instance of this class
OCCDemo_Presentation* OCCDemo_Presentation::Current = new Transform_Presentation;

// Initialization of array of samples
const Transform_Presentation::PSampleFuncType Transform_Presentation::SampleFuncs[] =
{
  &Transform_Presentation::sample1,
  &Transform_Presentation::sample2
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Transform_Presentation::Transform_Presentation()
{
  myNbSamples = sizeof(SampleFuncs)/sizeof(PSampleFuncType);
  setName ("Transform shapes");
}

//////////////////////////////////////////////////////////////////////
// Sample execution
//////////////////////////////////////////////////////////////////////

void Transform_Presentation::DoSample()
{
  getAISContext()->EraseAll();
  if (myIndex >=0 && myIndex < myNbSamples)
    (this->*SampleFuncs[myIndex])();
}

//////////////////////////////////////////////////////////////////////
// Auxiliary functions
//////////////////////////////////////////////////////////////////////

Standard_Boolean Transform_Presentation::readShape(const Standard_CString theFileName,
                                                   TopoDS_Shape& theShape)
{
  theShape.Nullify();
  TCollection_AsciiString aPath;
  aPath = aPath + GetDataDir() + DELIM + theFileName;
  BRep_Builder aBld;
  Standard_Boolean isRead = BRepTools::Read (theShape, aPath.ToCString(), aBld);
  if (!isRead)
  {
    aPath = aPath + " is not found";
    setResultText(aPath.ToCString());
  }
  return isRead;
}

// apply standard transformation and get transformed shape
static TopoDS_Shape Transform(const TopoDS_Shape& aShape, const gp_Trsf& aTrsf)
{
  BRepBuilderAPI_Transform aTransform(aTrsf);
  Standard_Boolean toCopy = Standard_False;  // share entities if possible
  aTransform.Perform(aShape, toCopy);
  if (aTransform.IsDone())
    return aTransform.Shape();
  return TopoDS_Shape();
}

// apply general transformation and get transformed shape
static TopoDS_Shape GTransform(const TopoDS_Shape& aShape, const gp_GTrsf& aTrsf)
{
  BRepBuilderAPI_GTransform aTransform(aTrsf);
  Standard_Boolean toCopy = Standard_False;  // share entities if possible
  aTransform.Perform(aShape, toCopy);
  if (aTransform.IsDone())
    return aTransform.Shape();
  return TopoDS_Shape();
}

//////////////////////////////////////////////////////////////////////
// Sample functions
//////////////////////////////////////////////////////////////////////

void Transform_Presentation::sample1()
{
  SetViewScale(4);
  setResultTitle ("Standard transformations");

  // define initial shape
  static TopoDS_Shape aShape;
  TopoDS_Shape aNewShape;
  if (aShape.IsNull() && !readShape("shape1.brep",aShape))
    return;

  TCollection_AsciiString aText;
  aText +=
    "// Apply standard transformation and get transformed shape" EOL
    "TopoDS_Shape Transform(const TopoDS_Shape& aShape, const gp_Trsf& aTrsf)" EOL
    "{" EOL
    "  BRepBuilderAPI_Transform aTransform(aTrsf);" EOL
    "  Standard_Boolean toCopy = Standard_False;  // share entities if possible" EOL
    "  aTransform.Perform(aShape, toCopy);" EOL
    "  if (aTransform.IsDone())" EOL
    "    return aTransform.Shape();" EOL
    "  return TopoDS_Shape();" EOL
    "}" EOL EOL

    "// ..." EOL EOL

    "  // define initial shape and shape used for result" EOL
    "  TopoDS_Shape aShape, aNewShape;" EOL
    "  // aShape = ... ;" EOL EOL

    "  // create standard transformation" EOL
    "  gp_Trsf aTrsf;" EOL EOL

    "  // define translation" EOL
    "  Standard_Real dx=-80, dy=-60, dz=-50;" EOL
    "  gp_Vec aVec(dx,dy,dz);" EOL
    "  aTrsf.SetTranslation(aVec);" EOL EOL

    "  aNewShape = Transform(aShape, aTrsf);" EOL EOL

    "  // add rotation" EOL
    "  gp_Trsf aRotTrsf;" EOL
    "  gp_Pnt ax1Pnt(dx,dy,dz);" EOL
    "  gp_Dir ax1Dir(0,0,1);" EOL
    "  gp_Ax1 ax1 (ax1Pnt,ax1Dir);" EOL
    "  Standard_Real angle = 60. * PI / 180;" EOL
    "  aRotTrsf.SetRotation(ax1, angle);" EOL
    "  aTrsf = aRotTrsf * aTrsf;" EOL EOL

    "  aNewShape = Transform(aShape, aTrsf);" EOL EOL

    "  // add scaling" EOL
    "  gp_Trsf aScaleTrsf;" EOL
    "  gp_Pnt scalePnt = ax1Pnt;" EOL
    "  Standard_Real scaleFactor = 0.6;" EOL
    "  aScaleTrsf.SetScale(scalePnt, scaleFactor);" EOL
    "  aTrsf = aScaleTrsf * aTrsf;" EOL EOL

    "  aNewShape = Transform(aShape, aTrsf);" EOL EOL

    "  // add point mirror" EOL
    "  gp_Trsf aMir1Trsf;" EOL
    "  gp_Pnt mirPnt(dx/4,dy/4,dz/4);" EOL
    "  aMir1Trsf.SetMirror(mirPnt);" EOL
    "  aTrsf = aMir1Trsf * aTrsf;" EOL EOL

    "  aNewShape = Transform(aShape, aTrsf);" EOL EOL

    "  // add axial mirror" EOL
    "  gp_Trsf aMir2Trsf;" EOL
    "  gp_Ax1 mirAx1(gp_Pnt(0,0,0),gp_Dir(0,0,1));" EOL
    "  aMir2Trsf.SetMirror(mirAx1);" EOL
    "  aTrsf = aMir2Trsf * aTrsf;" EOL EOL

    "  aNewShape = Transform(aShape, aTrsf);" EOL EOL

    "  // add planar mirror" EOL
    "  gp_Trsf aMir3Trsf;" EOL
    "  gp_Ax2 mirAx2(gp_Pnt(-dx/4,0,0),gp_Dir(1,0,0));" EOL
    "  aMir3Trsf.SetMirror(mirAx2);" EOL
    "  aTrsf = aMir3Trsf * aTrsf;" EOL EOL

    "  aNewShape = Transform(aShape, aTrsf);" EOL EOL;
  setResultText(aText.ToCString());

  Handle(AIS_InteractiveObject) aIShape = drawShape (aShape);
  if (WAIT_A_SECOND) return;

  // create standard transformation
  gp_Trsf aTrsf;

  // define translation
  Standard_Real dx=-80, dy=-60, dz=-50;
  gp_Vec aVec(dx,dy,dz);
  aTrsf.SetTranslation(aVec);

  Handle(AIS_InteractiveObject) aIVec = drawVector(gp::Origin(),aVec,Quantity_NOC_WHITE);
  if (WAIT_A_LITTLE) return;

  aNewShape = Transform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  Handle(AIS_InteractiveObject) oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIVec);

  // add rotation
  gp_Trsf aRotTrsf;
  gp_Pnt ax1Pnt(dx,dy,dz);
  gp_Dir ax1Dir(0,0,1);
  gp_Ax1 ax1 (ax1Pnt,ax1Dir);
  Standard_Real angle = 60. * PI / 180;
  aRotTrsf.SetRotation(ax1, angle);
  aTrsf = aRotTrsf * aTrsf;

  Handle(AIS_InteractiveObject) aIAx1 = drawCurve(new Geom_Line(ax1),Quantity_NOC_WHITE);
  gp_Ax2 ax2(ax1Pnt.Translated(gp_Vec(0,0,70)),ax1Dir);
  Handle(Geom_Curve) aCirc = new Geom_TrimmedCurve(new Geom_Circle(ax2,20),0,angle);
  Handle(AIS_InteractiveObject) aICirc = drawCurve(aCirc,Quantity_NOC_WHITE,Standard_False);
  Handle(Prs3d_Drawer) aDrawer = new Prs3d_Drawer;
  aDrawer->Link()->SetLineArrowDraw(Standard_True);
  aDrawer->Link()->ArrowAspect()->SetLength(2);
  getAISContext()->SetLocalAttributes (aICirc, aDrawer);
  getAISContext()->Display (aICirc);
  if (WAIT_A_LITTLE) return;

  aNewShape = Transform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIAx1);
  getAISContext()->Erase(aICirc);

  // add scaling
  gp_Trsf aScaleTrsf;
  gp_Pnt scalePnt = ax1Pnt;
  Standard_Real scaleFactor = 0.6;
  aScaleTrsf.SetScale(scalePnt, scaleFactor);
  aTrsf = aScaleTrsf * aTrsf;

  Handle(AIS_InteractiveObject) aIPnt = drawPoint(scalePnt, Quantity_NOC_WHITE);
  if (WAIT_A_LITTLE) return;

  aNewShape = Transform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIPnt);

  // add point mirror
  gp_Trsf aMir1Trsf;
  gp_Pnt mirPnt(dx/4,dy/4,dz/4);
  aMir1Trsf.SetMirror(mirPnt);
  aTrsf = aMir1Trsf * aTrsf;

  aIPnt = drawPoint(mirPnt, Quantity_NOC_WHITE);
  if (WAIT_A_LITTLE) return;

  aNewShape = Transform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIPnt);

  // add axial mirror
  gp_Trsf aMir2Trsf;
  gp_Ax1 mirAx1(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  aMir2Trsf.SetMirror(mirAx1);
  aTrsf = aMir2Trsf * aTrsf;

  aIAx1 = drawCurve(new Geom_Line(mirAx1),Quantity_NOC_WHITE);
  if (WAIT_A_LITTLE) return;

  aNewShape = Transform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIAx1);

  // add planar mirror
  gp_Trsf aMir3Trsf;
  gp_Ax2 mirAx2(gp_Pnt(-dx/4,0,0),gp_Dir(1,0,0));
  aMir3Trsf.SetMirror(mirAx2);
  aTrsf = aMir3Trsf * aTrsf;

  Handle(AIS_InteractiveObject) aIAx2 = drawSurface(
    new Geom_RectangularTrimmedSurface(new Geom_Plane(mirAx2),-50.,50.,-50.,50.),
    Quantity_NOC_WHITE);
  if (WAIT_A_LITTLE) return;

  aNewShape = Transform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIAx2);
}

void Transform_Presentation::sample2()
{
  ResetView();
  SetViewCenter(219.25218771377, 60.449092616063);
  SetViewScale(3.4554379699768); 

  setResultTitle ("General transformations");

  // define initial shape
  static TopoDS_Shape aShape;
  TopoDS_Shape aNewShape;
  if (aShape.IsNull() && !readShape("Pump_Nut.brep",aShape))
    return;

  TCollection_AsciiString aText;
  aText +=
    "// Apply general transformation and get transformed shape" EOL
    "TopoDS_Shape GTransform(const TopoDS_Shape& aShape, const gp_GTrsf& aTrsf)" EOL
    "{" EOL
    "  BRepBuilderAPI_GTransform aTransform(aTrsf);" EOL
    "  Standard_Boolean toCopy = Standard_False;  // share entities if possible" EOL
    "  aTransform.Perform(aShape, toCopy);" EOL
    "  if (aTransform.IsDone())" EOL
    "    return aTransform.Shape();" EOL
    "  return TopoDS_Shape();" EOL
    "}" EOL EOL

    "// ..." EOL EOL

    "  // define initial shape and shape used for result" EOL
    "  TopoDS_Shape aShape, aNewShape;" EOL
    "  // aShape = ... ;" EOL EOL

    "  // create general transformation" EOL
    "  gp_GTrsf aTrsf;" EOL EOL
    
    "  // define affinity respect to line" EOL
    "  gp_Ax1 ax1(gp_Pnt(81,280,0),gp_Dir(0,1,0));" EOL
    "  Standard_Real ratio = 2;" EOL
    "  aTrsf.SetAffinity(ax1, ratio);" EOL EOL

    "  aNewShape = GTransform(aShape, aTrsf);" EOL EOL

    "  // add affinity respect to plane" EOL
    "  gp_GTrsf aTrsf1;" EOL
    "  gp_Ax2 ax2(gp_Pnt(81,280,0),gp_Dir(0,1,0));" EOL
    "  ratio = 8;" EOL
    "  aTrsf1.SetAffinity(ax2, ratio);" EOL
    "  aTrsf = aTrsf1 * aTrsf;" EOL EOL

    "  aNewShape = GTransform(aShape, aTrsf);" EOL EOL;
  setResultText(aText.ToCString());

  Handle(AIS_InteractiveObject) aIShape = drawShape (aShape);
  if (WAIT_A_SECOND) return;

  // create general transformation
  gp_GTrsf aTrsf;

  // define affinity respect to line
  gp_Ax1 ax1(gp_Pnt(81,280,0),gp_Dir(0,1,0));
  Standard_Real ratio = 2;
  aTrsf.SetAffinity(ax1, ratio);

  Handle(AIS_InteractiveObject) aIAx1 = drawCurve(new Geom_Line(ax1),Quantity_NOC_WHITE);
  if (WAIT_A_LITTLE) return;

  aNewShape = GTransform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  Handle(AIS_InteractiveObject) oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIAx1);

  // add affinity respect to plane
  gp_GTrsf aTrsf1;
  gp_Ax2 ax2(gp_Pnt(81,280,0),gp_Dir(0,1,0));
  ratio = 8;
  aTrsf1.SetAffinity(ax2, ratio);
  aTrsf.PreMultiply(aTrsf1);

  Handle(AIS_InteractiveObject) aIAx2 = drawSurface(
    new Geom_RectangularTrimmedSurface(new Geom_Plane(ax2),-100.,100.,-100.,100.),
    Quantity_NOC_WHITE);
  if (WAIT_A_LITTLE) return;

  aNewShape = GTransform(aShape, aTrsf);

  if (aNewShape.IsNull())
  {
    aText += EOL "Error: transformation failure" EOL;
    setResultText(aText.ToCString());
    return;
  }
  oldObj = aIShape;
  aIShape = drawShape (aNewShape);
  getAISContext()->Erase(oldObj);
  if (WAIT_A_SECOND) return;
  getAISContext()->Erase(aIAx2);
}
