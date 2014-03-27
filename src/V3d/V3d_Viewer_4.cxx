// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <V3d_Viewer.jxx>

#include <Graphic3d_ArrayOfPoints.hxx>
#include <Visual3d_View.hxx>

// =======================================================================
// function : Grid
// purpose  :
// =======================================================================
Handle(Aspect_Grid) V3d_Viewer::Grid() const
{
  switch (myGridType)
  {
    case Aspect_GT_Circular:    return myCGrid;
    case Aspect_GT_Rectangular: return myRGrid;
  }
  return myRGrid;
}

// =======================================================================
// function : GridType
// purpose  :
// =======================================================================
Aspect_GridType V3d_Viewer::GridType() const
{
  return myGridType;
}

// =======================================================================
// function : GridDrawMode
// purpose  :
// =======================================================================
Aspect_GridDrawMode V3d_Viewer::GridDrawMode() const
{
  return Grid()->DrawMode();
}

// =======================================================================
// function : ActivateGrid
// purpose  :
// =======================================================================
void V3d_Viewer::ActivateGrid (const Aspect_GridType     theType,
                               const Aspect_GridDrawMode theMode)
{
  Grid()->Erase();
  myGridType = theType;
  Grid()->SetDrawMode (theMode);
  if (theMode != Aspect_GDM_None)
  {
    Grid()->Display();
  }
  Grid()->Activate();
  for (InitActiveViews(); MoreActiveViews(); NextActiveViews())
  {
    ActiveView()->SetGrid (myPrivilegedPlane, Grid());
  }
  Update();
}

// =======================================================================
// function : DeactivateGrid
// purpose  :
// =======================================================================
void V3d_Viewer::DeactivateGrid()
{
  Grid()->Erase();
  myGridType = Aspect_GT_Rectangular;
  Grid()->Deactivate();
  for (InitActiveViews(); MoreActiveViews(); NextActiveViews())
  {
    ActiveView()->SetGridActivity (Standard_False);
    if (myGridEcho
    && !myGridEchoStructure.IsNull())
    {
      ActiveView()->View()->EraseImmediate (myGridEchoStructure);
    }
  }
  Update();
}

// =======================================================================
// function : IsActive
// purpose  :
// =======================================================================
Standard_Boolean V3d_Viewer::IsActive() const
{
  return Grid()->IsActive();
}

// =======================================================================
// function : RectangularGridValues
// purpose  :
// =======================================================================
void V3d_Viewer::RectangularGridValues (Quantity_Length&     theXOrigin,
                                        Quantity_Length&     theYOrigin,
                                        Quantity_Length&     theXStep,
                                        Quantity_Length&     theYStep,
                                        Quantity_PlaneAngle& theRotationAngle) const
{
  theXOrigin       = myRGrid->XOrigin();
  theYOrigin       = myRGrid->YOrigin();
  theXStep         = myRGrid->XStep();
  theYStep         = myRGrid->YStep();
  theRotationAngle = myRGrid->RotationAngle();
}

// =======================================================================
// function : SetRectangularGridValues
// purpose  :
// =======================================================================
void V3d_Viewer::SetRectangularGridValues (const Quantity_Length     theXOrigin,
                                           const Quantity_Length     theYOrigin,
                                           const Quantity_Length     theXStep,
                                           const Quantity_Length     theYStep,
                                           const Quantity_PlaneAngle theRotationAngle)
{
  myRGrid->SetGridValues (theXOrigin, theYOrigin, theXStep, theYStep, theRotationAngle);
  for (InitActiveViews(); MoreActiveViews(); NextActiveViews())
  {
    ActiveView()->SetGrid (myPrivilegedPlane, myRGrid);
  }
  Update();
}

// =======================================================================
// function : CircularGridValues
// purpose  :
// =======================================================================
void V3d_Viewer::CircularGridValues (Quantity_Length&     theXOrigin,
                                     Quantity_Length&     theYOrigin,
                                     Quantity_Length&     theRadiusStep,
                                     Standard_Integer&    theDivisionNumber,
                                     Quantity_PlaneAngle& theRotationAngle) const
{
  theXOrigin        = myCGrid->XOrigin();
  theYOrigin        = myCGrid->YOrigin();
  theRadiusStep     = myCGrid->RadiusStep();
  theDivisionNumber = myCGrid->DivisionNumber();
  theRotationAngle  = myCGrid->RotationAngle();
}

// =======================================================================
// function : SetCircularGridValues
// purpose  :
// =======================================================================
void V3d_Viewer::SetCircularGridValues (const Quantity_Length     theXOrigin,
                                        const Quantity_Length     theYOrigin,
                                        const Quantity_Length     theRadiusStep,
                                        const Standard_Integer    theDivisionNumber,
                                        const Quantity_PlaneAngle theRotationAngle)
{
  myCGrid->SetGridValues (theXOrigin, theYOrigin, theRadiusStep,
                          theDivisionNumber, theRotationAngle);
  for (InitActiveViews(); MoreActiveViews(); NextActiveViews())
  {
    ActiveView()->SetGrid (myPrivilegedPlane, myCGrid);
  }
  Update();
}

