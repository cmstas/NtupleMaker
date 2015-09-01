// -*- C++ -*-
//
// Package:    FatJetMaker
// Class:      FatJetMaker
// 
/**\class FatJetMaker SubJetMaker.cc temp/SubJetMaker/src/SubJetMaker.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Puneeth Devanand KALAVASE
//         Created:  Tue Sep  1 22:18:18 CEST 2009
// $Id: FatJetMaker.h,v 1.9 2012/05/13 04:22:36 fgolf Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

//
// class decleration
//

class FatJetMaker : public edm::EDProducer {
public:
  explicit FatJetMaker(const edm::ParameterSet&);
  ~FatJetMaker();

private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
      
  // ----------member data ---------------------------
  edm::InputTag pfJetsInputTag_;
  double         pfJetPtCut_;
  std::string aliasprefix_;
//  std::string subjetprefix_;
  std::string NjettinessPrefix_;
  std::string LinksPrefix_;
  std::string PFJetCorrectorL2L3_;
  std::string PFJetCorrectorL1L2L3_;
  std::string PFJetCorrectorL1FastL2L3_;
  std::string PFJetCorrectorL1Fast_;
  std::string PFJetCorrectorL1FastL2L3residual_;
};
