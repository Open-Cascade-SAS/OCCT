// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _MathUtils_Gauss_HeaderFile
#define _MathUtils_Gauss_HeaderFile

//! Modern math solver utilities.
namespace MathUtils
{

//! Gauss-Legendre points for n=3.
inline constexpr double THE_GAUSS_POINTS_3[] = {-0.7745966692414834, 0.0, 0.7745966692414834};

//! Gauss-Legendre weights for n=3.
inline constexpr double THE_GAUSS_WEIGHTS_3[] = {0.5555555555555556,
                                                 0.8888888888888888,
                                                 0.5555555555555556};

//! Gauss-Legendre points for n=4.
inline constexpr double THE_GAUSS_POINTS_4[] = {-0.8611363115940526,
                                                -0.3399810435848563,
                                                0.3399810435848563,
                                                0.8611363115940526};

//! Gauss-Legendre weights for n=4.
inline constexpr double THE_GAUSS_WEIGHTS_4[] = {0.3478548451374538,
                                                 0.6521451548625461,
                                                 0.6521451548625461,
                                                 0.3478548451374538};

//! Gauss-Legendre points for n=5.
inline constexpr double THE_GAUSS_POINTS_5[] = {-0.9061798459386640,
                                                -0.5384693101056831,
                                                0.0,
                                                0.5384693101056831,
                                                0.9061798459386640};

//! Gauss-Legendre weights for n=5.
inline constexpr double THE_GAUSS_WEIGHTS_5[] = {0.2369268850561891,
                                                 0.4786286704993665,
                                                 0.5688888888888889,
                                                 0.4786286704993665,
                                                 0.2369268850561891};

//! Gauss-Legendre points for n=6.
inline constexpr double THE_GAUSS_POINTS_6[] = {-0.9324695142031521,
                                                -0.6612093864662645,
                                                -0.2386191860831969,
                                                0.2386191860831969,
                                                0.6612093864662645,
                                                0.9324695142031521};

//! Gauss-Legendre weights for n=6.
inline constexpr double THE_GAUSS_WEIGHTS_6[] = {0.1713244923791704,
                                                 0.3607615730481386,
                                                 0.4679139345726910,
                                                 0.4679139345726910,
                                                 0.3607615730481386,
                                                 0.1713244923791704};

//! Gauss-Legendre points for n=7.
inline constexpr double THE_GAUSS_POINTS_7[] = {-0.9491079123427585,
                                                -0.7415311855993945,
                                                -0.4058451513773972,
                                                0.0,
                                                0.4058451513773972,
                                                0.7415311855993945,
                                                0.9491079123427585};

//! Gauss-Legendre weights for n=7.
inline constexpr double THE_GAUSS_WEIGHTS_7[] = {0.1294849661688697,
                                                 0.2797053914892766,
                                                 0.3818300505051189,
                                                 0.4179591836734694,
                                                 0.3818300505051189,
                                                 0.2797053914892766,
                                                 0.1294849661688697};

//! Gauss-Legendre points for n=8.
inline constexpr double THE_GAUSS_POINTS_8[] = {-0.9602898564975363,
                                                -0.7966664774136268,
                                                -0.5255324099163290,
                                                -0.1834346424956498,
                                                0.1834346424956498,
                                                0.5255324099163290,
                                                0.7966664774136268,
                                                0.9602898564975363};

//! Gauss-Legendre weights for n=8.
inline constexpr double THE_GAUSS_WEIGHTS_8[] = {0.1012285362903763,
                                                 0.2223810344533745,
                                                 0.3137066458778873,
                                                 0.3626837833783620,
                                                 0.3626837833783620,
                                                 0.3137066458778873,
                                                 0.2223810344533745,
                                                 0.1012285362903763};

//! Gauss-Legendre points for n=10.
inline constexpr double THE_GAUSS_POINTS_10[] = {-0.9739065285171717,
                                                 -0.8650633666889845,
                                                 -0.6794095682990244,
                                                 -0.4333953941292472,
                                                 -0.1488743389816312,
                                                 0.1488743389816312,
                                                 0.4333953941292472,
                                                 0.6794095682990244,
                                                 0.8650633666889845,
                                                 0.9739065285171717};

//! Gauss-Legendre weights for n=10.
inline constexpr double THE_GAUSS_WEIGHTS_10[] = {0.0666713443086881,
                                                  0.1494513491505806,
                                                  0.2190863625159820,
                                                  0.2692667193099963,
                                                  0.2955242247147529,
                                                  0.2955242247147529,
                                                  0.2692667193099963,
                                                  0.2190863625159820,
                                                  0.1494513491505806,
                                                  0.0666713443086881};

//! Gauss-Legendre points for n=15.
inline constexpr double THE_GAUSS_POINTS_15[] = {-0.9879925180204854,
                                                 -0.9372733924007060,
                                                 -0.8482065834104272,
                                                 -0.7244177313601701,
                                                 -0.5709721726085388,
                                                 -0.3941513470775634,
                                                 -0.2011940939974345,
                                                 0.0,
                                                 0.2011940939974345,
                                                 0.3941513470775634,
                                                 0.5709721726085388,
                                                 0.7244177313601701,
                                                 0.8482065834104272,
                                                 0.9372733924007060,
                                                 0.9879925180204854};

//! Gauss-Legendre weights for n=15.
inline constexpr double THE_GAUSS_WEIGHTS_15[] = {0.0307532419961173,
                                                  0.0703660474881081,
                                                  0.1071592204671719,
                                                  0.1395706779261543,
                                                  0.1662692058169939,
                                                  0.1861610000155622,
                                                  0.1984314853271116,
                                                  0.2025782419255613,
                                                  0.1984314853271116,
                                                  0.1861610000155622,
                                                  0.1662692058169939,
                                                  0.1395706779261543,
                                                  0.1071592204671719,
                                                  0.0703660474881081,
                                                  0.0307532419961173};

//! Gauss-Legendre points for n=21.
inline constexpr double THE_GAUSS_POINTS_21[] = {-0.9937521706203895,
                                                 -0.9672268385663063,
                                                 -0.9200993341504008,
                                                 -0.8533633645833173,
                                                 -0.7684399634756779,
                                                 -0.6671388041974123,
                                                 -0.5516188358872198,
                                                 -0.4243421202074388,
                                                 -0.2880213168024011,
                                                 -0.1455618541608951,
                                                 0.0,
                                                 0.1455618541608951,
                                                 0.2880213168024011,
                                                 0.4243421202074388,
                                                 0.5516188358872198,
                                                 0.6671388041974123,
                                                 0.7684399634756779,
                                                 0.8533633645833173,
                                                 0.9200993341504008,
                                                 0.9672268385663063,
                                                 0.9937521706203895};

//! Gauss-Legendre weights for n=21.
inline constexpr double THE_GAUSS_WEIGHTS_21[] = {
  0.0160172282577743, 0.0369537897708525, 0.0571344254268572, 0.0761001136283793,
  0.0934444234560339, 0.1087972991671484, 0.1218314160537285, 0.1322689386333375,
  0.1398873947910731, 0.1445244039899700, 0.1460811336496904, 0.1445244039899700,
  0.1398873947910731, 0.1322689386333375, 0.1218314160537285, 0.1087972991671484,
  0.0934444234560339, 0.0761001136283793, 0.0571344254268572, 0.0369537897708525,
  0.0160172282577743};

//! Gauss-Legendre points for n=31 (high precision).
inline constexpr double THE_GAUSS_POINTS_31[] = {
  -0.9970874818194770, -0.9846859096651652, -0.9625039250929496, -0.9307569978966481,
  -0.8897600299482696, -0.8399203201462673, -0.7817331484166244, -0.7157767845868534,
  -0.6427067229242604, -0.5632491614071489, -0.4781937820449025, -0.3883859016082329,
  -0.2947180699817016, -0.1981211993355706, -0.0995553121523415, 0.0,
  0.0995553121523415,  0.1981211993355706,  0.2947180699817016,  0.3883859016082329,
  0.4781937820449025,  0.5632491614071489,  0.6427067229242604,  0.7157767845868534,
  0.7817331484166244,  0.8399203201462673,  0.8897600299482696,  0.9307569978966481,
  0.9625039250929496,  0.9846859096651652,  0.9970874818194770};

//! Gauss-Legendre weights for n=31.
inline constexpr double THE_GAUSS_WEIGHTS_31[] = {
  0.0074708315792487, 0.0172953547354097, 0.0269785893254440, 0.0364259099519139,
  0.0455433538665749, 0.0542378613250555, 0.0624191330972525, 0.0700003462636801,
  0.0768994045904914, 0.0830398923041908, 0.0883519271671607, 0.0927724753653041,
  0.0962462948268430, 0.0987263019095116, 0.1001737388011984, 0.1005588858060619,
  0.1001737388011984, 0.0987263019095116, 0.0962462948268430, 0.0927724753653041,
  0.0883519271671607, 0.0830398923041908, 0.0768994045904914, 0.0700003462636801,
  0.0624191330972525, 0.0542378613250555, 0.0455433538665749, 0.0364259099519139,
  0.0269785893254440, 0.0172953547354097, 0.0074708315792487};

//! Get Gauss-Legendre points and weights for given order.
//! @param theOrder number of quadrature points (3, 4, 5, 6, 7, 8, 10, 15, 21, or 31)
//! @param[out] thePoints pointer to points array
//! @param[out] theWeights pointer to weights array
//! @return true if order is supported
inline bool GetGaussPointsAndWeights(int            theOrder,
                                     const double*& thePoints,
                                     const double*& theWeights)
{
  switch (theOrder)
  {
    case 3:
      thePoints  = THE_GAUSS_POINTS_3;
      theWeights = THE_GAUSS_WEIGHTS_3;
      return true;
    case 4:
      thePoints  = THE_GAUSS_POINTS_4;
      theWeights = THE_GAUSS_WEIGHTS_4;
      return true;
    case 5:
      thePoints  = THE_GAUSS_POINTS_5;
      theWeights = THE_GAUSS_WEIGHTS_5;
      return true;
    case 6:
      thePoints  = THE_GAUSS_POINTS_6;
      theWeights = THE_GAUSS_WEIGHTS_6;
      return true;
    case 7:
      thePoints  = THE_GAUSS_POINTS_7;
      theWeights = THE_GAUSS_WEIGHTS_7;
      return true;
    case 8:
      thePoints  = THE_GAUSS_POINTS_8;
      theWeights = THE_GAUSS_WEIGHTS_8;
      return true;
    case 10:
      thePoints  = THE_GAUSS_POINTS_10;
      theWeights = THE_GAUSS_WEIGHTS_10;
      return true;
    case 15:
      thePoints  = THE_GAUSS_POINTS_15;
      theWeights = THE_GAUSS_WEIGHTS_15;
      return true;
    case 21:
      thePoints  = THE_GAUSS_POINTS_21;
      theWeights = THE_GAUSS_WEIGHTS_21;
      return true;
    case 31:
      thePoints  = THE_GAUSS_POINTS_31;
      theWeights = THE_GAUSS_WEIGHTS_31;
      return true;
    default:
      thePoints  = nullptr;
      theWeights = nullptr;
      return false;
  }
}

} // namespace MathUtils

#endif // _MathUtils_Gauss_HeaderFile
