// Created on: 2012-07-18
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _Image_Color_H__
#define _Image_Color_H__

#include <Standard.hxx>

//! POD structure for packed RGB color value (3 bytes)
struct Image_ColorRGB
{

  //! Component type.
  typedef Standard_Byte ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 3;
  }

public: // access methods

  //! Alias to 1st component (red intensity).
  Standard_Byte r() const { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte g() const { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_Byte b() const { return v[2]; }

  //! Alias to 1st component (red intensity).
  Standard_Byte& r() { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte& g() { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_Byte& b() { return v[2]; }

public:

  Standard_Byte v[3];

};

//! POD structure for packed RGB color value (4 bytes with extra byte for alignment)
struct Image_ColorRGB32
{

  //! Component type.
  typedef Standard_Byte ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 3;
  }

  //! Alias to 1st component (red intensity).
  Standard_Byte r() const { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte g() const { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_Byte b() const { return v[2]; }

  //! Alias to 4th component (dummy).
  Standard_Byte a_() const { return v[3]; }

  //! Alias to 1st component (red intensity).
  Standard_Byte& r() { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte& g() { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_Byte& b() { return v[2]; }

  //! Alias to 4th component (dummy).
  Standard_Byte& a_() { return v[3]; }

public:

  Standard_Byte v[4];

};

//! POD structure for packed RGBA color value (4 bytes)
struct Image_ColorRGBA
{

  //! Component type.
  typedef Standard_Byte ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 4;
  }

  //! Alias to 1st component (red intensity).
  Standard_Byte r() const { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte g() const { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_Byte b() const { return v[2]; }

  //! Alias to 4th component (alpha value).
  Standard_Byte a() const { return v[3]; }

  //! Alias to 1st component (red intensity).
  Standard_Byte& r() { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte& g() { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_Byte& b() { return v[2]; }

  //! Alias to 4th component (alpha value).
  Standard_Byte& a() { return v[3]; }

public:

  Standard_Byte v[4];

};

//! POD structure for packed BGR color value (3 bytes)
struct Image_ColorBGR
{

  //! Component type.
  typedef Standard_Byte ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 3;
  }

  //! Alias to 3rd component (red intensity).
  Standard_Byte r() const { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte g() const { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_Byte b() const { return v[0]; }

  //! Alias to 3rd component (red intensity).
  Standard_Byte& r() { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte& g() { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_Byte& b() { return v[0]; }

public:

  Standard_Byte v[3];

};

//! POD structure for packed BGR color value (4 bytes with extra byte for alignment)
struct Image_ColorBGR32
{

  //! Component type.
  typedef Standard_Byte ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 3;
  }

  //! Alias to 3rd component (red intensity).
  Standard_Byte r() const { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte g() const { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_Byte b() const { return v[0]; }

  //! Alias to 4th component (dummy).
  Standard_Byte a_() const { return v[3]; }

  //! Alias to 3rd component (red intensity).
  Standard_Byte& r() { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte& g() { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_Byte& b() { return v[0]; }

  //! Alias to 4th component (dummy).
  Standard_Byte& a_() { return v[3]; }

public:

  Standard_Byte v[4];

};

//! POD structure for packed BGRA color value (4 bytes)
struct Image_ColorBGRA
{

  //! Component type.
  typedef Standard_Byte ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 4;
  }

  //! Alias to 3rd component (red intensity).
  Standard_Byte r() const { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte g() const { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_Byte b() const { return v[0]; }

  //! Alias to 4th component (alpha value).
  Standard_Byte a() const { return v[3]; }

  //! Alias to 3rd component (red intensity).
  Standard_Byte& r() { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_Byte& g() { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_Byte& b() { return v[0]; }

  //! Alias to 4th component (alpha value).
  Standard_Byte& a() { return v[3]; }

public:

  Standard_Byte v[4];

};

//! POD structure for packed float RGB color value (3 floats)
struct Image_ColorRGBF
{

  //! Component type.
  typedef Standard_ShortReal ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 3;
  }

  //! Alias to 1st component (red intensity).
  Standard_ShortReal r() const { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal g() const { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_ShortReal b() const { return v[2]; }

  //! Alias to 1st component (red intensity).
  Standard_ShortReal& r() { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal& g() { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_ShortReal& b() { return v[2]; }

public:

  Standard_ShortReal v[3];

};

//! POD structure for packed BGR float color value (3 floats)
struct Image_ColorBGRF
{

  //! Component type.
  typedef Standard_ShortReal ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 3;
  }

  //! Alias to 3rd component (red intensity).
  Standard_ShortReal r() const { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal g() const { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_ShortReal b() const { return v[0]; }

  //! Alias to 3rd component (red intensity).
  Standard_ShortReal& r() { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal& g() { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_ShortReal& b() { return v[0]; }

public:

  Standard_ShortReal v[3];

};

//! POD structure for packed RGBA color value (4 floats)
struct Image_ColorRGBAF
{

  //! Component type.
  typedef Standard_ShortReal ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 4;
  }

  //! Alias to 1st component (red intensity).
  Standard_ShortReal r() const { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal g() const { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_ShortReal b() const { return v[2]; }

  //! Alias to 4th component (alpha value).
  Standard_ShortReal a() const { return v[3]; }

  //! Alias to 1st component (red intensity).
  Standard_ShortReal& r() { return v[0]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal& g() { return v[1]; }

  //! Alias to 3rd component (blue intensity).
  Standard_ShortReal& b() { return v[2]; }

  //! Alias to 4th component (alpha value).
  Standard_ShortReal& a() { return v[3]; }

public:

  Standard_ShortReal v[4];

};

//! POD structure for packed float BGRA color value (4 floats)
struct Image_ColorBGRAF
{

  //! Component type.
  typedef Standard_ShortReal ComponentType_t;

  //! Returns the number of components.
  static Standard_Integer Length()
  {
    return 4;
  }

  //! Alias to 3rd component (red intensity).
  Standard_ShortReal r() const { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal g() const { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_ShortReal b() const { return v[0]; }

  //! Alias to 4th component (alpha value).
  Standard_ShortReal a() const { return v[3]; }

  //! Alias to 3rd component (red intensity).
  Standard_ShortReal& r() { return v[2]; }

  //! Alias to 2nd component (green intensity).
  Standard_ShortReal& g() { return v[1]; }

  //! Alias to 1st component (blue intensity).
  Standard_ShortReal& b() { return v[0]; }

  //! Alias to 4th component (alpha value).
  Standard_ShortReal& a() { return v[3]; }

public:

  Standard_ShortReal v[4];

};

//! Addition operator
template<typename ColorType_t>
inline ColorType_t Image_ColorSumm3 (const ColorType_t& theA, const ColorType_t& theB)
{
  ColorType_t aRes = {{typename ColorType_t::ComponentType_t (theA.v[0] + theB.v[0]),
                       typename ColorType_t::ComponentType_t (theA.v[1] + theB.v[1]),
                       typename ColorType_t::ComponentType_t (theA.v[2] + theB.v[2])}};
  return aRes;
}

inline Image_ColorRGB operator+ (const Image_ColorRGB& theA, const Image_ColorRGB& theB)
{
  return Image_ColorSumm3 (theA, theB);
}

inline Image_ColorBGR operator+ (const Image_ColorBGR& theA, const Image_ColorBGR& theB)
{
  return Image_ColorSumm3 (theA, theB);
}

inline Image_ColorRGBF operator+ (const Image_ColorRGBF& theA, const Image_ColorRGBF& theB)
{
  return Image_ColorSumm3 (theA, theB);
}

inline Image_ColorBGRF operator+ (const Image_ColorBGRF& theA, const Image_ColorBGRF& theB)
{
  return Image_ColorSumm3 (theA, theB);
}

template<typename ColorType_t>
inline ColorType_t Image_ColorSumm4 (const ColorType_t& theA, const ColorType_t& theB)
{
  ColorType_t aRes = {{typename ColorType_t::ComponentType_t (theA.v[0] + theB.v[0]),
                       typename ColorType_t::ComponentType_t (theA.v[1] + theB.v[1]),
                       typename ColorType_t::ComponentType_t (theA.v[2] + theB.v[2]),
                       typename ColorType_t::ComponentType_t (theA.v[3] + theB.v[3])}};
  return aRes;
}

inline Image_ColorRGBA operator+ (const Image_ColorRGBA& theA, const Image_ColorRGBA& theB)
{
  return Image_ColorSumm4 (theA, theB);
}

inline Image_ColorBGRA operator+ (const Image_ColorBGRA& theA, const Image_ColorBGRA& theB)
{
  return Image_ColorSumm4 (theA, theB);
}

inline Image_ColorRGB32 operator+ (const Image_ColorRGB32& theA, const Image_ColorRGB32& theB)
{
  return Image_ColorSumm4 (theA, theB);
}

inline Image_ColorBGR32 operator+ (const Image_ColorBGR32& theA, const Image_ColorBGR32& theB)
{
  return Image_ColorSumm4 (theA, theB);
}

inline Image_ColorRGBAF operator+ (const Image_ColorRGBAF& theA, const Image_ColorRGBAF& theB)
{
  return Image_ColorSumm4 (theA, theB);
}

inline Image_ColorBGRAF operator+ (const Image_ColorBGRAF& theA, const Image_ColorBGRAF& theB)
{
  return Image_ColorSumm4 (theA, theB);
}

//! Subtraction operator
template<typename ColorType_t>
inline ColorType_t Image_ColorSub3 (const ColorType_t& theA, const ColorType_t& theB)
{
  ColorType_t aRes = {{typename ColorType_t::ComponentType_t (theA.v[0] - theB.v[0]),
                       typename ColorType_t::ComponentType_t (theA.v[1] - theB.v[1]),
                       typename ColorType_t::ComponentType_t (theA.v[2] - theB.v[2])}};
  return aRes;
}

inline Image_ColorRGB operator- (const Image_ColorRGB& theA, const Image_ColorRGB& theB)
{
  return Image_ColorSub3 (theA, theB);
}

inline Image_ColorBGR operator- (const Image_ColorBGR& theA, const Image_ColorBGR& theB)
{
  return Image_ColorSub3 (theA, theB);
}

inline Image_ColorRGBF operator- (const Image_ColorRGBF& theA, const Image_ColorRGBF& theB)
{
  return Image_ColorSub3 (theA, theB);
}

inline Image_ColorBGRF operator- (const Image_ColorBGRF& theA, const Image_ColorBGRF& theB)
{
  return Image_ColorSub3 (theA, theB);
}

template<typename ColorType_t>
inline ColorType_t Image_ColorSub4 (const ColorType_t& theA, const ColorType_t& theB)
{
  ColorType_t aRes = {{typename ColorType_t::ComponentType_t (theA.v[0] - theB.v[0]),
                       typename ColorType_t::ComponentType_t (theA.v[1] - theB.v[1]),
                       typename ColorType_t::ComponentType_t (theA.v[2] - theB.v[2]),
                       typename ColorType_t::ComponentType_t (theA.v[3] - theB.v[3])}};
  return aRes;
}

inline Image_ColorRGBA operator- (const Image_ColorRGBA& theA, const Image_ColorRGBA& theB)
{
  return Image_ColorSub4 (theA, theB);
}

inline Image_ColorBGRA operator- (const Image_ColorBGRA& theA, const Image_ColorBGRA& theB)
{
  return Image_ColorSub4 (theA, theB);
}

inline Image_ColorRGB32 operator- (const Image_ColorRGB32& theA, const Image_ColorRGB32& theB)
{
  return Image_ColorSub4 (theA, theB);
}

inline Image_ColorBGR32 operator- (const Image_ColorBGR32& theA, const Image_ColorBGR32& theB)
{
  return Image_ColorSub4 (theA, theB);
}

inline Image_ColorRGBAF operator- (const Image_ColorRGBAF& theA, const Image_ColorRGBAF& theB)
{
  return Image_ColorSub4 (theA, theB);
}

inline Image_ColorBGRAF operator- (const Image_ColorBGRAF& theA, const Image_ColorBGRAF& theB)
{
  return Image_ColorSub4 (theA, theB);
}

#endif // _Image_Color_H__
