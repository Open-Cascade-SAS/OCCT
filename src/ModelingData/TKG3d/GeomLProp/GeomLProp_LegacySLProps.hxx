#ifndef _GeomLProp_LegacySLProps_HeaderFile
#define _GeomLProp_LegacySLProps_HeaderFile

#include <LProp_Status.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class Geom_Surface;

class GeomLProp_LegacySLProps
{
public:
  DEFINE_STANDARD_ALLOC

  GeomLProp_LegacySLProps(const occ::handle<Geom_Surface>& theSurface,
                          double                           theU,
                          double                           theV,
                          int                              theDerOrder,
                          double                           theResolution);
  GeomLProp_LegacySLProps(const occ::handle<Geom_Surface>& theSurface,
                          int                              theDerOrder,
                          double                           theResolution);
  GeomLProp_LegacySLProps(int theDerOrder, double theResolution);

  void SetSurface(const occ::handle<Geom_Surface>& theSurface);
  void SetParameters(double theU, double theV);

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
  occ::handle<Geom_Surface> mySurf;
  double                    myU;
  double                    myV;
  int                       myDerOrder;
  int                       myCN;
  double                    myLinTol;
  gp_Pnt                    myPnt;
  gp_Vec                    myD1u;
  gp_Vec                    myD1v;
  gp_Vec                    myD2u;
  gp_Vec                    myD2v;
  gp_Vec                    myDuv;
  gp_Dir                    myNormal;
  double                    myMinCurv;
  double                    myMaxCurv;
  gp_Dir                    myDirMinCurv;
  gp_Dir                    myDirMaxCurv;
  double                    myMeanCurv;
  double                    myGausCurv;
  int                       mySignificantFirstDerivativeOrderU;
  int                       mySignificantFirstDerivativeOrderV;
  LProp_Status              myUTangentStatus;
  LProp_Status              myVTangentStatus;
  LProp_Status              myNormalStatus;
  LProp_Status              myCurvatureStatus;
};

#endif
