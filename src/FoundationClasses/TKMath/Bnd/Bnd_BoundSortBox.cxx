// Created on: 1992-11-24
// Created by: Didier PIFFAULT
// Copyright (c) 1992-1999 Matra Datavision
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

#include <Bnd_BoundSortBox.hxx>

#include <gp_Pln.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NullValue.hxx>

#include <unordered_map>

//==================================================================================================
//--  lbr le 27 fev 97
//--
//--
//--      Initialisation:  Bounding box BE
//--                       List of boxes Bi (Bi in BE)
//--
//--      Compare(b) returns the list of Boxes Bi touched by b
//--
//--
//--      General principle:
//--
//--       1) Discretize box BE into N*N*N voxels
//--          Each box Bi touches a certain number of voxels
//--          Bi touches { Vijk with i0<=i<=i1 ...  k0<=k<=k1 }
//--       2) Project on each axis X,Y,Z boxes Bi
//--          to get the following structures :
//--
//--          Example :
//--
//--             box i1 touches voxels { Vijk  with 1<=i<=N-1  ... }
//--             box i2 touches voxels { Vijk  with 2<=i<=3  ... }
//--             box i3 touches voxels { Vijk  with 1<=i<=2  ... }
//--
//--
//--         X[.] |  1    2    3    4   ....   N-1   N
//--         -----|-------------------------------------
//--              |  i3   i1   i1   i1          i1
//--              |       i2   i2
//--              |       i3
//--
//--
//--         Produce the same thing for axes Y and Z
//--
//--         Obtain 3 tables (X,Y,Z) of lists of integers (indexes of boxes)
//--
//--       3) To find boxes contacting with box bt
//--             a) Find voxels touched by bt -> i0bt,i1bt ... k0bt,k1bt
//--             b) Find in list Z the boxes present in cases Z[k0bt ... k1bt]
//--             c) Find among these boxes the ones present in cases Y[j0bt ... j1bt]
//--             d) and the result is the intersection of the previous result with X[i0bt ... i1bt]
//--
//--
//--  Rejection of a higher level.
//--
//--       *) Preserve a table representation of bit of voxels of space BE
//--          that contains at least one box Bi.
//--       *) While box bt is tested, it is checked if this box includes in
//--          the table of bit at least one occupied voxel.
//--          If the occupied voxel is touched : no rejection
//--          Otherwise return
//--
//--      **) Another rejection was adopted. It consists in trying to locate in
//--          the above structures (tables X,Y,Z and  table of Bits) a box Bi which is greater than
//--          the bounding box BE.
//--
//--          The indices of these boxes are located in table myLargeBoxes, and these
//            boxes are compared systematically with bt.
//--
//--   Note : tables C replace here HArray1OfListOfInteger and other
//--          structures that are sufficient for data adding but slow for reading.
//--
//--          Here the data is added at start (Initialize and SortBoxes) and later,
//--          it takes much time to parse the tables. The slowly written, but quickly read
//--          structures are thus better.
//--
//==================================================================================================

//==================================================================================================

// This function determines the resolution of the voxel grid based on the number of boxes.
// The resolution is a power of 2 as in the original code. Current implementation supports
// resolutions any value of resolution.
// @param theBoxesCount The number of boxes to be sorted.
// @return The resolution of the voxel grid.
static Standard_Integer getBnd_VoxelGridResolution(const Standard_Integer theBoxesCount)
{
  if (theBoxesCount > 40000)
  {
    return 128;
  }
  if (theBoxesCount > 10000)
  {
    return 64;
  }
  if (theBoxesCount > 1000)
  {
    return 32;
  }
  if (theBoxesCount > 100)
  {
    return 16;
  }
  return 8;
}

//==================================================================================================

