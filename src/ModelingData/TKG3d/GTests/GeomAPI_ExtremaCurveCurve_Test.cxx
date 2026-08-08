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

#include <gtest/gtest.h>

#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <GeomAPI_ExtremaCurveSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <gp_XYZ.hxx>

// Test OCC862: GeomAPI_ExtremaCurveCurve - Extrema between BSpline and line
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC862_ExtremaBSplineAndLine)
{
  // Define BSpline curve data (from Glob_Poles, Glob_Knots, Glob_Mults)
  const int    aNbPoles       = 195;
  const double aPoles[195][3] = {{60000, 31.937047503393231, 799.36226142892554},
                                 {60000.000000027772, 16.712487623992825, 799.97053069028755},
                                 {59999.999999875639, 7.3723603687660546, 799.97042131653711},
                                 {60000.000000331296, 1.2070383839533065, 799.90534456032105},
                                 {59999.999999361171, -2.5807810139032785, 799.77442818789041},
                                 {60000.000000952597, -4.7405254527655112, 799.70146419719595},
                                 {59999.999998867599, -5.3922917628932563, 799.62943054158143},
                                 {60000.000001091154, -5.1399175234965044, 799.63364074172011},
                                 {59999.999999142601, -3.6992503431468067, 799.67179299415568},
                                 {60000.000000546061, -1.6910663098148713, 799.76799688166773},
                                 {59999.999999725333, 1.1886267067330731, 799.91818018455035},
                                 {60000.000000101914, 4.5979596894282677, 800.11782874041273},
                                 {59999.999999977204, 8.5921385739756673, 800.36980220999453},
                                 {60000.000000018619, 16.600658956791417, 800.90755378048414},
                                 {59999.999999985768, 26.040687605616604, 801.59827916977554},
                                 {60000.000000052918, 30.448324872994441, 801.9310695501531},
                                 {59999.999999883308, 35.089921752286457, 802.29194004854389},
                                 {60000.000000186978, 39.947755013962528, 802.68007678107051},
                                 {59999.99999976845, 45.005892525213071, 803.09465896567826},
                                 {60000.000000227068, 50.249778854659326, 803.53486876081706},
                                 {59999.999999821659, 55.665932960782108, 803.9998957687626},
                                 {60000.000000112479, 61.241734208767859, 804.48893829648762},
                                 {59999.999999943684, 66.965272918611916, 805.00120177704798},
                                 {60000.000000021486, 72.8252416954995, 805.53589657709654},
                                 {59999.99999999431, 78.810843745189473, 806.09223572876306},
                                 {60000.000000004045, 87.962119756404547, 806.95803383418911},
                                 {59999.99999999725, 97.349976765262795, 807.869010426567},
                                 {60000.000000017491, 100.50428991828356, 808.17759139897635},
                                 {59999.99999994829, 103.68239055379394, 808.49099684342912},
                                 {60000.000000105072, 106.88305649144334, 808.80912918631941},
                                 {59999.999999835563, 110.10508187457555, 809.13189073948308},
                                 {60000.000000207947, 113.34727572158297, 809.45918381767103},
                                 {59999.999999784653, 116.60846040037053, 809.79091039912782},
                                 {60000.000000180742, 119.88747020406682, 810.12697237079749},
                                 {59999.999999880296, 123.18314981484644, 810.46727114678265},
                                 {60000.000000059626, 126.49435287223255, 810.81170785335109},
                                 {59999.999999979838, 129.81994045902351, 811.16018308253979},
                                 {60000.000000015229, 134.8281992328765, 811.68880383859914},
                                 {59999.99999998945, 139.86373510893441, 812.2260602582727},
                                 {60000.000000059066, 141.54513692067022, 812.40609267860532},
                                 {59999.999999845008, 143.22928798266199, 812.58705944070834},
                                 {60000.000000278043, 144.91604799540772, 812.7689478797023},
                                 {59999.999999621934, 146.60527693312631, 812.95174532592102},
                                 {60000.000000407505, 148.29683501034282, 813.13543899059755},
                                 {59999.999999644526, 149.99058265886663, 813.32001613307352},
                                 {60000.000000252723, 151.68638048575326, 813.5054638442823},
                                 {59999.999999853622, 153.38408925794459, 813.69176926621947},
                                 {60000.000000068765, 155.08356985703421, 813.87891939578799},
                                 {59999.999999973967, 156.7846832610312, 814.06690122038856},
                                 {60000.00000002468, 159.33859412935516, 814.35010184114628},
                                 {59999.999999977736, 161.89555354531564, 814.63511466528189},
                                 {60000.000000145039, 162.74819471256941, 814.73031865086841},
                                 {59999.999999608634, 163.60113987644607, 814.82572069112655},
                                 {60000.000000709108, 164.45437167903589, 814.92131914784375},
                                 {59999.999999031643, 165.3078727497936, 815.0171123286641},
                                 {60000.000001042623, 166.16162573916125, 815.11309861889924},
                                 {59999.999999097396, 167.01561326831904, 815.20927628960794},
                                 {60000.000000630716, 167.86981798631649, 815.30564372626975},
                                 {59999.999999648215, 168.72422251240596, 815.40219920078994},
                                 {60000.000000151289, 169.57880948189478, 815.4989410625825},
                                 {59999.999999954052, 170.43356151298337, 815.5958676029403},
                                 {60000.000000034408, 171.71591108030151, 815.74153190228753},
                                 {59999.999999976135, 172.9985538007256, 815.88760410128577},
                                 {60000.000000129759, 173.42613177350728, 815.93633994702668},
                                 {59999.999999664251, 173.85373796967124, 815.98512067846866},
                                 {60000.00000060827, 174.28137021936112, 816.03394612011459},
                                 {59999.99999914426, 174.70902633885802, 816.08281598496671},
                                 {60000.000000969369, 175.13670416564241, 816.13173017155248},
                                 {59999.999999107844, 175.56440150794049, 816.18068832455049},
                                 {60000.00000066567, 175.9921162052986, 816.22969037578832},
                                 {59999.999999602165, 176.41984606692156, 816.27873598355484},
                                 {60000.00000018561, 176.84758892491178, 816.3278250228899},
                                 {59999.999999935775, 177.27534259516338, 816.37695722529566},
                                 {60000.000000054533, 177.91698605475608, 816.45071998561696},
                                 {59999.999999955864, 178.55864403901305, 816.52457892469988},
                                 {60000.000000281914, 178.7725313764083, 816.54920923644852},
                                 {59999.999999214248, 178.98641977813006, 816.57385016025694},
                                 {60000.000001492997, 179.20030898752373, 816.59850173903158},
                                 {59999.999997853804, 179.41419870083473, 816.62316380941468},
                                 {60000.000002424618, 179.62808869307457, 816.647836551757},
                                 {59999.999997816361, 179.84197863289955, 816.6725196831527},
                                 {60000.000001566797, 180.05586830671322, 816.69721343479125},
                                 {59999.999999121159, 180.26975739282756, 816.72191756454924},
                                 {60000.00000036486, 180.48364565146377, 816.74663219124841},
                                 {59999.999999904954, 180.69753279192835, 816.7713572079532},
                                 {60000.000000054562, 181.0183614269956, 816.8084603269557},
                                 {59999.999999978463, 181.33918632988278, 816.84558675379014},
                                 {60000.000000049782, 181.44612751539663, 816.85796481705847},
                                 {59999.999999914071, 181.55306821881811, 816.87034546240886},
                                 {60000.000000125139, 181.66000840292782, 816.88272868833144},
                                 {59999.999999843392, 181.76694804030186, 816.89511449082431},
                                 {60000.000000167289, 181.87388708628473, 816.90750286566583},
                                 {59999.999999847467, 181.98082551962966, 816.9198938103757},
                                 {60000.000000119864, 182.08776329350809, 816.93228732197861},
                                 {59999.999999917491, 182.19470038386558, 816.94468339453306},
                                 {60000.000000050633, 182.30163675019068, 816.9570820285785},
                                 {59999.999999972366, 182.40857236133851, 816.96948321649302},
                                 {60000.000000032509, 182.56897459276843, 816.98808882907122},
                                 {59999.999999965934, 182.7293749693155, 817.00670017517837},
                                 {60000.000000238462, 182.78284155215263, 817.01290459816312},
                                 {59999.999999345615, 182.83630791626743, 817.01910964099045},
                                 {60000.00000119608, 182.88977407140584, 817.02531535916364},
                                 {59999.999998362182, 182.94323998619015, 817.03152164472851},
                                 {60000.000001757064, 182.99670569682132, 817.03772865736005},
                                 {59999.999998499035, 183.05017115047181, 817.04393620474377},
                                 {60000.00000101691, 183.10363639052409, 817.05014447471717},
                                 {59999.999999469313, 183.15710137432194, 817.05635331513508},
                                 {60000.000000196254, 183.21056612270786, 817.06256282498509},
                                 {59999.999999961823, 183.26403061846696, 817.06877295237007},
                                 {60000.000000031148, 183.37095910597188, 817.0811944807366},
                                 {59999.999999975873, 183.47788656385964, 817.09361853568328},
                                 {60000.000000078871, 183.53291535756762, 817.10001332133618},
                                 {59999.999999822408, 183.57465447580566, 817.10486251586156},
                                 {60000.000000306398, 183.67093862701302, 817.11605868308925},
                                 {59999.999999580868, 183.62493711493508, 817.11070057755603},
                                 {60000.000000457883, 183.83983154073323, 817.13569840343439},
                                 {59999.999999600586, 183.70298550843307, 817.11977179379392},
                                 {60000.000000274122, 183.92091406059222, 817.14512405357789},
                                 {59999.999999859414, 183.87191278066462, 817.13941945054194},
                                 {60000.000000045809, 183.96882854112027, 817.15069318206758},
                                 {59999.99999999682, 184.01172713386404, 817.15568252551441},
                                 {60000.000000002117, 199.05860625274244, 818.90605990894926},
                                 {60000.000000011685, 213.77441984052732, 820.66779572748067},
                                 {59999.999999945896, 228.63273752574398, 822.50191665906459},
                                 {60000.000000137385, 243.60930053773387, 824.4160390659564},
                                 {59999.999999745312, 257.44864608947171, 826.20557537898253},
                                 {60000.000000371336, 272.37732017567259, 828.28374145687258},
                                 {59999.999999564294, 283.4609507719199, 829.76911975631128},
                                 {60000.000000411754, 297.87730027160165, 831.94635758332913},
                                 {59999.999999691667, 308.39894455125989, 833.48120705572103},
                                 {60000.000000175751, 321.04689540263291, 835.47802201470222},
                                 {59999.999999930937, 332.7974681685576, 837.36295119087083},
                                 {60000.000000012922, 344.23852019267594, 839.26003269871717},
                                 {59999.999999978521, 380.87159046869039, 845.50264944092885},
                                 {60000.000000031927, 417.67053058140004, 852.32838657260925},
                                 {59999.999999895757, 443.71876268540638, 857.33613824789597},
                                 {60000.00000023668, 470.42282907220516, 863.40925211937144},
                                 {59999.999999591026, 506.36442771103123, 868.70489233101273},
                                 {60000.000000559063, 504.29569767668107, 873.82665736030435},
                                 {59999.999999387175, 618.22680165126303, 889.69085497467483},
                                 {60000.00000054048, 520.96760954336719, 878.60259543512541},
                                 {59999.999999620944, 695.15397128954748, 909.33199431749756},
                                 {60000.000000202854, 652.21259368585868, 902.32746593221361},
                                 {59999.999999925931, 713.31441817738153, 914.25671409207371},
                                 {60000.000000011525, 744.93626878652026, 920.45067498376648},
                                 {59999.999999986139, 851.85145355849977, 941.08553924856312},
                                 {60000.000000015752, 955.13474569084099, 960.32812261133847},
                                 {59999.99999996031, 1019.9477908992111, 972.25457694191095},
                                 {60000.000000078573, 1109.7424197388102, 989.27219905591903},
                                 {59999.999999866697, 1103.4472241364576, 986.24901738812696},
                                 {60000.000000196575, 1283.5502566789903, 1025.857653448167},
                                 {59999.999999756859, 1241.170752696316, 1010.5034788173898},
                                 {60000.000000241002, 1380.0598100152401, 1044.321387312001},
                                 {59999.999999814674, 1417.3346489504095, 1047.4285724180056},
                                 {60000.000000106025, 1498.5882349740116, 1064.7148393296281},
                                 {59999.999999959189, 1562.4075471540843, 1076.5692870450682},
                                 {60000.000000006847, 1632.0446827427525, 1089.9951747091934},
                                 {59999.999999992855, 1819.187180960379, 1125.8766969823027},
                                 {60000.000000006774, 2005.7867190244558, 1161.6893152514506},
                                 {59999.999999993102, 2127.1610378717864, 1184.8982806691761},
                                 {59999.999999983789, 2224.9504474907289, 1204.7794295042536},
                                 {60000.000000076092, 2423.4034586285211, 1239.2849934128672},
                                 {59999.999999843334, 2352.5108743508281, 1233.9178625300717},
                                 {60000.00000021555, 2789.1958451831979, 1304.7841887599538},
                                 {59999.999999781059, 2516.5832651666447, 1267.6317850640198},
                                 {60000.000000168911, 2997.8479508975411, 1347.2273537129538},
                                 {59999.999999901927, 2872.9560675152225, 1329.8998228537541},
                                 {60000.000000040534, 3100.1634765888271, 1371.640798220885},
                                 {59999.999999990199, 3186.6552406648925, 1389.3336814233451},
                                 {60000.000000019143, 3570.5318534479206, 1462.8626017289478},
                                 {59999.999999969914, 3906.3811637172125, 1522.697974792497},
                                 {60000.000000105116, 4081.0496155485694, 1554.1195061318422},
                                 {59999.999999764274, 4440.4680391955444, 1682.9362787226369},
                                 {60000.000000389882, 4025.4579062184348, 1482.6543564450828},
                                 {59999.999999500047, 5340.2058728577267, 2096.5041911809035},
                                 {60000.000000509819, 4233.9915572764976, 1518.9299243967394},
                                 {59999.999999583706, 5430.8345183928068, 2200.8401562891845},
                                 {60000.000000267981, 5054.5402162751188, 1969.4793692255021},
                                 {59999.999999871681, 5428.966058504895, 2244.2141001944756},
                                 {60000.000000038024, 5546.4649349764723, 2352.3606428637349},
                                 {59999.999999998952, 5706.4753141683368, 2517.0835536242184},
                                 {59999.999999991771, 6129.2268892602051, 3030.8327236291348},
                                 {60000.000000020089, 6420.3065439166912, 3661.6495160497966},
                                 {59999.99999990526, 6235.8646808219391, 3444.1541944672813},
                                 {60000.000000247288, 8126.8536394010316, 8890.661747328244},
                                 {59999.99999954299, 2104.925285609057, -7996.1134321147983},
                                 {60000.00000064441, 14264.475617875083, 34652.739991203249},
                                 {59999.999999283253, -1684.3290491164355, -33238.811620330394},
                                 {60000.000000641696, 13782.902589951687, 46532.286752882486},
                                 {59999.999999532913, 2699.6074087223524, -19079.410940396814},
                                 {60000.000000275439, 9084.6892111341349, 21937.827319607131},
                                 {59999.999999871587, 6474.8937324409353, 5006.3713661893671},
                                 {60000.000000044485, 7350.4434791870981, 10709.698171043074},
                                 {59999.999999990541, 7306.414330937022, 10560.108503883888},
                                 {60000, 7397.0783263606427, 11218.963217145942}};

  const int    aNbKnots   = 17;
  const double aKnots[17] = {0,
                             0.0087664292723375857,
                             0.015654339342331427,
                             0.019098294377328347,
                             0.020820271894826808,
                             0.021681260653576038,
                             0.022111755032950653,
                             0.022327002222637962,
                             0.022434625817481617,
                             0.022488437614903441,
                             0.022542249412325268,
                             0.037523693790797889,
                             0.071526893170125505,
                             0.1421299556831544,
                             0.26514857375331263,
                             0.51350664396810353,
                             1};

  const int aMults[17] = {15, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 15};

  // Fill array of poles
  NCollection_Array1<gp_Pnt> aPolesArray(1, aNbPoles);
  for (int i = 0; i < aNbPoles; i++)
  {
    aPolesArray.SetValue(i + 1, gp_Pnt(aPoles[i][0], aPoles[i][1], aPoles[i][2]));
  }

  // Fill array of knots
  NCollection_Array1<double> aKnotsArray(1, aNbKnots);
  for (int i = 0; i < aNbKnots; i++)
  {
    aKnotsArray.SetValue(i + 1, aKnots[i]);
  }

  // Fill array of mults
  NCollection_Array1<int> aMultsArray(1, aNbKnots);
  for (int i = 0; i < aNbKnots; i++)
  {
    aMultsArray.SetValue(i + 1, aMults[i]);
  }

  // Create B-Spline curve
  const int                      aDegree = 14;
  occ::handle<Geom_BSplineCurve> aC1 =
    new Geom_BSplineCurve(aPolesArray, aKnotsArray, aMultsArray, aDegree);

  // Create trimmed line
  gp_XYZ                         aP1(60000, -7504.83, 6000);
  gp_XYZ                         aP2(60000, 7504.83, 6000);
  occ::handle<Geom_Line>         aLine = new Geom_Line(gp_Pnt(aP1), gp_Dir(aP2 - aP1));
  occ::handle<Geom_TrimmedCurve> aC2   = new Geom_TrimmedCurve(aLine, 0.0, (aP2 - aP1).Modulus());

  // Try to find extrema
  // IMPORTANT: it is not allowed to input infinite curves!
  GeomAPI_ExtremaCurveCurve anEx(aC1, aC2);

  // Check if curves are parallel
  if (anEx.Extrema().IsParallel())
  {
    // Parallel case - should have infinite number of extrema
    EXPECT_GT(anEx.LowerDistance(), 0.0) << "Parallel curves should have a distance";
  }
  else
  {
    // Check if extrema were found
    const int aNbEx = anEx.NbExtrema();
    EXPECT_GT(aNbEx, 0) << "Extrema should be found between BSpline and line";

    if (aNbEx > 0)
    {
      // Get minimal distance data
      gp_Pnt aPoint1, aPoint2;
      anEx.NearestPoints(aPoint1, aPoint2);

      double aU1, aU2;
      anEx.LowerDistanceParameters(aU1, aU2);

      const double aDistance = anEx.LowerDistance();

      // Verify minimal distance is reasonable
      EXPECT_GE(aDistance, 0.0) << "Minimal distance should be non-negative";

      // Verify parameters are within valid range [0, 1] for the curves
      EXPECT_GE(aU1, 0.0) << "Parameter U1 should be non-negative";
      EXPECT_LE(aU1, 1.0) << "Parameter U1 should not exceed 1.0";
      EXPECT_GE(aU2, 0.0) << "Parameter U2 should be non-negative";

      // Verify nearest points are valid
      EXPECT_FALSE(aPoint1.IsEqual(gp_Pnt(0, 0, 0), 1e-10)
                   && aPoint2.IsEqual(gp_Pnt(0, 0, 0), 1e-10))
        << "Nearest points should not both be at origin";
    }
  }
}

