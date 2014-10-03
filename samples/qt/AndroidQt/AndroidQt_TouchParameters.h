// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef ANDROIDQT_TOUCHPARAMETERS_H
#define ANDROIDQT_TOUCHPARAMETERS_H

#include <QPair>

//! Class holding touch event state.
class AndroidQt_TouchParameters
{

public:

  //! Empty constructor.
  AndroidQt_TouchParameters();

  //! Default constructor.
  AndroidQt_TouchParameters (const double theX,
                             const double theY);

  //! x coord
  QPair<double, double> X() const;
  double DevX() const;

  //! y coord
  QPair<double, double> Y() const;
  double DevY() const;

  //! Start coords
  void SetStarts (const double theXStart,
                  const double theYStart);

  //! End coords
  void SetEnds (const double theXEnd,
                const double theYEnd);

  void ClearDev();

private:

  double myXStart;
  double myXEnd;

  double myYStart;
  double myYEnd;

};

#endif // ANDROIDQT_TOUCHPARAMETERS_H
