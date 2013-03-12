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

void ISession2D_Shape::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
			     const Handle(Prs3d_Presentation)& aPresentation,
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


    switch(TheMode)
    {
    case (1):
      {
        DrawCompound(aPresentation, VCompound, aLineAspectHighlighted);
        break;
      }
    case (2):
      {
        DrawCompound(aPresentation, Rg1LineVCompound, aLineAspectHighlighted);
        break;
      }
    case (3):
      {
        DrawCompound(aPresentation, RgNLineVCompound, aLineAspectHighlighted);
        break;
      }
    case (4):
      {
        DrawCompound(aPresentation, OutLineVCompound, aLineAspectHighlighted);
        break;
      }
    default:
      {
        DrawCompound(aPresentation,VCompound, aLineAspect);
        DrawCompound(aPresentation,Rg1LineVCompound, aLineAspect);
        DrawCompound(aPresentation,RgNLineVCompound, aLineAspect);
        DrawCompound(aPresentation,OutLineVCompound, aLineAspect);
      }
    }

    if (DrawHiddenLine)
    {
      Handle(Prs3d_LineAspect) aLineAspectHighlighted = new Prs3d_LineAspect(Quantity_NOC_RED,
        Aspect_TOL_DOTDASH,2);
      Handle(Prs3d_LineAspect) aLineAspect = new Prs3d_LineAspect(Quantity_NOC_BLUE1,
        Aspect_TOL_DOTDASH,1);

      switch(TheMode)
      {
      case (6):
        {
          DrawCompound(aPresentation, HCompound, aLineAspectHighlighted);
          break;
        }
      case (7):
        {
          DrawCompound(aPresentation, Rg1LineHCompound, aLineAspectHighlighted);
          break;
        }
      case (8):
        {
          DrawCompound(aPresentation, RgNLineHCompound, aLineAspectHighlighted);
          break;
        }
      case (9):
        {
          DrawCompound(aPresentation, OutLineHCompound, aLineAspectHighlighted);
          break;
        }
      default:
        {
          DrawCompound(aPresentation, HCompound, aLineAspect);
          DrawCompound(aPresentation, Rg1LineHCompound, aLineAspect);
          DrawCompound(aPresentation, RgNLineHCompound, aLineAspect);
          DrawCompound(aPresentation, OutLineHCompound, aLineAspect);
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

    switch (TheMode)
    {
    case (1):
      {
        DrawCompound(aPresentation, VCompound, aLineAspectHighlighted);
        break;
      }
    case (2):
      {
        DrawCompound(aPresentation, Rg1LineVCompound, aLineAspectHighlighted);
        break;
      }
    case (3):
      {
        DrawCompound(aPresentation, RgNLineVCompound, aLineAspectHighlighted);
        break;
      }
    case (4):
      {
        DrawCompound(aPresentation, OutLineVCompound, aLineAspectHighlighted);
        break;
      }
    case (5):
      {
        DrawCompound(aPresentation, IsoLineVCompound, aLineAspectHighlighted);
        break;
      }
    default:
      {
        DrawCompound(aPresentation, VCompound, aLineAspect);
        DrawCompound(aPresentation, Rg1LineVCompound, aLineAspect);
        DrawCompound(aPresentation, RgNLineVCompound, aLineAspect);
        DrawCompound(aPresentation, OutLineVCompound, aLineAspect);
        DrawCompound(aPresentation, IsoLineVCompound , aLineAspect);
      }
    }

    if (DrawHiddenLine)
    {
      Handle(Prs3d_LineAspect) aLineAspectHighlighted = new Prs3d_LineAspect(Quantity_NOC_RED,
        Aspect_TOL_DOT,2);
      Handle(Prs3d_LineAspect) aLineAspect = new Prs3d_LineAspect(Quantity_NOC_ALICEBLUE,
        Aspect_TOL_DOT,1);  

            switch(TheMode)
      {
      case (6):
        {
          DrawCompound(aPresentation, HCompound, aLineAspectHighlighted);
          break;
        }
      case (7):
        {
          DrawCompound(aPresentation, Rg1LineHCompound, aLineAspectHighlighted);
          break;
        }
      case (8):
        {
          DrawCompound(aPresentation, RgNLineHCompound, aLineAspectHighlighted);
          break;
        }
      case (9):
        {
          DrawCompound(aPresentation, OutLineHCompound, aLineAspectHighlighted);
          break;
        }
      case (10):
        {
          DrawCompound(aPresentation, IsoLineHCompound, aLineAspectHighlighted);
          break;
        }
      default:
        {
          DrawCompound(aPresentation, HCompound, aLineAspect);
          DrawCompound(aPresentation, Rg1LineHCompound, aLineAspect);
          DrawCompound(aPresentation, RgNLineHCompound, aLineAspect);
          DrawCompound(aPresentation, OutLineHCompound, aLineAspect);
          DrawCompound(aPresentation, IsoLineHCompound, aLineAspect);
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
  StdPrs_WFDeflectionShape::Add(thePresentation,TopoDS_Shape(theCompound),myDrawer);
}

void ISession2D_Shape::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
				      const Standard_Integer aMode) 
{

}

