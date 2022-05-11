/*
 * CutContainer.cxx
 *
 *  Created on: 06-08-2013
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */

#include "CutContainer.h"

#include <FairLogger.h>
#include <TClass.h>
#include <TCollection.h>
#include <TList.h>
#include <TRegexp.h>
#include <iostream>

#include "EventComplexCut.h"
#include "HalStdString.h"
#include "Package.h"
#include "Parameter.h"
#include "TrackComplexCut.h"
#include "TwoTrackComplexCut.h"

namespace Hal {
  CutContainer::CutContainer() : TObject(), fInit(kFALSE), fSize(0), fTempCutMonitors(NULL), fCutContainers(NULL) {}

  CutContainer::CutContainer(ECutUpdate tiers) : fInit(kFALSE), fTempCutMonitors(NULL), fCutContainers(NULL) {
    switch (tiers) {
      case ECutUpdate::kEventUpdate: fSize = 1; break;
      case ECutUpdate::kTrackUpdate: fSize = 2; break;
      case ECutUpdate::kTwoTrackUpdate: fSize = 3; break;
      case ECutUpdate::kTwoTrackBackgroundUpdate: fSize = 4; break;
      default: LOG(ERROR) << "Unknown update ration in CutContainer"; break;
    }
    fCutContainers   = new TObjArray*[fSize];
    fTempCutMonitors = new TObjArray*[fSize];
    for (int i = 0; i < fSize; i++) {
      fCutContainers[i]   = new TObjArray();
      fTempCutMonitors[i] = new TObjArray();
    }
    FairLogger* l = FairLogger::GetLogger();  // TODO Remove this when mpd switch to newer fairroot
  }

