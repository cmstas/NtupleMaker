// -*- C++ -*-
//
// Package:    NtupleMaker
// Class:      BeamSpotMaker
// 
/**\class NtupleMaker NtupleMaker.cc CMS2/NtupleMaker/src/BeamSpotMaker.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  pts/4
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: BeamSpotMaker.h,v 1.3 2010/03/03 04:19:07 kalavase Exp $
//
//
#ifndef NTUPLEMAKER_BEAMSPOTMAKER_H
#define NTUPLEMAKER_BEAMSPOTMAKER_H

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class decleration
//

class BeamSpotMaker : public edm::EDProducer {
public:
     explicit BeamSpotMaker (const edm::ParameterSet&);
      ~BeamSpotMaker();

private:
     virtual void beginJob() ;
     virtual void produce(edm::Event&, const edm::EventSetup&);
     virtual void endJob() ;

   edm::EDGetTokenT<reco::BeamSpot> beamSpotToken;
    
	std::string aliasprefix_;
};


#endif
