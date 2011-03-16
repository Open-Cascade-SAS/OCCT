// Copyright: 	Matra-Datavision 1995
// File:	StdSelect_BRepSelectionTool.cxx
// Created:	Tue Mar 14 14:09:28 1995
// Author:	Robert COUBLANC
//		<rob>

#define OCC232	//GG_15/03/02 Enable to compute selection also
//		for COMPSOLID shape type.
//		Origin: TELCO Smart70563 (jbz)

#define OCC294   // SAV 22/10/02 catching exception from BRepAdaptor_Curve::Initialize
#define OCC872   // SAV 23/10/02 checking if TopoDS_Wire is null or not to avoid "access violation" exception.
										
#include <StdSelect_BRepSelectionTool.ixx>
#include <GeomAdaptor_Curve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Circle.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveWire.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveGroup.hxx>

#include <Select3D_ListIteratorOfListOfSensitive.hxx>
#include <Select3D_ListOfSensitiveTriangle.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Select3D_TypeOfSensitivity.hxx>
#include <Precision.hxx>
#include <gp_Circ.hxx>
#include <GCPnts_TangentialDeflection.hxx> 
#include <TopoDS_Wire.hxx> 
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_ErrorHandler.hxx>

//static Standard_Integer myArraySize = 2,myN=2;
static Standard_Integer myN=2;
static Standard_Boolean first = Standard_True;



//==================================================
// Function: Load 
// Purpose :  Version debuggee....
  //==================================================

void StdSelect_BRepSelectionTool
::Load (const Handle(SelectMgr_Selection)& aSelection,
	const TopoDS_Shape& aShap,
	const TopAbs_ShapeEnum aType,
  const Standard_Real theDeflection,
  const Standard_Real theDeviationAngle,
	const Standard_Boolean AutoTriangulation,
	const Standard_Integer aPriority,
	const Standard_Integer NbPOnEdge,
	const Standard_Real MaxParam)
{

  Standard_Integer Prior = (aPriority==-1)? GetStandardPriority(aShap,aType): aPriority;


  switch(aType) {
  case TopAbs_VERTEX:
  case TopAbs_EDGE:
  case TopAbs_WIRE:
  case TopAbs_FACE:
  case TopAbs_SHELL:
  case TopAbs_SOLID:
#ifdef OCC232
  case TopAbs_COMPSOLID:
#endif
    {
      TopTools_IndexedMapOfShape subshaps;
      TopExp::MapShapes(aShap,aType,subshaps);
      Standard_Boolean ComesFromDecomposition(Standard_True);
      if(subshaps.Extent()==1)
	if(aShap==subshaps(1))
	  ComesFromDecomposition = Standard_False;
      
      Handle(StdSelect_BRepOwner) BOwn;
      for (Standard_Integer I = 1;I<=subshaps.Extent();I++){
	BOwn = new StdSelect_BRepOwner(subshaps(I),Prior,ComesFromDecomposition);
//	BOwn->SetHilightMode(1);
	ComputeSensitive (subshaps(I),
			 BOwn,
			 aSelection,
       theDeflection,
       theDeviationAngle,
			 NbPOnEdge,
			 MaxParam,			   
			 AutoTriangulation);
      }
      break;
    }
  default:
    {
      Handle(StdSelect_BRepOwner) BOwn;
      BOwn = new StdSelect_BRepOwner(aShap,Prior);
//      BOwn->SetHilightMode(1);

      ComputeSensitive(aShap,
		       BOwn,
		       aSelection,
           theDeflection,
           theDeviationAngle,
		       NbPOnEdge,
		       MaxParam,
		       AutoTriangulation);
    }
    
  }
}


void StdSelect_BRepSelectionTool
::Load (const Handle(SelectMgr_Selection)& aSelection,
	const Handle(SelectMgr_SelectableObject)& aSO,
	const TopoDS_Shape& aShap,
	const TopAbs_ShapeEnum aType,
  const Standard_Real theDeflection,
  const Standard_Real theDeviationAngle,
	const Standard_Boolean AutoTriangulation,
	const Standard_Integer aPriority,
	const Standard_Integer NbPOnEdge,
	const Standard_Real MaxParam)
     
