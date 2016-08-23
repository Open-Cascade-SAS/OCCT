// Created on: 2011-10-27
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

#include <IVtkTools_ShapeObject.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <vtkActor.h>
#include <vtkObjectFactory.h> 
#include <vtkDataSet.h>
#include <vtkInformation.h>
#include <vtkInformationObjectBaseKey.h>
#include <vtkPolyData.h>

IVtkTools_ShapeObject::KeyPtr IVtkTools_ShapeObject::myKey = 0;

//============================================================================
//  Method: getKey
// Purpose: Static method to get vtkInformationKey for retrieving OccShape 
//          instance from the actor.
//============================================================================
IVtkTools_ShapeObject::KeyPtr IVtkTools_ShapeObject::getKey()
{
  if (!myKey)
  {
    myKey = new vtkInformationObjectBaseKey( "OccShapePtr", "IVtkTools_ShapeObject::Key" );
  }

  return myKey;
}

//============================================================================
//  Method: GetOccShape
// Purpose: Static method to get OCC shape from VTK actor's data from 
//          information object by key.
//============================================================================
IVtkOCC_Shape::Handle IVtkTools_ShapeObject::GetOccShape (vtkActor* theActor)
{
  IVtkOCC_Shape::Handle anOccShape;
  vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc = 
    IVtkTools_ShapeObject::GetShapeSource (theActor);
  if (aSrc.GetPointer() != NULL)
  {
    anOccShape = aSrc->GetShape();
  }
  return anOccShape;
}

//============================================================================
//  Method: GetShapeSource
// Purpose: Static method to get OCC shape source from VTK actor's data from 
//          information object by key.
//============================================================================
vtkSmartPointer<IVtkTools_ShapeDataSource> IVtkTools_ShapeObject
::GetShapeSource (vtkActor* theActor)
{
  vtkSmartPointer<IVtkTools_ShapeDataSource> anOccShapeSource;
  vtkSmartPointer<vtkInformation> anInfo = theActor->GetPropertyKeys();
  if (anInfo.GetPointer() != NULL)
  {
    KeyPtr aKey = getKey();
    if (aKey->Has(anInfo))
    {
      vtkSmartPointer<IVtkTools_ShapeObject> aShapeObj = 
        IVtkTools_ShapeObject::SafeDownCast(aKey->Get (anInfo));
      anOccShapeSource = aShapeObj->GetShapeSource();
    }
  }
  return anOccShapeSource;
}

//============================================================================
//  Method: SetShapeSource
// Purpose: Static method to set OCC shape source to VTK dataset in information 
//          object with key.
//============================================================================
void IVtkTools_ShapeObject::SetShapeSource (IVtkTools_ShapeDataSource* theDataSource,
                                            vtkDataSet*                theDataSet)
{
  if (!theDataSet->GetInformation() )
  {
    theDataSet->SetInformation (vtkSmartPointer<vtkInformation>::New());
  }
  vtkSmartPointer<vtkInformation> aDatasetInfo = theDataSet->GetInformation();
  KeyPtr aKey = getKey();
  vtkSmartPointer<IVtkTools_ShapeObject> aShapeObj = 
    vtkSmartPointer<IVtkTools_ShapeObject>::New();
  aShapeObj->SetShapeSource (theDataSource);
  aKey->Set(aDatasetInfo, aShapeObj);
}

//============================================================================
//  Method: SetShapeSource
// Purpose: Static method to set OCC shape source to VTK actor in information 
//          object with key.
//============================================================================
void IVtkTools_ShapeObject::SetShapeSource (IVtkTools_ShapeDataSource* theDataSource,
                                            vtkActor*                  theActor)
{
  if ( !theActor->GetPropertyKeys() )
  {
    theActor->SetPropertyKeys (vtkSmartPointer<vtkInformation>::New());
  }

  vtkSmartPointer<vtkInformation> anInfo = theActor->GetPropertyKeys();
  KeyPtr aKey = getKey();
  vtkSmartPointer<IVtkTools_ShapeObject> aShapeObj =
    vtkSmartPointer<IVtkTools_ShapeObject>::New();
  aShapeObj->SetShapeSource(theDataSource);
  aKey->Set (anInfo, aShapeObj);
}

//! @class IVtkTools_ShapeObject 
//! @brief VTK holder class for OCC shapes to pass them through pipelines.
vtkStandardNewMacro(IVtkTools_ShapeObject)

//============================================================================
//  Method: Constructor
// Purpose: Protected constructor.
//============================================================================
IVtkTools_ShapeObject::IVtkTools_ShapeObject()
{ }

//============================================================================
//  Method: Destructor
// Purpose: Protected destructor.
//============================================================================
IVtkTools_ShapeObject::~IVtkTools_ShapeObject()
{ }

//============================================================================
//  Method: SetShapeSource
// Purpose: 
//============================================================================
void IVtkTools_ShapeObject::SetShapeSource (IVtkTools_ShapeDataSource* theDataSource)
{
  myShapeSource = theDataSource;
}

//============================================================================
//  Method: GetShapeSource
// Purpose: 
//============================================================================
IVtkTools_ShapeDataSource* IVtkTools_ShapeObject::GetShapeSource() const
{
  return myShapeSource;
}

