// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _AIS_WalkDelta_HeaderFile
#define _AIS_WalkDelta_HeaderFile

#include <Standard_Real.hxx>

//! Walking translation components.
enum AIS_WalkTranslation
{
  AIS_WalkTranslation_Forward = 0, //!< translation delta, Forward walk
  AIS_WalkTranslation_Side,        //!< translation delta, Side walk
  AIS_WalkTranslation_Up,          //!< translation delta, Up walk
};

//! Walking rotation components.
enum AIS_WalkRotation
{
  AIS_WalkRotation_Yaw = 0,  //!< yaw   rotation angle
  AIS_WalkRotation_Pitch,    //!< pitch rotation angle
  AIS_WalkRotation_Roll,     //!< roll  rotation angle
};

//! Walking value.
struct AIS_WalkPart
{
  Standard_Real Value;    //!< value
  Standard_Real Pressure; //!< key pressure
  Standard_Real Duration; //!< duration

  //! Return TRUE if delta is empty.
  bool IsEmpty() const { return Abs (Value) <= RealSmall(); }

  //! Empty constructor.
  AIS_WalkPart() : Value (0.0), Pressure (1.0), Duration (0.0) {}
};

//! Walking values.
struct AIS_WalkDelta
{
  //! Empty constructor.
  AIS_WalkDelta()
  : myIsDefined (false), myIsJumping (false), myIsCrouching (false), myIsRunning (false) {}

  //! Return translation component.
  const AIS_WalkPart& operator[] (AIS_WalkTranslation thePart) const { return myTranslation[thePart]; }

  //! Return translation component.
  AIS_WalkPart&       operator[] (AIS_WalkTranslation thePart)       { return myTranslation[thePart]; }

  //! Return rotation component.
  const AIS_WalkPart& operator[] (AIS_WalkRotation thePart) const { return myRotation[thePart]; }

  //! Return rotation component.
  AIS_WalkPart&       operator[] (AIS_WalkRotation thePart)       { return myRotation[thePart]; }

  //! Return jumping state.
  bool IsJumping() const { return myIsJumping; }

  //! Set jumping state.
  void SetJumping (bool theIsJumping) { myIsJumping = theIsJumping; }

  //! Return crouching state.
  bool IsCrouching() const { return myIsCrouching; }

  //! Set crouching state.
  void SetCrouching (bool theIsCrouching) { myIsCrouching = theIsCrouching; }

  //! Return running state.
  bool IsRunning() const { return myIsRunning; }

  //! Set running state.
  void SetRunning (bool theIsRunning) { myIsRunning = theIsRunning; }

  //! Return TRUE if navigation keys are pressed even if delta from the previous frame is empty.
  bool IsDefined() const { return myIsDefined || !IsEmpty(); }

  //! Set if any navigation key is pressed.
  void SetDefined (bool theIsDefined) { myIsDefined = theIsDefined; }

  //! Return TRUE when both Rotation and Translation deltas are empty.
  bool IsEmpty() const { return !ToMove() && !ToRotate(); }

  //! Return TRUE if translation delta is defined.
  bool ToMove() const
  {
    return !myTranslation[AIS_WalkTranslation_Forward].IsEmpty()
        || !myTranslation[AIS_WalkTranslation_Side].IsEmpty()
        || !myTranslation[AIS_WalkTranslation_Up].IsEmpty();
  }

  //! Return TRUE if rotation delta is defined.
  bool ToRotate() const
  {
    return !myRotation[AIS_WalkRotation_Yaw].IsEmpty()
        || !myRotation[AIS_WalkRotation_Pitch].IsEmpty()
        || !myRotation[AIS_WalkRotation_Roll].IsEmpty();
  }

private:

  AIS_WalkPart myTranslation[3];
  AIS_WalkPart myRotation[3];
  bool myIsDefined;
  bool myIsJumping;
  bool myIsCrouching;
  bool myIsRunning;

};

#endif // _AIS_WalkDelta_HeaderFile
