// -*- C++ -*-
//
// Package:    CandToGenAssMaker
// Class:      CandToGenAssMaker
// 
/**\class CandToGenAssMaker CandToGenAssMaker.cc CMS3/NtupleMaker/src/CandToGenAssMaker.cc

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
*/
//
// Original Author:  Puneeth Kalavase
//         Created:  Tue Jul  22 11:07:38 CDT 2008
// $Id: CandToGenAssMaker.cc,v 1.21 2012/03/16 19:49:21 dbarge Exp $
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
#include "CMS3/NtupleMaker/interface/CandToGenAssMaker.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "Math/VectorUtil.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "CMS3/NtupleMaker/interface/MatchUtilities.h"
#include "CMS3/NtupleMaker/interface/MCUtilities.h"


typedef math::XYZTLorentzVectorF LorentzVector;
using std::vector;

CandToGenAssMaker::CandToGenAssMaker(const edm::ParameterSet& iConfig)
{
    ntuplePackedGenParticles_   = iConfig.getParameter<bool>                      ("ntuplePackedGenParticles");

    // electron matched to gen particle
    produces<vector<int>           >("elsmcid"            	).setBranchAlias("els_mc_id"          		); 
    produces<vector<int>           >("elsmcmotherid"      	).setBranchAlias("els_mc_motherid"    		);
    if (ntuplePackedGenParticles_) produces<vector<int>           >("elsmcidx"           	).setBranchAlias("els_mcidx"          		);
    produces<vector<LorentzVector> >("elsmcp4"            	).setBranchAlias("els_mc_p4"          		);
    produces<vector<LorentzVector> >("elsmcmotherp4"      	).setBranchAlias("els_mc_motherp4"    		);
    produces<vector<float>         >("elsmcdr"            	).setBranchAlias("els_mcdr"           		);
    produces<vector<int>           >("elsmc3id"           	).setBranchAlias("els_mc3_id"         		); 
    produces<vector<int>           >("elsmc3idx"          	).setBranchAlias("els_mc3idx"         		);
    produces<vector<int>           >("elsmc3motherid"     	).setBranchAlias("els_mc3_motherid"   		);
    produces<vector<int>           >("elsmc3motheridx"    	).setBranchAlias("els_mc3_motheridx"  		);
    produces<vector<float>         >("elsmc3dr"           	).setBranchAlias("els_mc3dr"          		);

    //photons matched to gen particles
    produces<vector<int>           >("photonsmcid"            	).setBranchAlias("photons_mc_id"          	); 
    produces<vector<int>           >("photonsmcmotherid"      	).setBranchAlias("photons_mc_motherid"    	);
    if (ntuplePackedGenParticles_) produces<vector<int>           >("photonsmcidx"           	).setBranchAlias("photons_mcidx"          	);
    produces<vector<LorentzVector> >("photonsmcp4"            	).setBranchAlias("photons_mc_p4"          	);
    produces<vector<LorentzVector> >("photonsmcmotherp4"      	).setBranchAlias("photons_mc_motherp4"    	);
    produces<vector<float>         >("photonsmcdr"            	).setBranchAlias("photons_mcdr"           	);
    produces<vector<int>           >("photonsmc3id"           	).setBranchAlias("photons_mc3_id"         	); 
    produces<vector<int>           >("photonsmc3idx"          	).setBranchAlias("photons_mc3idx"         	);
    produces<vector<int>           >("photonsmc3motherid"     	).setBranchAlias("photons_mc3_motherid"   	);
    produces<vector<int>           >("photonsmc3motheridx"    	).setBranchAlias("photons_mc3_motheridx"  	);
    produces<vector<float>         >("photonsmc3dr"           	).setBranchAlias("photons_mc3dr"          	);


    // muon matched to gen particle
    produces<vector<int>           >("musmcid"            	).setBranchAlias("mus_mc_id"          		);
    produces<vector<int>           >("musmcmotherid"      	).setBranchAlias("mus_mc_motherid"    		);
    if (ntuplePackedGenParticles_) produces<vector<int>           >("musmcidx"           	).setBranchAlias("mus_mcidx"          		);
    produces<vector<LorentzVector> >("musmcp4"            	).setBranchAlias("mus_mc_p4"          		);
    produces<vector<LorentzVector> >("musmcmotherp4"      	).setBranchAlias("mus_mc_motherp4"    		);
    produces<vector<float>         >("musmcdr"            	).setBranchAlias("mus_mcdr"           		);
    produces<vector<int>           >("musmc3id"           	).setBranchAlias("mus_mc3_id"         		);
    produces<vector<int>           >("musmc3motherid"     	).setBranchAlias("mus_mc3_motherid"   		);
    produces<vector<int>           >("musmc3idx"          	).setBranchAlias("mus_mc3idx"         		);
    produces<vector<int>           >("musmc3motheridx"    	).setBranchAlias("mus_mc3_motheridx"  		);
    produces<vector<float>         >("musmc3dr"           	).setBranchAlias("mus_mc3dr"          		);

    //info of matched genJet
    produces<vector<float>         >("pfjetsmcdr"           	).setBranchAlias("pfjets_mcdr"          	);
    produces<vector<LorentzVector> >("pfjetsmcp4"           	).setBranchAlias("pfjets_mc_p4"         	); // p4 of the matched GenJet

    //info of matched gen particle
    produces<vector<int>           >("pfjetsmcmotherid"     	).setBranchAlias("pfjets_mc_motherid"   	); // id of the status=1 particle matched to the jet
    //info of matched status 3 particle
    produces<vector<float>         >("pfjetsmc3dr"          	).setBranchAlias("pfjets_mc3dr"         	); // index of matched status==3 particle
    produces<vector<int>           >("pfjetsmc3idx"         	).setBranchAlias("pfjets_mc3idx"        	); // index of matched status==3 particle
    produces<vector<int>           >("pfjetsmc3id"          	).setBranchAlias("pfjets_mc3_id"        	); // id of matched status ==3 particle


    genParticlesTokenPacked_ = consumes<pat::PackedGenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticlesInputTagPacked"));
    genParticlesTokenPruned_ = consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticlesInputTagPruned"));
    genJetsToken_      = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genJetsInputTag"     ));
    muonsToken_        = consumes<vector<LorentzVector> >(iConfig.getParameter<edm::InputTag>("muonsInputTag"       ));
    electronsToken_    = consumes<vector<LorentzVector> >(iConfig.getParameter<edm::InputTag>("electronsInputTag"   ));
    photonsToken_      = consumes<vector<LorentzVector> >(iConfig.getParameter<edm::InputTag>("photonsInputTag"     ));
    pfJetsToken_       = consumes<vector<LorentzVector> >(iConfig.getParameter<edm::InputTag>("pfJetsInputTag"      ));

    jetsInputTag_         = iConfig.getParameter<edm::InputTag>("jetsInputTag"        );
    tracksInputTag_       = iConfig.getParameter<edm::InputTag>("tracksInputTag"      );
    vPIDsToExclude_       = iConfig.getUntrackedParameter<std::vector<int> >("vPIDsToExclude"   );
}

void CandToGenAssMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {


    using namespace edm;
    using namespace std;
    using namespace reco;


    unique_ptr<vector<int>           > vector_els_mc_motherid    (new vector<int>          );
    unique_ptr<vector<int>           > vector_els_mcidx          (new vector<int>          );
    unique_ptr<vector<int>           > vector_els_mc_id          (new vector<int>          );
    unique_ptr<vector<LorentzVector> > vector_els_mcp4           (new vector<LorentzVector>);
    unique_ptr<vector<LorentzVector> > vector_els_mc_motherp4    (new vector<LorentzVector>);
    unique_ptr<vector<float>         > vector_els_mcdr           (new vector<float>        );
    unique_ptr<vector<int>           > vector_els_mc3_motherid   (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<int>           > vector_els_mc3idx         (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<int>           > vector_els_mc3_id         (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<int>           > vector_els_mc3_motheridx  (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<float>         > vector_els_mc3dr          (new vector<float>        ); //matched status 3 part only


    unique_ptr<vector<int>           > vector_photons_mc_motherid    (new vector<int>          );
    unique_ptr<vector<int>           > vector_photons_mcidx          (new vector<int>          );
    unique_ptr<vector<int>           > vector_photons_mc_id          (new vector<int>          );
    unique_ptr<vector<LorentzVector> > vector_photons_mcp4           (new vector<LorentzVector>);
    unique_ptr<vector<LorentzVector> > vector_photons_mc_motherp4    (new vector<LorentzVector>);
    unique_ptr<vector<float>         > vector_photons_mcdr           (new vector<float>        );
    unique_ptr<vector<int>           > vector_photons_mc3_motherid   (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<int>           > vector_photons_mc3idx         (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<int>           > vector_photons_mc3_id         (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<int>           > vector_photons_mc3_motheridx  (new vector<int>          ); //matched status 3 part only
    unique_ptr<vector<float>         > vector_photons_mc3dr          (new vector<float>        ); //matched status 3 part only



    unique_ptr<vector<int>           > vector_mus_mc_id          (new vector<int>          );
    unique_ptr<vector<int>           > vector_mus_mc_motherid    (new vector<int>          );
    unique_ptr<vector<int>           > vector_mus_mcidx          (new vector<int>          );
    unique_ptr<vector<LorentzVector> > vector_mus_mcp4           (new vector<LorentzVector>);
    unique_ptr<vector<LorentzVector> > vector_mus_mc_motherp4    (new vector<LorentzVector>);
    unique_ptr<vector<float>         > vector_mus_mcdr           (new vector<float>        );
    unique_ptr<vector<int>           > vector_mus_mc3_id         (new vector<int>          );
    unique_ptr<vector<int>           > vector_mus_mc3_motherid   (new vector<int>          );
    unique_ptr<vector<int>           > vector_mus_mc3idx         (new vector<int>          );
    unique_ptr<vector<int>           > vector_mus_mc3_motheridx  (new vector<int>          );
    unique_ptr<vector<float>         > vector_mus_mc3dr          (new vector<float>        );

    // pfjets
    //info of matched genJet
    unique_ptr<vector<float>         > vector_pfjets_mcdr          (new vector<float>        );
    unique_ptr<vector<LorentzVector> > vector_pfjets_mc_p4         (new vector<LorentzVector>); 
    //info of matched gen particle
    unique_ptr<vector<int>           > vector_pfjets_mc_motherid   (new vector<int>          );
    //info of matched status 3 particle
    unique_ptr<vector<float>         > vector_pfjets_mc3dr         (new vector<float>        );
    unique_ptr<vector<int>           > vector_pfjets_mc3idx        (new vector<int>          );
    unique_ptr<vector<int>           > vector_pfjets_mc3_id        (new vector<int>          );  
  

    // get Packed Gen Particle collection (miniAOD) (all status 1 particles, compressed)
    edm::Handle<pat::PackedGenParticleCollection> genParticlesHandleStatus1;
    iEvent.getByToken(genParticlesTokenPacked_, genParticlesHandleStatus1);
    if( !genParticlesHandleStatus1.isValid() ) {
        throw cms::Exception("CandToGenAssMaker::produce: error getting genParticlesHandleStatus1 from Event!");
    }
    const vector<pat::PackedGenParticle> *v_genParticlesS1 = genParticlesHandleStatus1.product();

    // get Pruned Gen Particle collection (miniAOD) (all status 3, and some others)
    edm::Handle<reco::GenParticleCollection> genParticlesHandleStatus3;
    iEvent.getByToken(genParticlesTokenPruned_, genParticlesHandleStatus3);
    if( !genParticlesHandleStatus3.isValid() ) {
        throw cms::Exception("CandToGenAssMaker::produce: error getting genParticlesHandleStatus3 from Event!");
    }
    const vector<GenParticle> *v_genParticlesS3 = genParticlesHandleStatus3.product();

    //get MC Jets
    Handle<reco::GenJetCollection> genJetsHandle;
    iEvent.getByToken(genJetsToken_, genJetsHandle);
    if( !genJetsHandle.isValid() ) {
        throw cms::Exception("CandToGenAssMaker::produce: error getting genJets from Event!");
    }

    // get muons
    Handle<vector<LorentzVector> > muonHandle;
    iEvent.getByToken(muonsToken_, muonHandle);     
    if( !muonHandle.isValid() ) {
        throw cms::Exception("CandToGenAssMaker::produce: error getting muons from Event!");
    }

    // get electrons
    Handle<vector<LorentzVector> > electronHandle;
    iEvent.getByToken(electronsToken_, electronHandle);     
    if( !electronHandle.isValid() ) {
        throw cms::Exception("CandToGenAssMaker::produce: error getting electrons from Event!");
    }

    // get pf jets
    Handle<vector<LorentzVector> > pfJetsHandle;
    iEvent.getByToken(pfJetsToken_, pfJetsHandle);
    if( !pfJetsHandle.isValid() ) {
        throw cms::Exception("CandToGenAssMaker::produce: error getting pfJets from Event!");
    }

    //get the photons
    Handle<vector<LorentzVector> > photonsHandle;
    iEvent.getByToken(photonsToken_, photonsHandle);
    if( !photonsHandle.isValid() ) {
        throw cms::Exception("CandToGenAssMaker::produce: error getting photons from Event!");
    }

    // *********************************** Fill electrons ************************************//
    for (vector<LorentzVector>::const_iterator elsp4_it = electronHandle->begin();
	 elsp4_it != electronHandle->end(); ++elsp4_it) {

        //MC matching stuff
        int mcid = -9999, mom_mcid = -9999, genidx = -9999, mc3_motheridx = -9999;
        LorentzVector mc_p4(0,0,0,0);
        LorentzVector mc_motherp4(0,0,0,0);
        float dR = -9999;

    
        const pat::PackedGenParticle* matchedGenParticle = MatchUtilities::matchCandToGen(*elsp4_it, 
                                                                                          v_genParticlesS1,
                                                                                          genidx, 1, vPIDsToExclude_);
        if(matchedGenParticle != 0) {
            const GenParticle* matchedMotherParticle = MCUtilities::motherIDPacked(*matchedGenParticle); 
            mcid                = matchedGenParticle->pdgId();
            mc_p4               = matchedGenParticle->p4();
            if(matchedMotherParticle != 0) { 
                mom_mcid            = matchedMotherParticle->pdgId() ;
                mc_motherp4         = matchedMotherParticle->p4();
            }
            dR                  = ROOT::Math::VectorUtil::DeltaR(mc_p4, *elsp4_it);    
            //cout<<"Looking at electron with pt/eta/phi "<< elsp4_it->pt() <<" "<<elsp4_it->eta() <<" "<<elsp4_it->phi() <<endl;
            //cout<<"Found status 1 match with id/pt and mother id/pt "<< mcid<<"/"<<mc_p4.pt() <<" "<< mom_mcid<<"/"<<mc_motherp4.pt()<<endl;
        }

        // fill vector
        vector_els_mc_id      ->push_back(mcid        );
        vector_els_mc_motherid->push_back(mom_mcid    );
        if (ntuplePackedGenParticles_) vector_els_mcidx      ->push_back(genidx      );
        vector_els_mcp4       ->push_back(mc_p4       );
        vector_els_mc_motherp4->push_back(mc_motherp4 );
        vector_els_mcdr       ->push_back( dR         );

        mcid = -9999;
        mom_mcid = -9999;
        genidx = -9999;
        mc_p4 = LorentzVector(0,0,0,0);
        mc_motherp4 = LorentzVector(0,0,0,0);
        mc3_motheridx = -9999;
        dR = -9999;
        const GenParticle* matchedGenParticleDoc = MatchUtilities::matchCandToGen(*elsp4_it, 
                                                                                  v_genParticlesS3,
                                                                                  genidx, 999, vPIDsToExclude_);
        //now do the status==3 particles
        if(matchedGenParticleDoc != 0 ) {
            const GenParticle* matchedMotherParticle = MCUtilities::motherID(*matchedGenParticleDoc);
            mcid                = matchedGenParticleDoc->pdgId();
            mc_p4               = matchedGenParticleDoc->p4();
            mom_mcid            = matchedMotherParticle->pdgId();
            mc3_motheridx       = MatchUtilities::getMatchedGenIndex(*matchedMotherParticle, v_genParticlesS3, 999, vPIDsToExclude_);
            dR                  = ROOT::Math::VectorUtil::DeltaR(mc_p4, *elsp4_it);      
            //cout<<"Found status 3 match with pt and mother idx "<< mc_p4.pt() <<" "<<mc3_motheridx<<endl;
        }
    
        vector_els_mc3_id        ->push_back(mcid         );
        vector_els_mc3_motherid  ->push_back(mom_mcid     );
        vector_els_mc3idx        ->push_back(genidx       );
        vector_els_mc3_motheridx ->push_back(mc3_motheridx);
        vector_els_mc3dr         ->push_back( dR          );
    
    }
    // ****************************************************************************************************//
  
    // *********************************************  Fill Photons  ***************************************//
    for (vector<LorentzVector>::const_iterator photonsp4_it = photonsHandle->begin();
	 photonsp4_it != photonsHandle->end(); ++photonsp4_it) {

        //MC matching stuff
        int mcid = -9999, mom_mcid = -9999, genidx = -9999, mc3_motheridx = -9999;
        LorentzVector mc_p4(0,0,0,0);
        LorentzVector mc_motherp4(0,0,0,0);
        float dR = -9999;

    
        const pat::PackedGenParticle* matchedGenParticle = MatchUtilities::matchCandToGen(*photonsp4_it, 
                                                                                          v_genParticlesS1,
                                                                                          genidx, 1, vPIDsToExclude_);
        if(matchedGenParticle != 0) {
            const GenParticle* matchedMotherParticle = MCUtilities::motherIDPacked(*matchedGenParticle);
            mcid                = matchedGenParticle->pdgId();
            mc_p4               = matchedGenParticle->p4();
            if(matchedMotherParticle != 0) { 
                mom_mcid            = matchedMotherParticle->pdgId();
                mc_motherp4         = matchedMotherParticle->p4();
            }
            dR                  = ROOT::Math::VectorUtil::DeltaR(mc_p4, *photonsp4_it);      
        }

        // fill vector
        vector_photons_mc_id      ->push_back(mcid        );
        vector_photons_mc_motherid->push_back(mom_mcid    );
        if (ntuplePackedGenParticles_) vector_photons_mcidx      ->push_back(genidx      );
        vector_photons_mcp4       ->push_back(mc_p4       );
        vector_photons_mc_motherp4->push_back(mc_motherp4 );
        vector_photons_mcdr       ->push_back( dR         );

        mcid = -9999;
        mom_mcid = -9999;
        genidx = -9999;
        mc_p4 = LorentzVector(0,0,0,0);
        mc_motherp4 = LorentzVector(0,0,0,0);
        mc3_motheridx = -9999;
        dR = -9999;
        const GenParticle* matchedGenParticleDoc = MatchUtilities::matchCandToGen(*photonsp4_it, 
                                                                                  v_genParticlesS3,
                                                                                  genidx, 999, vPIDsToExclude_);
        //now do the status==3 particles
        if(matchedGenParticleDoc != 0 ) {
            const GenParticle* matchedMotherParticle = MCUtilities::motherID(*matchedGenParticleDoc);
            mcid                = matchedGenParticleDoc->pdgId();
            mc_p4               = matchedGenParticleDoc->p4();
            mom_mcid            = matchedMotherParticle->pdgId();
            mc3_motheridx       = MatchUtilities::getMatchedGenIndex(*matchedMotherParticle, v_genParticlesS3, 999, vPIDsToExclude_);
            dR                  = ROOT::Math::VectorUtil::DeltaR(mc_p4, *photonsp4_it);      
        }
    
        vector_photons_mc3_id        ->push_back(mcid         );
        vector_photons_mc3_motherid  ->push_back(mom_mcid     );
        vector_photons_mc3idx        ->push_back(genidx       );
        vector_photons_mc3_motheridx ->push_back(mc3_motheridx);
        vector_photons_mc3dr         ->push_back( dR          );
    
    }
    // ****************************************************************************************************//


    // ********************************************* Fill Muons ******************************************//
    for (vector<LorentzVector>::const_iterator musp4_it = muonHandle->begin(); 
	 musp4_it != muonHandle->end();
         ++musp4_it) {

        //MC matching stuff
        int mcid = -9999, mom_mcid = -9999, genidx = -9999, mc3_motheridx = -9999;
        LorentzVector mc_p4(0,0,0,0);
        LorentzVector mc_motherp4(0,0,0,0);
        float dR = -9999;
    
        const pat::PackedGenParticle* matchedGenParticle = MatchUtilities::matchCandToGen(*musp4_it,
                                                                                          v_genParticlesS1,
                                                                                          genidx, 1, vPIDsToExclude_);

        if(matchedGenParticle != 0) {
            const GenParticle* matchedMotherParticle = MCUtilities::motherIDPacked(*matchedGenParticle);
            mcid                = matchedGenParticle->pdgId();
            mc_p4               = matchedGenParticle->p4();
            if(matchedMotherParticle != 0) { 
                mom_mcid            = matchedMotherParticle->pdgId();
                mc_motherp4         = matchedMotherParticle->p4();
            }
            dR                  = ROOT::Math::VectorUtil::DeltaR(mc_p4, *musp4_it);
        }

        // fill vector
        vector_mus_mc_id      ->push_back(mcid        );
        vector_mus_mc_motherid->push_back(mom_mcid    );
        if (ntuplePackedGenParticles_) vector_mus_mcidx      ->push_back(genidx      );
        vector_mus_mcp4       ->push_back(mc_p4       );
        vector_mus_mc_motherp4->push_back(mc_motherp4 );
        vector_mus_mcdr       ->push_back( dR         );

        mcid = -9999;
        mom_mcid = -9999;
        genidx = -9999;
        mc_p4 = LorentzVector(0,0,0,0);
        mc3_motheridx = -9999;
        dR = -9999;
    
        const GenParticle* matchedGenParticleDoc = MatchUtilities::matchCandToGen(*musp4_it, 
                                                                                  v_genParticlesS3,
                                                                                  genidx, 999, vPIDsToExclude_);
        if(matchedGenParticleDoc != 0) {
            const GenParticle* matchedMotherParticle = MCUtilities::motherID(*matchedGenParticleDoc);
            mcid                = matchedGenParticleDoc->pdgId();
            mc_p4               = matchedGenParticleDoc->p4();
            mom_mcid            = matchedMotherParticle->pdgId();
            mc3_motheridx       = MatchUtilities::getMatchedGenIndex(*matchedMotherParticle, v_genParticlesS3, 999, vPIDsToExclude_);
            dR                  = ROOT::Math::VectorUtil::DeltaR(mc_p4, *musp4_it);
        }

        // fill vector
        vector_mus_mc3_id        ->push_back(mcid          );
        vector_mus_mc3_motherid  ->push_back(mom_mcid      );
        vector_mus_mc3idx        ->push_back(genidx        );
        vector_mus_mc3_motheridx ->push_back(mc3_motheridx );
        vector_mus_mc3dr         ->push_back( dR           );

    }

    // ***************************************  fill PFJets *************************************************//
    for(vector<LorentzVector>::const_iterator pfjetsp4_it = pfJetsHandle->begin();
        pfjetsp4_it != pfJetsHandle->end();
        pfjetsp4_it++) {

        int idx = -9999;
        const GenJet* matchedGenJet = MatchUtilities::matchCandToGenJet(*pfjetsp4_it,genJetsHandle.product(), idx);
    
        if ( matchedGenJet != 0 ) {
            vector_pfjets_mcdr          ->push_back(ROOT::Math::VectorUtil::DeltaR(*pfjetsp4_it, (*matchedGenJet).p4() ));
            vector_pfjets_mc_p4         ->push_back( LorentzVector( matchedGenJet->p4() ) );
        } else {
            vector_pfjets_mcdr           ->push_back(-9999  );
            vector_pfjets_mc_p4          ->push_back(LorentzVector(0,0,0,0));
        }

        int temp;
        const pat::PackedGenParticle* matchedGenParticle = MatchUtilities::matchCandToGen(*pfjetsp4_it, 
                                                                                          v_genParticlesS1,
                                                                                          temp, 1, vPIDsToExclude_);

        if ( matchedGenParticle != 0 ) {
            const GenParticle* matchedMotherParticle = MCUtilities::motherIDPacked(*matchedGenParticle			);
            vector_pfjets_mc_motherid	->push_back(matchedMotherParticle != 0 ? matchedMotherParticle->pdgId()              : -9999	         	);
        } else {
            vector_pfjets_mc_motherid ->push_back(-9999);
        }

        const GenParticle* matchedGenParticleDoc = MatchUtilities::matchCandToGen(*pfjetsp4_it, 
                                                                                  v_genParticlesS3,
                                                                                  temp, 23, vPIDsToExclude_);
        if ( matchedGenParticleDoc != 0 ) {
            vector_pfjets_mc3dr    ->push_back(ROOT::Math::VectorUtil::DeltaR(*pfjetsp4_it, (*matchedGenParticleDoc).p4() ));
            vector_pfjets_mc3idx   ->push_back(temp);
            vector_pfjets_mc3_id   ->push_back(matchedGenParticleDoc->pdgId());
        } else {
            vector_pfjets_mc3dr    ->push_back(-9999);
            vector_pfjets_mc3idx   ->push_back(-9999);
            vector_pfjets_mc3_id   ->push_back(-9999);
        }
    
    }//jets 
    // ****************************************************************************************//

    iEvent.put(std::move(vector_els_mc_id          		),"elsmcid"          	);
    iEvent.put(std::move(vector_els_mc_motherid    		),"elsmcmotherid"    	);
    if (ntuplePackedGenParticles_) iEvent.put(std::move(vector_els_mcidx          		),"elsmcidx"         	);
    iEvent.put(std::move(vector_els_mcp4           		),"elsmcp4"          	);
    iEvent.put(std::move(vector_els_mc_motherp4    		),"elsmcmotherp4"    	);
    iEvent.put(std::move(vector_els_mcdr           		),"elsmcdr"          	);
    iEvent.put(std::move(vector_els_mc3_id         		),"elsmc3id"         	);
    iEvent.put(std::move(vector_els_mc3_motherid   		),"elsmc3motherid"   	);
    iEvent.put(std::move(vector_els_mc3idx         		),"elsmc3idx"        	);
    iEvent.put(std::move(vector_els_mc3_motheridx  		),"elsmc3motheridx"  	);
    iEvent.put(std::move(vector_els_mc3dr          		),"elsmc3dr"         	);

    iEvent.put(std::move(vector_photons_mc_id          	),"photonsmcid"          );
    iEvent.put(std::move(vector_photons_mc_motherid    	),"photonsmcmotherid"    );
    if (ntuplePackedGenParticles_) iEvent.put(std::move(vector_photons_mcidx          	),"photonsmcidx"         );
    iEvent.put(std::move(vector_photons_mcp4           	),"photonsmcp4"          );
    iEvent.put(std::move(vector_photons_mc_motherp4    	),"photonsmcmotherp4"    );
    iEvent.put(std::move(vector_photons_mcdr           	),"photonsmcdr"          );
    iEvent.put(std::move(vector_photons_mc3_id         	),"photonsmc3id"         );
    iEvent.put(std::move(vector_photons_mc3_motherid   	),"photonsmc3motherid"   );
    iEvent.put(std::move(vector_photons_mc3idx         	),"photonsmc3idx"        );
    iEvent.put(std::move(vector_photons_mc3_motheridx  	),"photonsmc3motheridx"  );
    iEvent.put(std::move(vector_photons_mc3dr          	),"photonsmc3dr"         );

    iEvent.put(std::move(vector_mus_mc_id          		),"musmcid"          	);
    iEvent.put(std::move(vector_mus_mc_motherid    		),"musmcmotherid"    	);
    if (ntuplePackedGenParticles_) iEvent.put(std::move(vector_mus_mcidx          		),"musmcidx"         	);
    iEvent.put(std::move(vector_mus_mcp4           		),"musmcp4"          	);
    iEvent.put(std::move(vector_mus_mc_motherp4    		),"musmcmotherp4"    	);
    iEvent.put(std::move(vector_mus_mcdr           		),"musmcdr"          	);
    iEvent.put(std::move(vector_mus_mc3_id         		),"musmc3id"         	);
    iEvent.put(std::move(vector_mus_mc3_motherid   		),"musmc3motherid"   	);
    iEvent.put(std::move(vector_mus_mc3idx         		),"musmc3idx"        	);
    iEvent.put(std::move(vector_mus_mc3_motheridx  		),"musmc3motheridx"  	);
    iEvent.put(std::move(vector_mus_mc3dr          		),"musmc3dr"         	);

    iEvent.put(std::move(vector_pfjets_mcdr          	),"pfjetsmcdr"         	);
    iEvent.put(std::move(vector_pfjets_mc_p4         	),"pfjetsmcp4"         	);
    iEvent.put(std::move(vector_pfjets_mc_motherid   	),"pfjetsmcmotherid"   	);
    iEvent.put(std::move(vector_pfjets_mc3dr         	),"pfjetsmc3dr"        	);
    iEvent.put(std::move(vector_pfjets_mc3idx        	),"pfjetsmc3idx"       	);
    iEvent.put(std::move(vector_pfjets_mc3_id        	),"pfjetsmc3id"        	); // id of matched status ==3 particle
}

// ------------ method called once each job just before starting event loop  ------------
void 
CandToGenAssMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
CandToGenAssMaker::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(CandToGenAssMaker);
