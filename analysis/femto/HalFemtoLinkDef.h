#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace Hal::Femto;
#pragma link C++ enum Hal::EFemtoPairType;
#pragma link C++ enum Hal::EFitExtraMask;
#pragma link C++ enum Hal::Femto::EKinematics;

#pragma link C++ class Hal::FemtoPair + ;
#pragma link C++ class Hal::FemtoMiniPair + ;
#pragma link C++ class Hal::FemtoMicroPair + ;
#pragma link C++ class Hal::FemtoPicoPair + ;
#pragma link C++ class Hal::FemtoPairLCMS + ;
#pragma link C++ class Hal::FemtoPairPRF + ;
#pragma link C++ class Hal::FemtoPairPRFL + ;
#pragma link C++ class Hal::FemtoPair_SH + ;
#pragma link C++ class Hal::FemtoPairPRF_SH + ;
#pragma link C++ class Hal::FemtoPairLCMS_SH + ;
#pragma link C++ class Hal::FemtoPairDPhiDEta + ;
#pragma link C++ class Hal::FemtoBasicAna + ;
#pragma link C++ class Hal::FemtoFastCut + ;
#pragma link C++ class Hal::FemtoFastCutVirtual + ;
#pragma link C++ class Hal::FemtoFastCutPhi + ;
#pragma link C++ class Hal::FemtoFastCutKt + ;
#pragma link C++ class Hal::FemtoFastCutPtSum + ;
#pragma link C++ class Hal::FemtoSourceModel + ;
#pragma link C++ class Hal::FemtoSourceModel1D + ;
#pragma link C++ class Hal::FemtoSourceModel3D + ;
#pragma link C++ class Hal::FemtoSourceModelGauss1D + ;
#pragma link C++ class Hal::FemtoSourceModelExp1D + ;
#pragma link C++ class Hal::FemtoSourceModelGauss3D + ;
#pragma link C++ class Hal::FemtoSourceModelGausMt + ;
#pragma link C++ class Hal::FemtoSourceModelNumerical1D + ;
#pragma link C++ class Hal::FemtoFreezoutGenerator + ;
#pragma link C++ class Hal::FemtoFreezoutGeneratorLCMS + ;
#pragma link C++ class Hal::FemtoFreezoutGeneratorPRF + ;
#pragma link C++ class Hal::FemtoFreezoutsAna + ;
#pragma link C++ enum Hal::FemtoFreezoutsAna::EMode;
#pragma link C++ class Hal::FemtoEventBinsAna + ;
#pragma link C++ class Hal::FemtoFastCutEta + ;
#pragma link C++ class Hal::FemtoDumpPairAna + ;

#pragma link C++ class Hal::Femto1DCF + ;
#pragma link C++ class Hal::Femto3DCF + ;
#pragma link C++ class Hal::Femto3DCFQinv + ;
#pragma link C++ class Hal::FemtoDPhiDEta + ;

#pragma link C++ class Hal::CorrFit + ;
#pragma link C++ class Hal::CorrFitFunc + ;
#pragma link C++ class Hal::CorrFitMath1DCF + ;
#pragma link C++ class Hal::CorrFit1DCF_Sum + ;
#pragma link C++ class Hal::CorrFit1DCF_SumRsame + ;
#pragma link C++ class Hal::CorrFit1DCF_Multi + ;
#pragma link C++ class Hal::CorrFit1DCF_Poly + ;
#pragma link C++ class Hal::CorrFit1DCF_Poly2 + ;
#pragma link C++ class Hal::CorrFitMath3DCF + ;;
#pragma link C++ class Hal::CorrFit3DCF_Sum + ;
#pragma link C++ class Hal::CorrFit1DCF_Square + ;
#pragma link C++ class Hal::CorrFit3DCF_Square + ;
#pragma link C++ class Hal::CorrFit3DCF_Lorentz + ;
#pragma link C++ class Hal::CorrFit1DCF_Lorentz + ;
#pragma link C++ class Hal::CorrFit1DCFCumac + ;
#pragma link C++ class Hal::CorrFIt1DCFCumacLamLam + ;
#pragma link C++ class Hal::CorrFit1DCFCumacPLam + ;
#pragma link C++ class Hal::CorrFit1DCFCumacK0K0 + ;
#pragma link C++ class Hal::CorrFitDrawOptions + ;

