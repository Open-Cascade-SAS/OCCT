// Created on: 2016-07-04
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

#ifndef _BRepMesh_FaceChecker_HeaderFile
#define _BRepMesh_FaceChecker_HeaderFile

#include <IMeshTools_Parameters.hxx>
#include <Standard_Transient.hxx>
#include <IMeshData_Face.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Shared.hxx>

//! Auxiliary class checking wires of target face for self-intersections.
//! Explodes wires of discrete face on sets of segments using tessellation
//! data stored in model. Each segment is then checked for intersection with
//! other ones. All collisions are registered and returned as result of check.
class BRepMesh_FaceChecker : public Standard_Transient
{
public: //! @name mesher API
  //! Identifies segment inside face.
  struct Segment
  {
    IMeshData::IEdgePtr EdgePtr;
    gp_Pnt2d*           Point1; // \ Use explicit pointers to points instead of accessing
    gp_Pnt2d*           Point2; // / using indices.

    Segment()
        : EdgePtr(NULL),
          Point1(NULL),
          Point2(NULL)
    {
    }

    Segment(const IMeshData::IEdgePtr& theEdgePtr, gp_Pnt2d* thePoint1, gp_Pnt2d* thePoint2)
        : EdgePtr(theEdgePtr),
          Point1(thePoint1),
          Point2(thePoint2)
    {
    }
  };

  typedef NCollection_Shared<NCollection_Vector<Segment>>                         Segments;
  typedef NCollection_Shared<NCollection_Array1<Handle(Segments)>>                ArrayOfSegments;
  typedef NCollection_Shared<NCollection_Array1<Handle(IMeshData::BndBox2dTree)>> ArrayOfBndBoxTree;
  typedef NCollection_Shared<NCollection_Array1<Handle(IMeshData::MapOfIEdgePtr)>>
    ArrayOfMapOfIEdgePtr;

  //! Default constructor
  Standard_EXPORT BRepMesh_FaceChecker(const IMeshData::IFaceHandle& theFace,
                                       const IMeshTools_Parameters&  theParameters);

  //! Destructor
  Standard_EXPORT virtual ~BRepMesh_FaceChecker();

  //! Performs check wires of the face for intersections.
  //! @return True if there is no intersection, False elsewhere.
  Standard_EXPORT Standard_Boolean Perform();

  //! Returns intersecting edges.
  const Handle(IMeshData::MapOfIEdgePtr)& GetIntersectingEdges() const
  {
    return myIntersectingEdges;
  }

  //! Checks wire with the given index for intersection with others.
  void operator()(const Standard_Integer theWireIndex) const { perform(theWireIndex); }

  DEFINE_STANDARD_RTTIEXT(BRepMesh_FaceChecker, Standard_Transient)

private:
  //! Returns True in case if check can be performed in parallel mode.
  Standard_Boolean isParallel() const
  {
    return (myParameters.InParallel && myDFace->WiresNb() > 1);
  }

  //! Collects face segments.
  void collectSegments();

  //! Collects intersecting edges.
  void collectResult();

  //! Checks wire with the given index for intersection with others.
  void perform(const Standard_Integer theWireIndex) const;

private:
  BRepMesh_FaceChecker(const BRepMesh_FaceChecker& theOther);

  void operator=(const BRepMesh_FaceChecker& theOther);

private:
  IMeshData::IFaceHandle       myDFace;
  const IMeshTools_Parameters& myParameters;

  Handle(ArrayOfSegments)          myWiresSegments;
  Handle(ArrayOfBndBoxTree)        myWiresBndBoxTree;
  Handle(ArrayOfMapOfIEdgePtr)     myWiresIntersectingEdges;
  Handle(IMeshData::MapOfIEdgePtr) myIntersectingEdges;
};

#endif