// Migrated from tests/lowalgos/extcc/bug26269_1.  The finite trimming is
// essential: the corresponding infinite lines have infinitely many extrema.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC26269_1_TrimmedParallelLines)
{
  const occ::handle<Geom_Line> aLine1 = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const occ::handle<Geom_Line> aLine2 = new Geom_Line(gp_Pnt(2.0, 2.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const occ::handle<Geom_TrimmedCurve> aTrimmed1 = new Geom_TrimmedCurve(aLine1, 0.0, 1.0);
  const occ::handle<Geom_TrimmedCurve> aTrimmed2 = new Geom_TrimmedCurve(aLine2, 0.0, 1.0);

  GeomAPI_ExtremaCurveCurve anExtrema(aTrimmed1, aTrimmed2);
  ASSERT_FALSE(anExtrema.Extrema().IsParallel());
  ASSERT_EQ(anExtrema.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema.Distance(1), std::sqrt(5.0), 1.e-12);
}

// Migrated from tests/lowalgos/extcc/bug26269_2.  Restrict the two concentric
// circles to disjoint arcs and verify that the finite extrema are computed.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC26269_2_TrimmedConcentricArcs)
{
  const occ::handle<Geom_Circle> aCircle1 =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 10.0);
  const occ::handle<Geom_Circle> aCircle2 =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  const occ::handle<Geom_TrimmedCurve> aTrimmed1 = new Geom_TrimmedCurve(aCircle1, 0.0, 1.0);
  const occ::handle<Geom_TrimmedCurve> aTrimmed2 = new Geom_TrimmedCurve(aCircle2, 3.0, 4.0);

  GeomAPI_ExtremaCurveCurve anExtrema(aTrimmed1, aTrimmed2);
  ASSERT_FALSE(anExtrema.Extrema().IsParallel());
  ASSERT_EQ(anExtrema.NbExtrema(), 1);
  EXPECT_GT(anExtrema.Distance(1), 0.0);
}

namespace
{
void checkOCC29426CircleSphere(const double theCircleCenterY,
                               const double theCircleCenterZ,
                               const double theSphereRadius)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, theCircleCenterY, theCircleCenterZ),
                           gp_Dir(-3.67401855705945e-17, -0.943656647634893, 0.330926172090505),
                           gp_Dir(-2.2723161366922e-16, 0.330926172090505, 0.943656647634893)),
                    1.30000000000039);
  const occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(
    gp_Ax3(gp_Pnt(0.0, 2.425, 0.0), gp_Dir(0.0, 1.0, 0.0), gp_Dir(1.0, 0.0, 0.0)),
    theSphereRadius);

  GeomAPI_ExtremaCurveSurface anExtrema(aCircle, aSphere);
  ASSERT_TRUE(anExtrema.Extrema().IsDone());

  if (anExtrema.NbExtrema() > 0)
  {
    double aMinimumDistance = anExtrema.Distance(1);
    for (int anIndex = 2; anIndex <= anExtrema.NbExtrema(); ++anIndex)
    {
      const double aDistance = anExtrema.Distance(anIndex);
      aMinimumDistance       = aDistance < aMinimumDistance ? aDistance : aMinimumDistance;
    }
    EXPECT_NEAR(aMinimumDistance, 0.0, 1.e-10);
  }
}

