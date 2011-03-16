// File:	BOPTools_StateFiller.cxx
// Created:	Mon Feb  4 10:08:20 2002
// Author:	Peter KURNEV
//		<pkv@irinox>

//  Modified by skv - Tue Aug 24 12:31:16 2004 OCC6450

#include <BOPTools_StateFiller.ixx>

#include <Precision.hxx>

#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>

#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Solid.hxx>

#include <TopAbs_State.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

#include <BooleanOperations_StateOfShape.hxx>

#include <IntTools_Tools.hxx>
#include <IntTools_Context.hxx>
#include <Geom_Surface.hxx>

//=======================================================================
// function:  BOPTools_StateFiller::BOPTools_StateFiller
// purpose: 
//=======================================================================
  BOPTools_StateFiller::BOPTools_StateFiller(const BOPTools_PaveFiller& aFiller)
:
  myIsDone(Standard_False)
{
  myFiller=(BOPTools_PaveFiller*) &aFiller;
  myDS=myFiller->DS();
  myIntrPool=myFiller->InterfPool();
}
//=======================================================================
// function: Do 
// purpose: 
//=======================================================================
  void BOPTools_StateFiller::Do()
{
}
//=======================================================================
// function:  IsDone
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_StateFiller::IsDone() const
{
  return myIsDone;
}

//=======================================================================
// function:  ConvertState
// purpose: 
//=======================================================================
  BooleanOperations_StateOfShape BOPTools_StateFiller::ConvertState(const TopAbs_State aSt)
{
  BooleanOperations_StateOfShape aState;
  switch (aSt) {
    case TopAbs_IN:
      aState=BooleanOperations_IN;
      break;
    case TopAbs_OUT:
      aState=BooleanOperations_OUT;
      break;  
    case TopAbs_ON:
      aState=BooleanOperations_ON;
      break;  
    case TopAbs_UNKNOWN:
      aState=BooleanOperations_UNKNOWN;
      break;  
    default:
      aState=BooleanOperations_UNKNOWN;
      break;  
  }
  return aState;
}

//=======================================================================
// function:  ConvertState
// purpose: 
//=======================================================================
  TopAbs_State BOPTools_StateFiller::ConvertState(const BooleanOperations_StateOfShape aSt)
{
  TopAbs_State aState;
  
  switch (aSt) {
    case BooleanOperations_IN:
      aState=TopAbs_IN;
      break;
    case BooleanOperations_OUT:
      aState=TopAbs_OUT;
      break;  
    case BooleanOperations_ON:
      aState=TopAbs_ON;
      break;  
    case BooleanOperations_UNKNOWN:
      aState=TopAbs_UNKNOWN;
      break;  
    default:
      aState=TopAbs_UNKNOWN;
      break;  
  }
  return aState;
}
					    
//=======================================================================
// function:  ClassifyShapeByRef
// purpose: 
//=======================================================================
  BooleanOperations_StateOfShape BOPTools_StateFiller::ClassifyShapeByRef (const TopoDS_Shape& aS,
									   const TopoDS_Shape& aRef)
{
  TopAbs_ShapeEnum aType;
  aType=aS.ShapeType();

//  Modified by skv - Tue Aug 24 12:31:16 2004 OCC6450 Begin
  Standard_Boolean hasEdge = Standard_True;
//  Modified by skv - Tue Aug 24 12:31:17 2004 OCC6450 End
  TopoDS_Edge aE;
  if (aType!=TopAbs_EDGE) {
    TopTools_IndexedMapOfShape aME;
    TopExp::MapShapes(aS, TopAbs_EDGE, aME);

//  Modified by skv - Tue Aug 24 12:31:16 2004 OCC6450 Begin
    if (aME.Extent() == 0)
      hasEdge = Standard_False;
    else
//  Modified by skv - Tue Aug 24 12:31:16 2004 OCC6450 End
      aE=TopoDS::Edge(aME(1));
  }
  else {
    aE=TopoDS::Edge(aS);
  }
  
//  Modified by skv - Tue Aug 24 12:31:16 2004 OCC6450 Begin
//   TopAbs_State aSt=ClassifyEdgeToSolidByOnePoint(aE, aRef);
  TopAbs_State aSt;

  if (hasEdge) {
    aSt = ClassifyEdgeToSolidByOnePoint(aE, aRef);
  } else {
    TopTools_IndexedMapOfShape aMF;
    Standard_Boolean           hasFace = Standard_True;
    TopoDS_Face                aF;

    TopExp::MapShapes(aS, TopAbs_FACE, aMF);

//  Modified by skv - Tue Aug 24 12:31:16 2004 OCC6450 Begin
    if (aMF.Extent() == 0) {
      hasFace = Standard_False;
    } else {
      aF = TopoDS::Face(aMF(1));
    }

    if (!hasFace) {
      aSt = TopAbs_UNKNOWN;
    } else {
      Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aF);
      Standard_Real        aUMin;
      Standard_Real        aUMax;
      Standard_Real        aVMin;
      Standard_Real        aVMax;

      aSurf->Bounds(aUMin, aUMax, aVMin, aVMax);

      Standard_Boolean isMinInf = Precision::IsNegativeInfinite(aUMin);
      Standard_Boolean isMaxInf = Precision::IsPositiveInfinite(aUMax);
      Standard_Real    dT=10.;
      Standard_Real    aParU;
      Standard_Real    aParV;

      if (isMinInf && !isMaxInf) {
	aParU = aUMax - dT;
      } else if (!isMinInf && isMaxInf) {
	aParU = aUMin + dT;
      } else if (isMinInf && isMaxInf) {
	aParU = 0.;
      } else {
	aParU = IntTools_Tools::IntermediatePoint(aUMin, aUMax);
      }

      isMinInf = Precision::IsNegativeInfinite(aVMin);
      isMaxInf = Precision::IsPositiveInfinite(aVMax);

      if (isMinInf && !isMaxInf) {
	aParV = aVMax - dT;
      } else if (!isMinInf && isMaxInf) {
	aParV = aVMin + dT;
      } else if (isMinInf && isMaxInf) {
	aParV = 0.;
      } else {
	aParV = IntTools_Tools::IntermediatePoint(aVMin, aVMax);
      }

      gp_Pnt aP3d = aSurf->Value(aParU, aParV);

      const TopoDS_Solid          &aRefSolid = TopoDS::Solid(aRef);
      IntTools_Context            &aContext  = myFiller->ChangeContext();
      BRepClass3d_SolidClassifier &aSC       = 
	                               aContext.SolidClassifier(aRefSolid);
  //
      aSC.Perform(aP3d, 1e-7);
  //
      aSt = aSC.State();
    }
  }
