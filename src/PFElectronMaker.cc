//-*- C++ -*-
//
// Package:    PFElectronMaker
// Class:      PFElectronMaker
// 
/**\class PFElectronMaker PFElectronMaker.cc CMS2/PFElectronMaker/src/PFElectronMaker.cc

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
*/
//
// Original Author:  Puneeth Kalavase
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: PFElectronMaker.cc,v 1.6 2012/03/16 20:02:29 dbarge Exp $


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowReco/interface/PFBlock.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "CMS2/NtupleMaker/interface/PFElectronMaker.h"

//
typedef math::XYZTLorentzVectorF LorentzVector;
typedef math::XYZPoint Point;
typedef edm::ValueMap<reco::PFCandidatePtr> PFCandMap;

//
using namespace reco;
using namespace edm;
using namespace std;

//
PFElectronMaker::PFElectronMaker(const ParameterSet& iConfig) {
      
  //
  pfCandidatesTag_    = iConfig.getParameter<InputTag> ("pfCandidatesTag");
  isoc_vm_tag_        = iConfig.getParameter<InputTag> ("isoc_vm_tag");
  ison_vm_tag_        = iConfig.getParameter<InputTag> ("ison_vm_tag");
  isop_vm_tag_        = iConfig.getParameter<InputTag> ("isop_vm_tag");
  isoc04_vm_tag_      = iConfig.getParameter<InputTag> ("isoc04_vm_tag");
  ison04_vm_tag_      = iConfig.getParameter<InputTag> ("ison04_vm_tag");
  isop04_vm_tag_      = iConfig.getParameter<InputTag> ("isop04_vm_tag");
  pfAllElectrons_tag_ = iConfig.getParameter<InputTag> ("pfAllElectrons_tag");     

  //
  produces<vector<LorentzVector> >  ("pfelsp4"                  ).setBranchAlias("pfels_p4"                 );
  produces<vector<LorentzVector> >  ("pfelsposAtEcalp4"         ).setBranchAlias("pfels_posAtEcal_p4"       );
  produces<vector<float> >          ("pfelsecalE"               ).setBranchAlias("pfels_ecalE"              );
  produces<vector<float> >          ("pfelshcalE"               ).setBranchAlias("pfels_hcalE"              );
  produces<vector<float> >          ("pfelsrawEcalE"            ).setBranchAlias("pfels_rawEcalE"           );
  produces<vector<float> >          ("pfelsrawHcalE"            ).setBranchAlias("pfels_rawHcalE"           );
  produces<vector<float> >          ("pfelspS1E"                ).setBranchAlias("pfels_pS1E"               );
  produces<vector<float> >          ("pfelspS2E"                ).setBranchAlias("pfels_pS2E"               );
  produces<vector<float> >          ("pfelsdeltaP"              ).setBranchAlias("pfels_deltaP"             );
  produces<vector<float> >          ("pfelsmvaepi"              ).setBranchAlias("pfels_mva_epi"            );
  produces<vector<float> >          ("pfelsmvaemu"              ).setBranchAlias("pfels_mva_emu"            );
  produces<vector<float> >          ("pfelsmvapimu"             ).setBranchAlias("pfels_mva_pimu"           );
  produces<vector<float> >          ("pfelsmvanothinggamma"     ).setBranchAlias("pfels_mva_nothing_gamma"  );
  produces<vector<float> >          ("pfelsmvanothingnh"        ).setBranchAlias("pfels_mva_nothing_nh"     );
  produces<vector<int> >            ("pfelscharge"              ).setBranchAlias("pfels_charge"             );
  produces<vector<int> >            ("pfelsparticleId"          ).setBranchAlias("pfels_particleId"         );
  produces<vector<int> >            ("pfelsflag"                ).setBranchAlias("pfels_flag"               );
  produces<vector<float> >          ("pfelsisoChargedHadrons"   ).setBranchAlias("pfels_isoChargedHadrons"  );
  produces<vector<float> >          ("pfelsisoNeutralHadrons"   ).setBranchAlias("pfels_isoNeutralHadrons"  );
  produces<vector<float> >          ("pfelsisoPhotons"          ).setBranchAlias("pfels_isoPhotons"         );
  produces<vector<float> >          ("pfelsiso04ChargedHadrons" ).setBranchAlias("pfels_iso04ChargedHadrons");
  produces<vector<float> >          ("pfelsiso04NeutralHadrons" ).setBranchAlias("pfels_iso04NeutralHadrons");
  produces<vector<float> >          ("pfelsiso04Photons"        ).setBranchAlias("pfels_iso04Photons"       );

} //

