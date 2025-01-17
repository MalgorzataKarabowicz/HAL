# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.
#add_subdirectory(Weights)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-uninitialized -Wno-strict-aliasing")

Set(INCLUDE_DIRECTORIES
${HAL_DATA}
${HAL_FEATURES}
${HAL_ANA}
${HAL_HBT}
${HAL_CUTS}
)

Set(SYSTEM_INCLUDE_DIRECTORIES
 ${ROOT_INCLUDE_DIR} 
 ${BASE_INCLUDE_DIRECTORIES}
)

Include_Directories(
 ${INCLUDE_DIRECTORIES}
 ${GSL_INCLUDE_DIRS}
)
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})
Include_Directories(${GSL_DIR}/include)


Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${GSL_LIBRARY_DIRS} 
  ${FAIRROOT_LIBRARY_DIR}
)
 
Link_Directories(${LINK_DIRECTORIES})

set(SRCS

ana/FemtoBasicAna.cxx
ana/FemtoFreezoutsAna.cxx
ana/FemtoDumpPairAna.cxx
ana/FemtoEventBinsAna.cxx

base/FemtoDPhiDEta.cxx
base/Femto1DCF.cxx
base/Femto3DCF.cxx
base/Femto3DCFQinv.cxx
base/FemtoFastCut.cxx
base/FemtoFreezoutGenerator.cxx
base/FemtoPairKinematics.cxx
base/FemtoPair.cxx
base/FemtoMiniPair.cxx
base/FemtoSourceModel.cxx
base/FemtoConst.cxx
base/source_models/FemtoSourceModelExp.cxx
base/source_models/FemtoSourceModelGauss.cxx
base/source_models/FemtoSourceModelGausMt.cxx
base/source_models/FemtoSourceModelNumerical1D.cxx
base/shmath/FemtoSHSlice.cxx
base/shmath/FemtoYlmIndexes.cxx
base/shmath/FemtoYlmMath.cxx
base/shmath/FemtoYlmSolver.cxx

corrfit/CorrFitDrawOptions.cxx
corrfit/CorrFit.cxx
corrfit/CorrFitFunc.cxx
corrfit/CorrFitMath1DCF.cxx
corrfit/CorrFitMath3DCF.cxx
corrfit/CorrFit1DCF.cxx
corrfit/CorrFit3DCF.cxx
corrfit/CorrFitKisiel.cxx
corrfit/CorrFitWielanek.cxx
corrfit/CorrFitGUI.cxx
corrfit/CorrFitHDFunc.cxx
corrfit/CorrFitSmear1DCF.cxx
corrfit/CorrFitMapKstarRstar.cxx
corrfit/mapgenerators/Femto1DCFAnaMap.cxx
corrfit/roco/Femto1DCFAnaMapMCRoco.cxx
corrfit/mapgenerators/Femto1DMapGenerator.cxx
corrfit/mapgenerators/Femto1DCFAnaMapMC.cxx
corrfit/mapgenerators/FemtoWielanekMapGenerator.cxx
corrfit/mapgenerators/Femto1DCFAnaMapPairsDumped.cxx
corrfit/fittingfunctions/CorrFit1DCFSquare.cxx
corrfit/fittingfunctions/CorrFit1DCFLorentz.cxx
corrfit/fittingfunctions/CorrFit3DCFSquare.cxx
corrfit/fittingfunctions/CorrFit3DCFLorentz.cxx
corrfit/fittingfunctions/CorrFit1DCFExp.cxx
corrfit/fittingfunctions/CorrFit1DCFGauss.cxx
corrfit/fittingfunctions/CorrFit1DCFPoly.cxx
corrfit/fittingfunctions/CorrFit3DCFGauss.cxx
corrfit/fittingfunctions/CorrFit3DCFGauss2.cxx
corrfit/fittingfunctions/CorrFit3DCFGaussCross.cxx
corrfit/fittingfunctions/CorrFit1DCFCumac.cxx
corrfit/fittingfunctions/CorrFit3DCFBowlerSinyukov.cxx
corrfit/fittingfunctions/CorrFit3DCFBowlerSinyukovExtra.cxx
corrfit/sh/CorrFitSHFrom3D.cxx


weights/FemtoFsiParsed.cxx
weights/FemtoWeightGeneratorLednicky2.cxx
weights/FemtoWeightGenerator.cxx
weights/FemtoWeightGeneratorBasic.cxx
weights/FemtoWeightGeneratorLednicky.cxx
weights/FemtoWeightGeneratorResidual.cxx
weights/FemtoWeightGeneratorSupressed.cxx
weights/FemtoWeightGeneratorKisiel.cxx

base/FemtoCorrFunc.cxx
base/corrfunc/FemtoCorrFuncPt.cxx
base/corrfunc/FemtoCorrFuncKt.cxx
base/corrfunc/FemtoCorrFuncAzimuthalKt.cxx
base/corrfunc/FemtoCorrFuncSimple.cxx

imaging/FemtoImaging.cxx
imaging/FemtoImaging1D.cxx
imaging/FemtoImaging3D.cxx
base/FemtoSHCF.cxx
corrfit/CorrFitSHCF.cxx
)