//  Modified by skv - Tue Aug 24 12:31:16 2004 OCC6450 Begin

  BooleanOperations_StateOfShape aState=BOPTools_StateFiller::ConvertState(aSt) ;
  
  return aState;
}


//=======================================================================
// function:  ClassifyEdgeToSolidByOnePoint
// purpose: 
//=======================================================================
  TopAbs_State  BOPTools_StateFiller::ClassifyEdgeToSolidByOnePoint(const TopoDS_Edge& E,
								    const TopoDS_Shape& Ref)
{
  Standard_Real f2 = 0., l2 = 0., par = 0.;
  
  Handle(Geom_Curve) C3D = BRep_Tool::Curve(E, f2, l2);
  gp_Pnt aP3d;
	
  if(C3D.IsNull()) {
    //it means that we are in degenerated edge
    const TopoDS_Vertex& fv = TopExp::FirstVertex(E);
    if(fv.IsNull()){
      return TopAbs_UNKNOWN;
    }
    aP3d = BRep_Tool::Pnt(fv);
  }
  else {//usual case
    Standard_Boolean bF2Inf, bL2Inf;
    Standard_Real dT=10.;
    //
    bF2Inf = Precision::IsNegativeInfinite(f2);
    bL2Inf = Precision::IsPositiveInfinite(l2);
    //
    if (bF2Inf && !bL2Inf) {
      par=l2-dT;
    }
    else if (!bF2Inf && bL2Inf) {
      par=f2+dT;
    }
    else if (bF2Inf && bL2Inf) {
      par=0.;
    }
    else {
      par=IntTools_Tools::IntermediatePoint(f2, l2);
    }
    C3D -> D0(par, aP3d);
  }
  //
  const TopoDS_Solid& aRefSolid=TopoDS::Solid(Ref);
  IntTools_Context& aContext=myFiller->ChangeContext();
  BRepClass3d_SolidClassifier& aSC=aContext.SolidClassifier(aRefSolid);
  //
  aSC.Perform(aP3d, 1e-7);
  //
  TopAbs_State aState=aSC.State();
  
  return aState;
}
//=======================================================================
// function:  SubType
// purpose: 
//=======================================================================
  TopAbs_ShapeEnum BOPTools_StateFiller::SubType(const TopoDS_Shape& aS)
{
  TopAbs_ShapeEnum aSourceType, aReturnType;
  aSourceType=aS.ShapeType();

  switch (aSourceType) {
    case TopAbs_SOLID:
      aReturnType=TopAbs_SHELL;
      break;
    case TopAbs_SHELL:
      aReturnType=TopAbs_FACE;
      break;
    case TopAbs_FACE:
      aReturnType=TopAbs_WIRE;
      break;  
    case TopAbs_WIRE:
      aReturnType=TopAbs_EDGE;
      break;   
    case TopAbs_EDGE:
      aReturnType=TopAbs_VERTEX;
      break;     
    default:
      aReturnType=TopAbs_SHAPE;
      break;
  }
  return aReturnType;
}
