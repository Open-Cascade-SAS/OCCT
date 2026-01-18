// Created by: Peter KURNEV
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

#ifndef BOPDS_Interf_HeaderFile
#define BOPDS_Interf_HeaderFile

#include <IntTools_CommonPrt.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_Vector.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_Point.hxx>

#include <optional>

/**
 * The class BOPDS_Interf stores the information about
 * the interference between two shapes.
 * The class BOPDS_Interf is root class
 *
 */
//=================================================================================================

class BOPDS_Interf
{
public:
  //
  /**
   * Sets the indices of interferred shapes
   * @param theIndex1
   *   index of the first shape
   * @param theIndex2
   *   index of the second shape
   */
  void SetIndices(const int theIndex1, const int theIndex2)
  {
    myIndex1 = theIndex1;
    myIndex2 = theIndex2;
  }

  //
  /**
   * Returns the indices of interferred shapes
   * @param theIndex1
   *   index of the first shape
   * @param theIndex2
   *   index of the second shape
   */
  void Indices(int& theIndex1, int& theIndex2) const
  {
    theIndex1 = myIndex1;
    theIndex2 = myIndex2;
  }

  //
  /**
   * Sets the index of the first interferred shape
   * @param theIndex
   *   index of the first shape
   */
  void SetIndex1(const int theIndex) { myIndex1 = theIndex; }

  //
  /**
   * Sets the index of the second interferred shape
   * @param theIndex
   *   index of the second shape
   */
  void SetIndex2(const int theIndex) { myIndex2 = theIndex; }

  //
  /**
   * Returns the index of the first interferred shape
   * @return
   *   index of the first shape
   */
  int Index1() const { return myIndex1; }

  //
  /**
   * Returns the index of the second interferred shape
   * @return
   *   index of the second shape
   */
  int Index2() const { return myIndex2; }

  //
  /**
   * Returns the index of that are opposite to the given index
   * @param theI
   *   the index
   * @return
   *   index of opposite shape
   */
  int OppositeIndex(const int theI) const
  {
    if (theI == myIndex1)
    {
      return myIndex2;
    }
    else if (theI == myIndex2)
    {
      return myIndex1;
    }
    else
    {
      return -1;
    }
  }

  //
  /**
   * Returns true if the interference contains given index
   * @param theIndex
   *   the index
   * @return
   *   true if the interference contains given index
   */
  bool Contains(const int theIndex) const { return (myIndex1 == theIndex || myIndex2 == theIndex); }

  //
  /**
   * Sets the index of new shape
   * @param theIndex
   *   the index
   */
  void SetIndexNew(const int theIndex) { myIndexNew = theIndex; }

  //
  //
  /**
   * Returns the index of new shape
   * @return theIndex
   *   the index of new shape
   */
  int IndexNew() const { return myIndexNew; }

  //
  /**
   * Returns true if the interference has index of new shape
   * that is equal to the given index
   * @param theIndex
   *   the index
   * @return true if the interference has index of new shape
   * that is equal to the given index
   */
  bool HasIndexNew(int& theIndex) const
  {
    theIndex = myIndexNew;
    return (myIndexNew >= 0);
  }

  //
  /**
   * Returns true if the interference has index of new shape
   *   the index
   * @return true if the interference has index of new shape
   */
  bool HasIndexNew() const { return myIndexNew != -1; }

  //! Returns the index of new shape.
  //! If the index is not set, returns std::nullopt.
  std::optional<int> GetIndexNew() const
  {
    return myIndexNew != -1 ? std::optional<int>(myIndexNew) : std::nullopt;
  }

  //
protected:
  BOPDS_Interf()
      : myIndex1(-1),
        myIndex2(-1),
        myIndexNew(-1),
        myAllocator(NCollection_BaseAllocator::CommonBaseAllocator())
  {
  }

  //
  BOPDS_Interf(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : myIndex1(-1),
        myIndex2(-1),
        myIndexNew(-1),
        myAllocator(theAllocator)
  {
  }

  //
  virtual ~BOPDS_Interf() = default;

protected:
  int                                    myIndex1;
  int                                    myIndex2;
  int                                    myIndexNew;
  occ::handle<NCollection_BaseAllocator> myAllocator;
};

/**
 * The class BOPDS_InterfVV stores the information about
 * the interference of the type vertex/vertex.
 */
//=================================================================================================

class BOPDS_InterfVV : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfVV()

    = default;

  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfVV(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator)
  {
  }

  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfVV() override = default;

  //
};

/**
 * The class BOPDS_InterfVE stores the information about
 * the interference of the type vertex/edge.
 */
//=================================================================================================

class BOPDS_InterfVE : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfVE()
      : myParameter(0.)
  {
  }

  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfVE(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator),
        myParameter(0.)
  {
  }

  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfVE() override = default;

  //
  /**
   *  Modifier
   * Sets the value of parameter
   * of the point of the vertex
   * on the curve of the edge
   * @param theT
   *   value of parameter
   */
  void SetParameter(const double theT) { myParameter = theT; }

  //
  /**
   *  Selector
   * Returrns the value of parameter
   * of the point of the vertex
   * on the curve of the edge
   * @return
   *   value of parameter
   */
  double Parameter() const { return myParameter; }

protected:
  double myParameter;
};

/**
 * The class BOPDS_InterfVF stores the information about
 * the interference of the type vertex/face
 */
//=================================================================================================