  CutContainer::CutContainer(const CutContainer& cont) :
    TObject(cont), fInit(kFALSE), fSize(cont.fSize), fTempCutMonitors(NULL), fCutContainers(NULL) {
    if (cont.fInit == kTRUE) { LOG(ERROR) << "CutContainer already initialized"; }
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
    if (HalStd::FindParam(option, "im", kTRUE)) {
      Cut* tempcut = NULL;
      Bool_t nulls = HalStd::FindParam(option, "null", kTRUE);
      if (cut.InheritsFrom("TrackCut")) {
        tempcut = new TrackImaginaryCut(static_cast<const TrackCut&>(cut));
        if (nulls) static_cast<TrackImaginaryCut*>(tempcut)->AcceptNulls(kTRUE);
      } else if (cut.InheritsFrom("EventCut")) {
        tempcut = new EventImaginaryCut(static_cast<const EventCut&>(cut));
        if (nulls) static_cast<EventImaginaryCut*>(tempcut)->AcceptNulls(kTRUE);
      } else if (cut.InheritsFrom("TwoTrackCut")) {
        tempcut = new TwoTrackImaginaryCut(static_cast<const TwoTrackCut&>(cut));
        if (nulls) static_cast<TwoTrackImaginaryCut*>(tempcut)->AcceptNulls(kTRUE);
      }
      if (tempcut == nullptr) return;
      tempcut->SetCollectionID(cut.GetCollectionID());
      AddCut(*tempcut, option);
      delete tempcut;
      return;
    } else if (HalStd::FindParam(option, "re", kTRUE)) {
      Cut* tempcut = NULL;
      if (cut.InheritsFrom("TrackCut")) {
        tempcut = new TrackRealCut(static_cast<const TrackCut&>(cut));
      } else if (cut.InheritsFrom("EventCut")) {
        tempcut = new EventRealCut(static_cast<const EventCut&>(cut));
      } else if (cut.InheritsFrom("TwoTrackCut")) {
        tempcut = new TwoTrackRealCut(static_cast<const TwoTrackCut&>(cut));
      }
      if (tempcut == nullptr) return;
      tempcut->SetCollectionID(cut.GetCollectionID());
      AddCut(*tempcut, option);
      delete tempcut;
      return;
    }
    if (ExtrackRegExp2(cut, opt)) {
      LOG(DEBUG4) << "CutContainer using ExtrackRegExp2";
      return;
    }
    if (ExtractRegExp(cut, opt)) {
      LOG(DEBUG4) << "CutContainer using ExtrackRegExp";
      return;
    }
    Int_t collection = cut.GetCollectionID();
    ECutUpdate place = cut.GetUpdateRatio();
    if (CheckTwoTracksOptions(cut, opt)) {
      LOG(DEBUG4) << "CutContainer using CheckTwoTracksOptions";
      return;
    }
    if (HalStd::FindParam(option, "bckg")) place = ECutUpdate::kTwoTrackBackgroundUpdate;
    if (fSize <= static_cast<Int_t>(place)) {
      TString update_ratio_name;
      switch (place) {
        case ECutUpdate::kEventUpdate: update_ratio_name = "event"; break;
        case ECutUpdate::kTrackUpdate: update_ratio_name = "track"; break;
        case ECutUpdate::kTwoTrackUpdate: update_ratio_name = "two_track"; break;
        case ECutUpdate::kTwoTrackBackgroundUpdate: update_ratio_name = "two_track<background>"; break;
        default:
          LOG(WARNING) << "Unknown update ratio for " << cut.CutName() << std::endl;
          update_ratio_name = "unknown";
          break;
      }
      LOG(WARNING) << Form("CutContainer can't hold %s cut because it's update ratio (%s) is "
                           "too big, check fTries or call SetOption(backround) before adding cuts "
                           "or cut monitors",
                           cut.ClassName(),
                           update_ratio_name.Data());
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
    if (update == ECutUpdate::kNoUpdate) return;
    if (CheckTwoTracksOptions(*monitor_copy, option)) {
      delete monitor_copy;
      return;
    }
    if (HalStd::FindParam(option, "bckg")) { update = ECutUpdate::kTwoTrackBackgroundUpdate; }
    if (fSize <= static_cast<Int_t>(update)) {
      LOG(WARNING) << "CutContainer can't hold this cut because it's update "
                      "ratio is to big, check fTries or call "
                      "SetOption(backround) before adding cuts or cut monitors";
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
    if (opt_high == opt_low || opt_low > opt_high) { LOG(ERROR) << "Wrong ECut Update in Link Collections"; }
    if (static_cast<Int_t>(opt_high) >= fSize) {
      LOG(ERROR) << "To big opt_high, link will be ingored";
    } else if (static_cast<Int_t>(opt_low) >= fSize) {
      LOG(ERROR) << "To big opt_low, link will be ingored";
    }
    CutCollection* low  = NULL;
    CutCollection* high = NULL;
    if (opt_low == ECutUpdate::kEventUpdate) {
      low = (CutCollection*) GetCutContainer(opt_low)->At(in_low);
    } else if (opt_low == ECutUpdate::kTrackUpdate) {
      low = (CutCollection*) GetCutContainer(opt_low)->At(in_low);
    } else {
      LOG(ERROR) << "Invalid opt_low argument in CutContainer::LinkCollections";
      return;
    }
    if (opt_high == ECutUpdate::kTrackUpdate) {
      high = (CutCollection*) GetCutContainer(opt_high)->At(in_high);
    } else if (opt_high == ECutUpdate::kTwoTrackUpdate || opt_high == ECutUpdate::kTwoTrackBackgroundUpdate) {
      high = (CutCollection*) GetCutContainer(opt_high)->At(in_high);
    } else {
      LOG(ERROR) << "Invalid opt_high argument in CutContainer::LinkCollections";
      return;
    }
    if (low == NULL || high == NULL) {
      LOG(ERROR) << "Cant link some collections";
      return;
    }
    high->AddPreviousAddr(in_low, 0);
    if (opt_high == ECutUpdate::kTwoTrackBackgroundUpdate) {
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
      if (obj->UncheckedAt(i) == NULL) { LOG(ERROR) << Form("Null cutsubcontainers at %i", i); }
      Int_t collection_no = ((CutCollection*) obj->UncheckedAt(i))->GetCollectionID();
      if (collection_no != i) { LOG(ERROR) << Form("Wrong order of cuts [%i]!=%i", i, collection_no); }
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
      LOG(WARNING) << "CutContainer has been initialized before";
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
            LOG(ERROR) << Form(" Collection %i for cut monitor %s not found", collection, clone->ClassName());
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
      LOG(DEBUG4) << "Initializing cut collection";
      for (int i = 0; i < fCutContainers[k]->GetEntriesFast(); i++) {
        LOG(DEBUG4) << "Initializing cut collection at " << k;
        ((CutCollection*) (fCutContainers[k]->UncheckedAt(i)))->Init(task_id);
      }
    }
    LOG(DEBUG4) << "Deleting temporary cut monitors";
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
      pack->AddObject(new ParameterInt("Event_collections_No", GetCutContainer(ECutUpdate::kEventUpdate)->GetEntriesFast()));
      TList* list1 = new TList();
      list1->SetOwner(kTRUE);
      list1->SetName("EventCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kEventUpdate)->GetEntriesFast(); i++) {
        list1->Add(((CutCollection*) GetCutContainer(ECutUpdate::kEventUpdate)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list1);
    }
    if (fSize > 1) {
      pack->AddObject(new ParameterInt("Track_collections_No", GetCutContainer(ECutUpdate::kTrackUpdate)->GetEntriesFast()));
      TList* list2 = new TList();
      list2->SetOwner(kTRUE);
      list2->SetName("TrackCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kTrackUpdate)->GetEntriesFast(); i++) {
        list2->Add(((CutCollection*) GetCutContainer(ECutUpdate::kTrackUpdate)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list2);
    }
    if (fSize > 2) {
      pack->AddObject(
        new ParameterInt("TwoTrack_collections_No", GetCutContainer(ECutUpdate::kTwoTrackUpdate)->GetEntriesFast()));
      TList* list3 = new TList();
      list3->SetOwner(kTRUE);
      list3->SetName("TwoTrackCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kTwoTrackUpdate)->GetEntriesFast(); i++) {
        list3->Add(((CutCollection*) GetCutContainer(ECutUpdate::kTwoTrackUpdate)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list3);
    }
    if (fSize > 3) {
      pack->AddObject(new ParameterInt("TwoTrack_collections_background_No",
                                       GetCutContainer(ECutUpdate::kTwoTrackBackgroundUpdate)->GetEntriesFast()));
      TList* list4 = new TList();
      list4->SetOwner(kTRUE);
      list4->SetName("TwoTrackBackgroundCutCollectionList");
      for (int i = 0; i < GetCutContainer(ECutUpdate::kTwoTrackBackgroundUpdate)->GetEntriesFast(); i++) {
        list4->Add(((CutCollection*) GetCutContainer(ECutUpdate::kTwoTrackBackgroundUpdate)->UncheckedAt(i))->Report());
      }
      pack->AddObject(list4);
    }
    pack->SetComment(this->ClassName());
    return pack;
  }

  Bool_t CutContainer::ExtractRegExp(const Cut& cut, Option_t* opt) {
    TString option = opt;
    Int_t number, jump;
    Bool_t found = HalStd::FindExpressionTwoValues(option, number, jump, kTRUE);
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
    Bool_t found = HalStd::FindExpressionTwoValues(option, number, jump, kTRUE);
    if (!found) return kFALSE;
    // found regular exprestion like {number x number}
    Int_t begin_collection = cut.GetCollectionID();
    if (begin_collection == -1) {
      LOG(WARNING) << Form("Adding cut monitor %s (first cut %s ) with collection no -1, with "
                           "regular expression like {AxB} set initial collection no to 0",
                           cut.ClassName(),
                           cut.GetCutName(0).Data());
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
    if (!(cutmon.GetUpdateRatio() == ECutUpdate::kTwoTrackUpdate
          || cutmon.GetUpdateRatio() == ECutUpdate::kTwoTrackBackgroundUpdate)) {
      return kFALSE;
    }
    TString option = opt;
    if (HalStd::FindParam(option, "bckg") && HalStd::FindParam(option, "sig")) {  // bckg + sig options
      HalStd::FindParam(option, "bckg", kTRUE);
      HalStd::FindParam(option, "im", kTRUE);
      AddMonitor(cutmon, option + "bckg");
      AddMonitor(cutmon, option + "sig");
      return kTRUE;
    } else if (HalStd::FindParam(option, "both", kTRUE)) {  // both options
      AddMonitor(cutmon, option + "+sig");
      AddMonitor(cutmon, option + "+bckg");
      return kTRUE;
    } else if (!HalStd::FindParam(option, "bckg") && !HalStd::FindParam(option, "sig")) {  // no specification
      AddMonitor(cutmon, option + "+sig");
      AddMonitor(cutmon, option + "+bckg");
      return kTRUE;
    }
    return kFALSE;  // valid option - sig or bckg
  }

  Bool_t CutContainer::CheckTwoTracksOptions(const Cut& cut, Option_t* opt) {
    if (!(cut.GetUpdateRatio() == ECutUpdate::kTwoTrackUpdate || cut.GetUpdateRatio() == ECutUpdate::kTwoTrackBackgroundUpdate)) {
      return kFALSE;
    }
    TString option = opt;
    if (HalStd::FindParam(option, "bckg") && HalStd::FindParam(option, "sig")) {  // bckg + sig options
      HalStd::FindParam(option, "bckg", kTRUE);
      HalStd::FindParam(option, "sig", kTRUE);
      AddCut(cut, option);
      AddCut(cut, option);
      return kTRUE;
    } else if (HalStd::FindParam(option, "both", kTRUE)) {  // both options
      AddCut(cut, option + "+sig");
      AddCut(cut, option + "+bckg");
      return kTRUE;
    } else if (!HalStd::FindParam(option, "bckg") && !HalStd::FindParam(option, "sig")) {  // no specification
      AddCut(cut, option + "+sig");
      AddCut(cut, option + "+bckg");
      return kTRUE;
    }
    return kFALSE;  // valid option - sig or bckg
  }

  Int_t CutContainer::GetEventCollectionsNo() const {
    if (fSize < 1) return 0;
    return GetCutContainer(ECutUpdate::kEventUpdate)->GetEntriesFast();
  }

  Int_t CutContainer::GetTrackCollectionsNo() const {
    if (fSize < 2) return 0;
    return GetCutContainer(ECutUpdate::kTrackUpdate)->GetEntriesFast();
  }

  Int_t CutContainer::GetTwoTrackCollectionsNo() const {
    if (fSize < 3) return 0;
    return GetCutContainer(ECutUpdate::kTwoTrackUpdate)->GetEntriesFast();
  }

  Int_t CutContainer::GetTwoTrackCollectionsBackgroundNo() const {
    if (fSize < 4) return 0;
    return GetCutContainer(ECutUpdate::kTwoTrackBackgroundUpdate)->GetEntriesFast();
  }

  Bool_t CutContainer::ExtrackRegExp2(const Cut& cut, Option_t* opt) {
    TString option = opt;
    Int_t number;
    if (!HalStd::FindExpressionSingleValue(option, number, kTRUE)) return kFALSE;
    Cut* temp_cut = cut.MakeCopy();
    temp_cut->SetCollectionID(number);
    AddCut(*temp_cut, option.Data());
    delete temp_cut;
    return kTRUE;
  }

  Bool_t CutContainer::ExtractRegExp2(const CutMonitor& monit, Option_t* opt) {
    TString option = opt;
    Int_t number;
    if (!HalStd::FindExpressionSingleValue(option, number, kTRUE)) return kFALSE;
    CutMonitor* temp_mon = monit.MakeCopy();
    temp_mon->SetCollectionID(number);
    AddMonitor(*temp_mon, option.Data());
    delete temp_mon;
    return kTRUE;
  }

  void CutContainer::MakeDummyCopies(ECutUpdate update, CutContainer* other, Bool_t copy_link) {
    if (static_cast<Int_t>(update) < 0) return;
    if (this->fSize <= static_cast<Int_t>(update)) {
      LOG(ERROR) << "Cannot Make Dummy copy of CutCollection!lack of space "
                    "in  target cut container!";
      return;
    }
    if (other->fSize <= static_cast<Int_t>(update)) {
      LOG(ERROR) << "Cannot Make Dummy copy of CutCollection!! lack of "
                    "object in source cut container";
      return;
    }
    if (this->fInit == kTRUE || other->fInit == kFALSE) {
      LOG(WARNING) << "You can't copy sub containers from not initialized cut "
                      "container to initialized cut container!! ";
    }
    for (int i = 0; i < other->GetCutContainer(update)->GetEntriesFast(); i++) {
      CutCollection* from = (CutCollection*) other->GetCutContainer(update)->UncheckedAt(i);
      CutCollection* to   = (CutCollection*) this->GetCutContainer(update)->UncheckedAt(i);
      to->MakeDummyCopy(from, copy_link);
    }
  }

  void CutContainer::ExtractComplexMonitor(CutMonitor* mon, TString& opt) {
    Int_t flag = 0;
    if (HalStd::FindParam(opt, "im") || HalStd::FindParam(opt, "re")) {
      if (opt.Contains("im")) flag = -1;
      if (opt.Contains("re")) flag = 1;
      opt.ReplaceAll("+im", "");
      opt.ReplaceAll("+re", "");
      opt.ReplaceAll("im", "");
      opt.ReplaceAll("re", "");
      Int_t size = 1;
      if (mon->InheritsFrom("CutMonitorXY")) size = 2;
      if (mon->InheritsFrom("CutMonitorXYZ")) size = 3;
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
    if (clas->InheritsFrom("EventCut")) {
      if (flag == -1) {  // im
        pattern = "EventImaginaryCut";
      } else {  // re
        pattern = "EventRealCut";
      }
    } else if (clas->InheritsFrom("TrackCut")) {
      if (flag == -1) {  // im
        pattern = "TrackImaginaryCut";
      } else {  // re
        pattern = "TrackRealCut";
      }
    } else {
      if (flag == -1) {  // im
        pattern = "TwoTrackImaginaryCut";
      } else {  // re
        pattern = "TwoTrackRealCut";
      }
    }
    mon->fCutNames[axis] = Form("%s(%s)", pattern.Data(), mon->GetCutName(axis).Data());
  }
}  // namespace Hal