// This class represents a 3D voxel grid used for spatial partitioning of boxes.
// It provides methods to add boxes, check occupancy, and retrieve slices of the grid.
// It can be populated with boxes by using the AddBox method, which takes a box index and its
// voxel coordinates.
class Bnd_VoxelGrid : public Standard_Transient
{
public:
  DEFINE_STANDARD_RTTIEXT(Bnd_VoxelGrid, Standard_Transient)

public:
  using VectorInt = std::vector<Standard_Integer, NCollection_Allocator<Standard_Integer>>;

private:
  using VectorBool = std::vector<bool, NCollection_Allocator<bool>>;
  using VoxelMap =
    std::unordered_map<Standard_Integer,
                       VectorInt,
                       std::hash<Standard_Integer>,
                       std::equal_to<Standard_Integer>,
                       NCollection_Allocator<std::pair<const Standard_Integer, VectorInt>>>;

public:
  // Constructor that initializes the Bnd_VoxelGrid object with a given size.
  // @param theSize The size of the grid in each dimension.
  Bnd_VoxelGrid(const Standard_Integer theResolution);

  // Adds a box to the voxel grid.
  // The box is defined by its minimum and maximum voxel coordinates.
  // @param theBoxIndex The index of the box to be added.
  // @param theVoxelBox An array of 6 integers representing the minimum and maximum voxel
  //                    coordinates of the box in the order: [minX, minY, minZ, maxX, maxY, maxZ].
  void AddBox(const Standard_Integer                 theBoxIndex,
              const std::array<Standard_Integer, 6>& theVoxelBox);

  // Returns the list of box indices that occupy the specified voxel index in the X direction.
  // In other words, the vector will contain the indices of boxes that occupy the voxels with
  // indices in the range
  // [[theVoxelIndex, 0, 0], [theVoxelIndex, 0, 1], ..., [theVoxelIndex, resolution, resolution]].
  // @param theVoxelIndex The index of the voxel in the X direction.
  // @return A pointer to a vector of integers representing the box indices.
  //         The vector can be null if no boxes occupy the voxel.
  const VectorInt* GetSliceX(const Standard_Integer theVoxelIndex) const;

  // Returns the list of box indices that occupy the specified voxel index in the Y direction.
  // In other words, the vector will contain the indices of boxes that occupy the voxels with
  // indices in the range
  // [[0, theVoxelIndex, 0], [1, theVoxelIndex, 0], ..., [resolution, theVoxelIndex, resolution]].
  // @param theVoxelIndex The index of the voxel in the Y direction.
  // @return A pointer to a vector of integers representing the box indices.
  //         The vector can be null if no boxes occupy the voxel.
  const VectorInt* GetSliceY(const Standard_Integer theVoxelIndex) const;

  // Returns the list of box indices that occupy the specified voxel index in the Z direction.
  // In other words, the vector will contain the indices of boxes that occupy the voxels with
  // indices in the range
  // [[0, 0, theVoxelIndex], [1, 0, theVoxelIndex], ..., [resolution, resolution, theVoxelIndex]].
  // @param theVoxelIndex The index of the voxel in the Z direction.
  // @return A pointer to a vector of integers representing the box indices.
  //         The vector can be null if no boxes occupy the voxel.
  const VectorInt* GetSliceZ(const Standard_Integer theVoxelIndex) const;

  // Returns true if the specified voxel box is occupied by any box previously added to the grid
  // via the AddBox method.
  // @param theVoxelBox An array of 6 integers representing the minimum and maximum voxel
  //                    coordinates of the box in the order: [minX, minY, minZ, maxX, maxY, maxZ].
  // @return True if the voxel box is occupied, false otherwise.
  bool IsOccupied(const std::array<Standard_Integer, 6>& theVoxelBox) const;

private:
  // Appends a slice of the voxel grid in the X direction.
  // This method is used to store the indices of boxes that occupy a specific voxel index in the X
  // direction.
  // @param theVoxelIndexMin The minimum voxel index in the X direction.
  // @param theVoxelIndexMax The maximum voxel index in the X direction.
  // @param theBoxIndex The index of the box to be added.
  void AppendSliceX(const Standard_Integer theVoxelIndexMin,
                    const Standard_Integer theVoxelIndexMax,
                    const Standard_Integer theBoxIndex);

  // Appends a slice of the voxel grid in the Y direction.
  // This method is used to store the indices of boxes that occupy a specific voxel index in the Y
  // direction.
  // @param theVoxelIndexMin The minimum voxel index in the Y direction.
  // @param theVoxelIndexMax The maximum voxel index in the Y direction.
  // @param theBoxIndex The index of the box to be added.
  void AppendSliceY(const Standard_Integer theVoxelIndexMin,
                    const Standard_Integer theVoxelIndexMax,
                    const Standard_Integer theBoxIndex);

