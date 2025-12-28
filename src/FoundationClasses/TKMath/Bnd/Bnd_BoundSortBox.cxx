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
#include <NCollection_Array1.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_NullValue.hxx>

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
static int getBnd_VoxelGridResolution(const int theBoxesCount)
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
  using VectorInt = NCollection_Vector<int>;

private:
  using SliceArray = NCollection_Array1<VectorInt>;

public:
  // Constructor that initializes the Bnd_VoxelGrid object with a given size.
  // @param theResolution The size of the grid in each dimension.
  // @param theExpectedBoxCount Expected number of boxes for pre-sizing vectors.
  Bnd_VoxelGrid(const int theResolution, const int theExpectedBoxCount);

  // Adds a box to the voxel grid.
  // The box is defined by its minimum and maximum voxel coordinates.
  // @param theBoxIndex The index of the box to be added.
  // @param theVoxelBox An array of 6 integers representing the minimum and maximum voxel
  //                    coordinates of the box in the order: [minX, minY, minZ, maxX, maxY, maxZ].
  void AddBox(const int theBoxIndex, const std::array<int, 6>& theVoxelBox);

  // Returns the list of box indices that occupy the specified voxel index in the X direction.
  // In other words, the vector will contain the indices of boxes that occupy the voxels with
  // indices in the range
  // [[theVoxelIndex, 0, 0], [theVoxelIndex, 0, 1], ..., [theVoxelIndex, resolution, resolution]].
  // @param theVoxelIndex The index of the voxel in the X direction.
  // @return A pointer to a vector of integers representing the box indices.
  //         The vector can be null if no boxes occupy the voxel.
  const VectorInt* GetSliceX(const int theVoxelIndex) const;

  // Returns the list of box indices that occupy the specified voxel index in the Y direction.
  // In other words, the vector will contain the indices of boxes that occupy the voxels with
  // indices in the range
  // [[0, theVoxelIndex, 0], [1, theVoxelIndex, 0], ..., [resolution, theVoxelIndex, resolution]].
  // @param theVoxelIndex The index of the voxel in the Y direction.
  // @return A pointer to a vector of integers representing the box indices.
  //         The vector can be null if no boxes occupy the voxel.
  const VectorInt* GetSliceY(const int theVoxelIndex) const;

  // Returns the list of box indices that occupy the specified voxel index in the Z direction.
  // In other words, the vector will contain the indices of boxes that occupy the voxels with
  // indices in the range
  // [[0, 0, theVoxelIndex], [1, 0, theVoxelIndex], ..., [resolution, resolution, theVoxelIndex]].
  // @param theVoxelIndex The index of the voxel in the Z direction.
  // @return A pointer to a vector of integers representing the box indices.
  //         The vector can be null if no boxes occupy the voxel.
  const VectorInt* GetSliceZ(const int theVoxelIndex) const;

private:
  // Appends a slice of the voxel grid in the X direction.
  // This method is used to store the indices of boxes that occupy a specific voxel index in the X
  // direction.
  // @param theVoxelIndexMin The minimum voxel index in the X direction.
  // @param theVoxelIndexMax The maximum voxel index in the X direction.
  // @param theBoxIndex The index of the box to be added.
  void AppendSliceX(const int theVoxelIndexMin, const int theVoxelIndexMax, const int theBoxIndex);

  // Appends a slice of the voxel grid in the Y direction.
  // This method is used to store the indices of boxes that occupy a specific voxel index in the Y
  // direction.
  // @param theVoxelIndexMin The minimum voxel index in the Y direction.
  // @param theVoxelIndexMax The maximum voxel index in the Y direction.
  // @param theBoxIndex The index of the box to be added.
  void AppendSliceY(const int theVoxelIndexMin, const int theVoxelIndexMax, const int theBoxIndex);

  // Appends a slice of the voxel grid in the Z direction.
  // This method is used to store the indices of boxes that occupy a specific voxel index in the Z
  // direction.
  // @param theVoxelIndexMin The minimum voxel index in the Z direction.
  // @param theVoxelIndexMax The maximum voxel index in the Z direction.
  // @param theBoxIndex The index of the box to be added.
  void AppendSliceZ(const int theVoxelIndexMin, const int theVoxelIndexMax, const int theBoxIndex);

private:
  occ::handle<NCollection_IncAllocator> myAllocator; //< Allocator for all vectors.
  SliceArray                            mySlicesX; //< Array of box indices lists for each X slice.
  SliceArray                            mySlicesY; //< Array of box indices lists for each Y slice.
  SliceArray                            mySlicesZ; //< Array of box indices lists for each Z slice.
};

IMPLEMENT_STANDARD_RTTIEXT(Bnd_VoxelGrid, Standard_Transient)

