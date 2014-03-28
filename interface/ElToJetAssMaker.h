// -*- C++ -*-
//
// Package:    NtupleMaker
// Class:      ElToJetAssMaker
// 
/**\class ElToJetAssMaker ElToJetAssMaker.h CMS2/NtupleMaker/interface/ElToJetAssMaker.h

Description: make associations between electrons and jets

*/
//
// Original Author:  Frank Golf
//         Created:  Wed Jun 25 18:32:24 UTC 2008
// $Id: ElToJetAssMaker.h,v 1.4 2010/03/18 02:19:31 kalavase Exp $
//
//
#ifndef CMS2_ELTOJETASSMAKER_H
#define CMS2_ELTOJETASSMAKER_H

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Math/interface/LorentzVector.h"

typedef math::XYZTLorentzVectorF LorentzVector;

//
// class declaration
//

class ElToJetAssMaker : public edm::EDProducer {
public:
  explicit ElToJetAssMaker (const edm::ParameterSet&);

private:
  virtual void beginJob() ;
  virtual void produce(edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
      
  // ----------member data ---------------------------
  double m_minDR_;
  edm::EDGetTokenT<std::vector<LorentzVector> > elsToken_;
  edm::EDGetTokenT<std::vector<LorentzVector> > jetsToken_;
  std::string aliasprefix_;
};

#endif