  // Appends a slice of the voxel grid in the Z direction.
  // This method is used to store the indices of boxes that occupy a specific voxel index in the Z
  // direction.
  // @param theVoxelIndexMin The minimum voxel index in the Z direction.
  // @param theVoxelIndexMax The maximum voxel index in the Z direction.
  // @param theBoxIndex The index of the box to be added.
  void AppendSliceZ(const Standard_Integer theVoxelIndexMin,
                    const Standard_Integer theVoxelIndexMax,
                    const Standard_Integer theBoxIndex);

  // Marks the voxel at the specified coordinates as occupied.
  // @param theX The x-coordinate of the voxel to mark.
  // @param theY The y-coordinate of the voxel to mark.
  // @param theZ The z-coordinate of the voxel to mark.
  inline void SetOccupied(const Standard_Integer theX,
                          const Standard_Integer theY,
                          const Standard_Integer theZ)
  {
    myVoxelBits[toIndex(theX, theY, theZ)] = true;
  }

  // Returns true if the voxel at the specified coordinates is marked as occupied.
  // @param theX The x-coordinate of the voxel to check.
  // @param theY The y-coordinate of the voxel to check.
  // @param theZ The z-coordinate of the voxel to check.
  // @return True if the voxel is occupied, false otherwise.
  inline bool IsOccupied(const Standard_Integer theX,
                         const Standard_Integer theY,
                         const Standard_Integer theZ) const
  {
    return myVoxelBits[toIndex(theX, theY, theZ)];
  }

  // Converts the 3D grid coordinates to a 1D index.
  // The index is calculated based on the formula:
  // index = x * dimensionSize^2 + y * dimensionSize + z
  // This formula maps the 3D coordinates to a unique index in the 1D array.
  // @param theX The x-coordinate in the grid.
  // @param theY The y-coordinate in the grid.
  // @param theZ The z-coordinate in the grid.
  // @return The calculated index in the 1D array.
  inline Standard_Integer toIndex(const Standard_Integer theX,
                                  const Standard_Integer theY,
                                  const Standard_Integer theZ) const
  {
    return theX * myResolution * myResolution + theY * myResolution + theZ;
  }

private:
  const Standard_Integer myResolution; //< Number of voxels in one dimension of the grid.
  VectorBool myVoxelBits; //< Array of bits representing the occupancy of voxels in the grid.
  VoxelMap   mySlicesX;   //< Map of voxel indices to lists of box indices for the X-axis.
  VoxelMap   mySlicesY;   //< Map of voxel indices to lists of box indices for the Y-axis.
  VoxelMap   mySlicesZ;   //< Map of voxel indices to lists of box indices for the Z-axis.
};

IMPLEMENT_STANDARD_RTTIEXT(Bnd_VoxelGrid, Standard_Transient)

//==================================================================================================

Bnd_VoxelGrid::Bnd_VoxelGrid(const Standard_Integer theResolution)
    : myResolution(theResolution),
      myVoxelBits(theResolution * theResolution * theResolution),
      mySlicesX(),
      mySlicesY(),
      mySlicesZ()
{
}

//==================================================================================================

void Bnd_VoxelGrid::AddBox(const Standard_Integer                 theBoxIndex,
                           const std::array<Standard_Integer, 6>& theVoxelBox)
{
  const Standard_Integer aMinVoxelX = theVoxelBox[0];
  const Standard_Integer aMinVoxelY = theVoxelBox[1];
  const Standard_Integer aMinVoxelZ = theVoxelBox[2];
  const Standard_Integer aMaxVoxelX = theVoxelBox[3];
  const Standard_Integer aMaxVoxelY = theVoxelBox[4];
  const Standard_Integer aMaxVoxelZ = theVoxelBox[5];

  AppendSliceX(aMinVoxelX, aMaxVoxelX, theBoxIndex);
  AppendSliceY(aMinVoxelY, aMaxVoxelY, theBoxIndex);
  AppendSliceZ(aMinVoxelZ, aMaxVoxelZ, theBoxIndex);
  // Fill table with bits.
  for (Standard_Integer aVoxelX = aMinVoxelX; aVoxelX <= aMaxVoxelX; ++aVoxelX)
  {
    for (Standard_Integer aVoxelY = aMinVoxelY; aVoxelY <= aMaxVoxelY; ++aVoxelY)
    {
      for (Standard_Integer aVoxelZ = aMinVoxelZ; aVoxelZ <= aMaxVoxelZ; ++aVoxelZ)
      {
        SetOccupied(aVoxelX, aVoxelY, aVoxelZ);
      }
    }
  }
}

