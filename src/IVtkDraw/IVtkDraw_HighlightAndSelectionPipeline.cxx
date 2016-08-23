// Created on: 2012-04-02 

// Copyright (c) 2012-2014 OPEN CASCADE SAS 
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

#include <IVtkDraw_HighlightAndSelectionPipeline.hxx>

#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>

#include <IVtkOCC_Shape.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkTools_ShapeObject.hxx>


IMPLEMENT_STANDARD_RTTIEXT(IVtkDraw_HighlightAndSelectionPipeline,Standard_Transient)

//===========================================================
// Function : Constructor
// Purpose  :
//===========================================================

IVtkDraw_HighlightAndSelectionPipeline::IVtkDraw_HighlightAndSelectionPipeline (const TopoDS_Shape& theShape,
                                                                                const Standard_Integer theShapeID)
: Standard_Transient()
{
  /* ===========================
   *  Allocate involved filters
   * =========================== */

  myFilterMap.Bind (Filter_DM_Shape, vtkSmartPointer<IVtkTools_DisplayModeFilter>::New());
  myFilterMap.Bind (Filter_DM_Hili,  vtkSmartPointer<IVtkTools_DisplayModeFilter>::New());
  myFilterMap.Bind (Filter_DM_Sel,   vtkSmartPointer<IVtkTools_DisplayModeFilter>::New());
  myFilterMap.Bind (Filter_SUB_Hili, vtkSmartPointer<IVtkTools_SubPolyDataFilter>::New());
  myFilterMap.Bind (Filter_SUB_Sel,  vtkSmartPointer<IVtkTools_SubPolyDataFilter>::New());

  /* ========================
   *  Build primary pipeline
   * ======================== */

  myActor = vtkSmartPointer<vtkActor>::New();
  IVtkOCC_Shape::Handle anIVtkShape = new IVtkOCC_Shape (theShape);
  anIVtkShape->SetId (theShapeID);
  vtkSmartPointer<IVtkTools_ShapeDataSource> aDataSource = vtkSmartPointer<IVtkTools_ShapeDataSource>::New();
  aDataSource->SetShape (anIVtkShape);

  IVtkTools_DisplayModeFilter*
    aDMFilter = IVtkTools_DisplayModeFilter::SafeDownCast (myFilterMap.Find(Filter_DM_Shape));

  aDMFilter->AddInputConnection (aDataSource->GetOutputPort());
  aDMFilter->SetDisplayMode (DM_Wireframe);

  myMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  myMapper->AddInputConnection(aDMFilter->GetOutputPort());
  myActor->SetMapper(myMapper);
  IVtkTools_ShapeObject::SetShapeSource (aDataSource, myActor);

  myMapper->ScalarVisibilityOn();
  myMapper->SetScalarModeToUseCellFieldData();
  IVtkTools::InitShapeMapper (myMapper);
  myMapper->Update();

  /* =================================
   *  Build pipeline for highlighting
   * ================================= */

  IVtkTools_DisplayModeFilter*
    aDMFilterH = IVtkTools_DisplayModeFilter::SafeDownCast (myFilterMap.Find(Filter_DM_Hili) );
  IVtkTools_SubPolyDataFilter*
    aSUBFilterH = IVtkTools_SubPolyDataFilter::SafeDownCast (myFilterMap.Find(Filter_SUB_Hili) );

  // No highligthing exists initially
  aSUBFilterH->SetInputConnection (aDataSource->GetOutputPort() );
  aDMFilterH->SetInputConnection(aSUBFilterH->GetOutputPort());

  myHiliMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  myHiliMapper->SetInputConnection (aDMFilterH->GetOutputPort() );

  // Create non-pickable actor
  myHiliActor = vtkSmartPointer<vtkActor>::New();
  myHiliActor->SetPickable(0);
  myHiliActor->SetVisibility(1);
  myHiliActor->GetProperty()->SetColor(0, 1, 1);
  myHiliActor->GetProperty()->SetOpacity(1);
  myHiliActor->GetProperty()->SetPointSize (myHiliActor->GetProperty()->GetPointSize() + 4 );
  myHiliActor->GetProperty()->SetLineWidth (myHiliActor->GetProperty()->GetLineWidth() + 2 );

  // Set maper for actor
  myHiliActor->SetMapper (myHiliMapper);
  myHiliMapper->ScalarVisibilityOff();

  /* ==============================
   *  Build pipeline for selection
   * ============================== */

  IVtkTools_DisplayModeFilter*
    aDMFilterS = IVtkTools_DisplayModeFilter::SafeDownCast (myFilterMap.Find(Filter_DM_Sel) );
  IVtkTools_SubPolyDataFilter*
    aSUBFilterS = IVtkTools_SubPolyDataFilter::SafeDownCast (myFilterMap.Find(Filter_SUB_Sel) );

  // No highligthing exists initially
  aSUBFilterS->SetInputConnection (aDataSource->GetOutputPort() );
  aDMFilterS->SetInputConnection(aSUBFilterS->GetOutputPort());

  mySelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mySelMapper->SetInputConnection (aDMFilterS->GetOutputPort() );

  // Create non-pickable actor
  mySelActor = vtkSmartPointer<vtkActor>::New();
  mySelActor->SetPickable (0);
  mySelActor->SetVisibility (1);
  mySelActor->GetProperty()->SetColor (1, 1, 1);
  mySelActor->GetProperty()->SetOpacity (1);
  mySelActor->GetProperty()->SetPointSize (myHiliActor->GetProperty()->GetPointSize() + 4 );
  mySelActor->GetProperty()->SetLineWidth (myHiliActor->GetProperty()->GetLineWidth() + 2 );

  // Set maper for actor
  mySelActor->SetMapper (mySelMapper);
  mySelMapper->ScalarVisibilityOff();
}

