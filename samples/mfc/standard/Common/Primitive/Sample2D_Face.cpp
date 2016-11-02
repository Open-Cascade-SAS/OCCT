#include "stdafx.h"

#include "Sample2D_Face.h"

IMPLEMENT_STANDARD_RTTIEXT(Sample2D_Face,AIS_InteractiveObject)

Sample2D_Face::Sample2D_Face (const TopoDS_Shape& theFace)
:AIS_InteractiveObject()
{
  myshape = theFace;
  myFORWARDColor  = Quantity_NOC_BLUE1;
  myREVERSEDColor = Quantity_NOC_YELLOW;
  myINTERNALColor = Quantity_NOC_RED1;
  myEXTERNALColor = Quantity_NOC_MAGENTA1;
  myWidthIndex         = 1;
  myTypeIndex          = 1;

  myForwardNum=0;
  myReversedNum=0;
  myInternalNum=0;
  myExternalNum=0;

  SetAutoHilight(Standard_False);

  FillData(Standard_True);
}

void Sample2D_Face::DrawMarker(const Handle(Geom2d_TrimmedCurve)& theCurve, const Handle(Prs3d_Presentation)& thePresentation)
{
  Standard_Real aCenterParam = (theCurve->FirstParameter()+theCurve->LastParameter())/2; 
  gp_Pnt2d p;
  gp_Vec2d v;
  theCurve->D1(aCenterParam,p,v);
  if (v.Magnitude() > gp::Resolution()) 
  {
    gp_Vec aDir(v.X(),v.Y(),0.);
    gp_Pnt aPoint(p.X(),p.Y(),0.);
    aDir.Normalize();
    aDir.Reverse();
    gp_Dir aZ(0,0,1);
    gp_Pnt aLeft(aPoint.Translated(aDir.Rotated(gp_Ax1(aPoint,aZ), M_PI/6)*5)) ;
    gp_Pnt aRight(aPoint.Translated(aDir.Rotated(gp_Ax1(aPoint,aZ), M_PI*11/6)*5));

    Handle(Graphic3d_ArrayOfPolylines) anArrow = new Graphic3d_ArrayOfPolylines(3);
    anArrow->AddVertex(aLeft);
    anArrow->AddVertex(aPoint);
    anArrow->AddVertex(aRight);

    Prs3d_Root::CurrentGroup(thePresentation)->AddPrimitiveArray(anArrow);
  }
}