class BOPDS_InterfVF : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfVF()
      : myU(0.),
        myV(0.)
  {
  }

  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfVF(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator),
        myU(0.),
        myV(0.)
  {
  }

  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfVF() override = default;

  //
  /**
   * Modifier
   * Sets the value of parameters
   * of the point of the vertex
   * on the surface of of the face
   * @param theU
   *   value of U parameter
   * @param theV
   *   value of U parameter
   */
  void SetUV(const double theU, const double theV)
  {
    myU = theU;
    myV = theV;
  }

  //
  /**
   * Selector
   * Returns the value of parameters
   * of the point of the vertex
   * on the surface of of the face
   * @param theU
   *   value of U parameter
   * @param theV
   *   value of U parameter
   */
  void UV(double& theU, double& theV) const
  {
    theU = myU;
    theV = myV;
  }

protected:
  double myU;
  double myV;
};

/**
 * The class BOPDS_InterfEE stores the information about
 * the interference of the type edge/edge.
 */
//=================================================================================================

class BOPDS_InterfEE : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfEE()

    = default;

  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfEE(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator)
  {
  }

  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfEE() override = default;

  //
  /**
   *  Modifier
   * Sets the info of common part
   * @param theCP
   *   common part
   */
  void SetCommonPart(const IntTools_CommonPrt& theCP) { myCommonPart = theCP; }

  //
  /**
   *  Selector
   * Returns the info of common part
   * @return
   *   common part
   */
  const IntTools_CommonPrt& CommonPart() const { return myCommonPart; }

protected:
  IntTools_CommonPrt myCommonPart;
};

/**
 * The class BOPDS_InterfEF stores the information about
 * the interference of the type edge/face.
 */
//=================================================================================================

class BOPDS_InterfEF : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfEF()

    = default;

  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfEF(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator)
  {
  }

  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfEF() override = default;

  //
  /**
   * Modifier
   * Sets the info of common part
   * @param theCP
   *   common part
   */
  void SetCommonPart(const IntTools_CommonPrt& theCP) { myCommonPart = theCP; }

  //
  /**
   *  Selector
   * Returns the info of common part
   * @return
   *   common part
   */
  const IntTools_CommonPrt& CommonPart() const { return myCommonPart; }

  //
protected:
  IntTools_CommonPrt myCommonPart;
}
/**
 * The class BOPDS_InterfFF stores the information about
 * the interference of the type face/face.
 */
;

//=================================================================================================

class BOPDS_InterfFF : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfFF()
      : myTangentFaces(false),
        myCurves(0, myAllocator),
        myPoints(0, myAllocator)
  {
  }

  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfFF() override = default;

  //
  /**
   * Initializer
   * @param theNbCurves
   *   number of intersection curves
   * @param theNbPoints
   *   number of intersection points
   */

  void Init(const int theNbCurves, const int theNbPoints)
  {
    if (theNbCurves > 0)
    {
      myCurves.SetIncrement(theNbCurves);
    }
    if (theNbPoints > 0)
    {
      myPoints.SetIncrement(theNbPoints);
    }
  }

  /**
   * Modifier
   * Sets the flag of whether the faces are tangent
   * @param theFlag
   *   the flag
   */
  void SetTangentFaces(const bool theFlag) { myTangentFaces = theFlag; }

  /**
   * Selector
   * Returns the flag whether the faces are tangent
   * @return
   *   the flag
   */
  bool TangentFaces() const { return myTangentFaces; }

  //
  /**
   * Selector
   * Returns the intersection curves
   * @return
   *   intersection curves
   */
  const NCollection_Vector<BOPDS_Curve>& Curves() const { return myCurves; }

  //
  /**
   * Selector/Modifier
   * Returns the intersection curves
   * @return
   *   intersection curves
   */
  NCollection_Vector<BOPDS_Curve>& ChangeCurves() { return myCurves; }

  //
  /**
   * Selector
   * Returns the intersection points
   * @return
   *   intersection points
   */
  const NCollection_Vector<BOPDS_Point>& Points() const { return myPoints; }

  //
  /**
   * Selector/Modifier
   * Returns the intersection points
   * @return
   *   intersection points
   */
  NCollection_Vector<BOPDS_Point>& ChangePoints() { return myPoints; }

  //
protected:
  bool                            myTangentFaces;
  NCollection_Vector<BOPDS_Curve> myCurves;
  NCollection_Vector<BOPDS_Point> myPoints;
};

/**
 * The class BOPDS_InterfVZ stores the information about
 * the interference of the type vertex/solid.
 */
//=================================================================================================

class BOPDS_InterfVZ : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfVZ() = default;
  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfVZ(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator) {};
  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfVZ() override = default;
  //
};

/**
 * The class BOPDS_InterfEZ stores the information about
 * the interference of the type edge/solid.
 */
//=================================================================================================

class BOPDS_InterfEZ : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfEZ() = default;
  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfEZ(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator) {};
  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfEZ() override = default;
  //
};

/**
 * The class BOPDS_InterfFZ stores the information about
 * the interference of the type face/solid.
 */
//=================================================================================================

class BOPDS_InterfFZ : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfFZ() = default;
  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfFZ(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator) {};
  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfFZ() override = default;
  //
};

/**
 * The class BOPDS_InterfZZ stores the information about
 * the interference of the type solid/solid.
 */
//=================================================================================================

class BOPDS_InterfZZ : public BOPDS_Interf
{
public:
  //
  /**
   *  Constructor
   */
  BOPDS_InterfZZ() = default;
  //
  /**
   *  Constructor
   * @param theAllocator
   *   allocator to manage the memory
   */
  BOPDS_InterfZZ(const occ::handle<NCollection_BaseAllocator>& theAllocator)
      : BOPDS_Interf(theAllocator) {};
  //
  /**
   *  Destructor
   */
  ~BOPDS_InterfZZ() override = default;
  //
};

#endif
