// File:	OCAFSample_CommonDriver.cxx
// Created:	Wed Feb 13 18:24:21 2002
// Author:	Michael KUZMITCHEV
//		<mkv@russox.nnov.matra-dtv.fr>
//Modified by:  Sergey RUIN (Naming)

#include "stdafx.h"

#include <OCAFSample_CommonDriver.ixx>
#include <OCAFSample_ICommon.hxx>
#include <OCAFSample_IShape.hxx>

#include "Tags.h"

#include <BRepAlgoAPI_Common.hxx>
#include <TDF_Reference.hxx>
#include <TDocStd_Modified.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

#include <BRepNaming_Common.hxx>

#include <TDataStd_TreeNode.hxx>
//#include <.hxx>

#include <BRepAlgo.hxx>

#define OK_OPERATION 0
#define TREENODE_NOT_FOUND 1
#define LABEL_NOT_FOUND 2
#define NAMED_SHAPE_NOT_FOUND 3
#define NAMED_SHAPE_EMPTY 4
#define OPERATION_NOT_DONE 8
#define NULL_OPERATION 9

// OCCT RTTI
IMPLEMENT_STANDARD_RTTIEXT (OCAFSample_CommonDriver, OCAFSample_Driver)

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================

OCAFSample_CommonDriver::OCAFSample_CommonDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================

Standard_Integer OCAFSample_CommonDriver::Execute(Handle(TFunction_Logbook)& theLogbook) const
{
  Handle(TDF_Reference) aReference;
  TopoDS_Shape aMaster, aTool;

  Handle(TDataStd_TreeNode) aNode;
  if(!Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode)) return TREENODE_NOT_FOUND;

  OCAFSample_ICommon anInterface(aNode);
  aMaster = anInterface.GetContext();
  aTool = anInterface.GetTool();

  BRepAlgoAPI_Common mkCommon(aMaster, aTool);
  if (!mkCommon.IsDone()) return OPERATION_NOT_DONE;
  if (mkCommon.Shape().IsNull()) return NULL_OPERATION;
  if (!BRepAlgo::IsValid(mkCommon.Shape()))
	  return OPERATION_NOT_DONE;


  // Name result
  TDF_Label ResultLabel = Label().FindChild(RESULTS_TAG);
  
  BRepNaming_Common aNaming(ResultLabel);
  aNaming.Load(mkCommon);

  OCAFSample_IShape::AddLabels(aNode, theLogbook);

  TDocStd_Modified::Add(aNode->Father()->Label());

  theLogbook->SetImpacted(Label());
  TDocStd_Modified::Add(Label());  

  theLogbook->SetImpacted(ResultLabel);

  TDF_ChildIterator anIterator(ResultLabel);
  for(; anIterator.More(); anIterator.Next()) {
    theLogbook->SetImpacted(anIterator.Value());
  }  

  return OK_OPERATION;
}
