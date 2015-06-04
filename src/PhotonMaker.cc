//-*- C++ -*-
//
// Package:    PhotonMaker
// Class:      PhotonMaker
// 
/**\class PhotonMaker PhotonMaker.cc CMS2/PhotonMaker/src/PhotonMaker.cc

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
*/
//
// Original Author:  Puneeth Kalavase
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: PhotonMaker.cc,v 1.22 2012/07/19 22:49:07 dbarge Exp $
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

#include "CMS3/NtupleMaker/interface/PhotonMaker.h"
// #include "CMS3/NtupleMaker/interface/MatchUtilities.h"
// #include "CMS3/NtupleMaker/interface/MCUtilities.h"
// #include "CMS3/NtupleMaker/interface/CaloTowerMaker.h"
// #include "CMS3/NtupleMaker/interface/ElUtilities.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "Math/VectorUtil.h"

// #include "DataFormats/EgammaReco/interface/SuperCluster.h"
// #include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
// #include "DataFormats/EgammaReco/interface/BasicCluster.h"
// #include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
// #include "DataFormats/EgammaReco/interface/ClusterShape.h"
// #include "DataFormats/EgammaReco/interface/ClusterShapeFwd.h"
// #include "DataFormats/EgammaReco/interface/ClusterShape.h"
// #include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"

// #include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
// #include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"

// #include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
// #include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

// #include "Geometry/CaloTopology/interface/CaloTopology.h"
// #include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

// #include "RecoEcal/EgammaCoreTools/interface/ClusterShapeAlgo.h"

// #include "DataFormats/TrackReco/interface/HitPattern.h"
// #include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
// #include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
// #include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
// #include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
// #include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
// #include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

#include "DataFormats/PatCandidates/interface/Photon.h"

#include "CMS3/NtupleMaker/interface/EgammaFiduciality.h"

typedef math::XYZTLorentzVectorF LorentzVector;
typedef math::XYZPoint Point;
using namespace reco;
using namespace edm;
using namespace std;

//
// class decleration
//

