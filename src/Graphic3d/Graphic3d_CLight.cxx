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

#include <Graphic3d_CLight.hxx>

#include <Standard_Atomic.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_CLight, Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_LIGHT_COUNTER = 0;
}

// =======================================================================
// function : makeId
// purpose  :
// =======================================================================
void Graphic3d_CLight::makeId()
{
  TCollection_AsciiString aTypeSuffix;
  switch (myType)
  {
    case Graphic3d_TypeOfLightSource_Ambient:     aTypeSuffix = "amb"; break;
    case Graphic3d_TypeOfLightSource_Directional: aTypeSuffix = "dir"; break;
    case Graphic3d_TypeOfLightSource_Positional:  aTypeSuffix = "pos"; break;
    case Graphic3d_TypeOfLightSource_Spot:        aTypeSuffix = "spot"; break;
  }

  myId = TCollection_AsciiString ("Graphic3d_CLight_") + aTypeSuffix
       + TCollection_AsciiString (Standard_Atomic_Increment (&THE_LIGHT_COUNTER));
}

// =======================================================================
// function : Graphic3d_CLight
// purpose  :
// =======================================================================
Graphic3d_CLight::Graphic3d_CLight (Graphic3d_TypeOfLightSource theType)
: myPosition   (0.0,  0.0,  0.0),
  myColor      (1.0f, 1.0f, 1.0f, 1.0f),
  myDirection  (0.0f, 0.0f, 0.0f, 0.0f),
  myParams     (0.0f, 0.0f, 0.0f, 0.0f),
  mySmoothness (0.0f),
  myIntensity  (1.0f),
  myType       (theType),
  myRevision   (0),
  myIsHeadlight(false),
  myIsEnabled  (true),
  myToCastShadows (false)
{
  switch (theType)
  {
    case Graphic3d_TypeOfLightSource_Ambient:
    {
      break;
    }
    case Graphic3d_TypeOfLightSource_Directional:
    {
      mySmoothness = 0.2f;
      myIntensity  = 20.0f;
      break;
    }
    case Graphic3d_TypeOfLightSource_Positional:
    {
      changeConstAttenuation()  = 1.0f;
      changeLinearAttenuation() = 0.0f;
      break;
    }
    case Graphic3d_TypeOfLightSource_Spot:
    {
      changeConstAttenuation()  = 1.0f;
      changeLinearAttenuation() = 0.0f;
      changeConcentration()     = 1.0f;
      changeAngle()             = 0.523599f;
      break;
    }
  }
  makeId();
}

// =======================================================================
// function : CopyFrom
// purpose  :
// =======================================================================
void Graphic3d_CLight::CopyFrom (const Handle(Graphic3d_CLight)& theLight)
{
  myName      = theLight->myName;
  myIsEnabled = theLight->myIsEnabled;
  myIntensity = theLight->myIntensity;
  myColor     = theLight->myColor;
  if (myType != Graphic3d_TypeOfLightSource_Ambient)
  {
    myPosition    = theLight->myPosition;
    myIsHeadlight = theLight->myIsHeadlight;
  }
  if ((myType == Graphic3d_TypeOfLightSource_Spot
    || myType == Graphic3d_TypeOfLightSource_Directional)
   && (theLight->myType == Graphic3d_TypeOfLightSource_Spot
    || theLight->myType == Graphic3d_TypeOfLightSource_Directional))
  {
    myDirection = theLight->myDirection;
  }
  if (myType == Graphic3d_TypeOfLightSource_Directional)
  {
    myToCastShadows = theLight->myToCastShadows;
  }
  if (myType == theLight->myType)
  {
    myParams     = theLight->myParams;
    mySmoothness = theLight->mySmoothness;
  }
}

// =======================================================================
// function : SetColor
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetColor (const Quantity_Color& theColor)
{
  updateRevisionIf (myColor.GetRGB().IsDifferent (theColor));
  myColor.SetRGB (theColor);
}

