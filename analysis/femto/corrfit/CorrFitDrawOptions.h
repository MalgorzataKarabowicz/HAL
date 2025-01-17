/*
 * CorrFitDrawOptions.h
 *
 *  Created on: 11 sie 2022
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef HAL_ANALYSIS_FEMTO_CORRFIT_CORRFITDRAWOPTIONS_H_
#define HAL_ANALYSIS_FEMTO_CORRFIT_CORRFITDRAWOPTIONS_H_

#include <RtypesCore.h>
#include <TString.h>

namespace Hal {

  class CorrFitDrawOptions {
  public:
    enum eCommon { kNorm = 0, kSame = 1, kNumError = 2, kLegend = 3, kDrawCF = 4, kDrawChi2 = 5 };
    enum e3d { kRgb = 101, kDiag1 = 102, kDiag2 = 103 };

  private:
    Bool_t fAutoNorm     = {kFALSE};
    Bool_t fSame         = {kFALSE};
    Bool_t fDrawCF       = {kFALSE};
    Bool_t fDrawNumErr   = {kFALSE};
    Bool_t fDrawLegend   = {kFALSE};
    Bool_t fDrawChi2     = {kFALSE};
    Bool_t fDiag1        = {kFALSE};
    Bool_t fDiag2        = {kFALSE};
    Bool_t fRgb          = {kFALSE};
    Bool_t fLegendPosSet = {kFALSE};
    Bool_t fDrawRealOnly = {kTRUE};
    Bool_t fDrawNegative = {kFALSE};
    Double_t fMin = {0}, fMax = {0};
    Double_t fRangeMin = {0}, fRangeMax = {0};
    Double_t fXLegend[4] = {0, 0, 0, 0};
    TString fRawCommand;

  protected:
    void ExtrDraw(TString& opt);
    void SetIntOpt(Int_t opt);
    void CheckConflicts();

  public:
    CorrFitDrawOptions();
    CorrFitDrawOptions(TString options);
    CorrFitDrawOptions(std::initializer_list<int> opt, std::initializer_list<Double_t> val = {0, 0});
    virtual ~CorrFitDrawOptions();
    CorrFitDrawOptions(const CorrFitDrawOptions& other) = default;
    CorrFitDrawOptions& operator=(const CorrFitDrawOptions& other) = default;
    TString GetRawOpt() const { return fRawCommand; }
    Bool_t AutoNorm() const { return fAutoNorm; }
    Bool_t DrawCf() const { return fDrawCF; }
    Bool_t DrawLegend() const { return fDrawLegend; }
    Bool_t DrawNumErr() const { return fDrawNumErr; }
    Bool_t DrawMinMax() const {
      if (fMin >= fMax) return kFALSE;
      return kTRUE;
    }
    Bool_t Same() const { return fSame; }
    Bool_t Diag1() const { return fDiag1; }
    Bool_t Diag2() const { return fDiag2; };
    Bool_t Rgb() const { return fRgb; }
    Bool_t Chi2() const { return fDrawChi2; }
    Bool_t LegendPos() const { return (fXLegend[0] == fXLegend[1] ? kFALSE : kTRUE); }
    Double_t GetMax() const { return fMax; }
    Double_t GetMin() const { return fMin; }
    Double_t GetLegendPos(Int_t id) const;
    void SetLegendPos(Double_t x1, Double_t x2, Double_t y1, Double_t y2);
    void SetAutoNorm(Bool_t autoNorm = kTRUE) { fAutoNorm = autoNorm; }
    void SetDrawCf(Bool_t drawCf = kTRUE) { fDrawCF = drawCf; }
    void SetDrawLegend(Bool_t drawLegend = kTRUE) { fDrawLegend = drawLegend; }
    void SetDrawNumErr(Bool_t drawNumErr = kTRUE) { fDrawNumErr = drawNumErr; }
    void SetMinMax(Double_t min = 0, Double_t max = 0) {
      fMin = min;
      fMax = max;
    }
    void SetSame(Bool_t same = kTRUE) { fSame = same; }
    ClassDef(CorrFitDrawOptions, 1);
  };

} /* namespace Hal */

#endif /* HAL_ANALYSIS_FEMTO_CORRFIT_CORRFITDRAWOPTIONS_H_ */