static occ::handle<Geom_BSplineCurve> makeLinearBSpline(const gp_Pnt& thePoint1,
                                                        const gp_Pnt& thePoint2)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1) = thePoint1;
  aPoles(2) = thePoint2;

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities(1) = 2;
  aMultiplicities(2) = 2;
  return new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
}

static void checkOCC29712ConcentricCircles(const double theFirstCircleLastParameter,
                                           const double theSecondCircleFirstParameter,
                                           const double theSecondCircleLastParameter)
{
  const occ::handle<Geom_Circle> aCircle1 =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 100.0);
  const occ::handle<Geom_Circle> aCircle2 =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 50.0);
  const occ::handle<Geom_TrimmedCurve> aTrimmed1 =
    new Geom_TrimmedCurve(aCircle1, 0.0, theFirstCircleLastParameter);
  const occ::handle<Geom_TrimmedCurve> aTrimmed2 =
    new Geom_TrimmedCurve(aCircle2, theSecondCircleFirstParameter, theSecondCircleLastParameter);

  GeomAPI_ExtremaCurveCurve anExtrema12(aTrimmed1, aTrimmed2);
  ASSERT_TRUE(anExtrema12.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema12.LowerDistance(), 50.0, 1.e-7);

  GeomAPI_ExtremaCurveCurve anExtrema21(aTrimmed2, aTrimmed1);
  ASSERT_TRUE(anExtrema21.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema21.LowerDistance(), 50.0, 1.e-7);
}

