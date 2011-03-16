#include "stdafx.h"

#include "ISession2D_Shape.h"
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <TopExp.hxx>

IMPLEMENT_STANDARD_HANDLE(ISession2D_Shape,AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(ISession2D_Shape,AIS_InteractiveObject)

ISession2D_Shape::ISession2D_Shape ()
    :AIS_InteractiveObject(PrsMgr_TOP_ProjectorDependant)
{}

void ISession2D_Shape::Add(const TopoDS_Shape& aShape)
{
  myListOfShape.Append(aShape);
  myAlgo.Nullify();
  myPolyAlgo.Nullify();
  Update(); // protected method used to specify that the presentation are not up to date 
}

void ISession2D_Shape::SetProjector(HLRAlgo_Projector& aProjector) 
{
  myProjector= aProjector;
  myAlgo.Nullify();
  myPolyAlgo.Nullify();
  Update(); // protected method used to specify that the presentation are not up to date 
};


void ISession2D_Shape::SetNbIsos(Standard_Integer& aNbIsos)
{ 
	myNbIsos= aNbIsos; 
	myAlgo.Nullify(); 
    
	// declare the mode 100 to 110 as non valid
	for (int i=100;i<=110;i++)
       Update(i,Standard_False); // protected method used to specify that the presentation are not up to date 

	// declare the mode 1100 to 1110 as non valid
	for (int i=1100;i<=1110;i++)
       Update(i,Standard_False); // protected method used to specify that the presentation are not up to date 

};


/* virtual private */ void ISession2D_Shape::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,const Handle(Prs3d_Presentation)& aPresentation,const Standard_Integer aMode) 
{
}
/* virtual private */ void ISession2D_Shape::Compute(const Handle(Prs3d_Projector)& aProjector,const Handle(Prs3d_Presentation)& aPresentation)
{
}

void ISession2D_Shape::BuildAlgo() 
{
  myAlgo = new HLRBRep_Algo();
  TopTools_ListIteratorOfListOfShape anIterator(myListOfShape);
  for (;anIterator.More();anIterator.Next()) myAlgo->Add(anIterator.Value(),myNbIsos);
  myAlgo->Projector(myProjector);
  myAlgo->Update();
  myAlgo->Hide();

}
void ISession2D_Shape::BuildPolyAlgo() 
{
  myPolyAlgo = new HLRBRep_PolyAlgo();
  TopTools_ListIteratorOfListOfShape anIterator(myListOfShape);
  for (;anIterator.More();anIterator.Next()) myPolyAlgo->Load(anIterator.Value());
  myPolyAlgo->Projector(myProjector);
  myPolyAlgo->Update();
}