void Sample2D_Face::FillData(Standard_Boolean isSizesRecompute)
{
  if(myshape.IsNull() || myshape.ShapeType()!=TopAbs_FACE) return;

  Standard_Real f,l;
  TopExp_Explorer ex;
  TopoDS_Face aFace = TopoDS::Face(myshape);

  //count number of verteces and bounds in primitive arrays
  if(isSizesRecompute)
  {
    mySeq_FORWARD.Clear();
    mySeq_REVERSED.Clear();
    mySeq_INTERNAL.Clear();
    mySeq_EXTERNAL.Clear();

    myshape.Orientation(TopAbs_FORWARD);
    ex.Init(myshape,TopAbs_EDGE);
    while (ex.More())
    {
      BRepAdaptor_Curve2d aCurveOnEdge(TopoDS::Edge(ex.Current()),aFace);
      GCPnts_QuasiUniformDeflection anEdgeDistrib(aCurveOnEdge,1.e-2);
      if(anEdgeDistrib.IsDone())
        switch (ex.Current().Orientation())
      {
        case TopAbs_FORWARD: {
          myForwardNum+=anEdgeDistrib.NbPoints();
          myForwardBounds++;
          break;
                             }
        case TopAbs_REVERSED: {
          myReversedNum+=anEdgeDistrib.NbPoints();
          myReversedBounds++;
          break;
                              }
        case TopAbs_INTERNAL: {
          myInternalNum+=anEdgeDistrib.NbPoints();
          myInternalBounds++;
          break;
                              }
        case TopAbs_EXTERNAL: {
          myExternalNum+=anEdgeDistrib.NbPoints();
          myExternalBounds++;
          break;
                              }
        default : break;
      }//end switch
      ex.Next();
    }
  }

  myForwardArray = new Graphic3d_ArrayOfPolylines(myForwardNum,myForwardBounds);
  myReversedArray = new Graphic3d_ArrayOfPolylines(myReversedNum, myReversedBounds);
  myInternalArray = new Graphic3d_ArrayOfPolylines(myInternalNum, myInternalBounds);
  myExternalArray = new Graphic3d_ArrayOfPolylines(myExternalNum, myExternalBounds);

  //fill primitive arrays
  ex.Init(myshape,TopAbs_EDGE);
  while (ex.More()) {
    const Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface
      (TopoDS::Edge(ex.Current()),aFace,f,l);

    Handle(Geom2d_TrimmedCurve) aTrimmedCurve = new Geom2d_TrimmedCurve(aCurve,f,l);
    TopoDS_Edge CurrentEdge= TopoDS::Edge(ex.Current());
    if(!aTrimmedCurve.IsNull())
    {
      Handle(Geom_Curve) aCurve3d = GeomLib::To3d(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1)),aTrimmedCurve);
      BRepAdaptor_Curve2d aCurveOnEdge(CurrentEdge,aFace);
      GCPnts_QuasiUniformDeflection anEdgeDistrib(aCurveOnEdge,1.e-2);
      if(anEdgeDistrib.IsDone())
      {
        switch (ex.Current().Orientation())
        {
        case TopAbs_FORWARD: {
          myForwardArray->AddBound(anEdgeDistrib.NbPoints());
          for(Standard_Integer i=1;i<=anEdgeDistrib.NbPoints();++i)
          {
            myForwardArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if(isSizesRecompute)
            mySeq_FORWARD.Append(aCurve3d);
          break;
                             }
        case TopAbs_REVERSED: {
          myReversedArray->AddBound(anEdgeDistrib.NbPoints());
          for(Standard_Integer i=1;i<=anEdgeDistrib.NbPoints();++i)
          {
            myReversedArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if(isSizesRecompute)
            mySeq_REVERSED.Append(aCurve3d);
          break;
                              }
        case TopAbs_INTERNAL: {
          myInternalArray->AddBound(anEdgeDistrib.NbPoints());
          for(Standard_Integer i=1;i<=anEdgeDistrib.NbPoints();++i)
          {
            myInternalArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if(isSizesRecompute)
            mySeq_INTERNAL.Append(aCurve3d);
          break;
                              }
        case TopAbs_EXTERNAL: {
          myExternalArray->AddBound(anEdgeDistrib.NbPoints());
          for(Standard_Integer i=1;i<=anEdgeDistrib.NbPoints();++i)
          {
            myExternalArray->AddVertex(anEdgeDistrib.Value(i));
          }
          if(isSizesRecompute)
            mySeq_EXTERNAL.Append(aCurve3d);
          break;
                              }
        default : break;
        }//end switch
      }
    }//end else
    ex.Next();
  }
}

void Sample2D_Face::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                             const Handle(Prs3d_Presentation)& thePresentation,
                             const Standard_Integer /*theMode*/)
{
  thePresentation->Clear();
  myDrawer->SetWireDraw(1);

  if(myshape.IsNull() || myshape.ShapeType()!=TopAbs_FACE) return;

  Handle(Graphic3d_AspectLine3d) aLineAspect_FORWARD =
    new Graphic3d_AspectLine3d(myFORWARDColor, Aspect_TOL_SOLID,1);
  Handle(Graphic3d_AspectLine3d) aLineAspect_REVERSED =
    new Graphic3d_AspectLine3d(myREVERSEDColor, Aspect_TOL_SOLID,1);
  Handle(Graphic3d_AspectLine3d) aLineAspect_INTERNAL =
    new Graphic3d_AspectLine3d(myINTERNALColor, Aspect_TOL_SOLID,1);
  Handle(Graphic3d_AspectLine3d) aLineAspect_EXTERNAL =
    new Graphic3d_AspectLine3d(myEXTERNALColor, Aspect_TOL_SOLID,1);

  Standard_Real f,l;
  TopoDS_Face aFace = TopoDS::Face(myshape);
  //estimating number of verteces in primitive arrays
  TopExp_Explorer ex(myshape,TopAbs_EDGE);
  ex.Init(myshape,TopAbs_EDGE);
  while (ex.More())
  {
    const Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface
      (TopoDS::Edge(ex.Current()),aFace,f,l);

    Handle(Geom2d_TrimmedCurve) aTrimmedCurve = new Geom2d_TrimmedCurve(aCurve,f,l);
    TopoDS_Edge aCurrentEdge= TopoDS::Edge(ex.Current());
    //make a 3D curve from 2D trimmed curve to display it
    Handle(Geom_Curve) aCurve3d = GeomLib::To3d(gp_Ax2(gp_Pnt(0,0,0),gp_Dir(0,0,1)),aTrimmedCurve);
    //make distribution of points
    BRepAdaptor_Curve2d aCurveOnEdge(aCurrentEdge,aFace);
    GCPnts_QuasiUniformDeflection anEdgeDistrib(aCurveOnEdge,1.e-2);
    if(anEdgeDistrib.IsDone())
    {
      switch (ex.Current().Orientation())
      {
      case TopAbs_FORWARD: {

        Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_FORWARD);  
        DrawMarker(aTrimmedCurve, thePresentation);
        break;
                           }
      case TopAbs_REVERSED: {

        Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_REVERSED);  
        DrawMarker(aTrimmedCurve, thePresentation);
        break;
                            }
      case TopAbs_INTERNAL: {

        Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_INTERNAL);  
        DrawMarker(aTrimmedCurve, thePresentation);

        mySeq_INTERNAL.Append(aCurve3d);
        break;
                            }
      case TopAbs_EXTERNAL: {

        Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_EXTERNAL);  
        DrawMarker(aTrimmedCurve, thePresentation);
        break;
                            }
      default : break;
      }//end switch
    }
    ex.Next();
  }
  //add all primitives to the presentation
  Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_FORWARD);  
  Prs3d_Root::CurrentGroup(thePresentation)->AddPrimitiveArray(myForwardArray);

  Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_REVERSED);
  Prs3d_Root::CurrentGroup(thePresentation)->AddPrimitiveArray(myReversedArray);

  Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_INTERNAL);
  Prs3d_Root::CurrentGroup(thePresentation)->AddPrimitiveArray(myInternalArray);

  Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect(aLineAspect_EXTERNAL);
  Prs3d_Root::CurrentGroup(thePresentation)->AddPrimitiveArray(myExternalArray);
}

