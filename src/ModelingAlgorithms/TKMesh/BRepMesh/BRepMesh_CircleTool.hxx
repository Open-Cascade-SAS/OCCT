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

#ifndef _BRepMesh_CircleTool_HeaderFile
#define _BRepMesh_CircleTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <Standard_Real.hxx>
#include <BRepMesh_CircleInspector.hxx>
#include <gp_XY.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <IMeshData_Types.hxx>
#include <NCollection_Array1.hxx>

class gp_Circ2d;

//! Create sort and destroy the circles used in triangulation. <br>
class BRepMesh_CircleTool
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor.
  //! @param theAllocator memory allocator to be used by internal structures.
  Standard_EXPORT BRepMesh_CircleTool(const Handle(NCollection_IncAllocator)& theAllocator);

  //! Constructor.
  //! @param theReservedSize size to be reserved for vector of circles.
  //! @param theAllocator memory allocator to be used by internal structures.
  Standard_EXPORT BRepMesh_CircleTool(const Standard_Integer                  theReservedSize,
                                      const Handle(NCollection_IncAllocator)& theAllocator);

  //! Initializes the tool.
  //! @param theReservedSize size to be reserved for vector of circles.
  void Init(const Standard_Integer /*theReservedSize*/) { myTolerance = Precision::PConfusion(); }

  //! Sets new size for cell filter.
  //! @param theSize cell size to be set for X and Y dimensions.
  void SetCellSize(const Standard_Real theSize) { myCellFilter.Reset(theSize, myAllocator); }

  //! Sets new size for cell filter.
  //! @param theSizeX cell size to be set for X dimension.
  //! @param theSizeY cell size to be set for Y dimension.
  void SetCellSize(const Standard_Real theSizeX, const Standard_Real theSizeY)
  {
    Standard_Real                     aCellSizeC[2] = {theSizeX, theSizeY};
    NCollection_Array1<Standard_Real> aCellSize(aCellSizeC[0], 1, 2);
    myCellFilter.Reset(aCellSize, myAllocator);
  }

  //! Sets limits of inspection area.
  //! @param theMin bottom left corner of inspection area.
  //! @param theMax top right corner of inspection area.
  void SetMinMaxSize(const gp_XY& theMin, const gp_XY& theMax)
  {
    myFaceMin = theMin;
    myFaceMax = theMax;
  }

  //! Returns true if cell filter contains no circle.
  Standard_Boolean IsEmpty() const { return mySelector.Circles().IsEmpty(); }

  //! Binds the circle to the tool.
  //! @param theIndex index a circle should be bound with.
  //! @param theCircle circle to be bound.
  Standard_EXPORT void Bind(const Standard_Integer theIndex, const gp_Circ2d& theCircle);

  //! Computes circle on three points.
  //! @param thePoint1 first point.
  //! @param thePoint2 second point.
  //! @param thePoint3 third point.
  //! @param[out] theLocation center of computed circle.
  //! @param[out] theRadius radius of computed circle.
  //! @return FALSE in case of impossibility to build a circle
  //! on the given points, TRUE elsewhere.
  Standard_EXPORT static Standard_Boolean MakeCircle(const gp_XY&   thePoint1,
                                                     const gp_XY&   thePoint2,
                                                     const gp_XY&   thePoint3,
                                                     gp_XY&         theLocation,
                                                     Standard_Real& theRadius);

  //! Computes circle on three points and bind it to the tool.
  //! @param theIndex index a circle should be bound with.
  //! @param thePoint1 first point.
  //! @param thePoint2 second point.
  //! @param thePoint3 third point.
  //! @return FALSE in case of impossibility to build a circle
  //! on the given points, TRUE elsewhere.
  Standard_EXPORT Standard_Boolean Bind(const Standard_Integer theIndex,
                                        const gp_XY&           thePoint1,
                                        const gp_XY&           thePoint2,
                                        const gp_XY&           thePoint3);

  //! Binds implicit zero circle.
  //! @param theIndex index a zero circle should be bound with.
  Standard_EXPORT void MocBind(const Standard_Integer theIndex);

  //! Deletes a circle from the tool.
  //! @param theIndex index of a circle to be removed.
  Standard_EXPORT void Delete(const Standard_Integer theIndex);

  //! Select the circles shot by the given point.
  //! @param thePoint bullet point.
  Standard_EXPORT IMeshData::ListOfInteger& Select(const gp_XY& thePoint);

private:
  //! Creates circle with the given parameters and binds it to the tool.
  //! @param theIndex index a circle should be bound with.
  //! @param theLocation location of a circle.
  //! @param theRadius radius of a circle.
  void bind(const Standard_Integer theIndex,
            const gp_XY&           theLocation,
            const Standard_Real    theRadius);

private:
  Standard_Real                    myTolerance;
  Handle(NCollection_IncAllocator) myAllocator;
  IMeshData::CircleCellFilter      myCellFilter;
  BRepMesh_CircleInspector         mySelector;
  gp_XY                            myFaceMax;
  gp_XY                            myFaceMin;
};

#endif