//
// constructors and destructor
//
PhotonMaker::PhotonMaker(const edm::ParameterSet& iConfig) {
         
  //
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

  // 
  produces<unsigned int>   ( "evtn" + branchprefix            ).setBranchAlias( "evt_n"      + branchprefix      ); //number of photons in event--NO ET cut // works
  produces<vector<float> >          (branchprefix + "mass"    ).setBranchAlias( aliasprefix_ + "_mass"           );
  produces<vector<LorentzVector> >  (branchprefix + "p4"      ).setBranchAlias( aliasprefix_ + "_p4"             );// works
  produces<vector<int> >   ( branchprefix + "fiduciality"     ).setBranchAlias( aliasprefix_ + "_fiduciality"    ); //missing in scmaker // works

  produces<vector<float> > ( branchprefix + "eSC"            ).setBranchAlias( aliasprefix_ + "_eSC"             );
  produces<vector<float> > ( branchprefix + "etaSC"          ).setBranchAlias( aliasprefix_ + "_etaSC"           );
  produces<vector<float> > ( branchprefix + "phiSC"          ).setBranchAlias( aliasprefix_ + "_phiSC"           );
  produces<vector<float> > ( branchprefix + "eSCRaw"         ).setBranchAlias( aliasprefix_ + "_eSCRaw"          );
  produces<vector<float> > ( branchprefix + "eSCPresh"       ).setBranchAlias( aliasprefix_ + "_eSCPresh"        );

  produces<vector<float> > ( branchprefix + "hOverE"          ).setBranchAlias( aliasprefix_ + "_hOverE"         );
  produces<vector<float> > ( branchprefix + "hOverEtowBC"     ).setBranchAlias( aliasprefix_ + "_hOverEtowBC"    );
  produces<vector<float> > ( branchprefix + "e1x5"            ).setBranchAlias( aliasprefix_ + "_e1x5"           );
  produces<vector<float> > ( branchprefix + "e3x3"            ).setBranchAlias( aliasprefix_ + "_e3x3"           );
  produces<vector<float> > ( branchprefix + "e5x5"            ).setBranchAlias( aliasprefix_ + "_e5x5"           );
  produces<vector<float> > ( branchprefix + "e2x5Max"         ).setBranchAlias( aliasprefix_ + "_e2x5Max"        );
  produces<vector<float> > ( branchprefix + "sigmaEtaEta"     ).setBranchAlias( aliasprefix_ + "_sigmaEtaEta"    );
  produces<vector<float> > ( branchprefix + "sigmaIEtaIEta"   ).setBranchAlias( aliasprefix_ + "_sigmaIEtaIEta"  );

  produces<vector<float> > ( branchprefix + "full5x5hOverE"          ).setBranchAlias( aliasprefix_ + "_full5x5_hOverE"         );
  produces<vector<float> > ( branchprefix + "full5x5e1x5"            ).setBranchAlias( aliasprefix_ + "_full5x5_e1x5"           );
  produces<vector<float> > ( branchprefix + "full3x3e3x3"            ).setBranchAlias( aliasprefix_ + "_full3x3_e3x3"           );
  produces<vector<float> > ( branchprefix + "full5x5e5x5"            ).setBranchAlias( aliasprefix_ + "_full5x5_e5x5"           );
  produces<vector<float> > ( branchprefix + "full5x5e2x5Max"         ).setBranchAlias( aliasprefix_ + "_full5x5_e2x5Max"        );
  produces<vector<float> > ( branchprefix + "full5x5sigmaEtaEta"     ).setBranchAlias( aliasprefix_ + "_full5x5_sigmaEtaEta"    );
  produces<vector<float> > ( branchprefix + "full5x5sigmaIEtaIEta"   ).setBranchAlias( aliasprefix_ + "_full5x5_sigmaIEtaIEta"  );
  produces<vector<float> > ( branchprefix + "full5x5hOverEtowBC"     ).setBranchAlias( aliasprefix_ + "_full5x5_hOverEtowBC"    );
  produces<vector<float> > ( branchprefix + "full5x5r9"              ).setBranchAlias( aliasprefix_ + "_full5x5_r9"             );
  produces<vector<int>   > ( branchprefix + "photonIDloose"          ).setBranchAlias( aliasprefix_ + "_photonID_loose"         );
  produces<vector<int>   > ( branchprefix + "photonIDtight"          ).setBranchAlias( aliasprefix_ + "_photonID_tight"         );	

  produces<vector<float> > ( branchprefix + "tkIsoHollow03"   ).setBranchAlias( aliasprefix_ + "_tkIsoHollow03"  );
  produces<vector<float> > ( branchprefix + "tkIsoSolid03"    ).setBranchAlias( aliasprefix_ + "_tkIsoSolid03"   );
  produces<vector<float> > ( branchprefix + "ntkIsoHollow03"  ).setBranchAlias( aliasprefix_ + "_ntkIsoHollow03" );
  produces<vector<float> > ( branchprefix + "ntkIsoSolid03"   ).setBranchAlias( aliasprefix_ + "_ntkIsoSolid03"  );
  produces<vector<float> > ( branchprefix + "ecalIso03"       ).setBranchAlias( aliasprefix_ + "_ecalIso03"      );
  produces<vector<float> > ( branchprefix + "hcalIso03"       ).setBranchAlias( aliasprefix_ + "_hcalIso03"      );
  produces<vector<float> > ( branchprefix + "tkIsoHollow04"   ).setBranchAlias( aliasprefix_ + "_tkIsoHollow04"  );
  produces<vector<float> > ( branchprefix + "tkIsoSolid04"    ).setBranchAlias( aliasprefix_ + "_tkIsoSolid04"   );
  produces<vector<float> > ( branchprefix + "ntkIsoHollow04"  ).setBranchAlias( aliasprefix_ + "_ntkIsoHollow04" );
  produces<vector<float> > ( branchprefix + "ntkIsoSolid04"   ).setBranchAlias( aliasprefix_ + "_ntkIsoSolid04"  );
  produces<vector<float> > ( branchprefix + "ecalIso04"       ).setBranchAlias( aliasprefix_ + "_ecalIso04"      );
  produces<vector<float> > ( branchprefix + "hcalIso04"       ).setBranchAlias( aliasprefix_ + "_hcalIso04"      );
  produces<vector<float> > ( branchprefix + "chargedHadronIso").setBranchAlias( aliasprefix_ + "_chargedHadronIso");
  produces<vector<float> > ( branchprefix + "neutralHadronIso").setBranchAlias( aliasprefix_ + "_neutralHadronIso");
  produces<vector<float> > ( branchprefix + "photonIso"       ).setBranchAlias( aliasprefix_ + "_photonIso");
  produces<vector<float> > ( branchprefix + "recoChargedHadronIso").setBranchAlias( aliasprefix_ + "_recoChargedHadronIso");
  produces<vector<float> > ( branchprefix + "recoNeutralHadronIso").setBranchAlias( aliasprefix_ + "_recoNeutralHadronIso");
  produces<vector<float> > ( branchprefix + "recoPhotonIso"       ).setBranchAlias( aliasprefix_ + "_recoPhotonIso");
  ///////////////////
  // Added for 53x //
  ///////////////////
  produces<vector<float> > ( branchprefix + "hcalTowerSumEtBcConeDR04"      ).setBranchAlias( aliasprefix_ + "_hcalTowerSumEtBcConeDR04"      );
  produces<vector<float> > ( branchprefix + "hcalDepth1TowerSumEtBcConeDR04").setBranchAlias( aliasprefix_ + "_hcalDepth1TowerSumEtBcConeDR04");
  produces<vector<float> > ( branchprefix + "hcalDepth2TowerSumEtBcConeDR04").setBranchAlias( aliasprefix_ + "_hcalDepth2TowerSumEtBcConeDR04");
  produces<vector<float> > ( branchprefix + "hcalTowerSumEtBcConeDR03"      ).setBranchAlias( aliasprefix_ + "_hcalTowerSumEtBcConeDR03"      );
  produces<vector<float> > ( branchprefix + "hcalDepth1TowerSumEtBcConeDR03").setBranchAlias( aliasprefix_ + "_hcalDepth1TowerSumEtBcConeDR03");
  produces<vector<float> > ( branchprefix + "hcalDepth2TowerSumEtBcConeDR03").setBranchAlias( aliasprefix_ + "_hcalDepth2TowerSumEtBcConeDR03");

// does not work in 7_X_X
  // produces<vector<int> >   ( branchprefix + "scindex"         ).setBranchAlias( aliasprefix_ + "_scindex"        );
  // produces<vector<float> > ( branchprefix + "swissSeed"       ).setBranchAlias( aliasprefix_ + "_swissSeed"      ); //The swiss cross about the seed crystal--missing in sc
  produces<vector<bool> >  ( branchprefix + "haspixelSeed"    ).setBranchAlias( aliasprefix_ + "_haspixelSeed"   ); //for electron matching

  produces<vector<float> > ( branchprefix + "ecalPFClusterIso"       ).setBranchAlias( aliasprefix_ + "_ecalPFClusterIso");
  produces<vector<float> > ( branchprefix + "hcalPFClusterIso"       ).setBranchAlias( aliasprefix_ + "_hcalPFClusterIso");

  produces<vector<vector<int>   >   >       ( branchprefix + "pfcandidx"    ).setBranchAlias( branchprefix + "_PFCand_idx"    );


  //
  photonsInputTag_          = iConfig.getParameter<InputTag>("photonsInputTag");
  minEt_                    = iConfig.getParameter<double>("minEt");
  ecalRecHitsInputTag_EE_   = iConfig.getParameter<edm::InputTag>("ecalRecHitsInputTag_EE");
  ecalRecHitsInputTag_EB_   = iConfig.getParameter<edm::InputTag>("ecalRecHitsInputTag_EB");
  ecalRecHitsInputTag_EE_Token_ = consumes<EcalRecHitCollection>(ecalRecHitsInputTag_EE_);
  ecalRecHitsInputTag_EB_Token_ = consumes<EcalRecHitCollection>(ecalRecHitsInputTag_EB_);


  // cms2scsseeddetidInputTag_ = iConfig.getParameter<edm::InputTag>("cms2scsseeddetidInputTag");
  clusterTools_    = 0;

}