//Method for advanced customizable selection of picked object
void Sample2D_Face::HilightSelected 
( const Handle(PrsMgr_PresentationManager3d)& thePM,
 const SelectMgr_SequenceOfOwner& theOwners)
{
  Handle( Prs3d_Presentation ) aSelectionPrs;

  aSelectionPrs = GetSelectPresentation( thePM );

  Handle(Graphic3d_AspectLine3d) aLineAspect =
    new Graphic3d_AspectLine3d(Quantity_NOC_ANTIQUEWHITE, Aspect_TOL_SOLID,2);
  if( HasPresentation() )
    aSelectionPrs->SetTransformPersistence (Presentation()->TransformPersistence());

  Standard_Integer aLength = theOwners.Length();
  Handle (SelectMgr_EntityOwner) anOwner;

  aSelectionPrs->Clear();
  FillData();

  Prs3d_Root::NewGroup ( aSelectionPrs );
  Handle (Graphic3d_Group) aSelectGroup = Prs3d_Root::CurrentGroup ( aSelectionPrs);

  for(Standard_Integer i=1; i<=aLength; ++i)
  {
    anOwner = theOwners.Value(i);
    //check priority of owner to add primitives in one of array
    //containing primitives with certain type of orientation
    switch(anOwner->Priority())
    {
    case 7:
      {
        //add to objects with forward orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myForwardArray);
        break;
      }
    case 6:
      {
        //add to objects with reversed orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myReversedArray);
        break;
      }
    case 5:
      {
        //add to objects with internal orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myInternalArray);
        break;
      }
    case 4:
      {
        //add to objects with external orientation
        aSelectGroup->SetGroupPrimitivesAspect(aLineAspect);
        aSelectGroup->AddPrimitiveArray(myExternalArray);
        break;
      }
    }

  }
  aSelectionPrs->Display();

}