/* virtual private */ void ISession2D_Shape::Compute(const Handle(PrsMgr_PresentationManager2d)& aPresentationManager,
			     const Handle(Graphic2d_GraphicObject)& aGrObj,
			     const Standard_Integer aMode) 
{
  Standard_Integer TheMode = aMode;

  Standard_Boolean DrawHiddenLine= Standard_True;
  if (TheMode >= 1000)
  {
    DrawHiddenLine = Standard_False;
    TheMode -= 1000;
  }

  Standard_Boolean UsePolyAlgo= Standard_True;
  if (TheMode >= 100)
  {
    UsePolyAlgo = Standard_False;
    TheMode -= 100;
  }
  TopoDS_Shape VCompound;
  TopoDS_Shape Rg1LineVCompound;
  TopoDS_Shape RgNLineVCompound;
  TopoDS_Shape OutLineVCompound;
  TopoDS_Shape IsoLineVCompound;  // only fro Exact algo
  TopoDS_Shape HCompound;
  TopoDS_Shape Rg1LineHCompound;
  TopoDS_Shape RgNLineHCompound;
  TopoDS_Shape OutLineHCompound;
  TopoDS_Shape IsoLineHCompound;  // only fro Exact algo

  if (UsePolyAlgo)
    {
      if (myPolyAlgo.IsNull()) BuildPolyAlgo();
      HLRBRep_PolyHLRToShape aPolyHLRToShape;
      aPolyHLRToShape.Update(myPolyAlgo);

      VCompound        = aPolyHLRToShape.VCompound();
      Rg1LineVCompound = aPolyHLRToShape.Rg1LineVCompound();
      RgNLineVCompound = aPolyHLRToShape.RgNLineVCompound();
      OutLineVCompound = aPolyHLRToShape.OutLineVCompound();
      HCompound        = aPolyHLRToShape.HCompound();
      Rg1LineHCompound = aPolyHLRToShape.Rg1LineHCompound();
      RgNLineHCompound = aPolyHLRToShape.RgNLineHCompound();
      OutLineHCompound = aPolyHLRToShape.OutLineHCompound();
    }
    else
    {
      if (myAlgo.IsNull()) BuildAlgo();
      HLRBRep_HLRToShape aHLRToShape(myAlgo);

      VCompound        = aHLRToShape.VCompound();
      Rg1LineVCompound = aHLRToShape.Rg1LineVCompound();
      RgNLineVCompound = aHLRToShape.RgNLineVCompound();
      OutLineVCompound = aHLRToShape.OutLineVCompound();
      IsoLineVCompound = aHLRToShape.IsoLineVCompound();
      HCompound        = aHLRToShape.HCompound();
      Rg1LineHCompound = aHLRToShape.Rg1LineHCompound();
      RgNLineHCompound = aHLRToShape.RgNLineHCompound();
      OutLineHCompound = aHLRToShape.OutLineHCompound();
      IsoLineHCompound = aHLRToShape.IsoLineHCompound();
    }

  if (UsePolyAlgo)
    {
      Handle(Graphic2d_SetOfSegments) aSetOfVSegmentsHighLighted = new Graphic2d_SetOfSegments(aGrObj);
      Handle(Graphic2d_SetOfSegments) aSetOfVSegments            = new Graphic2d_SetOfSegments(aGrObj);

      if (TheMode == 1) DrawCompound(VCompound         , aSetOfVSegmentsHighLighted);
      else            DrawCompound(VCompound         , aSetOfVSegments);
      if (TheMode == 2) DrawCompound(Rg1LineVCompound  , aSetOfVSegmentsHighLighted);
      else            DrawCompound(Rg1LineVCompound  , aSetOfVSegments);      
      if (TheMode == 3) DrawCompound(RgNLineVCompound  , aSetOfVSegmentsHighLighted);
      else            DrawCompound(RgNLineVCompound  , aSetOfVSegments);
      if (TheMode == 4) DrawCompound(OutLineVCompound  , aSetOfVSegmentsHighLighted);
      else            DrawCompound(OutLineVCompound  , aSetOfVSegments);

      aSetOfVSegmentsHighLighted->SetColorIndex (1);
      aSetOfVSegmentsHighLighted->SetWidthIndex (1);
      aSetOfVSegmentsHighLighted->SetTypeIndex  (1);
      aSetOfVSegments->SetColorIndex (2);
      aSetOfVSegments->SetWidthIndex (2);
     if (DrawHiddenLine)
      {
        Handle(Graphic2d_SetOfSegments) aSetOfHSegmentsHighLighted = new Graphic2d_SetOfSegments(aGrObj);
        Handle(Graphic2d_SetOfSegments) aSetOfHSegments            = new Graphic2d_SetOfSegments(aGrObj);
        if (TheMode == 6) DrawCompound(HCompound         , aSetOfHSegmentsHighLighted);
        else            DrawCompound(HCompound         , aSetOfHSegments);
        if (TheMode == 7) DrawCompound(Rg1LineHCompound  , aSetOfHSegmentsHighLighted);
        else            DrawCompound(Rg1LineHCompound  , aSetOfHSegments);
        if (TheMode == 8) DrawCompound(RgNLineHCompound  , aSetOfHSegmentsHighLighted);
        else            DrawCompound(RgNLineHCompound  , aSetOfHSegments);
        if (TheMode == 9) DrawCompound(OutLineHCompound  , aSetOfHSegmentsHighLighted);
        else            DrawCompound(OutLineHCompound  , aSetOfHSegments);

        aSetOfVSegments->SetTypeIndex  (2);
        aSetOfHSegmentsHighLighted->SetColorIndex (3);
        aSetOfHSegmentsHighLighted->SetWidthIndex (3);
        aSetOfHSegmentsHighLighted->SetTypeIndex  (3);
        aSetOfHSegments->SetColorIndex (4);
        aSetOfHSegments->SetWidthIndex (4);
        aSetOfHSegments->SetTypeIndex  (4);
      }
  }
  else
  {
      Handle(Graphic2d_SetOfCurves) aSetOfVCurvesHighLighted = new Graphic2d_SetOfCurves(aGrObj);
      Handle(Graphic2d_SetOfCurves) aSetOfVCurves            = new Graphic2d_SetOfCurves(aGrObj);

      if (TheMode == 1)  DrawCompound(VCompound         , aSetOfVCurvesHighLighted);
      else             DrawCompound(VCompound         , aSetOfVCurves);
      if (TheMode == 2)  DrawCompound(Rg1LineVCompound  , aSetOfVCurvesHighLighted);
      else             DrawCompound(Rg1LineVCompound  , aSetOfVCurves);      
      if (TheMode == 3)  DrawCompound(RgNLineVCompound  , aSetOfVCurvesHighLighted);
      else             DrawCompound(RgNLineVCompound  , aSetOfVCurves);
      if (TheMode == 4)  DrawCompound(OutLineVCompound  , aSetOfVCurvesHighLighted);
      else             DrawCompound(OutLineVCompound  , aSetOfVCurves);
      if (TheMode == 5)  DrawCompound(IsoLineVCompound  , aSetOfVCurvesHighLighted);
      else             DrawCompound(IsoLineVCompound  , aSetOfVCurves);
      aSetOfVCurvesHighLighted->SetColorIndex (1);
      aSetOfVCurvesHighLighted->SetWidthIndex (1);
      aSetOfVCurvesHighLighted->SetTypeIndex  (1);
      aSetOfVCurves->SetColorIndex (2);
      aSetOfVCurves->SetWidthIndex (2);
      aSetOfVCurves->SetTypeIndex  (2);

     if (DrawHiddenLine)
      {
        Handle(Graphic2d_SetOfCurves) aSetOfHCurvesHighLighted = new Graphic2d_SetOfCurves(aGrObj);
        Handle(Graphic2d_SetOfCurves) aSetOfHCurves            = new Graphic2d_SetOfCurves(aGrObj);
        if (TheMode == 6)  DrawCompound(HCompound         , aSetOfHCurvesHighLighted);
        else               DrawCompound(HCompound         , aSetOfHCurves);
        if (TheMode == 7)  DrawCompound(Rg1LineHCompound  , aSetOfHCurvesHighLighted);
        else               DrawCompound(Rg1LineHCompound  , aSetOfHCurves);
        if (TheMode == 8)  DrawCompound(RgNLineHCompound  , aSetOfHCurvesHighLighted);
        else               DrawCompound(RgNLineHCompound  , aSetOfHCurves);
        if (TheMode == 9)  DrawCompound(OutLineHCompound  , aSetOfHCurvesHighLighted);
        else               DrawCompound(OutLineHCompound  , aSetOfHCurves);
        if (TheMode == 10) DrawCompound(IsoLineHCompound  , aSetOfHCurvesHighLighted);
        else               DrawCompound(IsoLineHCompound  , aSetOfHCurves);

        aSetOfHCurvesHighLighted->SetColorIndex (3);
        aSetOfHCurvesHighLighted->SetWidthIndex (3);
        aSetOfHCurvesHighLighted->SetTypeIndex  (3);
        aSetOfHCurves->SetColorIndex (4);
        aSetOfHCurves->SetWidthIndex (4);
        aSetOfHCurves->SetTypeIndex  (4);
      }
  }
}