static void checkParallelExtrema(const occ::handle<Geom_Curve>& theCurve1,
                                 const occ::handle<Geom_Curve>& theCurve2,
                                 const double                   theExpectedDistance)
{
  GeomAPI_ExtremaCurveCurve anExtrema12(theCurve1, theCurve2);
  ASSERT_TRUE(anExtrema12.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema12.LowerDistance(), theExpectedDistance, 1.e-8);

  GeomAPI_ExtremaCurveCurve anExtrema21(theCurve2, theCurve1);
  ASSERT_TRUE(anExtrema21.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema21.LowerDistance(), theExpectedDistance, 1.e-8);
}

static void checkOneFiniteExtremum(const occ::handle<Geom_Curve>& theCurve1,
                                   const occ::handle<Geom_Curve>& theCurve2,
                                   const double                   theExpectedDistance)
{
  GeomAPI_ExtremaCurveCurve anExtrema12(theCurve1, theCurve2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  ASSERT_EQ(anExtrema12.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema12.Distance(1), theExpectedDistance, 2.e-8);

  GeomAPI_ExtremaCurveCurve anExtrema21(theCurve2, theCurve1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  ASSERT_EQ(anExtrema21.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema21.Distance(1), theExpectedDistance, 2.e-8);
}

static occ::handle<Geom_Curve> makeOCC29712TrimmedCircle(const double theRadius,
                                                         const double theFirstParameter,
                                                         const double theLastParameter)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), theRadius);
  return new Geom_TrimmedCurve(aCircle, theFirstParameter, theLastParameter);
}