//==================================================================================================

const Bnd_VoxelGrid::VectorInt* Bnd_VoxelGrid::GetSliceX(const Standard_Integer theVoxelIndex) const
{
  auto it = mySlicesX.find(theVoxelIndex);
  return it != mySlicesX.end() ? &it->second : nullptr;
}

//==================================================================================================

const Bnd_VoxelGrid::VectorInt* Bnd_VoxelGrid::GetSliceY(const Standard_Integer theVoxelIndex) const
{
  auto it = mySlicesY.find(theVoxelIndex);
  return it != mySlicesY.end() ? &it->second : nullptr;
}

//==================================================================================================

const Bnd_VoxelGrid::VectorInt* Bnd_VoxelGrid::GetSliceZ(const Standard_Integer theVoxelIndex) const
{
  auto it = mySlicesZ.find(theVoxelIndex);
  return it != mySlicesZ.end() ? &it->second : nullptr;
}

//==================================================================================================

bool Bnd_VoxelGrid::IsOccupied(const std::array<Standard_Integer, 6>& theVoxelBox) const
{
  const Standard_Integer aMinVoxelX = theVoxelBox[0];
  const Standard_Integer aMinVoxelY = theVoxelBox[1];
  const Standard_Integer aMinVoxelZ = theVoxelBox[2];
  const Standard_Integer aMaxVoxelX = theVoxelBox[3];
  const Standard_Integer aMaxVoxelY = theVoxelBox[4];
  const Standard_Integer aMaxVoxelZ = theVoxelBox[5];

  for (Standard_Integer aVoxelX = aMinVoxelX; aVoxelX <= aMaxVoxelX; ++aVoxelX)
  {
    for (Standard_Integer aVoxelY = aMinVoxelY; aVoxelY <= aMaxVoxelY; ++aVoxelY)
    {
      for (Standard_Integer aVoxelZ = aMinVoxelZ; aVoxelZ <= aMaxVoxelZ; ++aVoxelZ)
      {
        if (IsOccupied(aVoxelX, aVoxelY, aVoxelZ))
        {
          return true; // Found an occupied voxel.
        }
      }
    }
  }
  return false; // No occupied voxels found.
}

//==================================================================================================

void Bnd_VoxelGrid::AppendSliceX(const Standard_Integer theVoxelIndexMin,
                                 const Standard_Integer theVoxelIndexMax,
                                 const Standard_Integer theBoxIndex)
{
  for (Standard_Integer i = theVoxelIndexMin; i <= theVoxelIndexMax; ++i)
  {
    mySlicesX[i].push_back(theBoxIndex);
  }
}

//==================================================================================================

void Bnd_VoxelGrid::AppendSliceY(const Standard_Integer theVoxelIndexMin,
                                 const Standard_Integer theVoxelIndexMax,
                                 const Standard_Integer theBoxIndex)
{
  for (Standard_Integer i = theVoxelIndexMin; i <= theVoxelIndexMax; ++i)
  {
    mySlicesY[i].push_back(theBoxIndex);
  }
}

//==================================================================================================

void Bnd_VoxelGrid::AppendSliceZ(const Standard_Integer theVoxelIndexMin,
                                 const Standard_Integer theVoxelIndexMax,
                                 const Standard_Integer theBoxIndex)
{
  for (Standard_Integer i = theVoxelIndexMin; i <= theVoxelIndexMax; ++i)
  {
    mySlicesZ[i].push_back(theBoxIndex);
  }
}

//==================================================================================================

Bnd_BoundSortBox::Bnd_BoundSortBox()
    : myEnclosingBox(),
      myBoxes(nullptr),
      myCoeffX(0.),
      myCoeffY(0.),
      myCoeffZ(0.),
      myResolution(0),
      myLastResult(),
      myLargeBoxes(),
      myVoxelGrid(nullptr)
{
}

