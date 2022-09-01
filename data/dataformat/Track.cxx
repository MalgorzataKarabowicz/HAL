/*
 * Track.cxx
 *
 *  Created on: 23-06-2014
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#include "Track.h"

#include <TMath.h>
#include <TString.h>

#include <iostream>

#include "Cout.h"
#include "DataFormat.h"
#include "Event.h"


namespace Hal
{

  Track::Track()
    : TObject()
    , fEvent(nullptr)
    , fID(0)
    , fStatus(0)
    , fHiddenInfo(0)
    , fMotherID(-1)
    , fThisID(-1)
    , fType(0)
    , fCharge(0)
  {
  }

  void Track::Print(Option_t* /*option*/) const
  {
    Cout::Database(3, "Px", "Py", "Pz");
    Cout::Database(3, Form("%4.5f", GetPx()), Form("%4.5f", GetPy()), Form("%4.5f", GetPz()));
  }

  Track::~Track() {}

  void Track::Boost(Double_t vx, Double_t vy, Double_t vz) { fP.Boost(vx, vy, vz); }

  void Track::SetPrimary()
  {
    SETBIT(fType, kPrimary);
    CLRBIT(fType, kMother);
  }

  TObject* Track::GetTrackPointer() const { return fEvent->GetTrackPointer(fThisID); }

  void Track::CopyData(Track* other)
  {
    fP          = other->fP;
    fID         = other->fID;
    fStatus     = other->fStatus;
    fMotherID   = other->fMotherID;
    fCharge     = other->fCharge;
    fType       = other->fType;
    fHiddenInfo = -1;
    if (IsV0()) {
<<<<<<< HEAD
      int poz = GetEvent()->fV0sHiddenInfo->GetEntriesFast();

      // std::cout << ClassName() << " " << poz << std::endl;
      V0Track* v0 = (V0Track*) GetEvent()->fV0sHiddenInfo->ConstructedAt(poz);
      // std::cout << "v0 " << v0 << std::endl;
      // std::cout << v0->ClassName() << std::endl;
=======
      fHiddenInfo = GetEvent()->fV0Counter++;
      V0Track* v0 = (V0Track*) GetEvent()->fV0sHiddenInfo->UncheckedAt(fHiddenInfo);
>>>>>>> 2b427e1ffaf4ca0b119abccb66dcd7d7a8e32b16
      v0->CopyData(other->GetV0Info());
    }
  }

  void Track::CopyAllData(Track* other)
  {
    fEvent  = other->fEvent;
    fThisID = other->fThisID;
    CopyData(other);
  }

  Track::Track(const Track& track) : TObject(track)
  {
    fEvent      = track.fEvent;
    fP          = track.fP;
    fID         = track.fID;
    fStatus     = track.fStatus;
    fMotherID   = track.fMotherID;
    fThisID     = track.fThisID;
    fType       = track.fType;
    fCharge     = track.fCharge;
    fHiddenInfo = track.fHiddenInfo;
  }

  std::vector<int> Track::GetLinks() const
  {
    std::vector<int> links;
    links.push_back(GetThisID());
    if (IsGoodSecondary()) { links.push_back(GetMotherIndex()); }
    if (IsGoodV0()) {
      links.push_back(GetV0Info()->GetPosId());
      links.push_back(GetV0Info()->GetNegId());
    }
    return links;
  }

  void Track::TranslateLinks(Int_t* map)
  {
    if (IsGoodSecondary()) { SetMotherIndex(map[GetMotherIndex()]); }
    if (IsGoodV0()) {
      auto v0Info = GetV0Info();
      v0Info->SetPosId(map[GetV0Info()->GetPosId()]);
      v0Info->SetNegId(map[GetV0Info()->GetNegId()]);
      v0Info->SetTrackId(GetThisID());
    }
  }