static occ::handle<Geom_Curve> makeOCC29712MirroredTrimmedCircle(const double theRadius,
                                                                 const double theFirstParameter,
                                                                 const double theLastParameter)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), theRadius);
  gp_Trsf aMirror;
  aMirror.SetMirror(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)));
  aCircle->Transform(aMirror);
  return new Geom_TrimmedCurve(aCircle, theFirstParameter, theLastParameter);
}

static void checkFiniteConcentricExtremum(const double theSecondCircleFirstParameter,
                                          const double theSecondCircleLastParameter)
{
  const occ::handle<Geom_Curve> aCurve1 = makeOCC29712TrimmedCircle(100.0, 0.0, 3.0);
  const occ::handle<Geom_Curve> aCurve2 =
    makeOCC29712TrimmedCircle(50.0, theSecondCircleFirstParameter, theSecondCircleLastParameter);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCurve1, aCurve2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  ASSERT_EQ(anExtrema12.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema12.Distance(1), 50.0, 1.e-7);

  GeomAPI_ExtremaCurveCurve anExtrema21(aCurve2, aCurve1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  ASSERT_EQ(anExtrema21.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema21.Distance(1), 50.0, 1.e-7);
}

static void checkFiniteConcentricExtremumExists(const double theSecondCircleFirstParameter,
                                                const double theSecondCircleLastParameter)
{
  const occ::handle<Geom_Curve> aCurve1 = makeOCC29712TrimmedCircle(100.0, 0.0, 3.0);
  const occ::handle<Geom_Curve> aCurve2 =
    makeOCC29712TrimmedCircle(50.0, theSecondCircleFirstParameter, theSecondCircleLastParameter);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCurve1, aCurve2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  EXPECT_GT(anExtrema12.NbExtrema(), 0);

  GeomAPI_ExtremaCurveCurve anExtrema21(aCurve2, aCurve1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  EXPECT_GT(anExtrema21.NbExtrema(), 0);
}
} // namespace

// Migrated from tests/lowalgos/extcs/bug29426_1.  The source DRAW test
// accepts an explicit intersection-point report as well as a zero extremum.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29426_1_CircleSphereMinimumDistance)
{
  checkOCC29426CircleSphere(11.6635040382361, -3.23970209548358, 9.876);
}

// Migrated from tests/lowalgos/extcs/bug29426_2.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29426_2_CircleSphereMinimumDistance)
{
  checkOCC29426CircleSphere(11.5073628633636, -3.18494573739955, 9.712);
}

// Migrated from tests/lowalgos/extcc/bug29712_2.  Bounded parallel B-splines
// must be treated as finite curves rather than as an infinite parallel pair.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_2_BoundedParallelBSplines)
{
  const occ::handle<Geom_BSplineCurve> aCurve1 =
    makeLinearBSpline(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(1.0, 0.0, 0.0));
  const occ::handle<Geom_BSplineCurve> aCurve2 =
    makeLinearBSpline(gp_Pnt(3.0, 4.0, 0.0), gp_Pnt(4.0, 4.0, 0.0));

  GeomAPI_ExtremaCurveCurve anExtrema(aCurve1, aCurve2);
  ASSERT_FALSE(anExtrema.Extrema().IsParallel());
  ASSERT_EQ(anExtrema.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema.Distance(1), std::sqrt(20.0), 2.e-8);
}

// Migrated from tests/lowalgos/extcc/bug29712_10.  Keep both argument orders
// covered because the original regression checked both extrema orientations.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_10_BoundedParallelLines)
{
  const occ::handle<Geom_Line> aLine1 = new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const occ::handle<Geom_Line> aLine2 = new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const occ::handle<Geom_TrimmedCurve> aTrimmed1 = new Geom_TrimmedCurve(aLine1, -1.0e100, 5.0);
  const occ::handle<Geom_TrimmedCurve> aTrimmed2 = new Geom_TrimmedCurve(aLine2, 5.0, 20.0);

  GeomAPI_ExtremaCurveCurve anExtrema12(aTrimmed1, aTrimmed2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  ASSERT_EQ(anExtrema12.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema12.Distance(1), 4.0, 2.e-8);

  GeomAPI_ExtremaCurveCurve anExtrema21(aTrimmed2, aTrimmed1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  ASSERT_EQ(anExtrema21.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema21.Distance(1), 4.0, 2.e-8);
}

// Migrated from tests/lowalgos/extcc/bug29712_20.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_20_ConcentricTrimmedCircles)
{
  checkOCC29712ConcentricCircles(3.0, -3.3, -1.3);
}

// Migrated from tests/lowalgos/extcc/bug29712_30.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_30_ConcentricTrimmedCircles)
{
  checkOCC29712ConcentricCircles(3.0, 4.3, 6.3);
}

// Migrated from tests/lowalgos/extcc/bug29712_40.  A short segment on the
// axis of a circle is still reported as an infinite-extrema configuration.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_40_CircleAndAxisSegment)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const occ::handle<Geom_TrimmedCurve> aSegment = new Geom_TrimmedCurve(aLine, -1.0, -5.e-8);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCircle, aSegment);
  ASSERT_TRUE(anExtrema12.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema12.LowerDistance(), 5.0, 1.e-7);

  GeomAPI_ExtremaCurveCurve anExtrema21(aSegment, aCircle);
  ASSERT_TRUE(anExtrema21.Extrema().IsParallel());
  EXPECT_NEAR(anExtrema21.LowerDistance(), 5.0, 1.e-7);
}

// Migrated from tests/lowalgos/extcc/bug29712_43.  The reversed tiny line
// segment has no extrema in either argument order.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_43_CircleAndDegenerateSegment)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  const occ::handle<Geom_TrimmedCurve> aSegment = new Geom_TrimmedCurve(aLine, -1.5e-7, -1.0);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCircle, aSegment);
  EXPECT_FALSE(anExtrema12.Extrema().IsParallel());
  EXPECT_EQ(anExtrema12.NbExtrema(), 0);

  GeomAPI_ExtremaCurveCurve anExtrema21(aSegment, aCircle);
  EXPECT_FALSE(anExtrema21.Extrema().IsParallel());
  EXPECT_EQ(anExtrema21.NbExtrema(), 0);
}

// Migrated from tests/lowalgos/extcc/bug29712_3.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_3_BoundedParallelSegments)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), 0.0, 1.0);
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(3.0, 4.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), 0.0, 1.0);
  checkOneFiniteExtremum(aCurve1, aCurve2, std::sqrt(20.0));
}