PFElectronMaker::~PFElectronMaker() {}
void  PFElectronMaker::beginRun(const edm::Run&, const EventSetup& es ) {}
void PFElectronMaker::beginJob() {}
void PFElectronMaker::endJob() {}

//
void PFElectronMaker::produce( Event& iEvent, const EventSetup& iSetup ) {

  auto_ptr<vector<LorentzVector> > pfels_p4                  (new vector<LorentzVector> );
  auto_ptr<vector<LorentzVector> > pfels_posAtEcal_p4        (new vector<LorentzVector> );
  auto_ptr<vector<float> >         pfels_ecalE               (new vector<float>        );
  auto_ptr<vector<float> >         pfels_hcalE               (new vector<float>  );
  auto_ptr<vector<float> >         pfels_rawEcalE            (new vector<float>  );
  auto_ptr<vector<float> >         pfels_rawHcalE            (new vector<float>  );
  auto_ptr<vector<float> >         pfels_pS1E                (new vector<float>  );
  auto_ptr<vector<float> >         pfels_pS2E                (new vector<float>  );
  auto_ptr<vector<float> >         pfels_deltaP              (new vector<float>  );
  auto_ptr<vector<float> >         pfels_mva_epi             (new vector<float>  );
  auto_ptr<vector<float> >         pfels_mva_emu             (new vector<float>  );
  auto_ptr<vector<float> >         pfels_mva_pimu            (new vector<float>  );
  auto_ptr<vector<float> >         pfels_mva_nothing_gamma   (new vector<float>  );
  auto_ptr<vector<float> >         pfels_mva_nothing_nh      (new vector<float>  );
  auto_ptr<vector<int> >           pfels_charge              (new vector<int>    );
  auto_ptr<vector<int> >           pfels_particleId          (new vector<int>    );
  auto_ptr<vector<int> >           pfels_flag                (new vector<int>    );
  auto_ptr<vector<float> >         pfels_isoChargedHadrons   (new vector<float>  );
  auto_ptr<vector<float> >         pfels_isoNeutralHadrons   (new vector<float>  );
  auto_ptr<vector<float> >         pfels_isoPhotons          (new vector<float>  );  
  auto_ptr<vector<float> >         pfels_iso04ChargedHadrons (new vector<float>  );
  auto_ptr<vector<float> >         pfels_iso04NeutralHadrons (new vector<float>  );
  auto_ptr<vector<float> >         pfels_iso04Photons        (new vector<float>  );  


  //
  Handle<PFCandMap > pfCandidatesHandle;
  iEvent.getByLabel( pfCandidatesTag_, pfCandidatesHandle );
  const ValueMap<reco::PFCandidatePtr> *pfCandidates  = pfCandidatesHandle.product();
  
  //
  PFCandMap::const_iterator pf_pit = pfCandidates->begin();
  unsigned int nC = pf_pit.size();
  for( unsigned int iC = 0; iC < nC; ++iC ) {

   const PFCandidatePtr& pf_it = pf_pit[iC];
   if ( pf_it.isNull() ) continue;
   int pfflags = 0;

   for( unsigned int i = 0; i < 17; i++ ) {
     if(pf_it->flag((PFCandidate::Flags)i)) pfflags |= (1<<i);
   }
  
   pfels_p4               ->push_back(LorentzVector(pf_it->px(), pf_it->py(), pf_it->pz(), pf_it->p()) );
   pfels_posAtEcal_p4     ->push_back(LorentzVector(pf_it->positionAtECALEntrance().x(), pf_it->positionAtECALEntrance().y(), pf_it->positionAtECALEntrance().z(), 0.0)       );
   pfels_ecalE            ->push_back( isfinite( pf_it->ecalEnergy() ) ? pf_it->ecalEnergy() : -9999.  );
   pfels_hcalE            ->push_back( pf_it->hcalEnergy()       );
   pfels_rawEcalE         ->push_back( pf_it->rawEcalEnergy()      );
   pfels_rawHcalE         ->push_back( pf_it->rawHcalEnergy()      );
   pfels_pS1E             ->push_back( pf_it->pS1Energy()       );
   pfels_pS2E             ->push_back( pf_it->pS2Energy()       );
   pfels_deltaP           ->push_back( pf_it->deltaP()       );
   pfels_mva_epi          ->push_back( pf_it->mva_e_pi()       );
   pfels_mva_emu          ->push_back( pf_it->mva_e_mu()       );
   pfels_mva_pimu         ->push_back( pf_it->mva_pi_mu()       );
   pfels_mva_nothing_gamma->push_back( pf_it->mva_nothing_gamma()      );
   pfels_mva_nothing_nh   ->push_back( pf_it->mva_nothing_nh()      );
   pfels_charge           ->push_back( pf_it->charge()       );
   pfels_particleId       ->push_back( pf_it->translateTypeToPdgId(pf_it->particleId())   );
   pfels_flag             ->push_back( pfflags                                                          );    
    
  } //
  

  //
  Handle<ValueMap<double> > isocHandle;
  iEvent.getByLabel( isoc_vm_tag_, isocHandle );
  ValueMap<double> isoc03_data = *isocHandle;

  Handle<ValueMap<double> > isonHandle;
  iEvent.getByLabel(ison_vm_tag_, isonHandle);
  ValueMap<double> ison03_data = *isonHandle;

  Handle<ValueMap<double> > isopHandle;
  iEvent.getByLabel(isop_vm_tag_, isopHandle);
  ValueMap<double> isop03_data = *isopHandle;

  Handle<ValueMap<double> > isoc04Handle;
  iEvent.getByLabel(isoc04_vm_tag_, isoc04Handle);
  ValueMap<double> isoc04_data = *isoc04Handle;

  Handle<ValueMap<double> > ison04Handle;
  iEvent.getByLabel(ison04_vm_tag_, ison04Handle);
  ValueMap<double> ison04_data = *ison04Handle;

  Handle<ValueMap<double> > isop04Handle;
  iEvent.getByLabel(isop04_vm_tag_, isop04Handle);
  ValueMap<double> isop04_data = *isop04Handle;

  //
  Handle<reco::PFCandidateCollection> pfAllElectronsHandle;
  iEvent.getByLabel( pfAllElectrons_tag_, pfAllElectronsHandle );
  for( unsigned int  pfit = 0; pfit < pfAllElectronsHandle->size(); pfit++ ) {
      
    reco::PFCandidateRef pfref(pfAllElectronsHandle, pfit);

    float isoc03 = (isoc03_data)[pfref];
    float ison03 = (ison03_data)[pfref];
    float isop03 = (isop03_data)[pfref];

    float isoc04 = (isoc04_data)[pfref];
    float ison04 = (ison04_data)[pfref];
    float isop04 = (isop04_data)[pfref];

    pfels_isoChargedHadrons   ->push_back(isoc03);
    pfels_isoNeutralHadrons   ->push_back(ison03);
    pfels_isoPhotons          ->push_back(isop03);

    pfels_iso04ChargedHadrons ->push_back(isoc04);
    pfels_iso04NeutralHadrons ->push_back(ison04);
    pfels_iso04Photons        ->push_back(isop04);

  } //


  //
  iEvent.put( pfels_p4                  , "pfelsp4"                 );
  iEvent.put( pfels_posAtEcal_p4        , "pfelsposAtEcalp4"        );
  iEvent.put( pfels_ecalE               , "pfelsecalE"              );
  iEvent.put( pfels_hcalE               , "pfelshcalE"              );
  iEvent.put( pfels_rawEcalE            , "pfelsrawEcalE"           );
  iEvent.put( pfels_rawHcalE            , "pfelsrawHcalE"           );
  iEvent.put( pfels_pS1E                , "pfelspS1E"               );
  iEvent.put( pfels_pS2E                , "pfelspS2E"               );
  iEvent.put( pfels_deltaP              , "pfelsdeltaP"             );
  iEvent.put( pfels_mva_epi             , "pfelsmvaepi"             );
  iEvent.put( pfels_mva_emu             , "pfelsmvaemu"             );
  iEvent.put( pfels_mva_pimu            , "pfelsmvapimu"            );
  iEvent.put( pfels_mva_nothing_gamma   , "pfelsmvanothinggamma"    );
  iEvent.put( pfels_mva_nothing_nh      , "pfelsmvanothingnh"       );
  iEvent.put( pfels_charge              , "pfelscharge"             );
  iEvent.put( pfels_particleId          , "pfelsparticleId"         );
  iEvent.put( pfels_flag                , "pfelsflag"               );
  iEvent.put( pfels_isoChargedHadrons   , "pfelsisoChargedHadrons"  );
  iEvent.put( pfels_isoNeutralHadrons   , "pfelsisoNeutralHadrons"  );
  iEvent.put( pfels_isoPhotons          , "pfelsisoPhotons"         );
  iEvent.put( pfels_iso04ChargedHadrons , "pfelsiso04ChargedHadrons");
  iEvent.put( pfels_iso04NeutralHadrons , "pfelsiso04NeutralHadrons");
  iEvent.put( pfels_iso04Photons        , "pfelsiso04Photons"       );
}

//define this as a plug-in
DEFINE_FWK_MODULE(PFElectronMaker);