void ISession2D_Shape::DrawCompound(TopoDS_Shape& aCompound,const Handle(Graphic2d_SetOfSegments)& aSetOfSegments)
{
  if (aCompound.IsNull())
    return;

  TopExp_Explorer ex(aCompound,TopAbs_EDGE);
  while (ex.More()) {
    const TopoDS_Edge& CurrentEdge = TopoDS::Edge(ex.Current());
    const TopoDS_Vertex& FirstVertex=TopExp::FirstVertex(CurrentEdge);
    const TopoDS_Vertex& LastVertex =TopExp::LastVertex(CurrentEdge);
    gp_Pnt FirstPoint = BRep_Tool::Pnt(FirstVertex);
    gp_Pnt LastPoint  = BRep_Tool::Pnt(LastVertex);
    aSetOfSegments->Add(FirstPoint.X(),FirstPoint.Y(),LastPoint.X(),LastPoint.Y());
    ex.Next();
  }
}

void ISession2D_Shape::DrawCompound(TopoDS_Shape& aCompound,const Handle(Graphic2d_SetOfCurves)& aSetOfCurves)
{
  if (aCompound.IsNull())
    return;

  TopExp_Explorer ex(aCompound,TopAbs_EDGE);
  Handle(Geom2d_Curve) aCurve;
  Handle(Geom_Surface) aSurface;
  TopLoc_Location L;
  Standard_Real f,l;
  while (ex.More()) {
    const TopoDS_Edge& CurrentEdge = TopoDS::Edge(ex.Current());
    ASSERT(CurrentEdge.Location().IsIdentity());
    BRep_Tool::CurveOnSurface(CurrentEdge,aCurve,aSurface,L,f,l);
    ASSERT(L.IsIdentity());
    Handle(Geom2d_TrimmedCurve) c= new Geom2d_TrimmedCurve(aCurve,f,l);
    ASSERT(!c.IsNull());
    aSetOfCurves->Add(c);
    ex.Next();
  }
}

void ISession2D_Shape::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				      const Standard_Integer aMode) 
{ 
}

