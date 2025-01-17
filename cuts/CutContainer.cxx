/*
 * CutContainer.cxx
 *
 *  Created on: 06-08-2013
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#include "CutContainer.h"

#include <TClass.h>
#include <TCollection.h>
#include <TList.h>
#include <TRegexp.h>
#include <iostream>

#include "Cout.h"
#include "EventComplexCut.h"
#include "EventVirtualCut.h"
#include "Package.h"
#include "Parameter.h"
#include "StdString.h"
#include "TrackComplexCut.h"
#include "TrackVirtualCut.h"
#include "TwoTrackComplexCut.h"
#include "TwoTrackVirtualCut.h"

namespace Hal {
  CutContainer::CutContainer() : TObject(), fInit(kFALSE), fSize(0), fTempCutMonitors(NULL), fCutContainers(NULL) {}

  CutContainer::CutContainer(ECutUpdate tiers) : fInit(kFALSE), fTempCutMonitors(NULL), fCutContainers(NULL) {
    switch (tiers) {
      case ECutUpdate::kEvent: fSize = 1; break;
      case ECutUpdate::kTrack: fSize = 2; break;
      case ECutUpdate::kTwoTrack: fSize = 3; break;
      case ECutUpdate::kTwoTrackBackground: fSize = 4; break;
      default: {
        Cout::PrintInfo("Unknown update ration in CutContainer", EInfo::kError);
      } break;
    }
    fCutContainers   = new TObjArray*[fSize];
    fTempCutMonitors = new TObjArray*[fSize];
    for (int i = 0; i < fSize; i++) {
      fCutContainers[i]   = new TObjArray();
      fTempCutMonitors[i] = new TObjArray();
    }
  }

  CutContainer::CutContainer(const CutContainer& cont) :
    TObject(cont), fInit(kFALSE), fSize(cont.fSize), fTempCutMonitors(NULL), fCutContainers(NULL) {
    if (cont.fInit == kTRUE) { Cout::PrintInfo("CutContainer already initialized", EInfo::kError); }
    fCutContainers   = new TObjArray*[fSize];
    fTempCutMonitors = new TObjArray*[fSize];
    for (int i = 0; i < fSize; i++) {
      fCutContainers[i] = new TObjArray();
      for (int j = 0; j < cont.fCutContainers[i]->GetEntries(); j++) {
        CutCollection* subcont = (CutCollection*) cont.fCutContainers[i]->UncheckedAt(j);
        fCutContainers[i]->Add(subcont->MakeNewCopy(fCutContainers));
      }
      fTempCutMonitors[i] = new TObjArray();
      for (int j = 0; j < cont.fTempCutMonitors[i]->GetEntries(); j++) {
        CutMonitor* mon = (CutMonitor*) cont.fTempCutMonitors[i]->At(j);
        fTempCutMonitors[i]->Add(mon->MakeCopy());
      }
    }
  }

  void CutContainer::AddCut(const Cut& cut, Option_t* opt) {
    TString option = opt;
    if (Hal::Std::FindParam(option, "im", kTRUE)) {
      Cut* tempcut = NULL;
      Bool_t nulls = Hal::Std::FindParam(option, "null", kTRUE);
      if (cut.InheritsFrom("Hal::TrackCut")) {
        tempcut = new TrackImaginaryCut(static_cast<const TrackCut&>(cut));
        if (nulls) static_cast<TrackImaginaryCut*>(tempcut)->AcceptNulls(kTRUE);
      } else if (cut.InheritsFrom("Hal::EventCut")) {
        tempcut = new EventImaginaryCut(static_cast<const EventCut&>(cut));
        if (nulls) static_cast<EventImaginaryCut*>(tempcut)->AcceptNulls(kTRUE);
      } else if (cut.InheritsFrom("Hal::TwoTrackCut")) {
        tempcut = new TwoTrackImaginaryCut(static_cast<const TwoTrackCut&>(cut));
        if (nulls) static_cast<TwoTrackImaginaryCut*>(tempcut)->AcceptNulls(kTRUE);
      }
      if (tempcut == nullptr) return;
      tempcut->SetCollectionID(cut.GetCollectionID());
      AddCut(*tempcut, option);
      delete tempcut;
      return;
    } else if (Hal::Std::FindParam(option, "re", kTRUE)) {
      Cut* tempcut = NULL;
      if (cut.InheritsFrom("Hal::TrackCut")) {
        tempcut = new TrackRealCut(static_cast<const TrackCut&>(cut));
      } else if (cut.InheritsFrom("Hal::EventCut")) {
        tempcut = new EventRealCut(static_cast<const EventCut&>(cut));
      } else if (cut.InheritsFrom("Hal::TwoTrackCut")) {
        tempcut = new TwoTrackRealCut(static_cast<const TwoTrackCut&>(cut));
      }
      if (tempcut == nullptr) return;
      tempcut->SetCollectionID(cut.GetCollectionID());
      AddCut(*tempcut, option);
      delete tempcut;
      return;
    }
    if (ExtrackRegExp2(cut, opt)) {
#ifdef HAL_DEBUG
      Cout::PrintInfo("CutContainer using ExtrackRegExp2", EInfo::kDebugInfo);
#endif
      return;
    }
    if (ExtractRegExp(cut, opt)) {
#ifdef HAL_DEBUG
      Cout::PrintInfo("CutContainer using ExtrackRegExp", EInfo::kDebugInfo);
#endif
      return;
    }
    Int_t collection = cut.GetCollectionID();
    ECutUpdate place = cut.GetUpdateRatio();
    if (CheckTwoTracksOptions(cut, opt)) {
#ifdef HAL_DEBUG
      Cout::PrintInfo("CutContainer using CheckTwoTracksOptions", EInfo::kDebugInfo);
#endif
      return;
    }
    if (Hal::Std::FindParam(option, "bckg")) place = ECutUpdate::kTwoTrackBackground;
    if (fSize <= static_cast<Int_t>(place)) {
      TString update_ratio_name;
      switch (place) {
        case ECutUpdate::kEvent: update_ratio_name = "event"; break;
        case ECutUpdate::kTrack: update_ratio_name = "track"; break;
        case ECutUpdate::kTwoTrack: update_ratio_name = "two_track"; break;
        case ECutUpdate::kTwoTrackBackground: update_ratio_name = "two_track<background>"; break;
        default:
          Cout::PrintInfo(Form("Unknown update ratio for  %s", cut.CutName().Data()), EInfo::kLowWarning);
          update_ratio_name = "unknown";
          break;
      }
      Cout::PrintInfo(Form("CutContainer can't hold %s cut because it's update ratio (%s) is "
                           "too big, check fTries or call SetOption(backround) before adding cuts "
                           "or cut monitors",
                           cut.ClassName(),
                           update_ratio_name.Data()),
                      EInfo::kLowWarning);
      return;
    }
    // add to cut containers
    if (GetCutContainer(place)->At(collection) == NULL) {
      CutCollection* subcont = new CutCollection(fCutContainers, fSize, place, collection);
      subcont->AddCut(cut.MakeCopy(), opt);
      GetCutContainer(place)->AddAtAndExpand(subcont, collection);
    } else if ((collection >= GetCutContainer(place)->GetEntriesFast())) {
      CutCollection* subcont = new CutCollection(fCutContainers, fSize, place, collection);
      subcont->AddCut(cut.MakeCopy(), opt);
      GetCutContainer(place)->AddAtAndExpand(subcont, collection);
    } else {
      ((CutCollection*) GetCutContainer(place)->UncheckedAt(collection))->AddCut(cut.MakeCopy(), opt);
    }
  }

  void CutContainer::AddMonitor(const CutMonitor& monitor, Option_t* opt) {
    TString option           = opt;
    CutMonitor* monitor_copy = monitor.MakeCopy();
    ExtractComplexMonitor(monitor_copy, option);
    if (ExtractRegExp2(*monitor_copy, option)) {
      delete monitor_copy;
      return;
    }
    if (ExtractRegExp(*monitor_copy, option)) {
      delete monitor_copy;
      return;
    }
    ECutUpdate update = monitor.GetUpdateRatio();
    if (update == ECutUpdate::kNo) return;
    if (CheckTwoTracksOptions(*monitor_copy, option)) {
      delete monitor_copy;
      return;
    }
    if (Hal::Std::FindParam(option, "bckg")) { update = ECutUpdate::kTwoTrackBackground; }
    if (fSize <= static_cast<Int_t>(update)) {
      Cout::PrintInfo("CutContainer can't hold this cut because it's update ratio is to big, check fTries or call "
                      "SetOption(backround) before adding cuts or cut monitors",
                      EInfo::kLowWarning);
      return;
    }
    fTempCutMonitors[static_cast<Int_t>(update)]->AddLast(monitor_copy);
  }

  void CutContainer::InitReport() const {
    for (int i = 0; i < fSize; i++) {
      for (int j = 0; j < fCutContainers[i]->GetEntriesFast(); j++) {
        ((CutCollection*) fCutContainers[i]->UncheckedAt(j))->PrintInfo();
      }
    }
  }

  void CutContainer::LinkCollections(ECutUpdate opt_low, Int_t in_low, ECutUpdate opt_high, Int_t in_high) {
    if (opt_high == opt_low || opt_low > opt_high) { Cout::PrintInfo("Wrong ECut Update in Link Collections", EInfo::kError); }
    if (static_cast<Int_t>(opt_high) >= fSize) {
      Cout::PrintInfo("To big opt_high, link will be ingored", EInfo::kError);
    } else if (static_cast<Int_t>(opt_low) >= fSize) {
      Cout::PrintInfo("To big opt_low, link will be ingored", EInfo::kError);
    }
    CutCollection* low  = NULL;
    CutCollection* high = NULL;
    if (opt_low == ECutUpdate::kEvent) {
      low = (CutCollection*) GetCutContainer(opt_low)->At(in_low);
    } else if (opt_low == ECutUpdate::kTrack) {
      low = (CutCollection*) GetCutContainer(opt_low)->At(in_low);
    } else {
      Cout::PrintInfo("Invalid opt_low argument in CutContainer::LinkCollections", EInfo::kError);
      return;
    }
    if (opt_high == ECutUpdate::kTrack) {
      high = (CutCollection*) GetCutContainer(opt_high)->At(in_high);
    } else if (opt_high == ECutUpdate::kTwoTrack || opt_high == ECutUpdate::kTwoTrackBackground) {
      high = (CutCollection*) GetCutContainer(opt_high)->At(in_high);
    } else {
      Cout::PrintInfo("Invalid opt_high argument in CutContainer::LinkCollections", EInfo::kError);
      return;
    }
    if (low == NULL || high == NULL) {
      Cout::PrintInfo("Cant link some collections", EInfo::kError);
      return;
    }
    high->AddPreviousAddr(in_low, 0);
    if (opt_high == ECutUpdate::kTwoTrackBackground) {
      if (!high->IsDummy()) low->AddNextAddr(in_high, kTRUE);
    } else {
      if (!high->IsDummy()) low->AddNextAddr(in_high, kFALSE);
    }
  }

  void CutContainer::ReplicateCollection(ECutUpdate type, Int_t collection_no, Int_t new_collection_no, Option_t* /*option*/) {
    CutCollection* old = NULL;
    old                = (CutCollection*) GetCutContainer(type)->UncheckedAt(collection_no);
    GetCutContainer(type)->AddAtAndExpand(old->Replicate(new_collection_no), new_collection_no);
  }

  void CutContainer::VerifyOrder(TObjArray* obj) {
    for (int i = 0; i < obj->GetEntriesFast(); i++) {
      if (obj->UncheckedAt(i) == NULL) { Cout::PrintInfo(Form("Null cutsubcontainers at %i", i), EInfo::kError); }
      Int_t collection_no = ((CutCollection*) obj->UncheckedAt(i))->GetCollectionID();
      if (collection_no != i) { Cout::PrintInfo(Form("Wrong order of cuts [%i]!=%i", i, collection_no), EInfo::kError); }
    }
  }

  void CutContainer::RemoveCollection(ECutUpdate update, Int_t collection) { GetCutContainer(update)->RemoveAt(collection); }

  CutContainer::~CutContainer() {
    for (int i = 0; i < fSize; i++) {
      if (fCutContainers[i] != NULL) fCutContainers[i]->Delete();
    }
    if (fCutContainers) delete fCutContainers;
    if (fTempCutMonitors) {
      for (int i = 0; i < fSize; i++) {
        fTempCutMonitors[i]->Delete();
      }
      delete[] fTempCutMonitors;
    }
  }

  void CutContainer::Init(const Int_t task_id) {
    if (fInit == kTRUE) {
      Cout::PrintInfo("CutContainer has been initialized before", EInfo::kLowWarning);
      return;
    }
    for (int i = 0; i < fSize; i++) {
      VerifyOrder(fCutContainers[i]);
    }
    for (int k = 0; k < fSize; k++) {
      for (int j = 0; j < fTempCutMonitors[k]->GetEntriesFast(); j++) {
        CutMonitor* cutmon = (CutMonitor*) fTempCutMonitors[k]->UncheckedAt(j);
        if (cutmon->GetCollectionID() != -1) {
          CutMonitor* clone = cutmon->MakeCopy();
          Int_t collection  = cutmon->GetCollectionID();
          if ((CutCollection*) (fCutContainers[k]->UncheckedAt(collection)) == NULL) {
            Cout::PrintInfo(Form(" Collection %i for cut monitor %s not found", collection, clone->ClassName()), EInfo::kError);
          } else {
            ((CutCollection*) (fCutContainers[k]->UncheckedAt(collection)))->AddCutMonitor(clone);
          }
        } else {
          for (int i = 0; i < fCutContainers[k]->GetEntriesFast(); i++) {
            CutMonitor* clone = cutmon->MakeCopy();
            ((CutCollection*) (fCutContainers[k]->UncheckedAt(i)))->AddCutMonitor(clone);
          }
        }
      }
#ifdef HAL_DEBUG
      Cout::PrintInfo("Initializing cut collection", EInfo::kDebugInfo);
#endif
      for (int i = 0; i < fCutContainers[k]->GetEntriesFast(); i++) {
#ifdef HAL_DEBUG
        Cout::PrintInfo(Form("Initializing cut collection at %i", k), EInfo::kDebugInfo);
#endif
        ((CutCollection*) (fCutContainers[k]->UncheckedAt(i)))->Init(task_id);
      }
    }
#ifdef HAL_DEBUG
    Cout::PrintInfo("Deleting temporary cut monitors", EInfo::kDebugInfo);
#endif
    for (int i = 0; i < fSize; i++) {
      fTempCutMonitors[i]->Delete();
    }
    delete[] fTempCutMonitors;
    fTempCutMonitors = NULL;
    fInit            = kTRUE;
  }

  Package* CutContainer::Report() const {
    Package* pack = new Package(this, kTRUE);
    if (fSize > 0) {
      pack->AddObject(new ParameterInt("Event_collections_No", GetCutContainer(ECutUpdate::kEvent)->GetEntriesFast()));
      TList* list1 = new TList();
      list1->SetOwner(kTRUE);
      list1->SetName("EventCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kEvent)->GetEntriesFast(); i++) {
        list1->Add(((CutCollection*) GetCutContainer(ECutUpdate::kEvent)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list1);
    }
    if (fSize > 1) {
      pack->AddObject(new ParameterInt("Track_collections_No", GetCutContainer(ECutUpdate::kTrack)->GetEntriesFast()));
      TList* list2 = new TList();
      list2->SetOwner(kTRUE);
      list2->SetName("TrackCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kTrack)->GetEntriesFast(); i++) {
        list2->Add(((CutCollection*) GetCutContainer(ECutUpdate::kTrack)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list2);
    }
    if (fSize > 2) {
      pack->AddObject(new ParameterInt("TwoTrack_collections_No", GetCutContainer(ECutUpdate::kTwoTrack)->GetEntriesFast()));
      TList* list3 = new TList();
      list3->SetOwner(kTRUE);
      list3->SetName("TwoTrackCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kTwoTrack)->GetEntriesFast(); i++) {
        list3->Add(((CutCollection*) GetCutContainer(ECutUpdate::kTwoTrack)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list3);
    }
    if (fSize > 3) {
      pack->AddObject(new ParameterInt("TwoTrack_collections_background_No",
                                       GetCutContainer(ECutUpdate::kTwoTrackBackground)->GetEntriesFast()));
      TList* list4 = new TList();
      list4->SetOwner(kTRUE);
      list4->SetName("TwoTrackBackgroundCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kTwoTrackBackground)->GetEntriesFast(); i++) {
        list4->Add(((CutCollection*) GetCutContainer(ECutUpdate::kTwoTrackBackground)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list4);
    }
    pack->SetComment(this->ClassName());
    return pack;
  }

  Bool_t CutContainer::ExtractRegExp(const Cut& cut, Option_t* opt) {
    TString option = opt;
    Int_t number, jump;
    Bool_t found = Hal::Std::FindExpressionTwoValues(option, number, jump, kTRUE);
    if (!found) return kFALSE;
    Int_t begin_collection = cut.GetCollectionID();
    Cut* temp_cut;
    for (int i = 0; i < number; i++) {
      temp_cut = cut.MakeCopy();
      temp_cut->SetCollectionID(begin_collection + i * jump);
      AddCut(*temp_cut, option.Data());
      delete temp_cut;
    }
    return kTRUE;
  }

  Bool_t CutContainer::ExtractRegExp(const CutMonitor& cut, Option_t* opt) {
    TString option = opt;
    TRegexp regexp("{[0-9]+x[0-9]+}");
    TString expr = option(regexp);
    if (expr.Length() <= 0) { return kFALSE; }
    Int_t number, jump;
    Bool_t found = Hal::Std::FindExpressionTwoValues(option, number, jump, kTRUE);
    if (!found) return kFALSE;
    // found regular exprestion like {number x number}
    Int_t begin_collection = cut.GetCollectionID();
    if (begin_collection == -1) {
      Cout::PrintInfo(Form("Adding cut monitor %s (first cut %s ) with collection no -1, with "
                           "regular expression like {AxB} set initial collection no to 0",
                           cut.ClassName(),
                           cut.GetCutName(0).Data()),
                      EInfo::kLowWarning);
      begin_collection = 0;
    }
    CutMonitor* temp_cut;
    for (int i = 0; i < number; i++) {
      temp_cut = cut.MakeCopy();
      temp_cut->SetCollectionID(begin_collection + i * jump);
      AddMonitor(*temp_cut, option.Data());
      delete temp_cut;
    }
    return kTRUE;
  }

  Bool_t CutContainer::CheckTwoTracksOptions(const CutMonitor& cutmon, Option_t* opt) {
    if (!(cutmon.GetUpdateRatio() == ECutUpdate::kTwoTrack || cutmon.GetUpdateRatio() == ECutUpdate::kTwoTrackBackground)) {
      return kFALSE;
    }
    TString option = opt;
    if (Hal::Std::FindParam(option, "bckg") && Hal::Std::FindParam(option, "sig")) {  // bckg + sig options
      Hal::Std::FindParam(option, "bckg", kTRUE);
      Hal::Std::FindParam(option, "im", kTRUE);
      AddMonitor(cutmon, option + "bckg");
      AddMonitor(cutmon, option + "sig");
      return kTRUE;
    } else if (Hal::Std::FindParam(option, "both", kTRUE)) {  // both options
      AddMonitor(cutmon, option + "+sig");
      AddMonitor(cutmon, option + "+bckg");
      return kTRUE;
    } else if (!Hal::Std::FindParam(option, "bckg") && !Hal::Std::FindParam(option, "sig")) {  // no specification
      AddMonitor(cutmon, option + "+sig");
      AddMonitor(cutmon, option + "+bckg");
      return kTRUE;
    }
    return kFALSE;  // valid option - sig or bckg
  }

  Bool_t CutContainer::CheckTwoTracksOptions(const Cut& cut, Option_t* opt) {
    if (!(cut.GetUpdateRatio() == ECutUpdate::kTwoTrack || cut.GetUpdateRatio() == ECutUpdate::kTwoTrackBackground)) {
      return kFALSE;
    }
    TString option = opt;
    if (Hal::Std::FindParam(option, "bckg") && Hal::Std::FindParam(option, "sig")) {  // bckg + sig options
      Hal::Std::FindParam(option, "bckg", kTRUE);
      Hal::Std::FindParam(option, "sig", kTRUE);
      AddCut(cut, option);
      AddCut(cut, option);
      return kTRUE;
    } else if (Hal::Std::FindParam(option, "both", kTRUE)) {  // both options
      AddCut(cut, option + "+sig");
      AddCut(cut, option + "+bckg");
      return kTRUE;
    } else if (!Hal::Std::FindParam(option, "bckg") && !Hal::Std::FindParam(option, "sig")) {  // no specification
      AddCut(cut, option + "+sig");
      AddCut(cut, option + "+bckg");
      return kTRUE;
    }
    return kFALSE;  // valid option - sig or bckg
  }

  Int_t CutContainer::GetEventCollectionsNo() const {
    if (fSize < 1) return 0;
    return GetCutContainer(ECutUpdate::kEvent)->GetEntriesFast();
  }

  Int_t CutContainer::GetTrackCollectionsNo() const {
    if (fSize < 2) return 0;
    return GetCutContainer(ECutUpdate::kTrack)->GetEntriesFast();
  }

  Int_t CutContainer::GetTwoTrackCollectionsNo() const {
    if (fSize < 3) return 0;
    return GetCutContainer(ECutUpdate::kTwoTrack)->GetEntriesFast();
  }

  Int_t CutContainer::GetTwoTrackCollectionsBackgroundNo() const {
    if (fSize < 4) return 0;
    return GetCutContainer(ECutUpdate::kTwoTrackBackground)->GetEntriesFast();
  }

  Bool_t CutContainer::ExtrackRegExp2(const Cut& cut, Option_t* opt) {
    TString option = opt;
    Int_t number;
    if (!Hal::Std::FindExpressionSingleValue(option, number, kTRUE)) return kFALSE;
    Cut* temp_cut = cut.MakeCopy();
    temp_cut->SetCollectionID(number);
    AddCut(*temp_cut, option.Data());
    delete temp_cut;
    return kTRUE;
  }

  Bool_t CutContainer::ExtractRegExp2(const CutMonitor& monit, Option_t* opt) {
    TString option = opt;
    Int_t number;
    if (!Hal::Std::FindExpressionSingleValue(option, number, kTRUE)) return kFALSE;
    CutMonitor* temp_mon = monit.MakeCopy();
    temp_mon->SetCollectionID(number);
    AddMonitor(*temp_mon, option.Data());
    delete temp_mon;
    return kTRUE;
  }

  void CutContainer::MakeDummyCopies(ECutUpdate update, CutContainer* other, Bool_t copy_link) {
    if (static_cast<Int_t>(update) < 0) return;
    if (this->fSize <= static_cast<Int_t>(update)) {
      Cout::PrintInfo("Cannot Make Dummy copy of CutCollection!lack of space "
                      "in  target cut container!",
                      EInfo::kError);
      return;
    }
    if (other->fSize <= static_cast<Int_t>(update)) {
      Cout::PrintInfo("Cannot Make Dummy copy of CutCollection!! lack of "
                      "object in source cut container",
                      EInfo::kError);
      return;
    }
    if (this->fInit == kTRUE || other->fInit == kFALSE) {
      Cout::PrintInfo("You can't copy sub containers from not initialized cut container to initialized cut container!! ",
                      EInfo::kLowWarning);
    }
    for (int i = 0; i < other->GetCutContainer(update)->GetEntriesFast(); i++) {
      CutCollection* from = (CutCollection*) other->GetCutContainer(update)->UncheckedAt(i);
      CutCollection* to   = (CutCollection*) this->GetCutContainer(update)->UncheckedAt(i);
      to->MakeDummyCopy(from, copy_link);
    }
  }

  void CutContainer::ExtractComplexMonitor(CutMonitor* mon, TString& opt) {
    Int_t flag = 0;
    if (Hal::Std::FindParam(opt, "im") || Hal::Std::FindParam(opt, "re")) {
      if (opt.Contains("im")) flag = -1;
      if (opt.Contains("re")) flag = 1;
      opt.ReplaceAll("+im", "");
      opt.ReplaceAll("+re", "");
      opt.ReplaceAll("im", "");
      opt.ReplaceAll("re", "");
      Int_t size = 1;
      if (mon->InheritsFrom("Hal::CutMonitorXY")) size = 2;
      if (mon->InheritsFrom("Hal::CutMonitorXYZ")) size = 3;
      switch (size) {
        case 1: {
          MakeComplexAxis(mon, 0, flag);
        } break;
        case 2: {
          MakeComplexAxis(mon, 0, flag);
          MakeComplexAxis(mon, 1, flag);
        } break;
        case 3: {
          MakeComplexAxis(mon, 0, flag);
          MakeComplexAxis(mon, 1, flag);
          MakeComplexAxis(mon, 2, flag);
        } break;
      }
    }
  }

  void CutContainer::MakeComplexAxis(CutMonitor* mon, Int_t axis, Int_t flag) {
    TString cut_name = mon->GetCutName(axis);
    TClass* clas     = TClass::GetClass(cut_name, kTRUE, kTRUE);
    TString pattern  = "";
    if (clas->InheritsFrom("Hal::EventCut")) {
      if (flag == -1) {  // im
        pattern = "Hal::EventImaginaryCut";
      } else {  // re
        pattern = "Hal::EventRealCut";
      }
    } else if (clas->InheritsFrom("Hal::TrackCut")) {
      if (flag == -1) {  // im
        pattern = "Hal::TrackImaginaryCut";
      } else {  // re
        pattern = "Hal::TrackRealCut";
      }
    } else {
      if (flag == -1) {  // im
        pattern = "Hal::TwoTrackImaginaryCut";
      } else {  // re
        pattern = "Hal::TwoTrackRealCut";
      }
    }
    mon->fCutNames[axis] = Form("%s(%s)", pattern.Data(), mon->GetCutName(axis).Data());
  }

  Int_t CutContainer::GetCollectionsNo(ECutUpdate update) const {
    switch (update) {
      case ECutUpdate::kEvent: {
        return GetEventCollectionsNo();
      } break;
      case ECutUpdate::kTrack: {
        return GetTrackCollectionsNo();
      } break;
      case ECutUpdate::kTwoTrack: {
        return GetTwoTrackCollectionsNo();
      } break;
      case ECutUpdate::kTwoTrackBackground: {
        return GetTwoTrackCollectionsBackgroundNo();
      } break;
      case ECutUpdate::kNo: {
        return 0;
        // DO nothing
      } break;
    }
    return 0;
  }

  void CutContainer::AddVirtualCut(ECutUpdate update, Int_t col) {
    switch (update) {
      case ECutUpdate::kEvent: {
        EventVirtualCut v;
        v.SetCollectionID(col);
        AddCut(v);
      } break;
      case ECutUpdate::kTrack: {
        TrackVirtualCut v;
        v.SetCollectionID(col);
        AddCut(v);
      } break;
      case ECutUpdate::kTwoTrack: {
        TwoTrackVirtualCut v;
        v.SetCollectionID(col);
        AddCut(v, "sig");
      } break;
      case ECutUpdate::kTwoTrackBackground: {
        TwoTrackVirtualCut v;
        v.SetCollectionID(col);
        AddCut(v, "bckg");
      } break;
      case ECutUpdate::kNo: {
        /* do nothing */
      } break;
    }
  }

  Bool_t CutContainer::LinkCollections(ECutUpdate first, ECutUpdate last, ELinkPolicy policy) {
    Int_t lowLinks  = GetCollectionsNo(first);
    Int_t highLinks = GetCollectionsNo(last);
    if (lowLinks == 0) AddVirtualCut(first, 0);
    if (highLinks == 0) AddVirtualCut(last, 0);
    switch (policy) {
      case ELinkPolicy::kOneToMany: {
        if (lowLinks != 1) {
          Cout::PrintInfo("EventAna::LinkCollections one-to-many to much first collections!", EInfo::kError);
          return kFALSE;
        }
        for (int i = 0; i < highLinks; i++) {
          LinkCollections(first, 0, last, i);
        }
        return kTRUE;
      } break;
      case ELinkPolicy::kEqual: {
        if (lowLinks > highLinks) {
          Int_t jump = lowLinks / highLinks;
          if (lowLinks % highLinks) {
            Cout::PrintInfo("EventAna::LinkCollections equal link cannot group cuts!", EInfo::kError);
            return kFALSE;
          }
          for (int i = 0; i < highLinks; i++) {
            for (int j = 0; j < jump; j++) {
              LinkCollections(first, i * jump + j, last, i);
            }
          }
          return kTRUE;
        } else {
          Int_t jump = highLinks / lowLinks;
          if (highLinks % lowLinks) {
            Cout::PrintInfo("EventAna::LinkCollections equal link cannot group cuts!", EInfo::kError);
            return kFALSE;
          }
          for (int i = 0; i < lowLinks; i++) {
            for (int j = 0; j < jump; j++) {
              LinkCollections(first, i, last, i * jump + j);
            }
          }
          return kTRUE;
        }
      } break;
      case ELinkPolicy::kReplicateLast: {
        for (int i = 1; i < lowLinks; i++) {
          for (int j = 0; j < highLinks; j++) {
            ReplicateCollection(last, j, i * highLinks + j);
          }
        }
        for (int i = 0; i < lowLinks; i++) {
          for (int j = 0; j < highLinks; j++) {
            LinkCollections(first, i, last, i * highLinks + j);
          }
        }
        return kTRUE;
      } break;
      case ELinkPolicy::kReplicateFirst: {
        for (int i = 1; i < highLinks; i++) {
          for (int j = 0; j < lowLinks; j++) {
            ReplicateCollection(first, j, i * lowLinks + j);
          }
        }
        for (int i = 0; i < highLinks; i++) {
          for (int j = 0; j < lowLinks; j++) {
            LinkCollections(first, i * lowLinks + j, last, i);
          }
        }
        return kTRUE;
      } break;
      case ELinkPolicy::kAnyToAny: {
        for (int i = 0; i < lowLinks; i++) {
          for (int j = 0; j < highLinks; j++) {
            LinkCollections(first, i, last, j);
          }
        }
        return kTRUE;
      } break;
    }
    return kFALSE;
  }
}  // namespace Hal