//==================================================================================================

Bnd_VoxelGrid::Bnd_VoxelGrid(const int theResolution, const int theExpectedBoxCount)
    : myAllocator(new NCollection_IncAllocator()),
      mySlicesX(0, theResolution - 1),
      mySlicesY(0, theResolution - 1),
      mySlicesZ(0, theResolution - 1)
{
  // Estimate boxes per slice: total boxes / resolution, with minimum of 16.
  const int anIncrement = std::max(theExpectedBoxCount / theResolution, 16);
  for (int i = 0; i < theResolution; ++i)
  {
    // Assign vectors with the incremental allocator for faster memory allocation.
    mySlicesX[i] = VectorInt(anIncrement, myAllocator);
    mySlicesY[i] = VectorInt(anIncrement, myAllocator);
    mySlicesZ[i] = VectorInt(anIncrement, myAllocator);
  }
}

//==================================================================================================

void Bnd_VoxelGrid::AddBox(const int theBoxIndex, const std::array<int, 6>& theVoxelBox)
{
  const int aMinVoxelX = theVoxelBox[0];
  const int aMinVoxelY = theVoxelBox[1];
  const int aMinVoxelZ = theVoxelBox[2];
  const int aMaxVoxelX = theVoxelBox[3];
  const int aMaxVoxelY = theVoxelBox[4];
  const int aMaxVoxelZ = theVoxelBox[5];

  AppendSliceX(aMinVoxelX, aMaxVoxelX, theBoxIndex);
  AppendSliceY(aMinVoxelY, aMaxVoxelY, theBoxIndex);
  AppendSliceZ(aMinVoxelZ, aMaxVoxelZ, theBoxIndex);
}

//==================================================================================================

const Bnd_VoxelGrid::VectorInt* Bnd_VoxelGrid::GetSliceX(const int theVoxelIndex) const
{
  const VectorInt& aSlice = mySlicesX[theVoxelIndex];
  return aSlice.IsEmpty() ? nullptr : &aSlice;
}

//==================================================================================================

const Bnd_VoxelGrid::VectorInt* Bnd_VoxelGrid::GetSliceY(const int theVoxelIndex) const
{
  const VectorInt& aSlice = mySlicesY[theVoxelIndex];
  return aSlice.IsEmpty() ? nullptr : &aSlice;
}

//==================================================================================================

const Bnd_VoxelGrid::VectorInt* Bnd_VoxelGrid::GetSliceZ(const int theVoxelIndex) const
{
  const VectorInt& aSlice = mySlicesZ[theVoxelIndex];
  return aSlice.IsEmpty() ? nullptr : &aSlice;
}

//==================================================================================================

void Bnd_VoxelGrid::AppendSliceX(const int theVoxelIndexMin,
                                 const int theVoxelIndexMax,
                                 const int theBoxIndex)
{
  for (int i = theVoxelIndexMin; i <= theVoxelIndexMax; ++i)
  {
    mySlicesX[i].Append(theBoxIndex);
  }
}

//==================================================================================================

void Bnd_VoxelGrid::AppendSliceY(const int theVoxelIndexMin,
                                 const int theVoxelIndexMax,
                                 const int theBoxIndex)
{
  for (int i = theVoxelIndexMin; i <= theVoxelIndexMax; ++i)
  {
    mySlicesY[i].Append(theBoxIndex);
  }
}

//==================================================================================================

void Bnd_VoxelGrid::AppendSliceZ(const int theVoxelIndexMin,
                                 const int theVoxelIndexMax,
                                 const int theBoxIndex)
{
  for (int i = theVoxelIndexMin; i <= theVoxelIndexMax; ++i)
  {
    mySlicesZ[i].Append(theBoxIndex);
  }
}

//==================================================================================================

Bnd_BoundSortBox::Bnd_BoundSortBox()
    : 
      myBoxes(nullptr),
      myCoeffX(0.),
      myCoeffY(0.),
      myCoeffZ(0.),
      myResolution(0),
      
      myVoxelGrid(nullptr)
{
}

//==================================================================================================