PhotonMaker::~PhotonMaker() {
  if (clusterTools_) delete clusterTools_;
}

void  PhotonMaker::beginJob() {}

void PhotonMaker::endJob() {}


// ------------ method called to produce the data  ------------
void PhotonMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  
  // Define vectors to be filled  
  auto_ptr<unsigned int>   evt_nphotons           ( new unsigned int  );
  auto_ptr<vector<LorentzVector> >  photons_p4              (new vector<LorentzVector>  );
  auto_ptr<vector<float> >          photons_mass            (new vector<float>          );
  auto_ptr<vector<int> >   photons_fiduciality    ( new vector<int>   );

  auto_ptr<vector<float> > photons_etaSC       (new vector<float> );
  auto_ptr<vector<float> > photons_phiSC       (new vector<float> );
  auto_ptr<vector<float> > photons_eSC         (new vector<float> );
  auto_ptr<vector<float> > photons_eSCRaw      (new vector<float> );
  auto_ptr<vector<float> > photons_eSCPresh    (new vector<float> );

  auto_ptr<vector<float> > photons_hOverE         ( new vector<float> );
  auto_ptr<vector<float> > photons_hOverEtowBC    ( new vector<float> );
  auto_ptr<vector<float> > photons_e1x5           ( new vector<float> );
  auto_ptr<vector<float> > photons_e3x3           ( new vector<float> );
  auto_ptr<vector<float> > photons_e5x5           ( new vector<float> );
  auto_ptr<vector<float> > photons_e2x5Max        ( new vector<float> );
  auto_ptr<vector<float> > photons_sigmaEtaEta    ( new vector<float> );
  auto_ptr<vector<float> > photons_sigmaIEtaIEta  ( new vector<float> );

  auto_ptr<vector<float> > photons_full5x5_hOverE         ( new vector<float> );
  auto_ptr<vector<float> > photons_full5x5_e1x5           ( new vector<float> );
  auto_ptr<vector<float> > photons_full3x3_e3x3           ( new vector<float> );
  auto_ptr<vector<float> > photons_full5x5_e5x5           ( new vector<float> );
  auto_ptr<vector<float> > photons_full5x5_e2x5Max        ( new vector<float> );
  auto_ptr<vector<float> > photons_full5x5_sigmaEtaEta    ( new vector<float> );
  auto_ptr<vector<float> > photons_full5x5_sigmaIEtaIEta  ( new vector<float> );
  auto_ptr<vector<float> > photons_full5x5_hOverEtowBC    ( new vector<float> );
  auto_ptr<vector<float> > photons_full5x5_r9             ( new vector<float> );
  auto_ptr<vector<int> >   photons_photonID_loose         ( new vector<int>   );
  auto_ptr<vector<int> >   photons_photonID_tight         ( new vector<int>   );
 
  auto_ptr<vector<float> > photons_tkIsoHollow03  ( new vector<float> );
  auto_ptr<vector<float> > photons_tkIsoSolid03   ( new vector<float> );
  auto_ptr<vector<float> > photons_ntkIsoHollow03 ( new vector<float> );
  auto_ptr<vector<float> > photons_ntkIsoSolid03  ( new vector<float> );
  auto_ptr<vector<float> > photons_ecalIso03      ( new vector<float> );
  auto_ptr<vector<float> > photons_hcalIso03      ( new vector<float> );
  auto_ptr<vector<float> > photons_tkIsoHollow04  ( new vector<float> );
  auto_ptr<vector<float> > photons_tkIsoSolid04   ( new vector<float> );
  auto_ptr<vector<float> > photons_ntkIsoHollow04 ( new vector<float> );
  auto_ptr<vector<float> > photons_ntkIsoSolid04  ( new vector<float> );
  auto_ptr<vector<float> > photons_ecalIso04      ( new vector<float> );
  auto_ptr<vector<float> > photons_hcalIso04      ( new vector<float> );
  auto_ptr<vector<float> > photons_chargedHadronIso( new vector<float> );
  auto_ptr<vector<float> > photons_neutralHadronIso( new vector<float> );
  auto_ptr<vector<float> > photons_photonIso       ( new vector<float> );
  auto_ptr<vector<float> > photons_recoChargedHadronIso( new vector<float> );
  auto_ptr<vector<float> > photons_recoNeutralHadronIso( new vector<float> );
  auto_ptr<vector<float> > photons_recoPhotonIso       ( new vector<float> );

  auto_ptr<vector<float> > photons_hcalTowerSumEtBcConeDR04       ( new vector<float> ); // Added for 53x //
  auto_ptr<vector<float> > photons_hcalDepth1TowerSumEtBcConeDR04 ( new vector<float> ); // Added for 53x //
  auto_ptr<vector<float> > photons_hcalDepth2TowerSumEtBcConeDR04 ( new vector<float> ); // Added for 53x //
  auto_ptr<vector<float> > photons_hcalTowerSumEtBcConeDR03       ( new vector<float> ); // Added for 53x //
  auto_ptr<vector<float> > photons_hcalDepth1TowerSumEtBcConeDR03 ( new vector<float> ); // Added for 53x //
  auto_ptr<vector<float> > photons_hcalDepth2TowerSumEtBcConeDR03 ( new vector<float> ); // Added for 53x //

  auto_ptr<vector<bool> >  photons_haspixelSeed   ( new vector<bool>  );

  auto_ptr<vector<float> > photons_ecalPFClusterIso       ( new vector<float> );
  auto_ptr<vector<float> > photons_hcalPFClusterIso       ( new vector<float> );

  // auto_ptr<vector<int> >   photons_scindex        ( new vector<int>   );   
  // auto_ptr<vector<float> > photons_swissSeed      ( new vector<float> );
  auto_ptr<vector<vector<int> > >           photons_PFCand_idx       (new vector<vector<int> >   );
 
  ///////////////////// 
  // Get the photons //
  /////////////////////
   // Handle<View<reco::Photon> > photons_h;
   Handle<View<pat::Photon> > photons_h;
  iEvent.getByLabel(photonsInputTag_, photons_h);
  // View<reco::Photon> photonColl = *(photons_h.product());

  // //get cms2scsseeddetid 
  // edm::InputTag cms2scsseeddetid_tag(cms2scsseeddetidInputTag_.label(),"scsdetIdSeed");
  // edm::Handle<std::vector<int> > cms2scsseeddetid_h;
  // iEvent.getByLabel(cms2scsseeddetid_tag, cms2scsseeddetid_h);
  // const vector<int> *cms2scsseeddetid = cms2scsseeddetid_h.product();

  // // Get tools to get cluster shape information
  