//==================================================================================================

void Bnd_BoundSortBox::Initialize(const Handle(Bnd_HArray1OfBox)& theSetOfBoxes)
{
  myBoxes = theSetOfBoxes;

  for (Standard_Integer aBoxIndex = myBoxes->Lower(); aBoxIndex <= myBoxes->Upper(); ++aBoxIndex)
  {
    const Bnd_Box& aBox = myBoxes->Value(aBoxIndex);
    if (!aBox.IsVoid())
    {
      myEnclosingBox.Add(aBox);
    }
  }

  myResolution = getBnd_VoxelGridResolution(myBoxes->Size());

  if (myEnclosingBox.IsVoid())
  {
    return;
  }

  calculateCoefficients();
  resetVoxelGrid();

  sortBoxes();
}

//==================================================================================================

void Bnd_BoundSortBox::Initialize(const Bnd_Box&                  theEnclosingBox,
                                  const Handle(Bnd_HArray1OfBox)& theSetOfBoxes)
{
  myBoxes        = theSetOfBoxes;
  myEnclosingBox = theEnclosingBox;
  myResolution   = getBnd_VoxelGridResolution(myBoxes->Size());

  if (myEnclosingBox.IsVoid())
  {
    return;
  }

  calculateCoefficients();
  resetVoxelGrid();

  sortBoxes();
}

//==================================================================================================

void Bnd_BoundSortBox::Initialize(const Bnd_Box& theEnclosingBox, const Standard_Integer theNbBoxes)
{
  Standard_NullValue_Raise_if(theNbBoxes <= 0, "Unexpected: theNbBoxes <= 0");
  myBoxes = new Bnd_HArray1OfBox(1, theNbBoxes);
  //***>>> JCD - 04.08.2000 - Array initialization is missing...
  Bnd_Box emptyBox;
  myBoxes->Init(emptyBox);
  //***<<< JCD - End
  myEnclosingBox = theEnclosingBox;
  myResolution   = getBnd_VoxelGridResolution(theNbBoxes);

  if (myEnclosingBox.IsVoid())
  {
    return;
  }

  calculateCoefficients();
  resetVoxelGrid();
}

//==================================================================================================

void Bnd_BoundSortBox::Add(const Bnd_Box& theBox, const Standard_Integer theIndex)
{
  Standard_MultiplyDefined_Raise_if(!(myBoxes->Value(theIndex).IsVoid()),
                                    " This box is already defined !");
  if (theBox.IsVoid())
  {
    return;
  }

  myBoxes->SetValue(theIndex, theBox);

  addBox(theBox, theIndex);
}

//==================================================================================================

const TColStd_ListOfInteger& Bnd_BoundSortBox::Compare(const Bnd_Box& theBox)

