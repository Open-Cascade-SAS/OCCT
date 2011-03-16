#include <gp_Vec.hxx>
#include <IntImp_ConstIsoparametric.hxx>

#if !defined(WNT) || defined(__ApproxInt_DLL) || defined(__IntImp_DLL) || defined(__IntWalk_DLL) || defined(__GeomInt_DLL) || defined(__IntPatch_DLL)
 Standard_EXPORTEXTERN const IntImp_ConstIsoparametric  *ChoixRef;
#else
 Standard_IMPORT const IntImp_ConstIsoparametric  *ChoixRef;
#endif

Standard_EXPORT Standard_Boolean IntImp_ComputeTangence(const gp_Vec DPuv[],
							const Standard_Real EpsUV[],
							Standard_Real Tgduv[],
							IntImp_ConstIsoparametric TabIso[]); 







