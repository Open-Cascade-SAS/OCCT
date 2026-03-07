#ifndef _LProp3d_LegacySLProps_HeaderFile
#define _LProp3d_LegacySLProps_HeaderFile

#include <LProp_Status.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class Adaptor3d_Surface;

class LProp3d_LegacySLProps
{
public:
  DEFINE_STANDARD_ALLOC

  LProp3d_LegacySLProps(const occ::handle<Adaptor3d_Surface>& theSurface,
                        double                                theU,
                        double                                theV,
                        int                                   theDerOrder,
                        double                                theResolution);
  const gp_Pnt& Value() const;
  const gp_Vec& D1U();
  const gp_Vec& D1V();
  const gp_Vec& D2U();
  const gp_Vec& D2V();
  const gp_Vec& DUV();
  bool IsTangentUDefined();
  void TangentU(gp_Dir& theDir);
  bool IsTangentVDefined();
  void TangentV(gp_Dir& theDir);
  bool IsNormalDefined();
  const gp_Dir& Normal();
  bool IsCurvatureDefined();
  bool IsUmbilic();
  double MaxCurvature();
  double MinCurvature();
  void CurvatureDirections(gp_Dir& theMax, gp_Dir& theMin);
  double MeanCurvature();
  double GaussianCurvature();
  LProp_Status UTangentStatus() const { return myUTangentStatus; }
  LProp_Status VTangentStatus() const { return myVTangentStatus; }
  LProp_Status NormalStatus() const { return myNormalStatus; }
  LProp_Status CurvatureStatus() const { return myCurvatureStatus; }

private:
  occ::handle<Adaptor3d_Surface> mySurf;
  double                         myU, myV;
  int                            myDerOrder, myCN;
  double                         myLinTol;
  gp_Pnt                         myPnt;
  gp_Vec                         myD1u, myD1v, myD2u, myD2v, myDuv;
  gp_Dir                         myNormal, myDirMinCurv, myDirMaxCurv;
  double                         myMinCurv, myMaxCurv, myMeanCurv, myGausCurv;
  int                            mySignificantFirstDerivativeOrderU, mySignificantFirstDerivativeOrderV;
  LProp_Status                   myUTangentStatus, myVTangentStatus, myNormalStatus, myCurvatureStatus;
};

#endif