{
  myLastResult.Clear();

  if (theBox.IsVoid() || theBox.IsOut(myEnclosingBox))
  {
    return myLastResult;
  }

  // Processing the large boxes. These boxes are not voxelized and are checked
  // against the given box directly which is presumably faster.
  for (auto& aBoxIndex : myLargeBoxes)
  {
    const Bnd_Box& aBox = myBoxes->Value(aBoxIndex);
    if (!aBox.IsOut(theBox))
    {
      myLastResult.Append(aBoxIndex);
    }
  }

  // Obtaining the box voxel coordinates.
  auto&& [aMinVoxelX, aMinVoxelY, aMinVoxelZ, aMaxVoxelX, aMaxVoxelY, aMaxVoxelZ] =
    getBoundingVoxels(theBox);

  // Check if the given box has any intersections within the voxel map.
  // If not, there is nothing to check.
  if (!myVoxelGrid->IsOccupied(
        {aMinVoxelX, aMinVoxelY, aMinVoxelZ, aMaxVoxelX, aMaxVoxelY, aMaxVoxelZ}))
  {
    return myLastResult;
  }

  // This vector is a structure to store the indices of boxes that occupy the same voxels
  // as the given box. Index of element in vector corresponds to the index of the box in
  // the array of boxes. The value of the element is a bit mask indicating which axes
  // the box occupies. The first bit (0b01) indicates the X-axis, and the second bit (0b10)
  // indicates the Y-axis. If both bits are set (0b11), and the box also occupies the Z-axis,
  // we check for actual intersection with the given box and add it to the result if
  // intersection occurs.
  std::vector<uint8_t> aResultIndices(myBoxes->Upper() + 1, 0);
  constexpr uint8_t    anOccupiedX  = 0b01;
  constexpr uint8_t    anOccupiedY  = 0b10;
  constexpr uint8_t    anOccupiedXY = 0b11;

  // Checking the voxels along X-axis.
  for (Standard_Integer aVoxelX = aMinVoxelX; aVoxelX <= aMaxVoxelX; ++aVoxelX)
  {
    const Bnd_VoxelGrid::VectorInt* aBoxIndices = myVoxelGrid->GetSliceX(aVoxelX);
    if (aBoxIndices == nullptr)
    {
      continue;
    }
    for (const auto& aBoxIndex : *aBoxIndices)
    {
      // Set the first bit (0b01) to indicate that the box occupies the X-axis.
      aResultIndices[aBoxIndex] |= anOccupiedX;
    }
  }

  // Checking the voxels along Y-axis.
  for (Standard_Integer aVoxelY = aMinVoxelY; aVoxelY <= aMaxVoxelY; ++aVoxelY)
  {
    const Bnd_VoxelGrid::VectorInt* aBoxIndices = myVoxelGrid->GetSliceY(aVoxelY);
    if (aBoxIndices == nullptr)
    {
      continue;
    }
    for (const auto& aBoxIndex : *aBoxIndices)
    {
      // Set the second bit (0b10) to indicate that the box occupies the Y-axis.
      aResultIndices[aBoxIndex] |= anOccupiedY;
    }
  }

  // Checking the voxels along Z-axis.
  for (Standard_Integer aVoxelZ = aMinVoxelZ; aVoxelZ <= aMaxVoxelZ; ++aVoxelZ)
  {
    const Bnd_VoxelGrid::VectorInt* aBoxIndices = myVoxelGrid->GetSliceZ(aVoxelZ);
    if (aBoxIndices == nullptr)
    {
      continue;
    }
    for (const auto& aBoxIndex : *aBoxIndices)
    {
      // If box also occupies X and Y axes, we will check for intersection with the given box.
      if (aResultIndices[aBoxIndex] == anOccupiedXY)
      {
        // Clear the bit mask for this box to avoid checking and adding it multiple times.
        aResultIndices[aBoxIndex] = 0;
        // Perform actual intersection check.
        if (!myBoxes->Value(aBoxIndex).IsOut(theBox))
        {
          // Add to the result if intersection occurs.
          myLastResult.Append(aBoxIndex);
        }
      }
    }
  }

  return myLastResult;
}

//==================================================================================================

const TColStd_ListOfInteger& Bnd_BoundSortBox::Compare(const gp_Pln& thePlane)

{
  myLastResult.Clear();
  for (Standard_Integer aBoxIndex = myBoxes->Lower(); aBoxIndex <= myBoxes->Upper(); ++aBoxIndex)
  {
    const Bnd_Box& aBox = myBoxes->Value(aBoxIndex);
    if (!aBox.IsOut(thePlane))
    {
      myLastResult.Append(aBoxIndex);
    }
  }
  return myLastResult;
}

//==================================================================================================

void Bnd_BoundSortBox::calculateCoefficients()
{
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  myEnclosingBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  myCoeffX = (aXmax - aXmin == 0. ? 0. : myResolution / (aXmax - aXmin));
  myCoeffY = (aYmax - aYmin == 0. ? 0. : myResolution / (aYmax - aYmin));
  myCoeffZ = (aZmax - aZmin == 0. ? 0. : myResolution / (aZmax - aZmin));
}

//==================================================================================================

void Bnd_BoundSortBox::resetVoxelGrid()
{
  myVoxelGrid = new Bnd_VoxelGrid(myResolution);
  myLargeBoxes.clear();
  myLargeBoxes.reserve(std::max(myBoxes->Size(), 2));
}

//==================================================================================================