{
  Load (aSelection,
        aShap,
        aType,
        theDeflection,
        theDeviationAngle,
        AutoTriangulation,
        aPriority,
        NbPOnEdge);

  //chargement des selectables...
  for (aSelection->Init();aSelection->More();aSelection->Next()) {
    Handle(SelectBasics_EntityOwner) BOwn = aSelection->Sensitive()->OwnerId();
    Handle(SelectMgr_EntityOwner) Own = *((Handle(SelectMgr_EntityOwner)*) &BOwn);
    Own->Set(aSO);
  }
}





/*void StdSelect_BRepSelectionTool
::Load (const Handle(SelectMgr_Selection)& aSelection,
	const SelectBasics_SequenceOfAddress& Users,
	const TopoDS_Shape& aShap,
	const TopAbs_ShapeEnum aType,
	const Standard_Boolean AutoTriangulation,
	const Standard_Integer aPriority,
	const Standard_Integer NbPOnEdge,
	const Standard_Real MaxParam)

{
  Load(aSelection,aShap,aType,AutoTriangulation,aPriority,NbPOnEdge);
  for (aSelection->Init();aSelection->More();aSelection->Next())
    {
      for(Standard_Integer i=1;i<=Users.Length();i++)
	{aSelection->Sensitive()->OwnerId()->Add(Users(i));}
    }
}*/