// =======================================================================
// function : SetEnabled
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetEnabled (Standard_Boolean theIsOn)
{
  updateRevisionIf (myIsEnabled != theIsOn);
  myIsEnabled = theIsOn;
}

// =======================================================================
// function : SetCastShadows
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetCastShadows (Standard_Boolean theToCast)
{
  if (myType != Graphic3d_TypeOfLightSource_Directional
   && myType != Graphic3d_TypeOfLightSource_Spot)
  {
    throw Standard_NotImplemented ("Graphic3d_CLight::SetCastShadows() is not implemented for this light type");
  }
  updateRevisionIf (myToCastShadows != theToCast);
  myToCastShadows = theToCast;
}

// =======================================================================
// function : SetHeadlight
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetHeadlight (Standard_Boolean theValue)
{
  if (myType == Graphic3d_TypeOfLightSource_Ambient)
  {
    throw Standard_ProgramError ("Graphic3d_CLight::SetHeadlight() is not applicable to ambient light");
  }
  updateRevisionIf (myIsHeadlight != theValue);
  myIsHeadlight = theValue;
}

// =======================================================================
// function : SetDirection
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetDirection (const gp_Dir& theDir)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Spot
                               && myType != Graphic3d_TypeOfLightSource_Directional,
                                  "Graphic3d_CLight::SetDirection(), incorrect light type");
  updateRevisionIf (Abs (myDirection.x() - static_cast<Standard_ShortReal> (theDir.X())) > ShortRealEpsilon()
                 || Abs (myDirection.y() - static_cast<Standard_ShortReal> (theDir.Y())) > ShortRealEpsilon()
                 || Abs (myDirection.z() - static_cast<Standard_ShortReal> (theDir.Z())) > ShortRealEpsilon());

  myDirection.x() = static_cast<Standard_ShortReal> (theDir.X());
  myDirection.y() = static_cast<Standard_ShortReal> (theDir.Y());
  myDirection.z() = static_cast<Standard_ShortReal> (theDir.Z());
}

// =======================================================================
// function : SetPosition
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetPosition (const gp_Pnt& thePosition)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Spot
                               && myType != Graphic3d_TypeOfLightSource_Positional,
                                  "Graphic3d_CLight::SetPosition(), incorrect light type");
  updateRevisionIf (!myPosition.IsEqual (thePosition, gp::Resolution()));
  myPosition = thePosition;
}

// =======================================================================
// function : SetDisplayPosition
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetDisplayPosition (const gp_Pnt& thePosition)
{
  Standard_ProgramError_Raise_if (myType == Graphic3d_TypeOfLightSource_Ambient,
                                  "Graphic3d_CLight::SetDisplayPosition(), incorrect light type");
  updateRevisionIf (!myPosition.IsEqual (thePosition, gp::Resolution()));
  myPosition = thePosition;
}

// =======================================================================
// function : SetIntensity
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetIntensity (Standard_ShortReal theValue)
{
  Standard_OutOfRange_Raise_if (theValue <= 0.0f, "Graphic3d_CLight::SetIntensity(), Negative value for intensity");
  updateRevisionIf (Abs (myIntensity - theValue) > ShortRealEpsilon());
  myIntensity = theValue;
}

// =======================================================================
// function : SetAngle
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetAngle (Standard_ShortReal theAngle)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Spot,
                                  "Graphic3d_CLight::SetAngle(), incorrect light type");
  Standard_OutOfRange_Raise_if (theAngle <= 0.0 || theAngle >= M_PI,
                                "Graphic3d_CLight::SetAngle(), bad angle");
  updateRevisionIf (Abs (changeAngle() - theAngle) > ShortRealEpsilon());
  changeAngle() = theAngle;
}

