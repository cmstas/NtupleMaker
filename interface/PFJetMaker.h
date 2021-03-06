// -*- C++ -*-
//
// Package:    PFJetMaker
// Class:      PFJetMaker
// 
/**\class PFJetMaker PFJetMaker.cc temp/PFJetMaker/src/PFJetMaker.cc

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
*/
//
// Original Author:  Puneeth Devanand KALAVASE
//         Created:  Tue Sep  1 22:18:18 CEST 2009
// $Id: PFJetMaker.h,v 1.9 2012/05/13 04:22:36 fgolf Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//For jet corrections
#include "DataFormats/PatCandidates/interface/JetCorrFactors.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

class PFJetMaker : public edm::stream::EDProducer<> {
public:
    explicit PFJetMaker(const edm::ParameterSet&);
    ~PFJetMaker();

private:
    virtual void beginJob() ;
    virtual void produce(edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;
      
    // ----------member data ---------------------------
    edm::EDGetTokenT<pat::PackedCandidateCollection> pfCandidatesToken;
    const pat::PackedCandidateCollection *pfCandidates;
    edm::EDGetTokenT<edm::View<pat::Jet> > pfJetsToken;
    edm::InputTag pfCandidatesTag_;
    std::string aliasprefix_;
    std::string PFJetCorrectorL2L3_;
    std::string PFJetCorrectorL1FastL2L3_;
    std::string PFJetCorrectorL1Fast_;             
};