void Sample2D_Face::ClearSelected ()
{
  Handle( Prs3d_Presentation ) aSelectionPrs = GetSelectPresentation( NULL );  
  if( !aSelectionPrs.IsNull() )
    aSelectionPrs->Clear(); 
}



//Method for advanced customizable highlighting of picked object
void Sample2D_Face::HilightOwnerWithColor ( const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const Handle(Prs3d_Drawer)& theStyle,
                                           const Handle(SelectMgr_EntityOwner)& theOwner)
{
  Handle( Prs3d_Presentation ) aHighlightPrs;
  aHighlightPrs = GetHilightPresentation( thePM );
  if( HasPresentation() )
    aHighlightPrs->SetTransformPersistence (Presentation()->TransformPersistence());
  if(theOwner.IsNull())
    return;
  aHighlightPrs->Clear();
  FillData();

  //Direct highlighting
  Prs3d_Root::NewGroup ( aHighlightPrs );
  Handle (Graphic3d_Group) aHilightGroup = Prs3d_Root::CurrentGroup(aHighlightPrs);
  Handle(Graphic3d_AspectLine3d) aLineAspect =
    new Graphic3d_AspectLine3d(theStyle->Color(), Aspect_TOL_SOLID,2);
  switch(theOwner->Priority())
  {
  case 7:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myForwardArray);
      break;
    }
  case 6:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myReversedArray);
      break;
    }
  case 5:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myInternalArray);
      break;
    }
  case 4:
    {
      aHilightGroup->SetGroupPrimitivesAspect(aLineAspect);
      aHilightGroup->AddPrimitiveArray(myExternalArray);
      break;
    }
  }
  if( thePM->IsImmediateModeOn() )
    thePM->AddToImmediateList( aHighlightPrs );

}



//for auto select
void Sample2D_Face::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                      const Standard_Integer /*theMode*/)
{
  if(myshape.IsNull()) 
    return;

  if(mySeq_FORWARD.Length()==0 && 
    mySeq_REVERSED.Length()==0 &&
    mySeq_INTERNAL.Length()==0 &&
    mySeq_EXTERNAL.Length()==0) return;

  //create entity owner for every part of the face
  //set different priorities for primitives of different orientation
  Handle(SelectMgr_EntityOwner) anOwner_Forward = new SelectMgr_EntityOwner(this,7);
  Handle(SelectMgr_EntityOwner) anOwner_Reversed = new SelectMgr_EntityOwner(this,6);
  Handle(SelectMgr_EntityOwner) anOwner_Internal = new SelectMgr_EntityOwner(this,5);
  Handle(SelectMgr_EntityOwner) anOwner_External = new SelectMgr_EntityOwner(this,4);

  //create a sensitive for every part
  Handle(Select3D_SensitiveGroup) aForwardGroup = new Select3D_SensitiveGroup(anOwner_Forward);
  Handle(Select3D_SensitiveGroup) aReversedGroup = new Select3D_SensitiveGroup(anOwner_Reversed);
  Handle(Select3D_SensitiveGroup) aInternalGroup = new Select3D_SensitiveGroup(anOwner_Internal);
  Handle(Select3D_SensitiveGroup) aExternalGroup = new Select3D_SensitiveGroup(anOwner_External);

  Standard_Integer aLength =  mySeq_FORWARD.Length();
  for(Standard_Integer i=1;i<=aLength;++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_Forward,mySeq_FORWARD(i));
    aForwardGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aForwardGroup);

  aLength =  mySeq_REVERSED.Length();
  for(Standard_Integer i=1;i<=aLength;++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_Reversed,mySeq_REVERSED(i));
    aReversedGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aReversedGroup);

  aLength =  mySeq_INTERNAL.Length();
  for(Standard_Integer i=1;i<=aLength;++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_Internal,mySeq_INTERNAL(i));
    aInternalGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aInternalGroup);

  aLength =  mySeq_EXTERNAL.Length();
  for(Standard_Integer i=1;i<=aLength;++i)
  {
    Handle(Select3D_SensitiveCurve) aSensitveCurve = new Select3D_SensitiveCurve(anOwner_External,mySeq_EXTERNAL(i));
    aExternalGroup->Add(aSensitveCurve);
  }
  theSelection->Add(aExternalGroup);
}