void StdSelect_BRepSelectionTool
::ComputeSensitive(const TopoDS_Shape& shap,
		   const Handle(StdSelect_BRepOwner)& Owner ,
		   const Handle(SelectMgr_Selection)& aSelection,
       const Standard_Real theDeflection,
       const Standard_Real theDeviationAngle,
		   const Standard_Integer NbPOnEdge,
		   const Standard_Real MaxParam,		   
		   const Standard_Boolean AutoTriangulation) 
{
  TopAbs_ShapeEnum TheShapeType = shap.ShapeType();

  switch (TheShapeType) {
  case TopAbs_VERTEX:
    {   
      aSelection->Add(new Select3D_SensitivePoint
		      (Owner , BRep_Tool::Pnt(TopoDS::Vertex(shap))));
      break;
    }
  case TopAbs_EDGE:
    {      
      Handle (Select3D_SensitiveEntity) aSensitive;
      GetEdgeSensitive (shap, Owner, aSelection, theDeflection, theDeviationAngle, NbPOnEdge, MaxParam, aSensitive);
      if(!aSensitive.IsNull())
	aSelection->Add(aSensitive);
      break;
    }
  case TopAbs_WIRE:
    {
      BRepTools_WireExplorer Exp(TopoDS::Wire(shap));
      Handle (Select3D_SensitiveEntity) aSensitive;

      Handle (Select3D_SensitiveWire) aWireSensitive = new Select3D_SensitiveWire(Owner);
      aSelection->Add(aWireSensitive);     
      while(Exp.More()){
	GetEdgeSensitive (Exp.Current(), Owner, aSelection, theDeflection, theDeviationAngle, NbPOnEdge, MaxParam, aSensitive);
	aWireSensitive->Add(aSensitive);
	Exp.Next();
      }
      break;
    }
  case TopAbs_FACE:
    {
      const TopoDS_Face& F = TopoDS::Face(shap);
      Select3D_ListOfSensitive LL;
      GetSensitiveForFace(F,Owner,LL,AutoTriangulation,NbPOnEdge,MaxParam);
      for(Select3D_ListIteratorOfListOfSensitive It(LL);It.More();It.Next())
	aSelection->Add(It.Value());
      break;
    }
  case TopAbs_SHELL:{
    TopTools_IndexedMapOfShape subshaps;
    TopExp::MapShapes(shap,TopAbs_FACE,subshaps);
    Handle(Select3D_SensitiveGroup) SG = new Select3D_SensitiveGroup(Owner);
    Select3D_ListOfSensitive LL;
    TopExp::MapShapes(shap,TopAbs_FACE,subshaps);
    for (Standard_Integer I = 1;I<=subshaps.Extent();I++) {
      GetSensitiveForFace(TopoDS::Face(subshaps(I)),
			  Owner,LL,AutoTriangulation,NbPOnEdge,MaxParam);
    }
    
    if(!LL.IsEmpty()){
      SG->Add(LL);
      aSelection->Add(SG);
    }
    break;
  }
  case TopAbs_SOLID:
  case TopAbs_COMPSOLID:
    {
      TopTools_IndexedMapOfShape subshaps;
      TopExp::MapShapes(shap,TopAbs_FACE,subshaps);
      for (Standard_Integer I = 1;I<=subshaps.Extent();I++)
	{
	  ComputeSensitive (subshaps(I), Owner, aSelection, theDeflection, theDeviationAngle, NbPOnEdge, MaxParam, AutoTriangulation);}
      break;
    }
  case TopAbs_COMPOUND:
  default:
    {

      TopExp_Explorer Exp;

      Standard_Boolean Ilibre = 0;
      for(Exp.Init(shap,TopAbs_VERTEX,TopAbs_EDGE);Exp.More();Exp.Next()){
	Ilibre++;
	ComputeSensitive (Exp.Current(), Owner, aSelection, theDeflection, theDeviationAngle, NbPOnEdge, MaxParam, AutoTriangulation);}

      Ilibre = 0;
      for(Exp.Init(shap,TopAbs_EDGE,TopAbs_FACE);Exp.More();Exp.Next()){
	Ilibre++;
	ComputeSensitive (Exp.Current(), Owner, aSelection, theDeflection, theDeviationAngle, NbPOnEdge, MaxParam, AutoTriangulation);}
      Ilibre = 0;
      for(Exp.Init(shap,TopAbs_WIRE,TopAbs_FACE);Exp.More();Exp.Next()){
	Ilibre++;
	ComputeSensitive (Exp.Current(), Owner, aSelection, theDeflection, theDeviationAngle, NbPOnEdge, MaxParam, AutoTriangulation);}
      
//      Standard_Integer Iface = 0;
      TopTools_IndexedMapOfShape subshaps;
      TopExp::MapShapes(shap,TopAbs_FACE,subshaps);

      for (Standard_Integer I = 1;I<=subshaps.Extent();I++) 
	{
	  ComputeSensitive (subshaps(I), Owner, aSelection, theDeflection, theDeviationAngle, NbPOnEdge, MaxParam, AutoTriangulation);
	}
      
    }      
  }  
}

