// -*- C++ -*-
//
// Package:    NtupleMaker
// Class:      HLTMaker
// 
/**\class HLTMaker CMS3/NtupleMaker/src/HLTMaker.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  pts/4
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: HLTMaker.h,v 1.9 2010/11/08 22:45:10 jribnik Exp $
//
//
#ifndef NTUPLEMAKER_HLTMAKER_H
#define NTUPLEMAKER_HLTMAKER_H

// system include files
#include <algorithm>
#include <string>
#include <vector>
#include <memory>

// user include files
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/Math/interface/LorentzVector.h"

//#include "HLTrigger/HLTcore/interface/HLTPrescaleProvider.h"
#include "CommonTools/TriggerUtils/interface/PrescaleWeightProvider.h"

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//NOT IN miniAOD #include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "HLTrigger/HLTcore/interface/TriggerExpressionData.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionEvaluator.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionParser.h"
#include "L1Trigger/L1TGlobal/interface/L1TGlobalUtil.h"

#include "TRegexp.h"
#include "TString.h"
#include "TBits.h"

typedef math::XYZTLorentzVectorF LorentzVector;

class HLTMaker : public edm::stream::EDProducer<> {
public:
  explicit HLTMaker(const edm::ParameterSet&);
  ~HLTMaker() {}
  
private:
  virtual void beginRun(const edm::Run&, const edm::EventSetup&);
  virtual void beginLuminosityBlock(const edm::LuminosityBlock&, const edm::EventSetup&);
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() {}
  
  /*void fillTriggerObjectInfo(unsigned int,
			     std::vector<int>&,
			     std::vector<math::XYZTLorentzVectorF>&,
			     std::vector<bool>&,
			     std::vector<TString>&) const;*/
  bool doPruneTriggerName(const std::string&) const;

  edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjectsToken;
  edm::EDGetTokenT<edm::TriggerResults> triggerResultsToken;
  edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescaleToken;
  
  edm::Handle<edm::TriggerResults> triggerResultsH_;
  edm::Handle<trigger::TriggerEvent> triggerEventH_; 
  edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjectStandAlonesH_;
  edm::TriggerNames triggerNames_;

  HLTPrescaleProvider hltConfig_;
  
  std::string processName_;
  bool fillTriggerObjects_;
  std::vector<std::string> prunedTriggerNames_;
  TString processNamePrefix_;
  std::string aliasprefix_;

  // auto_ptr<vector<unsigned int> > cached_prescales   ;
  vector<unsigned int> cached_prescales   ;
  vector<unsigned int> cached_l1prescales   ;

  edm::EDGetToken algToken_;
  std::shared_ptr<l1t::L1TGlobalUtil> l1GtUtils_;

  // auto_ptr<vector<unsigned int> > prescales   ;
  // auto_ptr<vector<unsigned int> > l1prescales ;
  // auto_ptr<TBits>                           bits      ;
  // auto_ptr<vector<TString> >                trigNames ;
  // auto_ptr<vector<vector<int> > >           trigObjsid;
  // auto_ptr<vector<vector<LorentzVector> > > trigObjsp4;
  // auto_ptr<vector<vector<bool> > >          trigObjspassLast;
  // auto_ptr<vector<vector<TString> > >       trigObjsfilters;

  bool doFillInformation;
  // bool haveFilledInformation;

};

#endif
