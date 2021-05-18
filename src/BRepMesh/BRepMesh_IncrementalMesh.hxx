// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_IncrementalMesh_HeaderFile
#define _BRepMesh_IncrementalMesh_HeaderFile

#include <BRepMesh_DiscretRoot.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshTools_Context.hxx>
#include <Standard_NumericError.hxx>

//! Builds the mesh of a shape with respect of their 
//! correctly triangulated parts 
class BRepMesh_IncrementalMesh : public BRepMesh_DiscretRoot
{
public: //! @name mesher API

  //! Default constructor
  Standard_EXPORT BRepMesh_IncrementalMesh();

  //! Destructor
  Standard_EXPORT virtual ~BRepMesh_IncrementalMesh();

  //! Constructor.
  //! Automatically calls method Perform.
  //! @param theShape shape to be meshed.
  //! @param theLinDeflection linear deflection.
  //! @param isRelative if TRUE deflection used for discretization of 
  //! each edge will be <theLinDeflection> * <size of edge>. Deflection 
  //! used for the faces will be the maximum deflection of their edges.
  //! @param theAngDeflection angular deflection.
  //! @param isInParallel if TRUE shape will be meshed in parallel.
  Standard_EXPORT BRepMesh_IncrementalMesh(const TopoDS_Shape&    theShape,
                                           const Standard_Real    theLinDeflection,
                                           const Standard_Boolean isRelative = Standard_False,
                                           const Standard_Real    theAngDeflection = 0.5,
                                           const Standard_Boolean isInParallel = Standard_False);

  //! Constructor.
  //! Automatically calls method Perform.
  //! @param theShape shape to be meshed.
  //! @param theParameters - parameters of meshing
  Standard_EXPORT BRepMesh_IncrementalMesh(const TopoDS_Shape&          theShape,
                                           const IMeshTools_Parameters& theParameters,
                                           const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Performs meshing of the shape.
  Standard_EXPORT virtual void Perform(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Performs meshing using custom context;
  Standard_EXPORT void Perform(const Handle(IMeshTools_Context)& theContext,
                               const Message_ProgressRange& theRange = Message_ProgressRange());
  
public: //! @name accessing to parameters.

  //! Returns meshing parameters
  const IMeshTools_Parameters& Parameters() const
  {
    return myParameters;
  }

  //! Returns modifiable meshing parameters
  IMeshTools_Parameters& ChangeParameters()
  {
    return myParameters;
  }

  //! Returns modified flag.
  Standard_Boolean IsModified() const
  {
    return myModified;
  }
  
  //! Returns accumulated status flags faced during meshing.
  Standard_Integer GetStatusFlags() const
  {
    return myStatus;
  }
  
private:

  //! Initializes specific parameters
  void initParameters()
  {
    if (myParameters.Deflection < Precision::Confusion())
    {
      throw Standard_NumericError ("BRepMesh_IncrementalMesh::initParameters : invalid parameter value");
    }
    if (myParameters.DeflectionInterior < Precision::Confusion())
    {
      myParameters.DeflectionInterior = myParameters.Deflection;
    }

    if (myParameters.MinSize < Precision::Confusion())
    {
      myParameters.MinSize =
        Max(IMeshTools_Parameters::RelMinSize() * Min(myParameters.Deflection,
                                                      myParameters.DeflectionInterior),
            Precision::Confusion());
    }

    if (myParameters.Angle < Precision::Angular())
    {
      throw Standard_NumericError ("BRepMesh_IncrementalMesh::initParameters : invalid parameter value");
    }
    if (myParameters.AngleInterior < Precision::Angular())
    {
      myParameters.AngleInterior = 2.0 * myParameters.Angle;
    }
  }

public: //! @name plugin API

  //! Plugin interface for the Mesh Factories.
  //! Initializes meshing algorithm with the given parameters.
  //! @param theShape shape to be meshed.
  //! @param theLinDeflection linear deflection.
  //! @param theAngDeflection angular deflection.
  //! @param[out] theAlgo pointer to initialized algorithm.
  Standard_EXPORT static Standard_Integer Discret(const TopoDS_Shape&    theShape,
                                                  const Standard_Real    theLinDeflection,
                                                  const Standard_Real    theAngDeflection,
                                                  BRepMesh_DiscretRoot* &theAlgo);
  
  //! Returns multi-threading usage flag set by default in 
  //! Discret() static method (thus applied only to Mesh Factories).
  Standard_EXPORT static Standard_Boolean IsParallelDefault();
  
  //! Setup multi-threading usage flag set by default in 
  //! Discret() static method (thus applied only to Mesh Factories).
  Standard_EXPORT static void SetParallelDefault(const Standard_Boolean isInParallel);

  DEFINE_STANDARD_RTTIEXT(BRepMesh_IncrementalMesh, BRepMesh_DiscretRoot)

protected:

  IMeshTools_Parameters myParameters;
  Standard_Boolean      myModified;
  Standard_Integer      myStatus;
};

#endif
