// -*- C++ -*-
//
// Package:    NtupleMaker
// Class:      EventMaker
// 
/**\class NtupleMaker NtupleMaker.cc CMS2/NtupleMaker/src/EventMaker.cc

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
*/
//
// Original Author:  pts/4
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: EventMaker.h,v 1.16 2010/04/15 11:28:59 jribnik Exp $
//
//
#ifndef NTUPLEMAKER_EVENTMAKER_H
#define NTUPLEMAKER_EVENTMAKER_H

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Scalers/interface/DcsStatus.h"

#include "TString.h"
//
// class decleration
//

class EventMaker : public edm::EDProducer {
public:
    explicit EventMaker (const edm::ParameterSet&);
    ~EventMaker();

private:
    virtual void beginJob() ;
    virtual void produce(edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;
    virtual void beginRun (const edm::Run& iRun, const edm::EventSetup& iSetup);

    std::string datasetName_;
    std::string CMS2tag_;

    edm::EDGetTokenT<DcsStatusCollection> dcsTag_;
    std::string aliasprefix_;
    bool isData_;
};


#endif
