// Copyright (c) 2015-... OPEN CASCADE SAS
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

#ifndef _CSLib_Offset_Headerfile
#define _CSLib_Offset_Headerfile

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard.hxx>

/** \namespace CSLib_Offset
 *  \brief Provides a number of static methods to calculate values and derivatives
 *         of an offset curves and surfaces using values and derivatives of
 *         a base curve/surface.
 */
namespace CSLib_Offset
{
  /** \brief Calculate value of offset curve in 2D
   *  \param[in]  theBasePoint         point on a base curve
   *  \param[in]  theBaseDeriv         derivative on a base curve
   *  \param[in]  theOffset            size of offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative (not used)
   *  \param[out] theResPoint          point on offset curve
   */
  Standard_EXPORT void D0(const gp_Pnt2d&  theBasePoint,
                          const gp_Vec2d&  theBaseDeriv,
                          Standard_Real    theOffset,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt2d&        theResPoint);
  /** \brief Calculate value of offset curve in 3D
   *  \param[in]  theBasePoint         point on a base curve
   *  \param[in]  theBaseDeriv         derivative on a base curve
   *  \param[in]  theOffsetDirection   direction of the offset
   *  \param[in]  theOffsetValue       length of the offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative (not used)
   *  \param[out] theResPoint          point on offset curve
   */
  Standard_EXPORT void D0(const gp_Pnt&    theBasePoint,
                          const gp_Vec&    theBaseDeriv,
                          const gp_Dir&    theOffsetDirection,
                          Standard_Real    theOffsetValue,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt&          theResPoint);


  /** \brief Calculate value and the first derivative of offset curve in 2D
   *  \param[in]  theBasePoint         point on a base curve
   *  \param[in]  theBaseD1            first derivative on a base curve
   *  \param[in]  theBaseD2            second derivative on a base curve
   *  \param[in]  theOffset            size of offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative (not used)
   *  \param[out] theResPoint          point on offset curve
   *  \param[out] theResDeriv          derivative on offset curve
   */
  Standard_EXPORT void D1(const gp_Pnt2d&  theBasePoint,
                          const gp_Vec2d&  theBaseD1,
                          const gp_Vec2d&  theBaseD2,
                          Standard_Real    theOffset,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt2d&        theResPoint,
                          gp_Vec2d&        theResDeriv);
  /** \brief Calculate value and the first derivative of offset curve in 3D
   *  \param[in]  theBasePoint         point on a base curve
   *  \param[in]  theBaseD1            first derivative on a base curve
   *  \param[in]  theBaseD2            second derivative on a base curve
   *  \param[in]  theOffsetDirection   direction of the offset
   *  \param[in]  theOffsetValue       length of the offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative (not used)
   *  \param[out] theResPoint          point on offset curve
   *  \param[out] theResDeriv          derivative on offset curve
   */
  Standard_EXPORT void D1(const gp_Pnt&    theBasePoint,
                          const gp_Vec&    theBaseD1,
                          const gp_Vec&    theBaseD2,
                          const gp_Dir&    theOffsetDirection,
                          Standard_Real    theOffsetValue,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt&          theResPoint,
                          gp_Vec&          theResDeriv);