// Migrated from tests/lowalgos/extcc/bug29712_4.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_4_ParallelTrimmedLines)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), 0.0, 1.0);
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 4.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), 0.0, 1.0);
  checkParallelExtrema(aCurve1, aCurve2, 4.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_5.  The source checks that both
// orientations still produce a finite result for separated trimmed lines.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_5_SeparatedTrimmedLines)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                          -1.0e100,
                          5.0);
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 10.0, 20.0);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCurve1, aCurve2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  EXPECT_EQ(anExtrema12.NbExtrema(), 1);
  GeomAPI_ExtremaCurveCurve anExtrema21(aCurve2, aCurve1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  EXPECT_EQ(anExtrema21.NbExtrema(), 1);
}

// Migrated from tests/lowalgos/extcc/bug29712_6.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_6_ParallelLines)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_Line(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 1.0, 0.0));
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_Line(gp_Pnt(10.0, 9.0, 8.0), gp_Dir(0.0, 1.0, 0.0));
  checkParallelExtrema(aCurve1, aCurve2, std::sqrt(106.0));
}

// Migrated from tests/lowalgos/extcc/bug29712_7.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_7_ParallelTrimmedLine)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_Line(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 1.0, 0.0));
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(10.0, 9.0, 8.0), gp_Dir(0.0, 1.0, 0.0)), -1.0, 2.0);
  checkParallelExtrema(aCurve1, aCurve2, std::sqrt(106.0));
}

