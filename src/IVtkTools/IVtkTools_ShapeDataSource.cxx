// Created on: 2011-10-14 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

// VIS includes
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkOCC_ShapeMesher.hxx>
#include <IVtkTools_ShapeObject.hxx>

// VTK includes
#include <vtkObjectFactory.h> 
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>
#include <vtkInformation.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

vtkStandardNewMacro(IVtkTools_ShapeDataSource)

//================================================================
// Function : Constructor
// Purpose  : 
//================================================================
IVtkTools_ShapeDataSource::IVtkTools_ShapeDataSource()
: myPolyData (new IVtkVTK_ShapeData),
  myIsFastTransformMode (Standard_False),
  myIsTransformOnly (Standard_False)
{
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//================================================================
// Function : Destructor
// Purpose  : 
//================================================================
IVtkTools_ShapeDataSource::~IVtkTools_ShapeDataSource()
{ }

//================================================================
// Function : SetShape
// Purpose  : 
//================================================================
void IVtkTools_ShapeDataSource::SetShape (const IVtkOCC_Shape::Handle& theOccShape)
{
  if (myIsFastTransformMode && !myOccShape.IsNull() &&
      theOccShape->GetShape().IsPartner (myOccShape->GetShape() ) )
  {
    myIsTransformOnly = Standard_True;
  }
  else
  {
    myIsTransformOnly = Standard_False;
  }

  myOccShape = theOccShape;
  this->Modified();
}

//================================================================
// Function : GetShape
// Purpose  : 
//================================================================
IVtkOCC_Shape::Handle IVtkTools_ShapeDataSource::GetShape()
{
  return myOccShape;
}

//================================================================
// Function : RequestData
// Purpose  : 
//================================================================
int IVtkTools_ShapeDataSource::RequestData(vtkInformation        *vtkNotUsed(theRequest),
                                           vtkInformationVector **vtkNotUsed(theInputVector),
                                           vtkInformationVector  *theOutputVector)
{
  vtkSmartPointer<vtkPolyData> aPolyData = vtkPolyData::GetData (theOutputVector);
  if (aPolyData.GetPointer() != NULL)
  {
    aPolyData->Allocate();
    vtkSmartPointer<vtkPoints> aPts = vtkSmartPointer<vtkPoints>::New();
    aPolyData->SetPoints (aPts);

    vtkSmartPointer<vtkPolyData> aTransformedData;
    TopoDS_Shape aShape = myOccShape->GetShape();
    TopLoc_Location aShapeLoc = aShape.Location();

    if (myIsTransformOnly)
    {
      vtkSmartPointer<vtkPolyData> aPrevData = myPolyData->getVtkPolyData();
      if ( !aShapeLoc.IsIdentity() )
      {
        aTransformedData = this->transform (aPrevData, aShapeLoc);
      }
      else
      {
        aTransformedData = aPrevData;
      }
    }
    else
    {
      IVtkOCC_Shape::Handle aShapeWrapperCopy;
      if ( myIsFastTransformMode && !aShapeLoc.IsIdentity() )
      {
        // Reset location before meshing
        aShape.Location (TopLoc_Location());
        aShapeWrapperCopy = new IVtkOCC_Shape (aShape);
        aShapeWrapperCopy->SetId (myOccShape->GetId());
      }
      else
      {
        aShapeWrapperCopy = myOccShape;
      }

      myPolyData = new IVtkVTK_ShapeData;
      IVtkOCC_ShapeMesher::Handle aMesher = new IVtkOCC_ShapeMesher;
      aMesher->Build (aShapeWrapperCopy, myPolyData);
      vtkSmartPointer<vtkPolyData> aMeshData = myPolyData->getVtkPolyData();

      if ( myIsFastTransformMode && !aShapeLoc.IsIdentity() )
      {
        aTransformedData = this->transform (aMeshData, aShapeLoc);
      }
      else
      {
        aTransformedData = aMeshData;
      }
    }

    aPolyData->CopyStructure (aTransformedData);  // Copy points and cells
    aPolyData->CopyAttributes (aTransformedData); // Copy data arrays (sub-shapes IDs)

    // We store the OccShape instance in a IVtkTools_ShapeObject
    // wrapper in vtkInformation object of vtkDataObject, then pass it
    // to the actors through pipelines, so selection logic can access
    // OccShape easily given the actor instance.
    IVtkTools_ShapeObject::SetShapeSource (this, aPolyData);
    aPolyData->GetAttributes (vtkDataObject::CELL)->SetPedigreeIds (SubShapeIDs());
  }

  return 1;
}

//================================================================
// Function : SubShapeIDs
// Purpose  : 
//================================================================
vtkSmartPointer<vtkIdTypeArray> IVtkTools_ShapeDataSource::SubShapeIDs()
{
  vtkSmartPointer<vtkDataArray> arr = 
    GetOutput()->GetCellData()->GetArray(IVtkVTK_ShapeData::ARRNAME_SUBSHAPE_IDS);
  return vtkSmartPointer<vtkIdTypeArray>( 
    vtkIdTypeArray::SafeDownCast(arr.GetPointer()) );
}

//================================================================
// Function : GetId
// Purpose  : 
//================================================================
IVtk_IdType IVtkTools_ShapeDataSource::GetId() const
{
  return myOccShape.IsNull() ? -1 : myOccShape->GetId();
}

//================================================================
// Function : Contains
// Purpose  : 
//================================================================
Standard_Boolean IVtkTools_ShapeDataSource::Contains (const IVtkOCC_Shape::Handle& shape) const
{
  return ((myOccShape == shape) ? Standard_True : Standard_False);
}

//================================================================
// Function : transform
// Purpose  : 
//================================================================
vtkSmartPointer<vtkPolyData> IVtkTools_ShapeDataSource::transform (vtkPolyData* theSource,
                                                                   const gp_Trsf& theTrsf) const
{
  vtkSmartPointer<vtkPolyData> aResult = vtkSmartPointer<vtkPolyData>::New();
  aResult->Allocate();
  vtkSmartPointer<vtkPoints> aPts = vtkSmartPointer<vtkPoints>::New();
  aResult->SetPoints (aPts);

  vtkSmartPointer<vtkTransform> aTransform = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkMatrix4x4> aMx = vtkSmartPointer<vtkMatrix4x4>::New();
  for (Standard_Integer aRow = 0; aRow < 3; ++aRow)
    for (Standard_Integer aCol = 0; aCol < 4; ++aCol)
    {
      aMx->SetElement (aRow, aCol, theTrsf.Value (aRow + 1, aCol + 1) );
    }

  aTransform->SetMatrix (aMx);
  vtkSmartPointer<vtkTransformPolyDataFilter> aTrsfFilter
    = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  aTrsfFilter->SetTransform (aTransform);
  aTrsfFilter->SetInputData (theSource);
  aTrsfFilter->Update();

  vtkSmartPointer<vtkPolyData> aTransformed = aTrsfFilter->GetOutput();
  aResult->CopyStructure (aTransformed);  // Copy points and cells
  aResult->CopyAttributes (aTransformed); // Copy data arrays (sub-shapes ids)

  return aResult;
}
