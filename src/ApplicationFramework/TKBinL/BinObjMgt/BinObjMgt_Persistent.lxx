// Created on: 2002-10-30
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#define BP_HEADSIZE ((int)(3 * sizeof(int)))
#define BP_PIECESIZE 102400

//=======================================================================
// function : SetId
// purpose  : Sets the Id of the object
//=======================================================================

inline void BinObjMgt_Persistent::SetId(const int theId)
{
  ((int*)myData(1))[1] = theId;
}

//=======================================================================
// function : SetTypeId
// purpose  : Sets the Id of the type of the object
//=======================================================================

inline void BinObjMgt_Persistent::SetTypeId(const int theTypeId)
{
  ((int*)myData(1))[0] = theTypeId;
  myStreamStart.Nullify();
}

//=======================================================================
// function : Id
// purpose  : Returns the Id of the object
//=======================================================================

inline int BinObjMgt_Persistent::Id() const
{
  return ((int*)myData(1))[1];
}

//=======================================================================
// function : TypeId
// purpose  : Returns the Id of the type of the object
//=======================================================================

inline int BinObjMgt_Persistent::TypeId() const
{
  return ((int*)myData(1))[0];
}

//=======================================================================
// function : Length
// purpose  : Returns the length of data
//=======================================================================

inline int BinObjMgt_Persistent::Length() const
{
  return mySize - BP_HEADSIZE;
}

//=======================================================================
// function : operator <<
// purpose  :
//=======================================================================

inline Standard_OStream& operator<<(Standard_OStream& theOS, BinObjMgt_Persistent& theObj)
{
  return theObj.Write(theOS);
}

//=======================================================================
// function : operator >>
// purpose  :
//=======================================================================

inline Standard_IStream& operator>>(Standard_IStream& theIS, BinObjMgt_Persistent& theObj)
{
  return theObj.Read(theIS);
}

//=======================================================================
// function : Position
// purpose  : Tells the current position for get/put
//=======================================================================

inline int BinObjMgt_Persistent::Position() const
{
  return (myIndex - 1) * BP_PIECESIZE + myOffset;
}

//=======================================================================
// function : SetPosition
// purpose  : Sets the current position for get/put.
//           Resets an error state depending on the validity of thePos.
//           Returns the new state (value of IsOK())
//=======================================================================

inline bool BinObjMgt_Persistent::SetPosition(const int thePos) const
{
  ((BinObjMgt_Persistent*)this)->myIndex   = thePos / BP_PIECESIZE + 1;
  ((BinObjMgt_Persistent*)this)->myOffset  = thePos % BP_PIECESIZE;
  ((BinObjMgt_Persistent*)this)->myIsError = thePos > mySize || thePos < BP_HEADSIZE;
  return !myIsError;
}

//=======================================================================
// function : Truncate
// purpose  : Truncates the buffer by current position,
//           i.e. updates mySize
//=======================================================================

inline void BinObjMgt_Persistent::Truncate()
{
  mySize = Position();
}

//=======================================================================
// function : IsError
// purpose  : Indicates an error after Get methods or SetPosition
//=======================================================================

inline bool BinObjMgt_Persistent::IsError() const
{
  return myIsError;
}

//=======================================================================
// function : IsOK
// purpose  : Indicates a good state after Get methods or SetPosition
//=======================================================================

inline bool BinObjMgt_Persistent::IsOK() const
{
  return !myIsError;
}

//=======================================================================
// function : alignOffset
// purpose  : Aligns myOffset to the given size;
//           enters the next piece if the end of the current one is reached;
//           toClear==true means to fill unused space by 0
//=======================================================================

inline void BinObjMgt_Persistent::alignOffset(const int theSize, const bool toClear) const
{
  unsigned alignMask = theSize - 1;
  int      anOffset  = (myOffset + alignMask) & ~alignMask;

  if (anOffset > myOffset)
  {
    if (toClear && anOffset <= BP_PIECESIZE)
      memset(((char*)myData(myIndex)) + myOffset, 0, anOffset - myOffset);
    ((BinObjMgt_Persistent*)this)->myOffset = anOffset;
  }

  // ensure there is a room for at least one item in the current piece
  if (myOffset >= BP_PIECESIZE)
  {
    ((BinObjMgt_Persistent*)this)->myIndex++;
    ((BinObjMgt_Persistent*)this)->myOffset = 0;
  }
}

//=======================================================================
// function : prepareForPut
// purpose  : Prepares the room for theSize bytes;
//           returns the number of pieces except for the current one
//           are to be occupied
//=======================================================================

inline int BinObjMgt_Persistent::prepareForPut(const int theSize)
{
  int nbPieces = (myOffset + theSize - 1) / BP_PIECESIZE;
  int nbToAdd  = myIndex + nbPieces - myData.Length();
  if (nbToAdd > 0)
    // create needed pieces
    incrementData(nbToAdd);
  int aNewPosition = Position() + theSize;
  if (aNewPosition > mySize)
    mySize = aNewPosition;
  return nbPieces;
}

//=======================================================================
// function : noMoreData
// purpose  : Checks if there is no more data of the given size starting
//           from the current position in myData
//=======================================================================

inline bool BinObjMgt_Persistent::noMoreData(const int theSize) const
{
  ((BinObjMgt_Persistent*)this)->myIsError = Position() + theSize > mySize;
  return myIsError;
}

//=================================================================================================

inline BinObjMgt_Persistent& BinObjMgt_Persistent::PutBoolean(const bool theValue)
{
  return PutInteger((int)theValue);
}

//=================================================================================================

inline const BinObjMgt_Persistent& BinObjMgt_Persistent::GetBoolean(bool& theValue) const
{
  //  int anIntVal = (int) theValue;
  int anIntVal;
  GetInteger(anIntVal);
  theValue = anIntVal != 0;
  return *this;
}