#pragma link C++ class Hal::CorrFitParamsSetup + ;
#pragma link C++ class Hal::CorrFit3DCFMultiDim + ;
#pragma link C++ class Hal::CorrFitFunctor + ;
#pragma link C++ class Hal::CorrFitDumpedPairAna + ;
#pragma link C++ class Hal::CorrFitInfo + ;
#pragma link C++ class Hal::CorrFitPairGeneratorYPt + ;
#pragma link C++ class Hal::CorrFitMapRebin + ;
#pragma link C++ class Hal::CorrFitGUI + ;
#pragma link C++ class Hal::CorrFitParButton + ;
#pragma link C++ class Hal::CorrFitChiSelector + ;
#pragma link C++ class Hal::CorrFitHDFunc + ;
#pragma link C++ class Hal::CorrFitHDFunc1D + ;
#pragma link C++ class Hal::CorrFitHDFunc3D + ;

#pragma link C++ class Hal::CorrFit1DCF + ;
#pragma link C++ class Hal::CorrFit3DCF + ;
#pragma link C++ class Hal::CorrFitKisiel + ;
#pragma link C++ class Hal::CorrFitSmear1DCF + ;
#pragma link C++ class Hal::CorrFitMapKstarRstar + ;
#pragma link C++ class Hal::CorrFitMapKstarRstarDiv + ;
#pragma link C++ class Hal::FemtoWielanekMapGenerator + ;
#pragma link C++ class Hal::Femto1DMapGenerator + ;
#pragma link C++ class Hal::Femto1DCFAnaMapMC + ;
#pragma link C++ class Hal::Femto1DCFAnaMapMCRoco + ;
#pragma link C++ class Hal::Femto1DCFAnaMapPairsDumped + ;
#pragma link C++ class Hal::FemtoCorrFuncKtMap1D + ;
#pragma link C++ class Hal::CorrFitWielanek + ;
#pragma link C++ class Hal::CorrFit3DCFBowlerSinyukov + ;
#pragma link C++ class Hal::CorrFit3DCFBowlerSinyukovEllipse + ;
#pragma link C++ class Hal::CorrFit3DCFBowlerSinyukovClassic + ;

#pragma link C++ class Hal::Femto1DCFAnaMap + ;

#pragma link C++ class Hal::FemtoCorrFunc + ;
#pragma link C++ class Hal::FemtoCorrFunc1D + ;
#pragma link C++ class Hal::FemtoCorrFunc2D + ;
#pragma link C++ class Hal::FemtoCorrFuncPt + ;
#pragma link C++ class Hal::FemtoCorrFuncKt + ;
#pragma link C++ class Hal::FemtoCorrFuncAzimuthalKt + ;
#pragma link C++ class Hal::FemtoCorrFuncSimple + ;

#pragma link C++ class Hal::CorrFit1DCF_Exp + ;
#pragma link C++ class Hal::CorrFit1DCF_Gauss + ;

#pragma link C++ class Hal::CorrFit3DCF_Gauss + ;
#pragma link C++ class Hal::CorrFit3DCF_Gauss2 + ;
#pragma link C++ class Hal::CorrFit3DCF_GaussCross + ;

#pragma link C++ class Hal::FemtoWeightGenerator + ;
#pragma link C++ class Hal::FemtoWeightGeneratorBasic + ;
#pragma link C++ class Hal::FemtoFsiParsed + ;
#pragma link C++ class Hal::FemtoWeightGeneratorLednicky + ;
#pragma link C++ class Hal::FemtoWeightGeneratorLednicky2 + ;
#pragma link C++ class Hal::FemtoWeightGeneratorResidual + ;
#pragma link C++ class Hal::FemtoWeightGeneratorSupressed + ;
#pragma link C++ class Hal::FemtoWeightGeneratorKisiel + ;

#ifndef GSL_DISABLE
#pragma link C++ class Hal::FemtoImaging + ;
#pragma link C++ class Hal::FemtoImaging1D + ;
#pragma link C++ class Hal::FemtoImaging3D + ;

#pragma link C++ class Hal::CorrFitSHCF + ;
#pragma link C++ class Hal::CorrFitSHFrom3D + ;
#pragma link C++ class Hal::FemtoYlmIndexes + ;
#pragma link C++ class Hal::FemtoYlm + ;
#pragma link C++ class Hal::FemtoSHCF + ;
#endif

//#pragma link C++ class Led+;

#endif