  /** \brief Calculate value and two derivatives of offset curve in 2D
   *  \param[in]  theBasePoint  point on a base curve
   *  \param[in]  theBaseD1     first derivative on a base curve
   *  \param[in]  theBaseD2     second derivative on a base curve
   *  \param[in]  theBaseD3     third derivative on a base curve
   *  \param[in]  theOffset     size of offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative
   *  \param[out] theResPoint   point on offset curve
   *  \param[out] theResD1      first derivative on offset curve
   *  \param[out] theResD2      second derivative on offset curve
   */
  Standard_EXPORT void D2(const gp_Pnt2d&  theBasePoint,
                          const gp_Vec2d&  theBaseD1,
                          const gp_Vec2d&  theBaseD2,
                          const gp_Vec2d&  theBaseD3,
                          Standard_Real    theOffset,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt2d&        theResPoint,
                          gp_Vec2d&        theResD1,
                          gp_Vec2d&        theResD2);
  /** \brief Calculate value and two derivatives of offset curve in 3D
   *  \param[in]  theBasePoint         point on a base curve
   *  \param[in]  theBaseD1            first derivative on a base curve
   *  \param[in]  theBaseD2            second derivative on a base curve
   *  \param[in]  theBaseD3            third derivative on a base curve
   *  \param[in]  theOffsetDirection   direction of the offset
   *  \param[in]  theOffsetValue       length of the offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative
   *  \param[out] theResPoint          point on offset curve
   *  \param[out] theResD1             first derivative on offset curve
   *  \param[out] theResD2             second derivative on offset curve
   */
  Standard_EXPORT void D2(const gp_Pnt&    theBasePoint,
                          const gp_Vec&    theBaseD1,
                          const gp_Vec&    theBaseD2,
                          const gp_Vec&    theBaseD3,
                          const gp_Dir&    theOffsetDirection,
                          Standard_Real    theOffsetValue,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt&          theResPoint,
                          gp_Vec&          theResD1,
                          gp_Vec&          theResD2);

  /** \brief Calculate value and three derivatives of offset curve in 2D
   *  \param[in]  theBasePoint         point on a base curve
   *  \param[in]  theBaseD1            first derivative on a base curve
   *  \param[in]  theBaseD2            second derivative on a base curve
   *  \param[in]  theBaseD3            third derivative on a base curve
   *  \param[in]  theBaseD4            fourth derivative on a base curve
   *  \param[in]  theOffset            size of offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative
   *  \param[out] theResPoint          point on offset curve
   *  \param[out] theResD1             first derivative on offset curve
   *  \param[out] theResD2             second derivative on offset curve
   *  \param[out] theResD3             third derivative on offset curve
   */
  Standard_EXPORT void D3(const gp_Pnt2d&  theBasePoint,
                          const gp_Vec2d&  theBaseD1,
                          const gp_Vec2d&  theBaseD2,
                          const gp_Vec2d&  theBaseD3,
                          const gp_Vec2d&  theBaseD4,
                          Standard_Real    theOffset,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt2d&        theResPoint,
                          gp_Vec2d&        theResD1,
                          gp_Vec2d&        theResD2,
                          gp_Vec2d&        theResD3);
  /** \brief Calculate value and three derivatives of offset curve in 3D
   *  \param[in]  theBasePoint         point on a base curve
   *  \param[in]  theBaseD1            first derivative on a base curve
   *  \param[in]  theBaseD2            second derivative on a base curve
   *  \param[in]  theBaseD3            third derivative on a base curve
   *  \param[in]  theBaseD4            fourth derivative on a base curve
   *  \param[in]  theOffsetDirection   direction of the offset
   *  \param[in]  theOffsetValue       length of the offset
   *  \param[in]  theIsDirectionChange shows that it is necessary to consider the direction of derivative
   *  \param[out] theResPoint          point on offset curve
   *  \param[out] theResD1             first derivative on offset curve
   *  \param[out] theResD2             second derivative on offset curve
   *  \param[out] theResD3             third derivative on offset curve
   */
  Standard_EXPORT void D3(const gp_Pnt&    theBasePoint,
                          const gp_Vec&    theBaseD1,
                          const gp_Vec&    theBaseD2,
                          const gp_Vec&    theBaseD3,
                          const gp_Vec&    theBaseD4,
                          const gp_Dir&    theOffsetDirection,
                          Standard_Real    theOffsetValue,
                          Standard_Boolean theIsDirectionChange,
                          gp_Pnt&          theResPoint,
                          gp_Vec&          theResD1,
                          gp_Vec&          theResD2,
                          gp_Vec&          theResD3);
}

#endif // _CSLib_Offset_Headerfile
