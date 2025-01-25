// Author: Ilya Khramov
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

#ifndef _Graphic3d_CubeMapOrder_HeaderFile
#define _Graphic3d_CubeMapOrder_HeaderFile

#include <Graphic3d_CubeMapSide.hxx>
#include <Standard_Macro.hxx>

class Graphic3d_ValidatedCubeMapOrder;

//! Graphic3d_CubeMapOrder maps sides of cubemap on tiles in packed cubemap image
//! to support different tiles order in such images.
//! Also it can be considered as permutation of numbers from 0 to 5.
//! It stores permutation in one integer as convolution.
class Graphic3d_CubeMapOrder
{

public:
  //! Default constructor.
  //! Creates empty order with zero convolution.
  Standard_EXPORT Graphic3d_CubeMapOrder();

  //! Initializes order with values.
  Standard_EXPORT Graphic3d_CubeMapOrder(unsigned char thePosXLocation,
                                         unsigned char theNegXLocation,
                                         unsigned char thePosYLocation,
                                         unsigned char theNegYLocation,
                                         unsigned char thePosZLocation,
                                         unsigned char theNegZLocation);

  //! Creates Graphic3d_CubeMapOrder using Graphic3d_ValidatedCubeMapOrder.
  Standard_EXPORT Graphic3d_CubeMapOrder(const Graphic3d_ValidatedCubeMapOrder& theOrder);

  //! Alias of 'operator='.
  Standard_EXPORT Graphic3d_CubeMapOrder& Set(const Graphic3d_CubeMapOrder& theOrder);

  //! Checks whether order is valid and returns object containing it.
  //! If order is invalid then exception will be thrown.
  //! This method is only way to create Graphic3d_ValidatedCubeMapOrder except copy constructor.
  Standard_EXPORT Graphic3d_ValidatedCubeMapOrder Validated() const;

public:
  //! Sets number of tile in packed cubemap image according passed cubemap side.
  Standard_EXPORT Graphic3d_CubeMapOrder& Set(Graphic3d_CubeMapSide theCubeMapSide,
                                              unsigned char         theValue);

  //! Sets default order (just from 0 to 5)
  Standard_EXPORT Graphic3d_CubeMapOrder& SetDefault();

  //! Applies another cubemap order as permutation for the current one.
  Standard_EXPORT Graphic3d_CubeMapOrder& Permute(const Graphic3d_ValidatedCubeMapOrder& anOrder);

  //! Returns permuted by other cubemap order copy of current one.
  Standard_EXPORT Graphic3d_CubeMapOrder
    Permuted(const Graphic3d_ValidatedCubeMapOrder& anOrder) const;

  //! Swaps values of two cubemap sides.
  Standard_EXPORT Graphic3d_CubeMapOrder& Swap(Graphic3d_CubeMapSide theFirstSide,
                                               Graphic3d_CubeMapSide theSecondSide);

  //! Returns copy of current order with swapped values of two cubemap sides.
  Standard_EXPORT Graphic3d_CubeMapOrder Swapped(Graphic3d_CubeMapSide theFirstSide,
                                                 Graphic3d_CubeMapSide theSecondSide) const;

  //! Returns value of passed cubemap side.
  Standard_EXPORT unsigned char Get(Graphic3d_CubeMapSide theCubeMapSide) const;

  //! Alias of 'Get'.
  Standard_EXPORT unsigned char operator[](Graphic3d_CubeMapSide theCubeMapSide) const;

  //! Makes order empty.
  Standard_EXPORT Graphic3d_CubeMapOrder& Clear();

  //! Checks whether order is empty.
  Standard_EXPORT bool IsEmpty() const;

  //! Checks whether order has repetitions.
  Standard_EXPORT bool HasRepetitions() const;

  //! Checks whether attempts to assign index greater than 5 to any side happed.
  Standard_EXPORT bool HasOverflows() const;

  //! Checks whether order is valid.
  //! Order is valid when it doesn't have repetitions
  //! and there were not attempts to assign indexes greater than 5.
  Standard_EXPORT bool IsValid() const;

public:
  //! Returns default order in protector container class.
  //! It is guaranteed to be valid.
  Standard_EXPORT static const Graphic3d_ValidatedCubeMapOrder& Default();

private:
  //! Alias of 'Get' with other parameter's type for more handful iteration.
  unsigned char get(unsigned char theCubeMapSide) const;

  //! Alias of 'set' with other parameter's type for more handful iteration and applying
  //! permutations.
  void set(unsigned char theCubeMapSide, unsigned char theValue);

  //! 'Set' without overflow's checking.
  void set(Graphic3d_CubeMapSide theCubeMapSide, unsigned char theValue);

private:
  unsigned int myConvolution;  //!< Contains all values of permutation as power convolution
  bool         myHasOverflows; //!< Indicates if there are attempts to assign index greater than 5
};

//! Graphic3d_ValidatedCubeMapOrder contains completely valid order object.
//! The only way to create this class except copy constructor is 'Validated' method of
//! Graphic3d_CubeMapOrder. This class can initialize Graphic3d_CubeMapOrder. It is supposed to be
//! used in case of necessity of completely valid order (in function argument as example). It helps
//! to automate order's valid checks.
class Graphic3d_ValidatedCubeMapOrder
{

public:
  friend class Graphic3d_CubeMapOrder;

  //! Allows skip access to 'Order' field and work directly.
  const Graphic3d_CubeMapOrder* operator->() const { return &Order; }

  //! Copy constructor.
  Graphic3d_ValidatedCubeMapOrder(const Graphic3d_ValidatedCubeMapOrder& theOther)
      : Order(theOther.Order)
  {
  }

public:
  const Graphic3d_CubeMapOrder Order; //!< Completely valid order

private:
  //! Only Graphic3d_CubeMapOrder can generate Graphic3d_ValidatedCubeMapOrder in 'Validated'
  //! method.
  Graphic3d_ValidatedCubeMapOrder(const Graphic3d_CubeMapOrder theOrder)
      : Order(theOrder)
  {
  }

  //! Deleted 'operator='
  Graphic3d_ValidatedCubeMapOrder& operator=(const Graphic3d_ValidatedCubeMapOrder&);
};

#endif // _Graphic3d_CubeMapOrder_HeaderFile