// =======================================================================
// function : SetAttenuation
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetAttenuation (Standard_ShortReal theConstAttenuation,
                                       Standard_ShortReal theLinearAttenuation)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Positional
                               && myType != Graphic3d_TypeOfLightSource_Spot,
                                  "Graphic3d_CLight::SetAttenuation(), incorrect light type");
  Standard_OutOfRange_Raise_if (theConstAttenuation  < 0.0f
                             || theLinearAttenuation < 0.0f
                             || theConstAttenuation + theLinearAttenuation == 0.0f, "Graphic3d_CLight::SetAttenuation(), bad coefficient");
  updateRevisionIf (Abs (changeConstAttenuation()  - theConstAttenuation)  > ShortRealEpsilon()
                 || Abs (changeLinearAttenuation() - theLinearAttenuation) > ShortRealEpsilon());
  changeConstAttenuation()  = theConstAttenuation;
  changeLinearAttenuation() = theLinearAttenuation;
}

// =======================================================================
// function : SetConcentration
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetConcentration (Standard_ShortReal theConcentration)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Spot, "Graphic3d_CLight::SetConcentration(), incorrect light type");
  Standard_OutOfRange_Raise_if (theConcentration < 0.0f || theConcentration > 1.0f,
                                "Graphic3d_CLight::SetConcentration(), bad coefficient");
  updateRevisionIf (Abs (changeConcentration() - theConcentration) > ShortRealEpsilon());
  changeConcentration() = theConcentration;
}

// =======================================================================
// function : SetSmoothRadius
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetSmoothRadius (Standard_ShortReal theValue)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Positional
                               && myType != Graphic3d_TypeOfLightSource_Spot,
                                  "Graphic3d_CLight::SetSmoothRadius(), incorrect light type");
  Standard_OutOfRange_Raise_if (theValue < 0.0f, "Graphic3d_CLight::SetSmoothRadius(), Bad value for smoothing radius");
  updateRevisionIf (Abs (mySmoothness - theValue) > ShortRealEpsilon());
  mySmoothness = theValue;
}

// =======================================================================
// function : SetSmoothAngle
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetSmoothAngle (Standard_ShortReal theValue)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Directional,
                                  "Graphic3d_CLight::SetSmoothAngle(), incorrect light type");
  Standard_OutOfRange_Raise_if (theValue < 0.0f || theValue > Standard_ShortReal(M_PI / 2.0),
                                "Graphic3d_CLight::SetSmoothAngle(), Bad value for smoothing angle");
  updateRevisionIf (Abs (mySmoothness - theValue) > ShortRealEpsilon());
  mySmoothness = theValue;
}

// =======================================================================
// function : SetRange
// purpose  :
// =======================================================================
void Graphic3d_CLight::SetRange (Standard_ShortReal theValue)
{
  Standard_ProgramError_Raise_if (myType != Graphic3d_TypeOfLightSource_Positional
                               && myType != Graphic3d_TypeOfLightSource_Spot,
                                  "Graphic3d_CLight::SetRange(), incorrect light type");
  Standard_OutOfRange_Raise_if (theValue < 0.0, "Graphic3d_CLight::SetRange(), Bad value for falloff range");
  updateRevisionIf (Abs (Range() - theValue) > ShortRealEpsilon());
  myDirection.w() = theValue;
};

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Graphic3d_CLight::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, this)
  
  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, myId)
  OCCT_DUMP_FIELD_VALUE_STRING (theOStream, myName)
  
  if (myType == Graphic3d_TypeOfLightSource_Spot || myType == Graphic3d_TypeOfLightSource_Positional)
  {
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPosition)
  }
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myColor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIntensity)

  if (myType == Graphic3d_TypeOfLightSource_Spot || myType == Graphic3d_TypeOfLightSource_Directional)
  {
    gp_Dir aDirection = Direction();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &aDirection)
  }
  if (myType == Graphic3d_TypeOfLightSource_Positional || myType == Graphic3d_TypeOfLightSource_Spot)
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, ConstAttenuation())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, LinearAttenuation())
  }
  if (myType == Graphic3d_TypeOfLightSource_Spot)
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Angle())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Concentration())
  }
  if (myType == Graphic3d_TypeOfLightSource_Positional || myType == Graphic3d_TypeOfLightSource_Spot)
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, Range())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, mySmoothness)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myRevision)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsHeadlight)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsEnabled)
}