void Bnd_BoundSortBox::Initialize(const occ::handle<NCollection_HArray1<Bnd_Box>>& theSetOfBoxes)
{
  myBoxes = theSetOfBoxes;

  for (int aBoxIndex = myBoxes->Lower(); aBoxIndex <= myBoxes->Upper(); ++aBoxIndex)
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

void Bnd_BoundSortBox::Initialize(const Bnd_Box&                                   theEnclosingBox,
                                  const occ::handle<NCollection_HArray1<Bnd_Box>>& theSetOfBoxes)
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

void Bnd_BoundSortBox::Initialize(const Bnd_Box& theEnclosingBox, const int theNbBoxes)
{
  Standard_NullValue_Raise_if(theNbBoxes <= 0, "Unexpected: theNbBoxes <= 0");
  myBoxes = new NCollection_HArray1<Bnd_Box>(1, theNbBoxes);
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

void Bnd_BoundSortBox::Add(const Bnd_Box& theBox, const int theIndex)
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

const NCollection_List<int>& Bnd_BoundSortBox::Compare(const Bnd_Box& theBox)

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
  for (int aVoxelX = aMinVoxelX; aVoxelX <= aMaxVoxelX; ++aVoxelX)
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
  for (int aVoxelY = aMinVoxelY; aVoxelY <= aMaxVoxelY; ++aVoxelY)
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
  for (int aVoxelZ = aMinVoxelZ; aVoxelZ <= aMaxVoxelZ; ++aVoxelZ)
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

const NCollection_List<int>& Bnd_BoundSortBox::Compare(const gp_Pln& thePlane)

{
  myLastResult.Clear();
  for (int aBoxIndex = myBoxes->Lower(); aBoxIndex <= myBoxes->Upper(); ++aBoxIndex)
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
  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  myEnclosingBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  myCoeffX = (aXmax - aXmin == 0. ? 0. : myResolution / (aXmax - aXmin));
  myCoeffY = (aYmax - aYmin == 0. ? 0. : myResolution / (aYmax - aYmin));
  myCoeffZ = (aZmax - aZmin == 0. ? 0. : myResolution / (aZmax - aZmin));
}

//==================================================================================================

void Bnd_BoundSortBox::resetVoxelGrid()
{
  myVoxelGrid = new Bnd_VoxelGrid(myResolution, myBoxes->Length());
  myLargeBoxes.Clear();
  // Set block size to reduce reallocations when many large boxes are expected.
  myLargeBoxes.SetIncrement(std::max(myBoxes->Length() / 16, 16));
}

//==================================================================================================

void Bnd_BoundSortBox::sortBoxes()
{
  for (int aBoxIndex = myBoxes->Lower(); aBoxIndex <= myBoxes->Upper(); ++aBoxIndex)
  {
    addBox(myBoxes->Value(aBoxIndex), aBoxIndex);
  }
}

//==================================================================================================

std::array<int, 6> Bnd_BoundSortBox::getBoundingVoxels(const Bnd_Box& theBox) const
{
  // Start point of the voxel grid.
  const gp_Pnt aGridStart = myEnclosingBox.CornerMin();

  double aXMin, aYMin, aZMin, aXMax, aYMax, aZmax;
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
  const int aXMinIndex =
    std::clamp(static_cast<int>((aXMin - aGridStart.X()) * myCoeffX) - 1, 0, myResolution - 1);
  const int aYMinIndex =
    std::clamp(static_cast<int>((aYMin - aGridStart.Y()) * myCoeffY) - 1, 0, myResolution - 1);
  const int aZMinIndex =
    std::clamp(static_cast<int>((aZMin - aGridStart.Z()) * myCoeffZ) - 1, 0, myResolution - 1);
  const int aXMaxIndex =
    std::clamp(static_cast<int>((aXMax - aGridStart.X()) * myCoeffX) + 1, 0, myResolution - 1);
  const int aYMaxIndex =
    std::clamp(static_cast<int>((aYMax - aGridStart.Y()) * myCoeffY) + 1, 0, myResolution - 1);
  const int aZMaxIndex =
    std::clamp(static_cast<int>((aZmax - aGridStart.Z()) * myCoeffZ) + 1, 0, myResolution - 1);

  return {aXMinIndex, aYMinIndex, aZMinIndex, aXMaxIndex, aYMaxIndex, aZMaxIndex};
}

//==================================================================================================

void Bnd_BoundSortBox::addBox(const Bnd_Box& theBox, const int theIndex)
{
  if (theBox.IsVoid())
  {
    return;
  }

  auto&& [aMinVoxelX, aMinVoxelY, aMinVoxelZ, aMaxVoxelX, aMaxVoxelY, aMaxVoxelZ] =
    getBoundingVoxels(theBox);

  const int aBoxMinSide =
    std::min({aMaxVoxelX - aMinVoxelX, aMaxVoxelY - aMinVoxelY, aMaxVoxelZ - aMinVoxelZ});

  // If the box spans a significant portion of the grid,
  // it is more efficient to test it normally rather than using voxel-based checks.
  if (aBoxMinSide * 4 > myResolution)
  {
    myLargeBoxes.Append(theIndex);
  }
  else
  {
    myVoxelGrid->AddBox(theIndex,
                        {aMinVoxelX, aMinVoxelY, aMinVoxelZ, aMaxVoxelX, aMaxVoxelY, aMaxVoxelZ});
  }
}