//===========================================================
// Function : AddToRenderer
// Purpose  :
//===========================================================
void IVtkDraw_HighlightAndSelectionPipeline::AddToRenderer (vtkRenderer* theRenderer)
{
  theRenderer->AddActor (myActor);
  theRenderer->AddActor (myHiliActor);
  theRenderer->AddActor (mySelActor);
}

//===========================================================
// Function : RemoveFromRenderer
// Purpose  :
//===========================================================
void IVtkDraw_HighlightAndSelectionPipeline::RemoveFromRenderer (vtkRenderer* theRenderer)
{
  theRenderer->RemoveActor (myActor);
  theRenderer->RemoveActor (myHiliActor);
  theRenderer->RemoveActor (mySelActor);

  vtkSmartPointer<vtkRenderWindow> aWin = theRenderer->GetRenderWindow();
  if (aWin != NULL)
  {
    myActor->ReleaseGraphicsResources(aWin);
    myHiliActor->ReleaseGraphicsResources(aWin);
    mySelActor->ReleaseGraphicsResources(aWin);
  }
}

//===========================================================
// Function : ClearHighlightFilters
// Purpose  :
//===========================================================
void IVtkDraw_HighlightAndSelectionPipeline::ClearHighlightFilters()
{
  this->GetHighlightFilter()->Clear();
  this->GetHighlightFilter()->SetDoFiltering (true);
  this->GetHighlightFilter()->Modified();
}

//===========================================================
// Function : ClearSelectionFilters
// Purpose  :
//===========================================================
void IVtkDraw_HighlightAndSelectionPipeline::ClearSelectionFilters()
{
  this->GetSelectionFilter()->Clear();
  this->GetSelectionFilter()->SetDoFiltering (true);
  this->GetSelectionFilter()->Modified();
}

//===========================================================
// Function : GetDisplayModeFilter
// Purpose  :
//===========================================================
IVtkTools_DisplayModeFilter* IVtkDraw_HighlightAndSelectionPipeline::GetDisplayModeFilter()
{
  return IVtkTools_DisplayModeFilter::SafeDownCast(myFilterMap.Find(Filter_DM_Shape));
}

//===========================================================
// Function : GetHighlightFilter
// Purpose  :
//===========================================================
IVtkTools_SubPolyDataFilter* IVtkDraw_HighlightAndSelectionPipeline::GetHighlightFilter()
{
  return IVtkTools_SubPolyDataFilter::SafeDownCast(myFilterMap.Find(Filter_SUB_Hili));
}

//===========================================================
// Function : GetSelectionFilter
// Purpose  :
//===========================================================
IVtkTools_SubPolyDataFilter* IVtkDraw_HighlightAndSelectionPipeline::GetSelectionFilter()
{
  return IVtkTools_SubPolyDataFilter::SafeDownCast(myFilterMap.Find(Filter_SUB_Sel));
}

//===========================================================
// Function : GetHighlightDMFilter
// Purpose  :
//===========================================================
IVtkTools_DisplayModeFilter* IVtkDraw_HighlightAndSelectionPipeline::GetHighlightDMFilter()
{
  return IVtkTools_DisplayModeFilter::SafeDownCast(myFilterMap.Find(Filter_DM_Hili));
}

//===========================================================
// Function : GetSelectionDMFilter
// Purpose  :
//===========================================================
IVtkTools_DisplayModeFilter* IVtkDraw_HighlightAndSelectionPipeline::GetSelectionDMFilter()
{
  return IVtkTools_DisplayModeFilter::SafeDownCast(myFilterMap.Find(Filter_DM_Sel));
}

//===========================================================
// Function : SharedVerticesSelectionOn
// Purpose  :
//===========================================================
void IVtkDraw_HighlightAndSelectionPipeline::SharedVerticesSelectionOn()
{
  this->GetHighlightDMFilter()->SetDisplaySharedVertices (Standard_True);
  this->GetSelectionDMFilter()->SetDisplaySharedVertices (Standard_True);
}

//===========================================================
// Function : SharedVerticesSelectionOff
// Purpose  :
//===========================================================
void IVtkDraw_HighlightAndSelectionPipeline::SharedVerticesSelectionOff()
{
  this->GetHighlightDMFilter()->SetDisplaySharedVertices (Standard_False);
  this->GetSelectionDMFilter()->SetDisplaySharedVertices (Standard_False);
}
