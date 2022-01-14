// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _gp_GTrsf2d_HeaderFile
#define _gp_GTrsf2d_HeaderFile

#include <gp_Ax2d.hxx>
#include <gp_Mat2d.hxx>
#include <gp_TrsfForm.hxx>
#include <gp_XY.hxx>
#include <Standard_OutOfRange.hxx>


//! Defines a non persistent transformation in 2D space.
//! This transformation is a general transformation.
//! It can be a gp_Trsf2d, an affinity, or you can
//! define your own transformation giving the corresponding matrix of transformation.
//!
//! With a gp_GTrsf2d you can transform only a doublet of coordinates gp_XY.
//! It is not possible to transform other geometric objects
//! because these transformations can change the nature of non-elementary geometric objects.
//! A gp_GTrsf2d is represented with a 2 rows * 3 columns matrix:
//! @code
//!    V1   V2   T        XY         XY
//! | a11  a12  a14 |   | x |      | x'|
//! | a21  a22  a24 |   | y |   =  | y'|
//! |  0    0    1  |   | 1 |      | 1 |
//! @endcode
//! where {V1, V2} defines the vectorial part of the
//! transformation and T defines the translation part of the transformation.
//! Warning
//! A gp_GTrsf2d transformation is only applicable on coordinates.
//! Be careful if you apply such a transformation to all the points of a geometric object,
//! as this can change the nature of the object and thus render it incoherent!
//! Typically, a circle is transformed into an ellipse by an affinity transformation.
//! To avoid modifying the nature of an object, use a gp_Trsf2d transformation instead,
//! as objects of this class respect the nature of geometric objects.
class gp_GTrsf2d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! returns identity transformation.
  gp_GTrsf2d()
  {
    shape = gp_Identity;
    matrix.SetScale (1.0);
    loc.SetCoord (0.0, 0.0);
    scale = 1.0;
  }

  //! Converts the gp_Trsf2d transformation theT into a
  //! general transformation.
  gp_GTrsf2d (const gp_Trsf2d& theT);

  //! Creates   a transformation based on the matrix theM and the
  //! vector theV where theM defines the vectorial part of the
  //! transformation, and theV the translation part.
  gp_GTrsf2d (const gp_Mat2d& theM, const gp_XY& theV)
  : matrix (theM),
    loc (theV)
  {
    shape = gp_Other;
    scale = 0.0;
  }

  //! Changes this transformation into an affinity of ratio theRatio
  //! with respect to the axis theA.
  //! Note: An affinity is a point-by-point transformation that
  //! transforms any point P into a point P' such that if H is
  //! the orthogonal projection of P on the axis theA, the vectors
  //! HP and HP' satisfy: HP' = theRatio * HP.
  Standard_EXPORT void SetAffinity (const gp_Ax2d& theA, const Standard_Real theRatio);

  //! Replaces   the coefficient (theRow, theCol) of the matrix representing
  //! this transformation by theValue,
  //! Raises OutOfRange if theRow < 1 or theRow > 2 or theCol < 1 or theCol > 3
  void SetValue (const Standard_Integer theRow, const Standard_Integer theCol, const Standard_Real theValue);

  //! Replaces the translation part of this
  //! transformation by the coordinates of the number pair theCoord.
  Standard_EXPORT void SetTranslationPart (const gp_XY& theCoord);

  //! Assigns the vectorial and translation parts of theT to this transformation.
  void SetTrsf2d (const gp_Trsf2d& theT);

  //! Replaces the vectorial part of this transformation by theMatrix.
  void SetVectorialPart (const gp_Mat2d& theMatrix)
  {
    matrix = theMatrix;
    shape = gp_Other;
    scale = 0.0;
  }

  //! Returns true if the determinant of the vectorial part of
  //! this transformation is negative.
  Standard_Boolean IsNegative() const { return matrix.Determinant() < 0.0; }

  //! Returns true if this transformation is singular (and
  //! therefore, cannot be inverted).
  //! Note: The Gauss LU decomposition is used to invert the
  //! transformation matrix. Consequently, the transformation
  //! is considered as singular if the largest pivot found is less
  //! than or equal to gp::Resolution().
  //! Warning
  //! If this transformation is singular, it cannot be inverted.
  Standard_Boolean IsSingular() const { return matrix.IsSingular(); }

  //! Returns the nature of the transformation.  It can be
  //! an identity transformation, a rotation, a translation, a mirror
  //! transformation (relative to a point or axis), a scaling
  //! transformation, a compound transformation or some
  //! other type of transformation.
  gp_TrsfForm Form() const { return shape; }

  //! Returns the translation part of the GTrsf2d.
  const gp_XY& TranslationPart() const { return loc; }

  //! Computes the vectorial part of the GTrsf2d. The returned
  //! Matrix is a 2*2 matrix.
  const gp_Mat2d& VectorialPart() const { return matrix; }

  //! Returns the coefficients of the global matrix of transformation.
  //! Raised OutOfRange if theRow < 1 or theRow > 2 or theCol < 1 or theCol > 3
  Standard_Real Value (const Standard_Integer theRow, const Standard_Integer theCol) const;

  Standard_Real operator() (const Standard_Integer theRow, const Standard_Integer theCol) const { return Value (theRow, theCol); }

  Standard_EXPORT void Invert();

  //! Computes the reverse transformation.
  //! Raised an exception if the matrix of the transformation
  //! is not inversible.
  Standard_NODISCARD gp_GTrsf2d Inverted() const
  {
    gp_GTrsf2d aT = *this;
    aT.Invert();
    return aT;
  }

  //! Computes the transformation composed with theT and <me>.
  //! In a C++ implementation you can also write Tcomposed = <me> * theT.
  //! Example :
  //! @code
  //! gp_GTrsf2d T1, T2, Tcomp; ...............
  //! //composition :
  //! Tcomp = T2.Multiplied(T1);         // or   (Tcomp = T2 * T1)
  //! // transformation of a point
  //! gp_XY P(10.,3.);
  //! gp_XY P1(P);
  //! Tcomp.Transforms(P1);               //using Tcomp
  //! gp_XY P2(P);
  //! T1.Transforms(P2);                  //using T1 then T2
  //! T2.Transforms(P2);                  // P1 = P2 !!!
  //! @endcode
  Standard_NODISCARD gp_GTrsf2d Multiplied (const gp_GTrsf2d& theT) const
  {
    gp_GTrsf2d aTres = *this;
    aTres.Multiply (theT);
    return aTres;
  }

  Standard_NODISCARD gp_GTrsf2d operator * (const gp_GTrsf2d& theT) const { return Multiplied (theT); }

  Standard_EXPORT void Multiply (const gp_GTrsf2d& theT);

  void operator *= (const gp_GTrsf2d& theT) { Multiply (theT); }

  //! Computes the product of the transformation theT and this
  //! transformation, and assigns the result to this transformation:
  //! this = theT * this
  Standard_EXPORT void PreMultiply (const gp_GTrsf2d& theT);

  Standard_EXPORT void Power (const Standard_Integer theN);

  //! Computes the following composition of transformations
  //! <me> * <me> * .......* <me>, theN time.
  //! if theN = 0 <me> = Identity
  //! if theN < 0 <me> = <me>.Inverse() *...........* <me>.Inverse().
  //!
  //! Raises an exception if theN < 0 and if the matrix of the
  //! transformation is not inversible.
  Standard_NODISCARD gp_GTrsf2d Powered (const Standard_Integer theN) const
  {
    gp_GTrsf2d aT = *this;
    aT.Power (theN);
    return aT;
  }

  void Transforms (gp_XY& theCoord) const;

  Standard_NODISCARD gp_XY Transformed (const gp_XY& theCoord) const
  {
    gp_XY aNewCoord = theCoord;
    Transforms (aNewCoord);
    return aNewCoord;
  }

  //! Applies this transformation to the coordinates:
  //! -   of the number pair Coord, or
  //! -   X and Y.
  //!
  //! Note:
  //! -   Transforms modifies theX, theY, or the coordinate pair Coord, while
  //! -   Transformed creates a new coordinate pair.
  void Transforms (Standard_Real& theX, Standard_Real& theY) const;

  //! Converts this transformation into a gp_Trsf2d transformation.
  //! Exceptions
  //! Standard_ConstructionError if this transformation
  //! cannot be converted, i.e. if its form is gp_Other.
  Standard_EXPORT gp_Trsf2d Trsf2d() const;