static Handle(TColgp_HArray1OfPnt) GetPointsFromPolygon (const TopoDS_Edge& theEdge,
                                                         const Standard_Real theDeflection)
{
  Handle(TColgp_HArray1OfPnt) aResultPoints;

  Standard_Real fi, la;
  Handle(Geom_Curve) CC3d = BRep_Tool::Curve (theEdge, fi, la);

  TopLoc_Location aLocation;
  Handle(Poly_Polygon3D) aPolygon = BRep_Tool::Polygon3D (theEdge, aLocation);
  if (!aPolygon.IsNull())
  {
    Standard_Boolean isOK = aPolygon->Deflection() <= theDeflection;
    isOK = isOK || (CC3d.IsNull());
    if (isOK)
    {
      const TColgp_Array1OfPnt& aNodes = aPolygon->Nodes();
      aResultPoints = new TColgp_HArray1OfPnt (1, aNodes.Length());
      if (aLocation.IsIdentity())
      {
        for (Standard_Integer aNodeId (aNodes.Lower()), aPntId (1); aNodeId <= aNodes.Upper(); ++aNodeId, ++aPntId)
        {
          aResultPoints->SetValue (aPntId, aNodes.Value (aNodeId));
        }
      }
      else
      {
        for (Standard_Integer aNodeId (aNodes.Lower()), aPntId (1); aNodeId <= aNodes.Upper(); ++aNodeId, ++aPntId)
        {
          aResultPoints->SetValue (aPntId, aNodes.Value (aNodeId).Transformed (aLocation));
        }
      }
      return aResultPoints;
    }
  }

  Handle(Poly_Triangulation) aTriangulation;
  Handle(Poly_PolygonOnTriangulation) anHIndices;
  BRep_Tool::PolygonOnTriangulation (theEdge, anHIndices, aTriangulation, aLocation);
  if (!anHIndices.IsNull())
  {
    Standard_Boolean isOK = anHIndices->Deflection() <= theDeflection;
    isOK = isOK || (CC3d.IsNull());
    if (isOK)
    {
      const TColStd_Array1OfInteger& anIndices = anHIndices->Nodes();
      const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();

      aResultPoints = new TColgp_HArray1OfPnt (1, anIndices.Length());

      if (aLocation.IsIdentity())
      {
        for (Standard_Integer anIndex (anIndices.Lower()), aPntId (1); anIndex <= anIndices.Upper(); ++anIndex, ++aPntId)
        {
          aResultPoints->SetValue (aPntId, aNodes (anIndices (anIndex)));
        }
      }
      else
      {
        for (Standard_Integer anIndex (anIndices.Lower()), aPntId (1); anIndex <= anIndices.Upper(); ++anIndex, ++aPntId)
        {
          aResultPoints->SetValue (aPntId, aNodes (anIndices (anIndex)).Transformed (aLocation));
        }
      }
      return aResultPoints;
    }
  }
  return aResultPoints;
}