<<<<<<< HEAD
  void Track::TranslateLinksVec(const std::vector<int>& vec)
  {
=======

  void Track::TranslateLinksVec(const std::vector<int>& vec) {
>>>>>>> 2b427e1ffaf4ca0b119abccb66dcd7d7a8e32b16
    if (IsGoodSecondary()) { SetMotherIndex(vec.at(GetMotherIndex())); }
    if (IsGoodV0()) {
      auto v0Info = GetV0Info();
      v0Info->SetPosId(vec.at(GetV0Info()->GetPosId()));
      v0Info->SetNegId(vec.at(GetV0Info()->GetNegId()));
      v0Info->SetTrackId(GetThisID());
    }
  }
<<<<<<< HEAD
  void Track::SetLinks(std::vector<int>& vec)
  {
=======

  void Track::SetLinks(std::vector<int>& vec) {
>>>>>>> 2b427e1ffaf4ca0b119abccb66dcd7d7a8e32b16
    this->SetThisID(vec[0]);
    if (IsGoodSecondary()) {
      SetMotherIndex(vec[1]);
      if (IsGoodV0()) {
        auto v0Info = GetV0Info();
        v0Info->SetTrackId(GetThisID());
        v0Info->SetPosId(vec[2]);
        v0Info->SetNegId(vec[3]);
      }
    }
    else {
      if (IsGoodV0()) {
        auto v0Info = GetV0Info();
        v0Info->SetTrackId(GetThisID());
        v0Info->SetPosId(vec[1]);
        v0Info->SetNegId(vec[2]);
      }
    }
  }

  void Track::SetMass(Double_t mass)
  {
    Double_t p = GetMomentum().P();
    Double_t e = TMath::Sqrt(p * p + mass * mass);
    fP.SetE(e);
  }

  void Track::SetSecondary(Bool_t parent)
  {
    CLRBIT(fType, kPrimary);
    if (parent) { SETBIT(fType, kMother); }
    else {
      CLRBIT(fType, kMother);
    }
  }

  void Track::EnableV0(Bool_t v0, Bool_t good)
  {
    if (v0) {
      SETBIT(fType, kV0);
      if (good) SETBIT(fType, kV0Daughters);
      else
        CLRBIT(fType, kV0Daughters);
<<<<<<< HEAD
      fHiddenInfo = fEvent->fV0sHiddenInfo->GetEntriesFast();
      fEvent->fV0sHiddenInfo->ConstructedAt(fEvent->fV0sHiddenInfo->GetEntriesFast());
    }
    else {
=======
      fHiddenInfo = fEvent->fV0Counter;
      fEvent->fV0sHiddenInfo->ConstructedAt(fEvent->fV0Counter++);
    } else {
>>>>>>> 2b427e1ffaf4ca0b119abccb66dcd7d7a8e32b16
      CLRBIT(fType, kV0);
    }
  }

  Float_t Track::GetFieldVal(Int_t fieldID) const
  {
    switch (fieldID) {
      case DataFieldID::ETrack::kPx: return fP.Px(); break;
      case DataFieldID::ETrack::kPy: return fP.Py(); break;
      case DataFieldID::ETrack::kPz: return fP.Pz(); break;
      case DataFieldID::ETrack::kE: return fP.E(); break;
      case DataFieldID::ETrack::kPhi: return fP.Phi(); break;

      case DataFieldID::ETrack::kPt: return fP.Pt(); break;
      case DataFieldID::ETrack::kTheta: return fP.Theta(); break;
      case DataFieldID::ETrack::kP: return fP.P(); break;
      case DataFieldID::ETrack::kEta: return fP.Eta(); break;
      case DataFieldID::ETrack::kRapidity: return fP.Rapidity(); break;

      case DataFieldID::ETrack::kCharge: return GetCharge(); break;
      case DataFieldID::ETrack::kStatus: return GetStatus(); break;
      case DataFieldID::ETrack::kPq: return GetCharge() * fP.P(); break;
      case DataFieldID::ETrack::kTrackZero: return 0; break;
    }
    if (fieldID > DataFieldID::Internal::EventStart) { return GetEvent()->GetFieldVal(fieldID); }

    if (IsV0()) {
      switch (fieldID) {
        case DataFieldID::EV0Track::kV0DauPosId: return GetV0Info()->GetPosId(); break;
        case DataFieldID::EV0Track::kV0DauNegId: return GetV0Info()->GetNegId(); break;
        case DataFieldID::EV0Track::kV0Pdg: return GetV0Info()->GetPdg(); break;
        case DataFieldID::EV0Track::kV0Alpha: return GetV0Info()->GetAlphaArm(); break;
        case DataFieldID::EV0Track::kV0PtArm: return GetV0Info()->GetPtArm(); break;
        case DataFieldID::EV0Track::kV0DauDist: return GetV0Info()->GetDauDist(); break;
        case DataFieldID::EV0Track::kV0DecLenght: return GetV0Info()->GetDecLength(); break;
        case DataFieldID::EV0Track::kV0MomPx: return GetV0Info()->GetMom().Px(); break;
        case DataFieldID::EV0Track::kV0MomPy: return GetV0Info()->GetMom().Py(); break;
        case DataFieldID::EV0Track::kV0MomPz: return GetV0Info()->GetMom().Pz(); break;
      }
    }

    return -FLT_MAX;
  }

  TString Track::GetFieldName(Int_t fieldID) const
  {
    switch (fieldID) {
      case DataFieldID::ETrack::kPx: return "P_{x} [GeV/c]"; break;
      case DataFieldID::ETrack::kPy: return "P_{y} [GeV/c]"; break;
      case DataFieldID::ETrack::kPz: return "P_{z} [GeV/c]"; break;
      case DataFieldID::ETrack::kE: return "E [GeV/c^{2}]"; break;
      case DataFieldID::ETrack::kPhi: return "#phi [rad]"; break;

      case DataFieldID::ETrack::kPt: return "P_{T} [GeV/c]"; break;
      case DataFieldID::ETrack::kTheta: return "#theta [rad]"; break;
      case DataFieldID::ETrack::kP: return "P [GeV/c]"; break;
      case DataFieldID::ETrack::kEta: return "#eta [AU]"; break;
      case DataFieldID::ETrack::kRapidity: return "y [AU]"; break;

      case DataFieldID::ETrack::kCharge: return "q [e]"; break;
      case DataFieldID::ETrack::kStatus: return "stat [AU]"; break;
      case DataFieldID::ETrack::kPq: return "Pq [GeV/c*e]"; break;

      case DataFieldID::EV0Track::kV0Alpha: return "#alpha"; break;
      case DataFieldID::EV0Track::kV0PtArm: return "P_{T} [GeV/c]"; break;
      case DataFieldID::EV0Track::kV0MomPx: return "P_{x} [GeV/c]"; break;
      case DataFieldID::EV0Track::kV0MomPy: return "P_{y} [GeV/c]"; break;
      case DataFieldID::EV0Track::kV0MomPz: return "P_{z} [GeV/c]"; break;
    }
    if (fieldID > DataFieldID::Internal::EventStart) { return GetEvent()->GetFieldName(fieldID); }
    Cout::PrintInfo(Form("Track::GetFieldName cannot find field with fieldID  %i", fieldID), EInfo::kLowWarning);
    return "[]";
  }

<<<<<<< HEAD
  void Track::Clear(Option_t* /*opt*/)
  {
    fMotherID = -1;
    fThisID   = -1;
    fType     = 0;
    fEvent    = nullptr;
    fID       = -1;
=======
  void Track::Clear(Option_t* /*opt*/) {
    fMotherID   = -1;
    fHiddenInfo = -1;
    fThisID     = -1;
    fType       = 0;
    fEvent      = nullptr;
    fID         = -1;
>>>>>>> 2b427e1ffaf4ca0b119abccb66dcd7d7a8e32b16
  }

  V0Track* Track::GetV0Info() const
  {
    if (fHiddenInfo == -1) return nullptr;
    return (V0Track*) fEvent->fV0sHiddenInfo->UncheckedAt(fHiddenInfo);
  }

  void Track::ResetTrack(Int_t thisID, Event* event)
  {
    fMotherID   = -1;
    fHiddenInfo = -1;
    fThisID     = thisID;
    fType       = 0;
    fEvent      = event;
    fID         = -1;
  }
}  // namespace Hal
