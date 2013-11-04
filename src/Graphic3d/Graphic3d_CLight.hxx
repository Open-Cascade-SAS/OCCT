// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Graphic3d_CLight_HeaderFile
#define _Graphic3d_CLight_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <Graphic3d_Vec.hxx>
#include <Standard_Type.hxx>

//! Light definition
struct Graphic3d_CLight
{

public:

  Graphic3d_Vec4     Color;       //!< light color
  Graphic3d_Vec4     Position;    //!< light position
  Graphic3d_Vec4     Direction;   //!< direction of directional/spot light
  Graphic3d_Vec4     Params;      //!< packed light parameters
  Standard_Integer   Type;        //!< Visual3d_TypeOfLightSource enumeration
  Standard_Boolean   IsHeadlight; //!< flag to mark head light

  //! Const attenuation factor of positional light source
  Standard_ShortReal  ConstAttenuation()  const { return Params.x();  }

  //! Linear attenuation factor of positional light source
  Standard_ShortReal  LinearAttenuation() const { return Params.y();  }

  //! Const, Linear attenuation factors of positional light source
  Graphic3d_Vec2      Attenuation()       const { return Params.xy(); }

  //! Angle in radians of the cone created by the spot
  Standard_ShortReal  Angle()             const { return Params.z();  }

  //! Intensity distribution of the spot light, with 0..1 range.
  Standard_ShortReal  Concentration()     const { return Params.w();  }

  Standard_ShortReal& ChangeConstAttenuation()  { return Params.x();  }
  Standard_ShortReal& ChangeLinearAttenuation() { return Params.y();  }
  Graphic3d_Vec2&     ChangeAttenuation()       { return Params.xy(); }
  Standard_ShortReal& ChangeAngle()             { return Params.z();  }
  Standard_ShortReal& ChangeConcentration()     { return Params.w();  }

public:

  //! Empty constructor
  Graphic3d_CLight()
  : Color         (1.0f, 1.0f, 1.0f, 1.0f),
    Position      (0.0f, 0.0f, 0.0f, 1.0f),
    Direction     (0.0f, 0.0f, 0.0f, 0.0f),
    Params        (0.0f, 0.0f, 0.0f, 0.0f),
    Type          (0),
    IsHeadlight   (Standard_False)
  {
    //
  }

public:

  DEFINE_STANDARD_ALLOC

};

#endif // Graphic3d_CLight_HeaderFile