static Standard_Boolean  FindLimits(const Adaptor3d_Curve& aCurve,
				    const Standard_Real  aLimit,
				    Standard_Real&       First,
				    Standard_Real&       Last)
{
  First = aCurve.FirstParameter();
  Last  = aCurve.LastParameter();
  Standard_Boolean firstInf = Precision::IsNegativeInfinite(First);
  Standard_Boolean lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf) {
    gp_Pnt P1,P2;
    Standard_Real delta = 1;
    Standard_Integer count = 0;
    if (firstInf && lastInf) {
      do {
	if (count++ == 100000) return Standard_False;
	delta *= 2;
	First = - delta;
	Last  =   delta;
	aCurve.D0(First,P1);
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (firstInf) {
      aCurve.D0(Last,P2);
      do {
	if (count++ == 100000) return Standard_False;
	delta *= 2;
	First = Last - delta;
	aCurve.D0(First,P1);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (lastInf) {
      aCurve.D0(First,P1);
      do {
	if (count++ == 100000) return Standard_False;
	delta *= 2;
	Last = First + delta;
	aCurve.D0(Last,P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }    
  return Standard_True;
}

//=====================================================
// Function : GetEdgeSensitive
// Purpose  : cree un edge sensible pour pouvoir l'ajouter 
//            dans computeselection a "aselection" (cas d'une selection d'un edge)
//            ou a "aSensitiveWire" (cas d'une selection d'un wire; dans ce cas la,
//            c'est le sensitive wire qui est ajoute a "aselection" )
//            odl - pour la selection par rectangle -
//=====================================================   
void StdSelect_BRepSelectionTool
::GetEdgeSensitive (const TopoDS_Shape& shap,
                    const Handle(StdSelect_BRepOwner)& Owner ,
                    const Handle(SelectMgr_Selection)& aSelection,
                    const Standard_Real theDeflection,
                    const Standard_Real theDeviationAngle,
                    const Standard_Integer NbPOnEdge,
                    const Standard_Real MaxParam,
Handle(Select3D_SensitiveEntity)& aSensitive) 
{
   BRepAdaptor_Curve cu3d;

#ifdef OCC294
   try {
     OCC_CATCH_SIGNALS
     cu3d.Initialize (TopoDS::Edge(shap));
   } catch ( Standard_NullObject ) {
     return;
   }
#else
   cu3d.Initialize (TopoDS::Edge(shap));
#endif

  // try to get points from existing polygons
  Handle(TColgp_HArray1OfPnt) aPoints = GetPointsFromPolygon (TopoDS::Edge(shap), theDeflection);
  if (!aPoints.IsNull() && aPoints->Length() > 0)
  {
    aSensitive = new Select3D_SensitiveCurve (Owner, aPoints);
    return;
  }

  Standard_Real wf,wl;  
  BRep_Tool::Range(TopoDS::Edge(shap), wf, wl);

  switch(cu3d.GetType()){
  case GeomAbs_Line:
    {
      aSensitive = new Select3D_SensitiveSegment
			  (Owner,
			   cu3d.Value(wf),cu3d.Value(wl));
      break;
    }
  case GeomAbs_Circle:
    {
      Handle (Geom_Circle) TheCircle =new Geom_Circle(cu3d.Circle());
      Standard_Real TheStart = cu3d.FirstParameter();
      Standard_Real TheEnd = cu3d.LastParameter();
      if(TheCircle->Radius()<=Precision::Confusion())
	aSelection->Add(new Select3D_SensitivePoint(Owner,TheCircle->Location()));
      else
	aSensitive = new Select3D_SensitiveCircle(Owner,TheCircle,TheStart,TheEnd,Standard_False,16);
      break;
    }
  default:
    {
      // modif-rob : on prend le meme deroulement que dans StdPrs_Curve
      // nb de points...
      //============================================

      //aLimit = myDrawer->MaximalParameterValue(); ??
      Standard_Real aLimit = 200.; // TODO (kgv) - do we need set MaxParam here?
      Standard_Real V1 = cu3d.FirstParameter();
      Standard_Real V2  = cu3d.LastParameter();
      Standard_Boolean firstInf = Precision::IsNegativeInfinite(V1);
      Standard_Boolean lastInf  = Precision::IsPositiveInfinite(V2);
      
      if (firstInf || lastInf) {
	gp_Pnt P1,P2;
	Standard_Real delta = 1;
	if (firstInf && lastInf) {
	  do {
	    delta *= 2;
	    V1 = - delta;
	    V2  =   delta;
	    cu3d.D0(V1,P1);
	    cu3d.D0(V2,P2);
	  } while (P1.Distance(P2) < aLimit);
	}
	else if (firstInf) {
	  cu3d.D0(V2,P2);
	  do {
	    delta *= 2;
	    V1 = V2 - delta;
	    cu3d.D0(V1,P1);
	  } while (P1.Distance(P2) < aLimit);
	}
	else if (lastInf) {
	  cu3d.D0(V1,P1);
	  do {
	    delta *= 2;
	    V2 = V1 + delta;
	    cu3d.D0(V2,P2);
	  } while (P1.Distance(P2) < aLimit);
	}
      }    

      Standard_Real aLimitV1, aLimitV2;
      Standard_Boolean isOK = FindLimits (cu3d, aLimit, aLimitV1, aLimitV2);
      //aLimitV1 = cu3d.FirstParameter();
      //aLimitV2 = cu3d.LastParameter();

      // reproduce drawing behavour
      // TODO (kgv) - remove copy-paste
      if (isOK)
      {
        Standard_Integer aNbIntervals = cu3d.NbIntervals (GeomAbs_C1);
        TColStd_Array1OfReal anIntervals (1, aNbIntervals + 1);
        cu3d.Intervals (anIntervals, GeomAbs_C1);
        Standard_Real aV1, aV2;
        Standard_Integer aNumberOfPoints;
        TColgp_SequenceOfPnt aPointsSeq;

        for (Standard_Integer anIntervalId = 1; anIntervalId <= aNbIntervals; ++anIntervalId)
        {
          aV1 = anIntervals (anIntervalId); aV2 = anIntervals (anIntervalId + 1);
          if (aV2 > aLimitV1 && aV1 < aLimitV2)
          {
            aV1 = Max (aV1, aLimitV1);
            aV2 = Min (aV2, aLimitV2);

            GCPnts_TangentialDeflection anAlgo (cu3d, aV1, aV2, theDeviationAngle, theDeflection);
            aNumberOfPoints = anAlgo.NbPoints();

            if (aNumberOfPoints > 0)
            {
              for (Standard_Integer i = 1; i < aNumberOfPoints; ++i)
              { 
                aPointsSeq.Append (anAlgo.Value (i)); 
              }
              if (anIntervalId == aNbIntervals)
              {
                aPointsSeq.Append (anAlgo.Value (aNumberOfPoints)); 
              }
            }
          }
        }

        aPoints = new TColgp_HArray1OfPnt (1, aPointsSeq.Length());
        for (Standard_Integer aPntId = 1; aPntId <= aPointsSeq.Length(); ++aPntId)
        {
          aPoints->SetValue (aPntId, aPointsSeq.Value (aPntId));
        }
        aSensitive = new Select3D_SensitiveCurve (Owner, aPoints);
        break;
      }

      // simple subdivisions
      Standard_Integer nbintervals = 1;
      
      if (cu3d.GetType() == GeomAbs_BSplineCurve) {
	nbintervals = cu3d.NbKnots() - 1;
	nbintervals = Max(1, nbintervals/3);}
      
      Standard_Real V;
      Standard_Integer N = Max(2, NbPOnEdge*nbintervals);
      Standard_Real DV = (V2-V1) / (N-1);
      gp_Pnt p;
      
      if (first) {
	myN = N;
	first = Standard_False;
      }
      if (myN == N) {
	
	Handle(TColgp_HArray1OfPnt) points = new TColgp_HArray1OfPnt(1, N);
	
	for (Standard_Integer i = 1; i <= N;i++) 
	  { 
	    V = V1 + (i-1)*DV;
	    p = cu3d.Value(V);
	    points->SetValue(i,p); 
	  }
	aSensitive = new Select3D_SensitiveCurve(Owner,points);
	
      }
      else {
	Handle(TColgp_HArray1OfPnt) pointsbis = new TColgp_HArray1OfPnt(1, N);
	
	for (Standard_Integer i = 1; i <= N;i++) 
	  { 
	    V = V1 + (i-1)*DV;
	    p = cu3d.Value(V);
	    pointsbis->SetValue(i,p);
	  }
	aSensitive = new Select3D_SensitiveCurve(Owner,pointsbis);
	
      }
    }
    break;
  }
}


Standard_Integer StdSelect_BRepSelectionTool::GetStandardPriority (const TopoDS_Shape& SH,
								   const TopAbs_ShapeEnum aType) 
{
  switch(aType) {
  case TopAbs_VERTEX:
    return 8;
  case TopAbs_EDGE:
    return 7;
  case TopAbs_WIRE:
    return 6;
  case TopAbs_FACE:
    return 5;
  case TopAbs_SHAPE:
  default:
    switch(SH.ShapeType())
    {
    case TopAbs_VERTEX:
      return 9;
    case TopAbs_EDGE:
      return 8;
    case TopAbs_WIRE:
      return 7;
    case TopAbs_FACE:
      return 6;
    case TopAbs_SHELL:
      return 5;
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    case TopAbs_SHAPE:
    default:
      return 4;
    }
  }
  
  return 0;
}

//=======================================================================
//function : GetSensitiveEntityForFace
//purpose  : 
//=======================================================================
Standard_Boolean StdSelect_BRepSelectionTool::GetSensitiveForFace(const TopoDS_Face& F,
								  const Handle(StdSelect_BRepOwner)& Owner,
								  Select3D_ListOfSensitive& LL,
								  const Standard_Boolean AutoTriangulation,
								  const Standard_Integer NbPOnEdge,
								  const Standard_Real MaxParam,
								  const Standard_Boolean InteriorFlag)
{
  // voyons s y a une triangulation de la face...
   BRepAdaptor_Curve cu3d;
  Handle(Poly_Triangulation) T;
  TopLoc_Location loc;
  T = BRep_Tool::Triangulation(F,loc);
  
  if(T.IsNull() && AutoTriangulation){
    Standard_Real Defl=.2,Ang=30*PI/180.;
    BRepMesh_IncrementalMesh(F,Defl,Standard_True,Ang);
    T = BRep_Tool::Triangulation(F,loc);
    
  }
  if(!T.IsNull()){
    Handle(Select3D_SensitiveTriangulation) STG = 
      new Select3D_SensitiveTriangulation(Owner,T,loc,InteriorFlag);
    LL.Append(STG);
    return Standard_True;
  }

  // pour les faces a bugs de triangulation ou sans autotriangulation ....
  // tres laid et  ne devrait meme plus exister ...
   BRepAdaptor_Surface BS;
  BS.Initialize (F);
  
  Standard_Real FirstU = BS.FirstUParameter()<=-Precision::Infinite()? -MaxParam:BS.FirstUParameter();
  Standard_Real LastU  = BS.LastUParameter() >=Precision::Infinite()? MaxParam:BS.LastUParameter();
  Standard_Real FirstV = BS.FirstVParameter()<=-Precision::Infinite()? -MaxParam:BS.FirstVParameter();
  Standard_Real LastV  = BS.LastVParameter()>=Precision::Infinite()? MaxParam:BS.LastVParameter();
  


  if(BS.GetType()==GeomAbs_Plane){
    gp_Pnt pcur;
    Handle(TColgp_HArray1OfPnt) P = new TColgp_HArray1OfPnt(1,5);
    BS.D0(FirstU,FirstV,pcur);
    P->SetValue(1,pcur);
    BS.D0(LastU,FirstV,pcur);
    P->SetValue(2,pcur);
    BS.D0(LastU,LastV,pcur);
    P->SetValue(3,pcur);
    BS.D0(FirstU,LastV,pcur);
    P->SetValue(4,pcur);
    P->SetValue(5,P->Value(1));
    // si le plan est "infini", on ne le rend sensible que sur sa frontiere delimitee par MaxParam
    if(FirstU ==-MaxParam && LastU==MaxParam && FirstV ==-MaxParam && LastV==MaxParam)
      LL.Append(new Select3D_SensitiveFace
		(Owner, P, Select3D_TOS_BOUNDARY));
    else{
      Select3D_TypeOfSensitivity TS = InteriorFlag ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
      LL.Append(new Select3D_SensitiveFace
		(Owner, P, TS));
    }
    return Standard_True;
  }
  
  // IL S'AGIT DE CONSTRUIRE UN POLYGONE "SENSIBLE" A PARTIR DU CONTOUR EXTERIEUR DE LA FACE...
  // CA N'EST PAS BEAU DU TOUT MAIS...
  TopoDS_Wire aWire;
//  Standard_Integer NbEdges=1;

//  Standard_Integer NbPoints(0);

  TopExp_Explorer EW(F,TopAbs_WIRE);
  if(EW.More())
    aWire = TopoDS::Wire(EW.Current()); // en esperant que c'est le premier... a voir

#ifdef OCC872
   if ( aWire.IsNull() )
     return Standard_False;
#endif

  BRepTools_WireExplorer Exp;
  Exp.Init(aWire); 
  
  TColgp_SequenceOfPnt WirePoints;
  Standard_Boolean FirstExp = Standard_True;
  Standard_Real wf,wl;
  
  for(;Exp.More();Exp.Next()){
    cu3d.Initialize (Exp.Current());	
    BRep_Tool::Range(Exp.Current(), wf, wl);
    if(Abs(wf-wl)<=Precision::Confusion()){
#ifdef DEB
      cout<<" StdSelect_BRepSelectionTool : Curve where ufirst = ulast ...."<<endl;
#endif
    }
    else{
      if(FirstExp){
	if(Exp.Orientation()==TopAbs_FORWARD ){
	  WirePoints.Append(cu3d.Value(wf));
	}
	else{
	  WirePoints.Append(cu3d.Value(wl));}
	FirstExp = Standard_False;}
      
      switch(cu3d.GetType()){
      case GeomAbs_Line:{
	if(Exp.Orientation()==TopAbs_FORWARD){
	  WirePoints.Append(cu3d.Value(wl));}
	else{
	  WirePoints.Append(cu3d.Value(wf));}
	
	break;}
      case GeomAbs_Circle:{
	if (2*PI - Abs(wl-wf)  <= Precision::Confusion()) {
	  if(BS.GetType()==GeomAbs_Cylinder ||
	     BS.GetType()==GeomAbs_Torus ||
	     BS.GetType()==GeomAbs_Cone  ||
	     BS.GetType()==GeomAbs_BSplineSurface) // beuurkk pour l'instant...
	    {
	      Standard_Real ff= wf ,ll= wl ;
	      Standard_Real dw
		=(Max(wf, wl)-Min(wf,wl))/(Standard_Real)Max(2, NbPOnEdge-1);
	      if (Exp.Orientation()==TopAbs_FORWARD){		  
		for (Standard_Real wc=wf+dw; wc <= wl; wc+=dw){
		  WirePoints.Append(cu3d.Value(wc));	    
		}
	      } else if (Exp.Orientation()==TopAbs_REVERSED){		
		for (Standard_Real wc=ll-dw; wc >= ff; wc-=dw){
		  WirePoints.Append(cu3d.Value(wc));	    
		}
	      }
	    }
	  
	  else {
	    if(cu3d.Circle().Radius() <= Precision::Confusion())
	      LL.Append(new Select3D_SensitivePoint(Owner,cu3d.Circle().Location()));
	    else
	      LL.Append(new Select3D_SensitiveCircle(Owner,new Geom_Circle(cu3d.Circle()),InteriorFlag,16));
	    
	  }
	}
	else {
	  Standard_Real ff= wf ,ll= wl ;
	  Standard_Real dw
	    =(Max(wf, wl)-Min(wf,wl))/(Standard_Real)Max(2, NbPOnEdge-1);
	  
	  if (Exp.Orientation()==TopAbs_FORWARD){		  
	    for (Standard_Real wc=wf+dw; wc <= wl; wc+=dw){
	      WirePoints.Append(cu3d.Value(wc));	    
	    }
	  } else if (Exp.Orientation()==TopAbs_REVERSED){		
	    for (Standard_Real wc=ll-dw; wc >= ff; wc-=dw){
	      WirePoints.Append(cu3d.Value(wc));	    
	    }
	  }
	}
	
	break;
      }
      default:
	{
	  Standard_Real ff= wf ,ll= wl ;
	  Standard_Real dw
	    =(Max(wf, wl)-Min(wf,wl))/(Standard_Real)Max(2, NbPOnEdge-1);
	  
	  
	  if (Exp.Orientation()==TopAbs_FORWARD){		
	    for (Standard_Real wc=wf+dw; wc <= wl; wc+=dw){
	      WirePoints.Append(cu3d.Value(wc));	    		  
	    }
	  } else if (Exp.Orientation()==TopAbs_REVERSED) {
	    for (Standard_Real wc=ll-dw; wc >= ff; wc-=dw){
	      WirePoints.Append(cu3d.Value(wc));	    		  
	    }
	  }
	}
      }
    }
  }
  Standard_Integer ArrayPosition = WirePoints.Length();
  
  Handle(TColgp_HArray1OfPnt)  facepoints = new TColgp_HArray1OfPnt(1,ArrayPosition);
  // beurk beurk beurk...
  for(Standard_Integer I=1 ;I<=ArrayPosition;I++)
    {facepoints->SetValue (I, WirePoints.Value(I));}
  
  if ((facepoints->Array1()).Length() > 1) { // 1 si un seul edge circulaire plein.
    Select3D_TypeOfSensitivity TS = InteriorFlag ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
    LL.Append(new Select3D_SensitiveFace
	       (Owner, facepoints, TS));
  }
  return Standard_True;
}