// Migrated from tests/lowalgos/extcc/bug29712_8.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_8_ParallelTrimmedFirstLine)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1.0, 2.0, 3.0), gp_Dir(0.0, 1.0, 0.0)),
                          -100.0,
                          200.0);
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_Line(gp_Pnt(10.0, 9.0, 8.0), gp_Dir(0.0, 1.0, 0.0));
  checkParallelExtrema(aCurve1, aCurve2, std::sqrt(106.0));
}

// Migrated from tests/lowalgos/extcc/bug29712_9.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_9_OppositeDirectionLines)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, -1.0)), -5.0, 2.0);
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                          -1.0e100,
                          9.9e-8);
  checkParallelExtrema(aCurve1, aCurve2, 4.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_11.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_11_SeparatedOppositeLines)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 0.0, 3.0);
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 0.0, -1.0)),
                          -9.9e-8,
                          1.0e100);
  checkOneFiniteExtremum(aCurve1, aCurve2, 4.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_12.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_12_OppositeTrimmedSegments)
{
  const occ::handle<Geom_Curve> aCurve1 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), 0.0, 1.0);
  const occ::handle<Geom_Curve> aCurve2 =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 4.0, 0.0), gp_Dir(-1.0, 0.0, 0.0)), -2.0, -1.0);
  checkOneFiniteExtremum(aCurve1, aCurve2, 4.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_13.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_13_SeparatedConcentricArcs)
{
  checkFiniteConcentricExtremumExists(4.0, 6.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_14.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_14_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, -8.1, -6.1);
}

// Migrated from tests/lowalgos/extcc/bug29712_15.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_15_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, -1.9, 0.1);
}

// Migrated from tests/lowalgos/extcc/bug29712_16.  The common radial arc
// produces a finite minimum and must not be reported as infinitely parallel.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_16_FiniteConcentricMinimum)
{
  checkFiniteConcentricExtremum(-2.0, 0.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_17.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_17_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, -3.6, -1.6);
}

// Migrated from tests/lowalgos/extcc/bug29712_18.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_18_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, -3.5, -1.5);
}

// Migrated from tests/lowalgos/extcc/bug29712_19.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_19_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, -3.4, -1.4);
}

// Migrated from tests/lowalgos/extcc/bug29712_21.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_21_SeparatedConcentricArcs)
{
  checkFiniteConcentricExtremumExists(-3.2, -1.2);
}

// Migrated from tests/lowalgos/extcc/bug29712_22.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_22_SeparatedConcentricArcs)
{
  checkFiniteConcentricExtremumExists(-3.1, -1.1);
}

// Migrated from tests/lowalgos/extcc/bug29712_23.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_23_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, -9.6, -7.6);
}

// Migrated from tests/lowalgos/extcc/bug29712_24.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_24_SeparatedConcentricArcs)
{
  checkFiniteConcentricExtremumExists(-9.5, -7.5);
}

// Migrated from tests/lowalgos/extcc/bug29712_25.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_25_SeparatedConcentricArcs)
{
  checkFiniteConcentricExtremumExists(-8.3, -6.3);
}

// Migrated from tests/lowalgos/extcc/bug29712_26.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_26_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, -8.2, -6.2);
}

// Migrated from tests/lowalgos/extcc/bug29712_27.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_27_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(3.0, 0.0, 2.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_28.  The crossing arc pair
// must return one finite extremum rather than an infinite solution set.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_28_FiniteConcentricMinimum)
{
  checkFiniteConcentricExtremum(3.0, 5.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_29.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_29_SeparatedConcentricArcs)
{
  checkFiniteConcentricExtremumExists(3.1, 5.1);
}

// Migrated from tests/lowalgos/extcc/bug29712_31.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_31_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(M_PI, -9.5, -7.5);
}

// Migrated from tests/lowalgos/extcc/bug29712_32.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_32_SeparatedConcentricArcs)
{
  checkFiniteConcentricExtremumExists(-9.4, -7.4);
}