void Bnd_BoundSortBox::sortBoxes()
{
  for (Standard_Integer aBoxIndex = myBoxes->Lower(); aBoxIndex <= myBoxes->Upper(); ++aBoxIndex)
  {
    addBox(myBoxes->Value(aBoxIndex), aBoxIndex);
  }
}

//==================================================================================================

std::array<Standard_Integer, 6> Bnd_BoundSortBox::getBoundingVoxels(const Bnd_Box& theBox) const
{
  // Start point of the voxel grid.
  const gp_Pnt aGridStart = myEnclosingBox.CornerMin();

  Standard_Real aXMin, aYMin, aZMin, aXMax, aYMax, aZmax;
  theBox.Get(aXMin, aYMin, aZMin, aXMax, aYMax, aZmax);

  // Calculate the voxel indices for the bounding box.
  // The math is as follows:
  // Voxel grid covers the space defined by the enclosing box: from
  // myEnclosingBox.CornerMin() to myEnclosingBox.CornerMax().
  // Therefore, we can say that for the particular dimension:
  // Vmax / Lmax == V / L, where
  // Vmax is the number of voxels in the grid along that dimension,
  // Lmax is the length of the enclosing bounding box along that dimension,
  // V is the voxel coordinate along that dimension that we want to calculate,
  // L is the distance from myEnclosingBox.CornerMin() to the coordinate of the box
  //   along that dimension.
  // Rearranging gives us:
  // V = (Vmax / Lmax) * L
  // Vmax / Lmax is myCoeff(X|Y|Z), precalculated during initialization.
  // L can be simply calculated by substracting the respective coordinate of
  // myEnclosingBox.CornerMin() from the coordinate of the box.
  // Note: adding and substracting one to make sure that that the box is inside the
  // voxel coordinates. Just a safety measure.
  const Standard_Integer aXMinIndex =
    std::clamp(static_cast<Standard_Integer>((aXMin - aGridStart.X()) * myCoeffX) - 1,
               0,
               myResolution - 1);
  const Standard_Integer aYMinIndex =
    std::clamp(static_cast<Standard_Integer>((aYMin - aGridStart.Y()) * myCoeffY) - 1,
               0,
               myResolution - 1);
  const Standard_Integer aZMinIndex =
    std::clamp(static_cast<Standard_Integer>((aZMin - aGridStart.Z()) * myCoeffZ) - 1,
               0,
               myResolution - 1);
  const Standard_Integer aXMaxIndex =
    std::clamp(static_cast<Standard_Integer>((aXMax - aGridStart.X()) * myCoeffX) + 1,
               0,
               myResolution - 1);
  const Standard_Integer aYMaxIndex =
    std::clamp(static_cast<Standard_Integer>((aYMax - aGridStart.Y()) * myCoeffY) + 1,
               0,
               myResolution - 1);
  const Standard_Integer aZMaxIndex =
    std::clamp(static_cast<Standard_Integer>((aZmax - aGridStart.Z()) * myCoeffZ) + 1,
               0,
               myResolution - 1);

  return {aXMinIndex, aYMinIndex, aZMinIndex, aXMaxIndex, aYMaxIndex, aZMaxIndex};
}

//==================================================================================================

void Bnd_BoundSortBox::addBox(const Bnd_Box& theBox, const Standard_Integer theIndex)
{
  if (theBox.IsVoid())
  {
    return;
  }

  auto&& [aMinVoxelX, aMinVoxelY, aMinVoxelZ, aMaxVoxelX, aMaxVoxelY, aMaxVoxelZ] =
    getBoundingVoxels(theBox);

  const Standard_Integer aBoxMinSide =
    std::min({aMaxVoxelX - aMinVoxelX, aMaxVoxelY - aMinVoxelY, aMaxVoxelZ - aMinVoxelZ});

  // If the box spans a significant portion of the grid,
  // it is more efficient to test it normally rather than using voxel-based checks.
  if (aBoxMinSide * 4 > myResolution)
  {
    myLargeBoxes.push_back(theIndex);
  }
  else
  {
    myVoxelGrid->AddBox(theIndex,
                        {aMinVoxelX, aMinVoxelY, aMinVoxelZ, aMaxVoxelX, aMaxVoxelY, aMaxVoxelZ});
  }
}
