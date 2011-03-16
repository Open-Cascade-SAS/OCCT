// File:      PlotMgt_ImageDriver.cxx
// Created:   25-NOV-98
// Author:    DCB
// Copyright: MatraDatavision 1998

#include <PlotMgt_ImageDriver.ixx>

//=============================================================
/*ARGSUSED*/
PlotMgt_ImageDriver::PlotMgt_ImageDriver (const Handle(PlotMgt_Plotter)& aPlotter,
                                          const Standard_CString aName)
                                        : PlotMgt_PlotterDriver (aPlotter, aName)
{
}

//=============================================================
/*ARGSUSED*/
PlotMgt_ImageDriver::PlotMgt_ImageDriver (const Standard_CString aName)
                                        : PlotMgt_PlotterDriver (aName)
{
}

//=============================================================
/*ARGSUSED*/
void PlotMgt_ImageDriver::BeginDraw ()
{
}

//=============================================================
/*ARGSUSED*/
void PlotMgt_ImageDriver::EndDraw (const Standard_Boolean /*dontFlush*/)
{
}