// Migrated from tests/lowalgos/extcc/bug29712_33.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_33_ParallelConcentricArcs)
{
  checkOCC29712ConcentricCircles(M_PI, 0.0, 2.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_34.  The bounded concentric
// arcs intentionally exercise the asymmetric extrema enumeration order.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_34_AsymmetricConcentricArcs)
{
  const occ::handle<Geom_Curve> aCurve1 = makeOCC29712TrimmedCircle(100.0, 0.0, M_PI);
  const occ::handle<Geom_Curve> aCurve2 = makeOCC29712TrimmedCircle(50.0, -2.0, 0.0);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCurve1, aCurve2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  ASSERT_EQ(anExtrema12.NbExtrema(), 2);
  const double aDistance121 = anExtrema12.Distance(1);
  const double aDistance122 = anExtrema12.Distance(2);
  EXPECT_NEAR(std::min(aDistance121, aDistance122), 50.0, 1.e-7);
  EXPECT_NEAR(std::max(aDistance121, aDistance122), 150.0, 1.e-7);

  GeomAPI_ExtremaCurveCurve anExtrema21(aCurve2, aCurve1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  ASSERT_EQ(anExtrema21.NbExtrema(), 1);
  EXPECT_NEAR(anExtrema21.Distance(1), 50.0, 1.e-7);
}

// Migrated from tests/lowalgos/extcc/bug29712_35.  Both argument orders
// produce the two finite radial extrema for these opposite half-arcs.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_35_OppositeConcentricArcs)
{
  const occ::handle<Geom_Curve> aCurve1 = makeOCC29712TrimmedCircle(100.0, 0.0, M_PI);
  const occ::handle<Geom_Curve> aCurve2 = makeOCC29712TrimmedCircle(50.0, M_PI, 2.0 * M_PI);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCurve1, aCurve2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  ASSERT_EQ(anExtrema12.NbExtrema(), 2);
  const double aDistance121 = anExtrema12.Distance(1);
  const double aDistance122 = anExtrema12.Distance(2);
  EXPECT_NEAR(std::min(aDistance121, aDistance122), 50.0, 1.e-7);
  EXPECT_NEAR(std::max(aDistance121, aDistance122), 150.0, 1.e-7);

  GeomAPI_ExtremaCurveCurve anExtrema21(aCurve2, aCurve1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  ASSERT_EQ(anExtrema21.NbExtrema(), 2);
  const double aDistance211 = anExtrema21.Distance(1);
  const double aDistance212 = anExtrema21.Distance(2);
  EXPECT_NEAR(std::min(aDistance211, aDistance212), 50.0, 1.e-7);
  EXPECT_NEAR(std::max(aDistance211, aDistance212), 150.0, 1.e-7);
}

// Migrated from tests/lowalgos/extcc/bug29712_36.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_36_ParallelFullCircleAndArc)
{
  checkOCC29712ConcentricCircles(2.0 * M_PI, -12.0, -10.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_37.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_37_SeparatedConcentricArcs)
{
  const occ::handle<Geom_Curve> aCurve1 = makeOCC29712MirroredTrimmedCircle(100.0, 3.0, 6.5);
  const occ::handle<Geom_Curve> aCurve2 = makeOCC29712TrimmedCircle(50.0, 4.0, 6.0);
  GeomAPI_ExtremaCurveCurve     anExtrema12(aCurve1, aCurve2);
  ASSERT_FALSE(anExtrema12.Extrema().IsParallel());
  EXPECT_GT(anExtrema12.NbExtrema(), 0);
  GeomAPI_ExtremaCurveCurve anExtrema21(aCurve2, aCurve1);
  ASSERT_FALSE(anExtrema21.Extrema().IsParallel());
  EXPECT_GT(anExtrema21.NbExtrema(), 0);
}

// Migrated from tests/lowalgos/extcc/bug29712_38.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_38_ParallelConcentricArcs)
{
  const occ::handle<Geom_Curve> aCurve1 = makeOCC29712MirroredTrimmedCircle(100.0, 0.0, 4.0);
  const occ::handle<Geom_Curve> aCurve2 = makeOCC29712TrimmedCircle(50.0, 4.0, 6.0);
  checkParallelExtrema(aCurve1, aCurve2, 50.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_39.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_39_CircleAndAxisSegment)
{
  const occ::handle<Geom_Curve> aCircle = makeOCC29712TrimmedCircle(5.0, 0.0, 2.0 * M_PI);
  const occ::handle<Geom_Curve> aSegment =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), -1.0, 1.0);
  checkParallelExtrema(aCircle, aSegment, 5.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_41.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_41_CircleAndNearAxisSegment)
{
  const occ::handle<Geom_Curve> aCircle = makeOCC29712TrimmedCircle(5.0, 0.0, 2.0 * M_PI);
  const occ::handle<Geom_Curve> aSegment =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 3.e-8, 1.0);
  checkParallelExtrema(aCircle, aSegment, 5.0);
}

// Migrated from tests/lowalgos/extcc/bug29712_42.
TEST(GeomAPI_ExtremaCurveCurve_Test, OCC29712_42_CircleAndTooNearAxisSegment)
{
  const occ::handle<Geom_Curve> aCircle = makeOCC29712TrimmedCircle(5.0, 0.0, 2.0 * M_PI);
  const occ::handle<Geom_Curve> aSegment =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 2.e-7, 1.0);

  GeomAPI_ExtremaCurveCurve anExtrema12(aCircle, aSegment);
  EXPECT_FALSE(anExtrema12.Extrema().IsParallel());
  EXPECT_EQ(anExtrema12.NbExtrema(), 0);
  GeomAPI_ExtremaCurveCurve anExtrema21(aSegment, aCircle);
  EXPECT_FALSE(anExtrema21.Extrema().IsParallel());
  EXPECT_EQ(anExtrema21.NbExtrema(), 0);
}
