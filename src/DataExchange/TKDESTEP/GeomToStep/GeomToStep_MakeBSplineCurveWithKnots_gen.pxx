// Created on: 1993-08-05
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

occ::handle<StepGeom_BSplineCurveWithKnots> BSWK;
int                                         Deg, N, i, Nknots, itampon;
double                                      rtampon;
occ::handle<StepGeom_CartesianPoint>        Pt = new StepGeom_CartesianPoint;
occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> Listpoints;
StepGeom_BSplineCurveForm                                              Form;
StepData_Logical                                                       Fermeture, Selfinter;
occ::handle<NCollection_HArray1<int>>                                  Mult;
occ::handle<NCollection_HArray1<double>>                               ListKnots, ListWeights;
GeomAbs_BSplKnotDistribution                                           Distribution;
StepGeom_KnotType                                                      KnotSpec;

Deg = BS->Degree();

N = BS->NbPoles();
Array1OfPnt_gen P(1, N);
BS->Poles(P);
Listpoints = new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, N);
for (i = P.Lower(); i <= P.Upper(); i++)
{
  GeomToStep_MakeCartesianPoint MkPoint(P.Value(i), theLocalFactors.LengthFactor());
  Pt = MkPoint.Value();
  Listpoints->SetValue(i, Pt);
}

Form = StepGeom_bscfUnspecified;

if (BS->IsClosed())
  Fermeture = StepData_LTrue;
else
  Fermeture = StepData_LFalse;

Selfinter = StepData_LFalse;

Nknots = BS->NbKnots();
NCollection_Array1<int> M(1, Nknots);
BS->Multiplicities(M);
Mult = new NCollection_HArray1<int>(1, Nknots);
for (i = M.Lower(); i <= M.Upper(); i++)
{
  itampon = M.Value(i);
  Mult->SetValue(i, itampon);
}

NCollection_Array1<double> K(1, Nknots);
BS->Knots(K);
ListKnots = new NCollection_HArray1<double>(1, Nknots);
for (i = K.Lower(); i <= K.Upper(); i++)
{
  rtampon = K.Value(i);
  ListKnots->SetValue(i, rtampon);
}

Distribution = BS->KnotDistribution();
if (Distribution == GeomAbs_NonUniform)
  KnotSpec = StepGeom_ktUnspecified;
else if (Distribution == GeomAbs_Uniform)
  KnotSpec = StepGeom_ktUniformKnots;
else if (Distribution == GeomAbs_QuasiUniform)
  KnotSpec = StepGeom_ktQuasiUniformKnots;
else
  KnotSpec = StepGeom_ktPiecewiseBezierKnots;

BSWK                                       = new StepGeom_BSplineCurveWithKnots;
occ::handle<TCollection_HAsciiString> name = new TCollection_HAsciiString("");
BSWK->Init(name, Deg, Listpoints, Form, Fermeture, Selfinter, Mult, ListKnots, KnotSpec);

theBSplineCurveWithKnots = BSWK;
done                     = true;
