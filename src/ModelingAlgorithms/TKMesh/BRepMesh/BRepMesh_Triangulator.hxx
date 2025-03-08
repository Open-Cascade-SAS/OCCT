// Created: 2017-12-28
//
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BRepMesh_Triangulator_HeaderFile
#define _BRepMesh_Triangulator_HeaderFile

#include <gp_Pln.hxx>
#include <Poly_Triangulation.hxx>
#include <IMeshData_Types.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>

class Message_Messenger;

//! Auxiliary tool to generate triangulation
class BRepMesh_Triangulator
{
public:
  DEFINE_STANDARD_ALLOC

  //! Performs conversion of the given list of triangles to Poly_Triangulation.
  Standard_EXPORT static Handle(Poly_Triangulation) ToPolyTriangulation(
    const TColgp_Array1OfPnt&              theNodes,
    const NCollection_List<Poly_Triangle>& thePolyTriangles);

public:
  //! Constructor. Initialized tool by the given parameters.
  Standard_EXPORT BRepMesh_Triangulator(const NCollection_Vector<gp_XYZ>&                  theXYZs,
                                        const NCollection_List<TColStd_SequenceOfInteger>& theWires,
                                        const gp_Dir&                                      theNorm);

  //! Performs triangulation of source wires and stores triangles the output list.
  Standard_EXPORT Standard_Boolean Perform(NCollection_List<Poly_Triangle>& thePolyTriangles);

  //! Set messenger for output information
  //! without this Message::DefaultMessenger() will be used
  void SetMessenger(const Handle(Message_Messenger)& theMess) { myMess = theMess; }

  BRepMesh_Triangulator& operator=(const BRepMesh_Triangulator& theOther);

private:
  // auxiliary for makeTrianglesUsingBRepMesh
  void addTriange34(const TColStd_SequenceOfInteger& theW,
                    NCollection_List<Poly_Triangle>& thePolyTriangles);

  // auxiliary for addTriange34
  Standard_Boolean checkCondition(const int (&theNodes)[4],
                                  const TColStd_SequenceOfInteger& theWire);

  // performs initialization of mesh data structure.
  Standard_Boolean prepareMeshStructure();

  // auxiliary for triangulation
  Standard_Boolean triangulate(NCollection_List<Poly_Triangle>& thePolyTriangles);

private:
  const NCollection_Vector<gp_XYZ>&                  myXYZs;
  const NCollection_List<TColStd_SequenceOfInteger>& myWires;
  gp_Pln                                             myPlane;
  Handle(Message_Messenger)                          myMess;

  Handle(BRepMesh_DataStructureOfDelaun) myMeshStructure;
  TColStd_DataMapOfIntegerInteger        myTmpMap;
  Handle(IMeshData::VectorOfInteger)     myIndices;
};

#endif // _BRepMesh_Triangulator_HeaderFile