// =======================================================================
// function : RectangularGridGraphicValues
// purpose  :
// =======================================================================
void V3d_Viewer::RectangularGridGraphicValues (Quantity_Length& theXSize,
                                               Quantity_Length& theYSize,
                                               Quantity_Length& theOffSet) const
{
  myRGrid->GraphicValues (theXSize, theYSize, theOffSet);
}

// =======================================================================
// function : SetRectangularGridGraphicValues
// purpose  :
// =======================================================================
void V3d_Viewer::SetRectangularGridGraphicValues (const Quantity_Length theXSize,
                                                  const Quantity_Length theYSize,
                                                  const Quantity_Length theOffSet)
{
  myRGrid->SetGraphicValues (theXSize, theYSize, theOffSet);
  for (InitActiveViews(); MoreActiveViews(); NextActiveViews())
  {
    ActiveView()->SetGridGraphicValues (myRGrid);
  }
  Update();
}

// =======================================================================
// function : CircularGridGraphicValues
// purpose  :
// =======================================================================
void V3d_Viewer::CircularGridGraphicValues (Quantity_Length& theRadius,
                                            Quantity_Length& theOffSet) const
{
  myCGrid->GraphicValues (theRadius, theOffSet);
}

// =======================================================================
// function : SetCircularGridGraphicValues
// purpose  :
// =======================================================================
void V3d_Viewer::SetCircularGridGraphicValues (const Quantity_Length theRadius,
                                               const Quantity_Length theOffSet)
{
  myCGrid->SetGraphicValues (theRadius, theOffSet);
  for (InitActiveViews(); MoreActiveViews(); NextActiveViews())
  {
    ActiveView()->SetGridGraphicValues (myCGrid);
  }
  Update();
}

// =======================================================================
// function : SetGridEcho
// purpose  :
// =======================================================================
void V3d_Viewer::SetGridEcho (const Standard_Boolean theToShowGrid)
{
  if (myGridEcho == theToShowGrid)
  {
    return;
  }

  myGridEcho = theToShowGrid;
  if (theToShowGrid
   || myGridEchoStructure.IsNull())
  {
    return;
  }

  for (InitActiveViews(); MoreActiveViews(); NextActiveViews())
  {
    ActiveView()->View()->EraseImmediate (myGridEchoStructure);
  }
}

// =======================================================================
// function : SetGridEcho
// purpose  :
// =======================================================================
void V3d_Viewer::SetGridEcho (const Handle(Graphic3d_AspectMarker3d)& theMarker)
{
  if (myGridEchoStructure.IsNull())
  {
    myGridEchoStructure = new Graphic3d_Structure (Viewer());
    myGridEchoGroup     = myGridEchoStructure->NewGroup();
  }

  myGridEchoAspect = theMarker;
  myGridEchoGroup->SetPrimitivesAspect (theMarker);
}

// =======================================================================
// function : GridEcho
// purpose  :
// =======================================================================
Standard_Boolean V3d_Viewer::GridEcho() const
{
  return myGridEcho;
}

// =======================================================================
// function : ShowGridEcho
// purpose  :
// =======================================================================
void V3d_Viewer::ShowGridEcho (const Handle(V3d_View)& theView,
                               const Graphic3d_Vertex& theVertex)
{
  if (!myGridEcho)
  {
    return;
  }

  if (myGridEchoStructure.IsNull())
  {
    myGridEchoStructure = new Graphic3d_Structure (Viewer());
    myGridEchoGroup     = myGridEchoStructure->NewGroup();

    myGridEchoAspect    = new Graphic3d_AspectMarker3d (Aspect_TOM_STAR, Quantity_Color (Quantity_NOC_GRAY90), 3.0);
    myGridEchoGroup->SetPrimitivesAspect (myGridEchoAspect);
  }

  if (theVertex.X() == myGridEchoLastVert.X()
   && theVertex.Y() == myGridEchoLastVert.Y()
   && theVertex.Z() == myGridEchoLastVert.Z())
  {
    return;
  }

  myGridEchoLastVert = theVertex;
  myGridEchoGroup->Clear();
  myGridEchoGroup->SetPrimitivesAspect (myGridEchoAspect);

  Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
  anArrayOfPoints->AddVertex (theVertex.X(), theVertex.Y(), theVertex.Z());
  myGridEchoGroup->AddPrimitiveArray (anArrayOfPoints);

  theView->View()->DisplayImmediate (myGridEchoStructure, Standard_True);
}

// =======================================================================
// function : HideGridEcho
// purpose  :
// =======================================================================
void V3d_Viewer::HideGridEcho (const Handle(V3d_View)& theView)
{
  if (myGridEchoStructure.IsNull())
  {
    return;
  }

  myGridEchoLastVert.SetCoord (ShortRealLast(), ShortRealLast(), ShortRealLast());
  theView->View()->EraseImmediate (myGridEchoStructure);
}