private:

  gp_Mat2d matrix;
  gp_XY loc;
  gp_TrsfForm shape;
  Standard_Real scale;

};

#include <gp_Trsf2d.hxx>

//=======================================================================
//function : SetTrsf2d
// purpose :
//=======================================================================
inline void gp_GTrsf2d::SetTrsf2d (const gp_Trsf2d& theT)
{
  shape = theT.shape;
  matrix = theT.matrix;
  loc = theT.loc;
  scale = theT.scale;
}

//=======================================================================
//function : gp_GTrsf2d
// purpose :
//=======================================================================
inline gp_GTrsf2d::gp_GTrsf2d (const gp_Trsf2d& theT)
{
  shape = theT.shape;
  matrix = theT.matrix;
  loc = theT.loc;
  scale = theT.scale;
}

//=======================================================================
//function : SetValue
// purpose :
//=======================================================================
inline void gp_GTrsf2d::SetValue (const Standard_Integer theRow,
                                  const Standard_Integer theCol,
                                  const Standard_Real theValue)
{
  Standard_OutOfRange_Raise_if
    (theRow < 1 || theRow > 2 || theCol < 1 || theCol > 3, " ");
  if (theCol == 3)
  {
    loc.SetCoord (theRow, theValue);
  }
  else
  {
    matrix.SetValue (theRow, theCol, theValue);
  }
  shape = gp_Other;
}

//=======================================================================
//function : Value
// purpose :
//=======================================================================
inline Standard_Real gp_GTrsf2d::Value (const Standard_Integer theRow,
                                        const Standard_Integer theCol) const
{
  Standard_OutOfRange_Raise_if
    (theRow < 1 || theRow > 2 || theCol < 1 || theCol > 3, " ");
  if (theCol == 3)
  {
    return loc.Coord (theRow);
  }
  if (shape == gp_Other)
  {
    return matrix.Value (theRow, theCol);
  }
  return scale * matrix.Value (theRow, theCol);
}

//=======================================================================
//function : Transforms
// purpose :
//=======================================================================
inline void gp_GTrsf2d::Transforms (gp_XY& theCoord) const
{
  theCoord.Multiply (matrix);
  if (!(shape == gp_Other) && !(scale == 1.0))
  {
    theCoord.Multiply (scale);
  }
  theCoord.Add (loc);
}

//=======================================================================
//function : Transforms
// purpose :
//=======================================================================
inline void gp_GTrsf2d::Transforms (Standard_Real& theX,
                                    Standard_Real& theY) const
{
  gp_XY aDoublet(theX, theY);
  aDoublet.Multiply (matrix);
  if (!(shape == gp_Other) && !(scale == 1.0))
  {
    aDoublet.Multiply (scale);
  }
  aDoublet.Add (loc);
  aDoublet.Coord (theX, theY);
}

#endif // _gp_GTrsf2d_HeaderFile
