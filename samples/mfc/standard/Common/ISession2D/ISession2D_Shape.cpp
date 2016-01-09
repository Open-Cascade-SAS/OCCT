#include "stdafx.h"

#include "ISession2D_Shape.h"
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <TopExp.hxx>

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

void ISession2D_Shape::Remove (const TopoDS_Shape& theShape)
{
  if (myListOfShape.Size() == 0)
  {
    return;
  }

  for (NCollection_List<TopoDS_Shape>::Iterator anIt (myListOfShape); anIt.More(); anIt.Next())
 {
   if (anIt.Value() == theShape)
   {
     myListOfShape.Remove (anIt);
     return;
   }
 }
}

void ISession2D_Shape::SetProjector (HLRAlgo_Projector& aProjector) 
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

void ISession2D_Shape::BuildAlgo() 
{
  myAlgo = new HLRBRep_Algo();
  NCollection_List<TopoDS_Shape>::Iterator anIterator(myListOfShape);
  for (;anIterator.More();anIterator.Next()) myAlgo->Add(anIterator.Value(),myNbIsos);
  myAlgo->Projector(myProjector);
  myAlgo->Update();
  myAlgo->Hide();

}
void ISession2D_Shape::BuildPolyAlgo() 
{
  myPolyAlgo = new HLRBRep_PolyAlgo();
  NCollection_List<TopoDS_Shape>::Iterator anIterator(myListOfShape);
  for (;anIterator.More();anIterator.Next()) myPolyAlgo->Load(anIterator.Value());
  myPolyAlgo->Projector(myProjector);
  myPolyAlgo->Update();
}

