// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshTools_Context_HeaderFile
#define _IMeshTools_Context_HeaderFile

#include <Standard_Type.hxx>
#include <IMeshTools_ModelBuilder.hxx>
#include <IMeshData_Model.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshTools_ModelAlgo.hxx>
#include <Message_ProgressRange.hxx>

//! Interface class representing context of BRepMesh algorithm.
//! Intended to cache discrete model and instances of tools for
//! its processing.
class IMeshTools_Context : public IMeshData_Shape
{
public:
  //! Constructor.
  IMeshTools_Context() {}

  //! Destructor.
  virtual ~IMeshTools_Context() {}

  //! Builds model using assigned model builder.
  //! @return True on success, False elsewhere.
  virtual bool BuildModel()
  {
    if (myModelBuilder.IsNull())
    {
      return false;
    }

    myModel = myModelBuilder->Perform(GetShape(), myParameters);

    return !myModel.IsNull();
  }

  //! Performs discretization of model edges using assigned edge discret algorithm.
  //! @return True on success, False elsewhere.
  virtual bool DiscretizeEdges()
  {
    if (myModel.IsNull() || myEdgeDiscret.IsNull())
    {
      return false;
    }

    // Discretize edges of a model.
    return myEdgeDiscret->Perform(myModel, myParameters, Message_ProgressRange());
  }

  //! Performs healing of discrete model built by DiscretizeEdges() method
  //! using assigned healing algorithm.
  //! @return True on success, False elsewhere.
  virtual bool HealModel()
  {
    if (myModel.IsNull())
    {
      return false;
    }

    return myModelHealer.IsNull()
             ? true
             : myModelHealer->Perform(myModel, myParameters, Message_ProgressRange());
  }

  //! Performs pre-processing of discrete model using assigned algorithm.
  //! Performs auxiliary actions such as cleaning shape from old triangulation.
  //! @return True on success, False elsewhere.
  virtual bool PreProcessModel()
  {
    if (myModel.IsNull())
    {
      return false;
    }

    return myPreProcessor.IsNull()
             ? true
             : myPreProcessor->Perform(myModel, myParameters, Message_ProgressRange());
  }

  //! Performs meshing of faces of discrete model using assigned meshing algorithm.
  //! @return True on success, False elsewhere.
  virtual bool DiscretizeFaces(const Message_ProgressRange& theRange)
  {
    if (myModel.IsNull() || myFaceDiscret.IsNull())
    {
      return false;
    }

    // Discretize faces of a model.
    return myFaceDiscret->Perform(myModel, myParameters, theRange);
  }

  //! Performs post-processing of discrete model using assigned algorithm.
  //! @return True on success, False elsewhere.
  virtual bool PostProcessModel()
  {
    if (myModel.IsNull())
    {
      return false;
    }

    return myPostProcessor.IsNull()
             ? true
             : myPostProcessor->Perform(myModel, myParameters, Message_ProgressRange());
  }

  //! Cleans temporary context data.
  virtual void Clean()
  {
    if (myParameters.CleanModel)
    {
      myModel.Nullify();
    }
  }

  //! Gets instance of a tool to be used to build discrete model.
  const occ::handle<IMeshTools_ModelBuilder>& GetModelBuilder() const { return myModelBuilder; }

  //! Sets instance of a tool to be used to build discrete model.
  void SetModelBuilder(const occ::handle<IMeshTools_ModelBuilder>& theBuilder)
  {
    myModelBuilder = theBuilder;
  }

  //! Gets instance of a tool to be used to discretize edges of a model.
  const occ::handle<IMeshTools_ModelAlgo>& GetEdgeDiscret() const { return myEdgeDiscret; }

  //! Sets instance of a tool to be used to discretize edges of a model.
  void SetEdgeDiscret(const occ::handle<IMeshTools_ModelAlgo>& theEdgeDiscret)
  {
    myEdgeDiscret = theEdgeDiscret;
  }

  //! Gets instance of a tool to be used to heal discrete model.
  const occ::handle<IMeshTools_ModelAlgo>& GetModelHealer() const { return myModelHealer; }

  //! Sets instance of a tool to be used to heal discrete model.
  void SetModelHealer(const occ::handle<IMeshTools_ModelAlgo>& theModelHealer)
  {
    myModelHealer = theModelHealer;
  }

  //! Gets instance of pre-processing algorithm.
  const occ::handle<IMeshTools_ModelAlgo>& GetPreProcessor() const { return myPreProcessor; }

  //! Sets instance of pre-processing algorithm.
  void SetPreProcessor(const occ::handle<IMeshTools_ModelAlgo>& thePreProcessor)
  {
    myPreProcessor = thePreProcessor;
  }

  //! Gets instance of meshing algorithm.
  const occ::handle<IMeshTools_ModelAlgo>& GetFaceDiscret() const { return myFaceDiscret; }

  //! Sets instance of meshing algorithm.
  void SetFaceDiscret(const occ::handle<IMeshTools_ModelAlgo>& theFaceDiscret)
  {
    myFaceDiscret = theFaceDiscret;
  }

  //! Gets instance of post-processing algorithm.
  const occ::handle<IMeshTools_ModelAlgo>& GetPostProcessor() const { return myPostProcessor; }

  //! Sets instance of post-processing algorithm.
  void SetPostProcessor(const occ::handle<IMeshTools_ModelAlgo>& thePostProcessor)
  {
    myPostProcessor = thePostProcessor;
  }

  //! Gets parameters to be used for meshing.
  const IMeshTools_Parameters& GetParameters() const { return myParameters; }

  //! Gets reference to parameters to be used for meshing.
  IMeshTools_Parameters& ChangeParameters() { return myParameters; }

  //! Returns discrete model of a shape.
  const occ::handle<IMeshData_Model>& GetModel() const { return myModel; }

  DEFINE_STANDARD_RTTIEXT(IMeshTools_Context, IMeshData_Shape)

private:
  occ::handle<IMeshTools_ModelBuilder> myModelBuilder;
  occ::handle<IMeshData_Model>         myModel;
  occ::handle<IMeshTools_ModelAlgo>    myEdgeDiscret;
  occ::handle<IMeshTools_ModelAlgo>    myModelHealer;
  occ::handle<IMeshTools_ModelAlgo>    myPreProcessor;
  occ::handle<IMeshTools_ModelAlgo>    myFaceDiscret;
  occ::handle<IMeshTools_ModelAlgo>    myPostProcessor;
  IMeshTools_Parameters           myParameters;
};

#endif