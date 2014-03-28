// -*- C++ -*-
//
// Package:    NtupleMaker
// Class:      ASkimFilter
// 
/**\class ASkimFilter ASkimFilter.h CMS2/NtupleMaker/interface/ASkimFilter.h

   Description: generic filter for cms2

   Implementation:
   - get list of names of momentum vectors as input
   - event passes if any of these vectors have pt larger than configured cut

*/
//
// Original Author:  Ingo Bloch
//         Created:  Wed Jun 18 19:59:33 UTC 2008  
// $Id: ASkimFilter.h,v 1.5 2011/04/12 21:09:03 fgolf Exp $
//
//
#ifndef CMS2_ASKIMFILTER_H
#define CMS2_ASKIMFILTER_H

// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Math/interface/LorentzVector.h"

//
// class decleration
//

class ASkimFilter : public edm::EDFilter {
public:
    explicit ASkimFilter (const edm::ParameterSet&);
    ~ASkimFilter();
  
private:
    virtual void beginJob() ;
    virtual bool filter(edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;
   
    // ----------member data ---------------------------
    edm::EDGetTokenT<edm::View<reco::GsfElectron> > electronsToken_;
    edm::EDGetTokenT<edm::View<reco::Muon> > muonsToken_;
    bool useSTAMuons_;
    double eleFilterPtCut_;
    double muFilterPtCut_;
};


#endif