void ISession2D_Shape::Compute(const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                               const Handle(Prs3d_Presentation)& thePresentation,
                               const Standard_Integer theMode)
{
  
  Standard_Integer aMode = theMode;
  Standard_Boolean DrawHiddenLine= Standard_True;
  thePresentation->Clear();
  if (aMode >= 1000)
  {
    DrawHiddenLine = Standard_False;
    aMode -= 1000;
  }

  Standard_Boolean UsePolyAlgo= Standard_True;
  if (aMode >= 100)
  {
    UsePolyAlgo = Standard_False;
    aMode -= 100;
  }
  TopoDS_Shape VCompound;
  TopoDS_Shape Rg1LineVCompound;
  TopoDS_Shape RgNLineVCompound;
  TopoDS_Shape OutLineVCompound;
  TopoDS_Shape IsoLineVCompound;  // only for Exact algo
  TopoDS_Shape HCompound;
  TopoDS_Shape Rg1LineHCompound;
  TopoDS_Shape RgNLineHCompound;
  TopoDS_Shape OutLineHCompound;
  TopoDS_Shape IsoLineHCompound;  // only for Exact algo

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
    Handle(Prs3d_LineAspect) aLineAspectHighlighted = new Prs3d_LineAspect(Quantity_NOC_ALICEBLUE,
      Aspect_TOL_DOTDASH,1);
    Handle(Prs3d_LineAspect) aLineAspect = new Prs3d_LineAspect(Quantity_NOC_WHITE,
      Aspect_TOL_SOLID,1);

  
    switch(aMode)
    {
    case (1):
      {
        DrawCompound(thePresentation, VCompound, aLineAspectHighlighted);
        break;
      }
    case (2):
      {
        DrawCompound(thePresentation, Rg1LineVCompound, aLineAspectHighlighted);
        break;
      }
    case (3):
      {
        DrawCompound(thePresentation, RgNLineVCompound, aLineAspectHighlighted);
        break;
      }
    case (4):
      {
        DrawCompound(thePresentation, OutLineVCompound, aLineAspectHighlighted);
        break;
      }
    default:
      {
        DrawCompound(thePresentation,VCompound, aLineAspect);
        DrawCompound(thePresentation,Rg1LineVCompound, aLineAspect);
        DrawCompound(thePresentation,RgNLineVCompound, aLineAspect);
        DrawCompound(thePresentation,OutLineVCompound, aLineAspect);
      }
    }

    if (DrawHiddenLine)
    {
      Handle(Prs3d_LineAspect) aLineAspectHighlightedHLR = new Prs3d_LineAspect(Quantity_NOC_RED,
        Aspect_TOL_DOTDASH,2);
      Handle(Prs3d_LineAspect) aLineAspectHLR = new Prs3d_LineAspect(Quantity_NOC_BLUE1,
        Aspect_TOL_DOTDASH,1);

      switch(aMode)
      {
      case (6):
        {
          DrawCompound(thePresentation, HCompound, aLineAspectHighlightedHLR);
          break;
        }
      case (7):
        {
          DrawCompound(thePresentation, Rg1LineHCompound, aLineAspectHighlightedHLR);
          break;
        }
      case (8):
        {
          DrawCompound(thePresentation, RgNLineHCompound, aLineAspectHighlightedHLR);
          break;
        }
      case (9):
        {
          DrawCompound(thePresentation, OutLineHCompound, aLineAspectHighlightedHLR);
          break;
        }
      default:
        {
          DrawCompound(thePresentation, HCompound, aLineAspectHLR);
          DrawCompound(thePresentation, Rg1LineHCompound, aLineAspectHLR);
          DrawCompound(thePresentation, RgNLineHCompound, aLineAspectHLR);
          DrawCompound(thePresentation, OutLineHCompound, aLineAspectHLR);
        }
      }
    }
  }
  else
  {
    Handle(Prs3d_LineAspect) aLineAspectHighlighted = new Prs3d_LineAspect(Quantity_NOC_RED,
      Aspect_TOL_SOLID,2);
    Handle(Prs3d_LineAspect) aLineAspect = new Prs3d_LineAspect(Quantity_NOC_WHITE,
      Aspect_TOL_SOLID,1);  

    switch (aMode)
    {
    case (1):
      {
        DrawCompound(thePresentation, VCompound, aLineAspectHighlighted);
        break;
      }
    case (2):
      {
        DrawCompound(thePresentation, Rg1LineVCompound, aLineAspectHighlighted);
        break;
      }
    case (3):
      {
        DrawCompound(thePresentation, RgNLineVCompound, aLineAspectHighlighted);
        break;
      }
    case (4):
      {
        DrawCompound(thePresentation, OutLineVCompound, aLineAspectHighlighted);
        break;
      }
    case (5):
      {
        DrawCompound(thePresentation, IsoLineVCompound, aLineAspectHighlighted);
        break;
      }
    default:
      {
        DrawCompound(thePresentation, VCompound, aLineAspect);
        DrawCompound(thePresentation, Rg1LineVCompound, aLineAspect);
        DrawCompound(thePresentation, RgNLineVCompound, aLineAspect);
        DrawCompound(thePresentation, OutLineVCompound, aLineAspect);
        DrawCompound(thePresentation, IsoLineVCompound , aLineAspect);
      }
    }

    if (DrawHiddenLine)
    {
      Handle(Prs3d_LineAspect) aLineAspectHighlightedHLR = new Prs3d_LineAspect(Quantity_NOC_RED,
        Aspect_TOL_DOT,2);

      switch (aMode)
      {
      case (6):
        {
          DrawCompound(thePresentation, HCompound, aLineAspectHighlightedHLR);
          break;
        }
      case (7):
        {
          DrawCompound(thePresentation, Rg1LineHCompound, aLineAspectHighlightedHLR);
          break;
        }
      case (8):
        {
          DrawCompound(thePresentation, RgNLineHCompound, aLineAspectHighlightedHLR);
          break;
        }
      case (9):
        {
          DrawCompound(thePresentation, OutLineHCompound, aLineAspectHighlightedHLR);
          break;
        }
      case (10):
        {
          DrawCompound(thePresentation, IsoLineHCompound, aLineAspectHighlightedHLR);
          break;
        }
      default:
        {
          Handle(Prs3d_LineAspect) aLineAspectHLR =
            new Prs3d_LineAspect(Quantity_NOC_ALICEBLUE, Aspect_TOL_DOT, 1);
          DrawCompound(thePresentation, HCompound, aLineAspectHLR);
          DrawCompound(thePresentation, Rg1LineHCompound, aLineAspectHLR);
          DrawCompound(thePresentation, RgNLineHCompound, aLineAspectHLR);
          DrawCompound(thePresentation, OutLineHCompound, aLineAspectHLR);
          DrawCompound(thePresentation, IsoLineHCompound, aLineAspectHLR);
        }
      }
    }
  }
}

void ISession2D_Shape::DrawCompound(const Handle(Prs3d_Presentation)& thePresentation,
                                   const TopoDS_Shape& theCompound, 
                                   const Handle(Prs3d_LineAspect) theAspect)
{
  if (theCompound.IsNull())
    return;
  myDrawer->SetWireAspect(theAspect);
  StdPrs_WFShape::Add(thePresentation,TopoDS_Shape(theCompound),myDrawer);
}

void ISession2D_Shape::ComputeSelection(const Handle(SelectMgr_Selection)& /*aSelection*/,
                                        const Standard_Integer /*aMode*/)
{

}