if(NOT DEFINED DISABLE_SQLITE)
	set(SRCS
		${SRCS}
		corrfit/mapgenerator_multidim/CorrFitParamsSetup.cxx
		corrfit/mapgenerator_multidim/CorrFitDumpedPairAna.cxx
		corrfit/mapgenerator_multidim/CorrFitInfo.cxx
		corrfit/mapgenerator_multidim/CorrFit3DCFMultiDim.cxx
		corrfit/mapgenerator_multidim/CorrFitFunctor.cxx
		corrfit/mapgenerator_multidim/CorrFitPairGeneratorYPt.cxx
		corrfit/mapgenerator_multidim/CorrFitMapRebin.cxx
    )
endif()


set(NO_DICT_SRCS
weights/FemtoFsiTools.F
weights/FemtoFsiWeightLednicky.F
)

set(EXT_HEADERS
base/FemtoConst.h
)

Set(HEADERS)
Set(LINKDEF HalFemtoLinkDef.h)
Set(LIBRARY_NAME HalFemto)


Set(DEPENDENCIES HalAna Minuit Minuit2 Fumili)
if(NOT DEFINED DISABLE_SQLITE)
   # Set(DEPENDENCIES ${DEPENDENCIES} sqlite3)
endif()

if(NOT DEFINED DISABLE_GSL)
	Set(DEPENDENCIES ${DEPENDENCIES} gsl gslcblas)
endif()

if (NOT DEFINED DISABLE_SQLITE)
	Set(DEPENDENCIES ${DEPENDENCIES} SQLIO)
endif()



install(FILES ${EXT_HEADERS}
        DESTINATION include
       )

GENERATE_LIBRARY()


#making executables


Set(Exe_Names
    hal-corrfit)
Set(Exe_Source
    corrfit/mapgenerator_multidim/hal_corrfit.cpp
    )
  List(LENGTH Exe_Names _length)
  Math(EXPR _length ${_length}-1)
  ForEach(_file RANGE 0 ${_length})
    List(GET Exe_Names ${_file} _name)
    List(GET Exe_Source ${_file} _src)
    Set(EXE_NAME ${_name})
    Set(SRCS ${_src})
    if(NOT DEFINED DISABLE_GSL)
    	Set(DEPENDENCIES gsl gslcblas HalFeatures Tree HalAna HalFemto )
    else()
    	Set(DEPENDENCIES HalFeatures Tree HalAna HalFemto)
    endif()
    if(NOT DEFINED DISABLE_SQLITE)
      #  Set(DEPENDENCIES ${DEPENDENCIES} sqlite3)
    endif()
    GENERATE_EXECUTABLE()
  EndForEach(_file RANGE 0 ${_length})