//   if (clusterTools_) delete clusterTools_;
//   clusterTools_ = new EcalClusterLazyTools(iEvent, iSetup, 
//   					   edm::InputTag("reducedEcalRecHitsEB"), 
//   					   edm::InputTag("reducedEcalRecHitsEE"));

   // get hits--this and topology are for new hit vars--remove if change to InterestingHitMaker
   ////edm::Handle<EcalRecHitCollection> rhcHandleEE;
   ////iEvent.getByLabel(ecalRecHitsInputTag_EE_, rhcHandleEE);
   ////const EcalRecHitCollection *recHitsEE = 0;
   ////
   ////edm::Handle<EcalRecHitCollection> rhcHandleEB;
   ////iEvent.getByLabel(ecalRecHitsInputTag_EB_, rhcHandleEB);
   ////const EcalRecHitCollection *recHitsEB = 0;
   ////
   ////bool haveHits = true;
   ////if(rhcHandleEB.failedToGet() || rhcHandleEE.failedToGet())
   ////  haveHits = false;
   ////
   ////
   ////if(haveHits) {
   ////  recHitsEE = rhcHandleEE.product();
   ////  recHitsEB = rhcHandleEB.product();
   ////}

   if (clusterTools_) delete clusterTools_;
   clusterTools_ = new noZS::EcalClusterLazyTools(iEvent, iSetup, 
   					   ecalRecHitsInputTag_EB_Token_, 
   					   ecalRecHitsInputTag_EE_Token_);


	 
  // // calo topology
  // edm::ESHandle<CaloTopology> pTopology;
  // iSetup.get<CaloTopologyRecord>().get(pTopology);
  // const CaloTopology *topology_ = pTopology.product();

  // fill number of photons variable : NO ET CUT
  *evt_nphotons = photons_h->size();

  //loop over photon collection
  size_t photonsIndex = 0;
  View<pat::Photon>::const_iterator photon;
  for(photon = photons_h->begin(); photon != photons_h->end(); photon++, photonsIndex++) {
	// throw out photons below minEt
	if (photon->et() < minEt_)
	  //instead of photon et, use sc et for alignment purposes (?)
	  continue;

	// Get photon and track objects
	const edm::RefToBase<pat::Photon> photonRef = photons_h->refAt(photonsIndex);
		
	// Lorentz Vectors	
	photons_p4                 ->push_back( LorentzVector( photon->p4() )    );

	photons_mass               ->push_back( photon->mass() );

	// set the mask that describes the egamma fiduciality flags
	// the enum is in interface/EgammaFiduciality.h
	int fiducialityMask = 0;
	if (photon->isEB()) 	fiducialityMask |= 1 << ISEB;
	if (photon->isEBEEGap())fiducialityMask |= 1 << ISEBEEGAP;
	if (photon->isEE())  fiducialityMask |= 1 << ISEE;
	if (photon->isEEGap())  fiducialityMask |= 1 << ISEEGAP;
	photons_fiduciality->push_back( fiducialityMask );

	photons_etaSC              ->push_back( photon->superCluster()->eta()             );
        photons_phiSC              ->push_back( photon->superCluster()->phi()             );
	photons_eSC                ->push_back( photon->superCluster()->energy()          );
        photons_eSCRaw             ->push_back( photon->superCluster()->rawEnergy()       );
        photons_eSCPresh           ->push_back( photon->superCluster()->preshowerEnergy() );
	photons_hOverE             ->push_back( photon->hadronicOverEm()       	 );
	photons_hOverEtowBC        ->push_back( photon->hadTowOverEm()           );
	photons_e1x5    		   ->push_back( photon->e1x5()					 );
	photons_e3x3               ->push_back( photon->e3x3()                   );
	photons_e5x5               ->push_back( photon->e5x5()                   );
	photons_e2x5Max            ->push_back( photon->e2x5()                   );
	photons_sigmaEtaEta        ->push_back( photon->sigmaEtaEta()          	 );
	photons_sigmaIEtaIEta      ->push_back( photon->sigmaIetaIeta()        	 );  		

	//Only there in 706:  photons_full5x5_hOverE             ->push_back( photon->full5x5_hadronicOverEm()       	 );
	//Only there in 706:  photons_full5x5_e1x5    	   ->push_back( photon->full5x5_e1x5()			 );
	//Only there in 706:  photons_full3x3_e3x3               ->push_back( photon->full3x3_e3x3()                   );
	//Only there in 706:  photons_full5x5_e5x5               ->push_back( photon->full5x5_e5x5()                   );
	//Only there in 706:  photons_full5x5_e2x5Max            ->push_back( photon->full5x5_e2x5()                   );
	//Only there in 706:  photons_full5x5_sigmaEtaEta        ->push_back( photon->full5x5_sigmaEtaEta()          	 );
	//Only there in 706:  photons_full5x5_sigmaIEtaIEta      ->push_back( photon->full5x5_sigmaIetaIeta()       	 );
	//Only there in 706:  photons_full5x5_hOverEtowBC        ->push_back( photon->full5x5_hadTowOverEm()        	 );  		
	//Only there in 706:  photons_full5x5_r9                 ->push_back( photon->full5x5_r9()               	 );  
	
	// For 720 need to get the 5x5 shapes by hand. Will be fixed in 730
	// Use the (later killed) code from https://github.com/cms-sw/cmssw/blob/CMSSW_7_0_6/DataFormats/PatCandidates/interface/Photon.h
	std::vector<float> vCov = clusterTools_->localCovariances( *(photon->superCluster()->seed()) );
	const float sieie = (isnan(vCov[0]) ? 0. : sqrt(vCov[0]));
	std::vector<float> vCovGlob = clusterTools_->covariances( *(photon->superCluster()->seed()) );
	const float see = (isnan(vCovGlob[0]) ? 0. : sqrt(vCovGlob[0]));
	float rawSCE = photon->superCluster()->rawEnergy();
	float SCE = photon->superCluster()->energy();
	float e5x5noZS = clusterTools_->e5x5(*(photon->superCluster()->seed()) );
	
	photons_full5x5_hOverE             ->push_back( photon->hadronicOverEm()       	 );
	photons_full5x5_e1x5    	   ->push_back( clusterTools_->e1x5(*(photon->superCluster()->seed()) )    );
	photons_full3x3_e3x3               ->push_back( clusterTools_->e3x3(*(photon->superCluster()->seed()) )    );
	photons_full5x5_e5x5               ->push_back( e5x5noZS                                                  );
	photons_full5x5_e2x5Max            ->push_back( clusterTools_->e2x5Max(*(photon->superCluster()->seed()) ) );
	photons_full5x5_sigmaEtaEta        ->push_back( see          	                                          );
	photons_full5x5_sigmaIEtaIEta      ->push_back( sieie       	                                          );
	photons_full5x5_hOverEtowBC        ->push_back( (photon->hadTowDepth1OverEm() + photon->hadTowDepth2OverEm()) * (SCE / e5x5noZS));  		
	photons_full5x5_r9                 ->push_back( clusterTools_->e3x3(*(photon->superCluster()->seed()) ) / rawSCE );  
	//	cout<<"photon->hadTowOverEm() "<<photon->hadTowOverEm()<<", photons_full5x5_hOverEtowBC  "<<(photon->hadTowDepth1OverEm() + photon->hadTowDepth2OverEm()) * (SCE / e5x5noZS)<<endl;
	//	cout<<" components were: photon->hadTowDepth1OverEm() "<<photon->hadTowDepth1OverEm()<<", photon->hadTowDepth2OverEm() "<<photon->hadTowDepth2OverEm()<<", SCE "<<SCE<<", e5x5noZS "<<e5x5noZS<<endl;
	photons_photonID_loose             ->push_back( photon->photonID("PhotonCutBasedIDLoose"));  		
	photons_photonID_tight             ->push_back( photon->photonID("PhotonCutBasedIDTight"));  		
	
	// Isolation  (all 0.3 cone size)
	photons_ecalIso03          ->push_back(	photon->ecalRecHitSumEtConeDR03() );
	photons_hcalIso03          ->push_back(	photon->hcalTowerSumEtConeDR03()  );	
	photons_tkIsoHollow03      ->push_back(	photon->trkSumPtHollowConeDR03()  );
	photons_tkIsoSolid03       ->push_back(	photon->trkSumPtSolidConeDR03()	  );
	photons_ntkIsoHollow03     ->push_back(	photon->nTrkHollowConeDR03()	  );
	photons_ntkIsoSolid03      ->push_back(	photon->nTrkSolidConeDR03()		  );

	// Isolation  (all 0.4 cone size)
	photons_ecalIso04          ->push_back(	photon->ecalRecHitSumEtConeDR04() );
	photons_hcalIso04          ->push_back(	photon->hcalTowerSumEtConeDR04()  );	
       	photons_tkIsoHollow04      ->push_back(	photon->trkSumPtHollowConeDR04()  );
	photons_tkIsoSolid04       ->push_back(	photon->trkSumPtSolidConeDR04()	  );
	photons_ntkIsoHollow04     ->push_back(	photon->nTrkHollowConeDR04()	  );
	photons_ntkIsoSolid04      ->push_back(	photon->nTrkSolidConeDR04()		  );

	// PFIso
	photons_chargedHadronIso   ->push_back(	photon->chargedHadronIso()        );	
	photons_neutralHadronIso   ->push_back(	photon->neutralHadronIso()        );	
	photons_photonIso          ->push_back(	photon->photonIso()               );

	// Testing PFIso of reco::photon
	photons_recoChargedHadronIso   ->push_back(photon->reco::Photon::chargedHadronIso()  );	
	photons_recoNeutralHadronIso   ->push_back(photon->reco::Photon::neutralHadronIso()  );	
	photons_recoPhotonIso          ->push_back(photon->reco::Photon::photonIso()         );


	// Added for 53x //
	photons_hcalTowerSumEtBcConeDR04       ->push_back( photon->hcalTowerSumEtBcConeDR04()       );
	photons_hcalDepth1TowerSumEtBcConeDR04 ->push_back( photon->hcalDepth1TowerSumEtBcConeDR04() );
	photons_hcalDepth2TowerSumEtBcConeDR04 ->push_back( photon->hcalDepth2TowerSumEtBcConeDR04() );
	photons_hcalTowerSumEtBcConeDR03       ->push_back( photon->hcalTowerSumEtBcConeDR03()       );
	photons_hcalDepth1TowerSumEtBcConeDR03 ->push_back( photon->hcalDepth1TowerSumEtBcConeDR03() );
	photons_hcalDepth2TowerSumEtBcConeDR03 ->push_back( photon->hcalDepth2TowerSumEtBcConeDR03() );

	// //get cms2scsseeddetid
	// bool foundseed = false;
	// for( unsigned int i=0; i<cms2scsseeddetid->size(); i++ ) {
	//   if(cms2scsseeddetid->at(i) == -9999)
	// 	continue;
	//   if(!(photon->superCluster()->seed().isAvailable()))
	// 	continue;
	//   if( uint32_t(cms2scsseeddetid->at(i)) == photon->superCluster()->seed()->seed() ) {
	// 	foundseed = true;
	// 	photons_scindex->push_back( i );
	// 	break;
	//   }
	// }

	//  if( !foundseed ) {
	//    photons_scindex->push_back( -1 );		
	//  }

	// //new from gammajetanalyzer.cc
	// // get geometry
	// edm::ESHandle<CaloGeometry> geoHandle;
	// iSetup.get<CaloGeometryRecord>().get(geoHandle);


	// reco::BasicCluster dummyCluster;
	// EcalClusterTools clusterTools;
	// if(haveHits && photon->superCluster()->seed().isAvailable()) {
	//   DetId seedId = photon->superCluster()->seed()->seed();
	//   if (seedId.det() == DetId::Ecal && seedId.subdetId() == EcalEndcap) {
	 
	// 	EcalRecHitCollection::const_iterator seedHit = recHitsEE->find(seedId);
	// 	if (seedHit != recHitsEE->end()) {
	// 	  float emSwiss = 0.;
	// 	  emSwiss += clusterTools.matrixEnergy(dummyCluster, recHitsEE, topology_, seedId, 0, 0, -1, 1);
	// 	  emSwiss += clusterTools.matrixEnergy(dummyCluster, recHitsEE, topology_, seedId, -1, 1, 0, 0); 
	// 	  emSwiss -= clusterTools.matrixEnergy(dummyCluster, recHitsEE, topology_, seedId, 0, 0, 0, 0); //center of cross was included twice above 
	// 	  photons_swissSeed->push_back( emSwiss );
	// 	} else {
	// 	  photons_swissSeed->push_back( -9999.99 );
	// 	}
	//   } else if (seedId.det() == DetId::Ecal && seedId.subdetId() == EcalBarrel) {
	//     EcalRecHitCollection::const_iterator seedHit = recHitsEB->find(seedId);
	//     if (seedHit != recHitsEB->end()) {
	// 	  float emSwiss = 0.;
	// 	  emSwiss += clusterTools.matrixEnergy(dummyCluster, recHitsEB, topology_, seedId, 0, 0, -1, 1);
	// 	  emSwiss += clusterTools.matrixEnergy(dummyCluster, recHitsEB, topology_, seedId, -1, 1, 0, 0); 
	// 	  emSwiss -= clusterTools.matrixEnergy(dummyCluster, recHitsEB, topology_, seedId, 0, 0, 0, 0); //center of cross was included twice above 
	// 	  photons_swissSeed->push_back( emSwiss );
	//     }
	//     else {
	// 	  photons_swissSeed->push_back( -9999.99 );
	//     }		
	//   }
	// } else 
	//   photons_swissSeed->push_back( -9999.99 );

	
		
	// //pixel seeds
	photons_haspixelSeed       ->push_back( photon->hasPixelSeed()             );

	photons_ecalPFClusterIso       ->push_back( photon->ecalPFClusterIso()             );
	photons_hcalPFClusterIso       ->push_back( photon->hcalPFClusterIso()             );


	// Loop over PF candidates and find those associated by the map to the gedGsfElectron1
	vector<int> v_PFCand_idx;
	for( const edm::Ref<pat::PackedCandidateCollection> & ref : photon->associatedPackedPFCandidates() )
	  v_PFCand_idx.push_back(ref.key());
	photons_PFCand_idx->push_back(v_PFCand_idx);


  }
 
  // Put the results into the event
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

  //
  iEvent.put( evt_nphotons           , "evtn"+branchprefix            );
  iEvent.put( photons_p4             , branchprefix+"p4"              );
  iEvent.put( photons_mass           , branchprefix+"mass"            );
  iEvent.put( photons_fiduciality    , branchprefix+"fiduciality"     );

  iEvent.put(photons_etaSC       , "photonsetaSC"       );
  iEvent.put(photons_phiSC       , "photonsphiSC"       );
  iEvent.put(photons_eSC         , "photonseSC"         );
  iEvent.put(photons_eSCRaw      , "photonseSCRaw"      );
  iEvent.put(photons_eSCPresh    , "photonseSCPresh"    );

  iEvent.put( photons_hOverE         , branchprefix+"hOverE"          );
  iEvent.put( photons_hOverEtowBC    , branchprefix+"hOverEtowBC"     );
  iEvent.put( photons_e1x5           , branchprefix+"e1x5"            );
  iEvent.put( photons_e3x3           , branchprefix+"e3x3"            );
  iEvent.put( photons_e5x5           , branchprefix+"e5x5"            );
  iEvent.put( photons_e2x5Max        , branchprefix+"e2x5Max"         );
  iEvent.put( photons_sigmaEtaEta    , branchprefix+"sigmaEtaEta"     );
  iEvent.put( photons_sigmaIEtaIEta  , branchprefix+"sigmaIEtaIEta"   );

  iEvent.put( photons_full5x5_hOverE         , branchprefix+"full5x5hOverE"          );
  iEvent.put( photons_full5x5_e1x5           , branchprefix+"full5x5e1x5"            );
  iEvent.put( photons_full3x3_e3x3           , branchprefix+"full3x3e3x3"            );
  iEvent.put( photons_full5x5_e5x5           , branchprefix+"full5x5e5x5"            );
  iEvent.put( photons_full5x5_e2x5Max        , branchprefix+"full5x5e2x5Max"         );
  iEvent.put( photons_full5x5_sigmaEtaEta    , branchprefix+"full5x5sigmaEtaEta"     );
  iEvent.put( photons_full5x5_sigmaIEtaIEta  , branchprefix+"full5x5sigmaIEtaIEta"   );
  iEvent.put( photons_full5x5_hOverEtowBC    , branchprefix+"full5x5hOverEtowBC"    );
  iEvent.put( photons_full5x5_r9             , branchprefix+"full5x5r9"             );
  iEvent.put( photons_photonID_loose         , branchprefix+"photonIDloose"         );
  iEvent.put( photons_photonID_tight         , branchprefix+"photonIDtight"         );		

  iEvent.put( photons_tkIsoHollow03  , branchprefix+"tkIsoHollow03"   );
  iEvent.put( photons_tkIsoSolid03   , branchprefix+"tkIsoSolid03"    );      
  iEvent.put( photons_ntkIsoHollow03 , branchprefix+"ntkIsoHollow03"  );
  iEvent.put( photons_ntkIsoSolid03  , branchprefix+"ntkIsoSolid03"   );      
  iEvent.put( photons_ecalIso03      , branchprefix+"ecalIso03"       );
  iEvent.put( photons_hcalIso03      , branchprefix+"hcalIso03"       );
  iEvent.put( photons_tkIsoHollow04  , branchprefix+"tkIsoHollow04"   );
  iEvent.put( photons_tkIsoSolid04   , branchprefix+"tkIsoSolid04"    );      
  iEvent.put( photons_ntkIsoHollow04 , branchprefix+"ntkIsoHollow04"  );
  iEvent.put( photons_ntkIsoSolid04  , branchprefix+"ntkIsoSolid04"   );      
  iEvent.put( photons_ecalIso04      , branchprefix+"ecalIso04"       );
  iEvent.put( photons_hcalIso04      , branchprefix+"hcalIso04"       );  
  iEvent.put( photons_chargedHadronIso, branchprefix+"chargedHadronIso");  
  iEvent.put( photons_neutralHadronIso, branchprefix+"neutralHadronIso");  
  iEvent.put( photons_photonIso       , branchprefix+"photonIso"       );  
  iEvent.put( photons_recoChargedHadronIso, branchprefix+"recoChargedHadronIso");  
  iEvent.put( photons_recoNeutralHadronIso, branchprefix+"recoNeutralHadronIso");  
  iEvent.put( photons_recoPhotonIso       , branchprefix+"recoPhotonIso"       );  

  iEvent.put( photons_hcalTowerSumEtBcConeDR04       , branchprefix + "hcalTowerSumEtBcConeDR04"      );// Added for 53x //
  iEvent.put( photons_hcalDepth1TowerSumEtBcConeDR04 , branchprefix + "hcalDepth1TowerSumEtBcConeDR04");// Added for 53x //
  iEvent.put( photons_hcalDepth2TowerSumEtBcConeDR04 , branchprefix + "hcalDepth2TowerSumEtBcConeDR04");// Added for 53x //
  iEvent.put( photons_hcalTowerSumEtBcConeDR03       , branchprefix + "hcalTowerSumEtBcConeDR03"      );// Added for 53x //
  iEvent.put( photons_hcalDepth1TowerSumEtBcConeDR03 , branchprefix + "hcalDepth1TowerSumEtBcConeDR03");// Added for 53x //
  iEvent.put( photons_hcalDepth2TowerSumEtBcConeDR03 , branchprefix + "hcalDepth2TowerSumEtBcConeDR03");// Added for 53x //
  iEvent.put( photons_haspixelSeed   , branchprefix+"haspixelSeed"    );

  iEvent.put( photons_ecalPFClusterIso  , branchprefix+"ecalPFClusterIso"    );
  iEvent.put( photons_hcalPFClusterIso  , branchprefix+"hcalPFClusterIso"    );

  // iEvent.put( photons_scindex        , branchprefix+"scindex"         );
  // iEvent.put( photons_swissSeed      , branchprefix+"swissSeed"       );
  iEvent.put( photons_PFCand_idx    , branchprefix+"pfcandidx"    );
 
}

//define this as a plug-in
DEFINE_FWK_MODULE(PhotonMaker);
