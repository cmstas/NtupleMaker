//-*- C++ -*-
//
// Package:    PhotonExtraMaker
// Class:      PhotonExtraMaker
// 
/**\class PhotonExtraMaker PhotonExtraMaker.cc CMS2/PhotonExtraMaker/src/PhotonExtraMaker.cc

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
*/
//
// Original Author:  Puneeth Kalavase
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: PhotonExtraMaker.cc,v 1.22 2012/07/19 22:49:07 dbarge Exp $
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

#include "CMS3/NtupleMaker/interface/PhotonExtraMaker.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "RecoEgamma/EgammaTools/interface/EcalClusterLocal.h"
#include "RecoEcal/EgammaCoreTools/interface/Mustache.h"
#include "TVector2.h"
#include "Math/VectorUtil.h"

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
PhotonExtraMaker::PhotonExtraMaker(const edm::ParameterSet& iConfig) {
         
    //
    aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix");
    std::string branchprefix = aliasprefix_;
    if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

    // 
    produces<vector<float> >          (branchprefix + "mass"    ).setBranchAlias( aliasprefix_ + "_mass"           );
    produces<vector<int> >   ( branchprefix + "fiduciality"     ).setBranchAlias( aliasprefix_ + "_fiduciality"    ); //missing in scmaker // works

    produces<vector<float> > ( branchprefix + "eSC"            ).setBranchAlias( aliasprefix_ + "_eSC"             );
    produces<vector<float> > ( branchprefix + "etaSC"          ).setBranchAlias( aliasprefix_ + "_etaSC"           );
    produces<vector<float> > ( branchprefix + "phiSC"          ).setBranchAlias( aliasprefix_ + "_phiSC"           );
    produces<vector<float> > ( branchprefix + "eSCRaw"         ).setBranchAlias( aliasprefix_ + "_eSCRaw"          );
    produces<vector<float> > ( branchprefix + "eSCPresh"       ).setBranchAlias( aliasprefix_ + "_eSCPresh"        );

    produces<vector<float> > ( branchprefix + "e1x5"            ).setBranchAlias( aliasprefix_ + "_e1x5"           );
    produces<vector<float> > ( branchprefix + "e3x3"            ).setBranchAlias( aliasprefix_ + "_e3x3"           );
    produces<vector<float> > ( branchprefix + "e5x5"            ).setBranchAlias( aliasprefix_ + "_e5x5"           );
    produces<vector<float> > ( branchprefix + "e2x5Max"         ).setBranchAlias( aliasprefix_ + "_e2x5Max"        );
    produces<vector<float> > ( branchprefix + "sigmaEtaEta"     ).setBranchAlias( aliasprefix_ + "_sigmaEtaEta"    );

    produces<vector<float> > ( branchprefix + "full5x5e1x5"            ).setBranchAlias( aliasprefix_ + "_full5x5_e1x5"           );
    produces<vector<float> > ( branchprefix + "full3x3e3x3"            ).setBranchAlias( aliasprefix_ + "_full3x3_e3x3"           );
    produces<vector<float> > ( branchprefix + "full5x5e5x5"            ).setBranchAlias( aliasprefix_ + "_full5x5_e5x5"           );
    produces<vector<float> > ( branchprefix + "full5x5e2x5Max"         ).setBranchAlias( aliasprefix_ + "_full5x5_e2x5Max"        );
    produces<vector<float> > ( branchprefix + "full5x5sigmaEtaEta"     ).setBranchAlias( aliasprefix_ + "_full5x5_sigmaEtaEta"    );
    produces<vector<float> > ( branchprefix + "full5x5r9"              ).setBranchAlias( aliasprefix_ + "_full5x5_r9"             );
    produces<vector<int>   > ( branchprefix + "photonIDloose"          ).setBranchAlias( aliasprefix_ + "_photonID_loose"         );
    produces<vector<int>   > ( branchprefix + "photonIDtight"          ).setBranchAlias( aliasprefix_ + "_photonID_tight"         );	

    produces<vector<float> > ( branchprefix + "tkIsoSolid03"    ).setBranchAlias( aliasprefix_ + "_tkIsoSolid03"   );
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

    ///////////////////
    // Added for 53x //
    ///////////////////
    produces<vector<float> > ( branchprefix + "hcalTowerSumEtBcConeDR04"      ).setBranchAlias( aliasprefix_ + "_hcalTowerSumEtBcConeDR04"      );
    produces<vector<float> > ( branchprefix + "hcalDepth1TowerSumEtBcConeDR04").setBranchAlias( aliasprefix_ + "_hcalDepth1TowerSumEtBcConeDR04");
    produces<vector<float> > ( branchprefix + "hcalDepth2TowerSumEtBcConeDR04").setBranchAlias( aliasprefix_ + "_hcalDepth2TowerSumEtBcConeDR04");
    produces<vector<float> > ( branchprefix + "hcalTowerSumEtBcConeDR03"      ).setBranchAlias( aliasprefix_ + "_hcalTowerSumEtBcConeDR03"      );
    produces<vector<float> > ( branchprefix + "hcalDepth1TowerSumEtBcConeDR03").setBranchAlias( aliasprefix_ + "_hcalDepth1TowerSumEtBcConeDR03");
    produces<vector<float> > ( branchprefix + "hcalDepth2TowerSumEtBcConeDR03").setBranchAlias( aliasprefix_ + "_hcalDepth2TowerSumEtBcConeDR03");



    //
    photonsToken = consumes<edm::View<pat::Photon> >(iConfig.getParameter<edm::InputTag>("photonsInputTag"));
    minEt_                    = iConfig.getParameter<double>("minEt");
  
    ///////////////////////////
    // For 7_X recalibration //
    ///////////////////////////

    ebReducedRecHitCollectionTag = iConfig.getParameter<edm::InputTag> ("ebReducedRecHitCollectionTag" );
    eeReducedRecHitCollectionTag = iConfig.getParameter<edm::InputTag> ("eeReducedRecHitCollectionTag" );
    esReducedRecHitCollectionTag = iConfig.getParameter<edm::InputTag> ("esReducedRecHitCollectionTag" );
  
    ebReducedRecHitCollection = mayConsume<EcalRecHitCollection>(ebReducedRecHitCollectionTag);
    eeReducedRecHitCollection = mayConsume<EcalRecHitCollection>(eeReducedRecHitCollectionTag);
    esReducedRecHitCollection = mayConsume<EcalRecHitCollection>(esReducedRecHitCollectionTag);

    ////////////////////////////
    // For calibration in 7_X //
    ////////////////////////////
    clusterTools_    = 0;

    produces<vector<int> > (branchprefix + "NECALClusters").setBranchAlias(aliasprefix_ + "_N_ECALClusters");
    produces<vector<int> > (branchprefix + "NPSClusters").setBranchAlias(aliasprefix_ + "_N_PSClusters");
    produces<vector<int> > (branchprefix + "isEB").setBranchAlias(aliasprefix_ + "_isEB");

    produces<vector<float> > (branchprefix + "scPhiWidth").setBranchAlias(aliasprefix_ + "scPhiWidth");
    produces<vector<float> > (branchprefix + "scEtaWidth").setBranchAlias(aliasprefix_ + "scEtaWidth");
    produces<vector<float> > (branchprefix + "scSeedRawEnergy").setBranchAlias(aliasprefix_ + "scSeedRawEnergy");
    produces<vector<float> > (branchprefix + "scSeedCalibratedEnergy").setBranchAlias(aliasprefix_ + "scSeedCalibratedEnergy");
    produces<vector<float> > (branchprefix + "scSeedSigmaIphiIphi").setBranchAlias(aliasprefix_ + "scSeedSigmaIphiIphi");

    produces<vector<float> > (branchprefix + "scPreshowerEnergyPlane1").setBranchAlias(aliasprefix_ +  "_scPreshowerEnergyPlane1");
    produces<vector<float> > (branchprefix + "scPreshowerEnergyPlane2").setBranchAlias(aliasprefix_ + "_scPreshowerEnergyPlane2");
    produces<vector<float> > (branchprefix + "scIsEB").setBranchAlias(aliasprefix_ + "_scIsEB");
    produces<vector<float> > (branchprefix + "scR").setBranchAlias(aliasprefix_ + "_scR");
    produces<vector<float> > (branchprefix + "scSeedEta").setBranchAlias(aliasprefix_ + "_scSeedEta");
    produces<vector<float> > (branchprefix + "scSeedPhi").setBranchAlias(aliasprefix_ + "_scSeedPhi");
    produces<vector<float> > (branchprefix + "scSeedSize").setBranchAlias(aliasprefix_ + "_scSeedSize");
    produces<vector<float> > (branchprefix + "scSeedE3x3").setBranchAlias(aliasprefix_ + "_scSeedE3x3");
    produces<vector<float> > (branchprefix + "scSeedEmax").setBranchAlias(aliasprefix_ + "_scSeedEmax");
    produces<vector<float> > (branchprefix + "scSeedE2nd").setBranchAlias(aliasprefix_ + "_scSeedE2nd");
    produces<vector<float> > (branchprefix + "scSeedELeft").setBranchAlias(aliasprefix_ + "_scSeedELeft");
    produces<vector<float> > (branchprefix + "scSeedERight").setBranchAlias(aliasprefix_ + "_scSeedERight");
    produces<vector<float> > (branchprefix + "scSeedETop").setBranchAlias(aliasprefix_ + "_scSeedETop");
    produces<vector<float> > (branchprefix + "scSeedEBottom").setBranchAlias(aliasprefix_ + "_scSeedEBottom");
    produces<vector<float> > (branchprefix + "scSeedE2x5Left").setBranchAlias(aliasprefix_ + "_scSeedE2x5Left");
    produces<vector<float> > (branchprefix + "scSeedE2x5Right").setBranchAlias(aliasprefix_ + "_scSeedE2x5Right");
    produces<vector<float> > (branchprefix + "scSeedE2x5Top").setBranchAlias(aliasprefix_ + "_scSeedE2x5Top");
    produces<vector<float> > (branchprefix + "scSeedE2x5Bottom").setBranchAlias(aliasprefix_ + "_scSeedE2x5Bottom");
    produces<vector<float> > (branchprefix + "scSeedLeftRightAsym").setBranchAlias(aliasprefix_ + "_scSeedLeftRightAsym");
    produces<vector<float> > (branchprefix + "scSeedTopBottomAsym").setBranchAlias(aliasprefix_ + "_scSeedTopBottomAsym");
    produces<vector<float> > (branchprefix + "scSeed2x5LeftRightAsym").setBranchAlias(aliasprefix_ + "_scSeed2x5LeftRightAsym");
    produces<vector<float> > (branchprefix + "scSeed2x5TopBottomAsym").setBranchAlias(aliasprefix_ + "_scSeed2x5TopBottomAsym");
    produces<vector<float> > (branchprefix + "scSeedSigmaIetaIphi").setBranchAlias(aliasprefix_ + "_scSeedSigmaIetaIphi");
    produces<vector<float> > (branchprefix + "scSeedCryEta").setBranchAlias(aliasprefix_ + "_scSeedCryEta");
    produces<vector<float> > (branchprefix + "scSeedCryPhi").setBranchAlias(aliasprefix_ + "_scSeedCryPhi");
    produces<vector<float> > (branchprefix + "scSeedCryIeta").setBranchAlias(aliasprefix_ + "_scSeedCryIeta");
    produces<vector<float> > (branchprefix + "scSeedCryIphi").setBranchAlias(aliasprefix_ + "_scSeedCryIphi");
    produces<vector<float> > (branchprefix + "scSeedCryX").setBranchAlias(aliasprefix_ + "_scSeedCryX");
    produces<vector<float> > (branchprefix + "scSeedCryY").setBranchAlias(aliasprefix_ + "_scSeedCryY");
    produces<vector<float> > (branchprefix + "scSeedCryIx").setBranchAlias(aliasprefix_ + "_scSeedCryIx");
    produces<vector<float> > (branchprefix + "scSeedCryIy").setBranchAlias(aliasprefix_ + "_scSeedCryIy");
    produces<vector<float> > (branchprefix + "clusterMaxDR").setBranchAlias(aliasprefix_ + "_clusterMaxDR");
    produces<vector<float> > (branchprefix + "clusterMaxDRDPhi").setBranchAlias(aliasprefix_ + "_clusterMaxDRDPhi");
    produces<vector<float> > (branchprefix + "clusterMaxDRDEta").setBranchAlias(aliasprefix_ + "_clusterMaxDRDEta");
    produces<vector<float> > (branchprefix + "clustersMeanDRToSeed").setBranchAlias(aliasprefix_ + "_clustersMeanDRToSeed");
    produces<vector<float> > (branchprefix + "clustersMeanDEtaToSeed").setBranchAlias(aliasprefix_ + "_clustersMeanDEtaToSeed");
    produces<vector<float> > (branchprefix + "clustersMeanDPhiToSeed").setBranchAlias(aliasprefix_ + "_clustersMeanDPhiToSeed");
    produces<vector<float> > (branchprefix + "clusterMaxDRRawEnergy").setBranchAlias(aliasprefix_ + "_clusterMaxDRRawEnergy");
    produces<vector<float> > (branchprefix + "clustersMeanRawEnergy").setBranchAlias(aliasprefix_ + "_clustersMeanRawEnergy");
    produces<vector<float> > (branchprefix + "clustersRMSRawEnergy").setBranchAlias(aliasprefix_ + "_clustersRMSRawEnergy");

    produces<vector<vector<int> > > (branchprefix + "clusterInMustache").setBranchAlias(aliasprefix_ + "_clusterInMustache");
    produces<vector<vector<int> > > (branchprefix + "clusterInDynDPhi").setBranchAlias(aliasprefix_ + "_clusterInDynDPhi");

    produces<vector<vector<float> > > (branchprefix + "clusterRawEnergy").setBranchAlias(aliasprefix_ + "_clusterRawEnergy");
    produces<vector<vector<float> > > (branchprefix + "clusterCalibEnergy").setBranchAlias(aliasprefix_ + "_clusterCalibEnergy");
    produces<vector<vector<float> > > (branchprefix + "clusterEta").setBranchAlias(aliasprefix_ + "_clusterEta");
    produces<vector<vector<float> > > (branchprefix + "clusterPhi").setBranchAlias(aliasprefix_ + "_clusterPhi");
    produces<vector<vector<float> > > (branchprefix + "clusterDPhiToSeed").setBranchAlias(aliasprefix_ + "_clusterDPhiToSeed");
    produces<vector<vector<float> > > (branchprefix + "clusterDEtaToSeed").setBranchAlias(aliasprefix_ + "_clusterDEtaToSeed");
    produces<vector<vector<float> > > (branchprefix + "clusterDPhiToCentroid").setBranchAlias(aliasprefix_ + "_clusterDPhiToCentroid");
    produces<vector<vector<float> > > (branchprefix + "clusterDEtaToCentroid").setBranchAlias(aliasprefix_ + "_clusterDEtaToCentroid");
    produces<vector<vector<float> > > (branchprefix + "psClusterRawEnergy").setBranchAlias(aliasprefix_ + "_psClusterRawEnergy");
    produces<vector<vector<float> > > (branchprefix + "psClusterEta").setBranchAlias(aliasprefix_ + "_psClusterEta");
    produces<vector<vector<float> > > (branchprefix + "psClusterPhi").setBranchAlias(aliasprefix_ + "_psClusterPhi");

}

PhotonExtraMaker::~PhotonExtraMaker() {
    if (clusterTools_) delete clusterTools_;
}

void  PhotonExtraMaker::beginJob() {}

void PhotonExtraMaker::endJob() {}


// ------------ method called to produce the data  ------------
void PhotonExtraMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  
    // Define vectors to be filled  
    unique_ptr<vector<float> >          photons_mass            (new vector<float>          );
    unique_ptr<vector<int> >   photons_fiduciality    ( new vector<int>   );

    unique_ptr<vector<float> > photons_etaSC       (new vector<float> );
    unique_ptr<vector<float> > photons_phiSC       (new vector<float> );
    unique_ptr<vector<float> > photons_eSC         (new vector<float> );
    unique_ptr<vector<float> > photons_eSCRaw      (new vector<float> );
    unique_ptr<vector<float> > photons_eSCPresh    (new vector<float> );

    unique_ptr<vector<float> > photons_e1x5           ( new vector<float> );
    unique_ptr<vector<float> > photons_e3x3           ( new vector<float> );
    unique_ptr<vector<float> > photons_e5x5           ( new vector<float> );
    unique_ptr<vector<float> > photons_e2x5Max        ( new vector<float> );
    unique_ptr<vector<float> > photons_sigmaEtaEta    ( new vector<float> );

    unique_ptr<vector<float> > photons_full5x5_e1x5           ( new vector<float> );
    unique_ptr<vector<float> > photons_full3x3_e3x3           ( new vector<float> );
    unique_ptr<vector<float> > photons_full5x5_e5x5           ( new vector<float> );
    unique_ptr<vector<float> > photons_full5x5_e2x5Max        ( new vector<float> );
    unique_ptr<vector<float> > photons_full5x5_sigmaEtaEta    ( new vector<float> );
 
    unique_ptr<vector<float> > photons_tkIsoSolid03   ( new vector<float> );
    unique_ptr<vector<float> > photons_ntkIsoSolid03  ( new vector<float> );
    unique_ptr<vector<float> > photons_ecalIso03      ( new vector<float> );
    unique_ptr<vector<float> > photons_hcalIso03      ( new vector<float> );
    unique_ptr<vector<float> > photons_tkIsoHollow04  ( new vector<float> );
    unique_ptr<vector<float> > photons_tkIsoSolid04   ( new vector<float> );
    unique_ptr<vector<float> > photons_ntkIsoHollow04 ( new vector<float> );
    unique_ptr<vector<float> > photons_ntkIsoSolid04  ( new vector<float> );
    unique_ptr<vector<float> > photons_ecalIso04      ( new vector<float> );
    unique_ptr<vector<float> > photons_hcalIso04      ( new vector<float> );
    unique_ptr<vector<float> > photons_chargedHadronIso( new vector<float> );
    unique_ptr<vector<float> > photons_neutralHadronIso( new vector<float> );
    unique_ptr<vector<float> > photons_photonIso       ( new vector<float> );

    unique_ptr<vector<float> > photons_hcalTowerSumEtBcConeDR04       ( new vector<float> ); // Added for 53x //
    unique_ptr<vector<float> > photons_hcalDepth1TowerSumEtBcConeDR04 ( new vector<float> ); // Added for 53x //
    unique_ptr<vector<float> > photons_hcalDepth2TowerSumEtBcConeDR04 ( new vector<float> ); // Added for 53x //
    unique_ptr<vector<float> > photons_hcalTowerSumEtBcConeDR03       ( new vector<float> ); // Added for 53x //
    unique_ptr<vector<float> > photons_hcalDepth1TowerSumEtBcConeDR03 ( new vector<float> ); // Added for 53x //
    unique_ptr<vector<float> > photons_hcalDepth2TowerSumEtBcConeDR03 ( new vector<float> ); // Added for 53x //


    // unique_ptr<vector<int> >   photons_scindex        ( new vector<int>   );   
    // unique_ptr<vector<float> > photons_swissSeed      ( new vector<float> );

    ///////////////////////////////
    // Added for 7_X calibration //
    ///////////////////////////////
    unique_ptr<vector<int> > photons_N_ECALClusters            (new vector<int>);
    unique_ptr<vector<int> > photons_N_PSClusters              (new vector<int>);
    unique_ptr<vector<int> > photons_isEB                      (new vector<int>);
  
    unique_ptr<vector<float> > photons_scPhiWidth              (new vector<float>);
    unique_ptr<vector<float> > photons_scEtaWidth              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedRawEnergy         (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCalibratedEnergy  (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedSigmaIphiIphi     (new vector<float>);

    unique_ptr<vector<float> > photons_scPreshowerEnergyPlane1 (new vector<float>);
    unique_ptr<vector<float> > photons_scPreshowerEnergyPlane2 (new vector<float>);
    unique_ptr<vector<float> > photons_scIsEB                  (new vector<float>);
    unique_ptr<vector<float> > photons_scR                     (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedEta               (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedPhi               (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedSize              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedE3x3              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedEmax              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedE2nd              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedELeft             (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedERight            (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedETop              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedEBottom           (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedE2x5Left          (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedE2x5Right         (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedE2x5Top           (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedE2x5Bottom        (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedLeftRightAsym     (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedTopBottomAsym     (new vector<float>);
    unique_ptr<vector<float> > photons_scSeed2x5LeftRightAsym  (new vector<float>);
    unique_ptr<vector<float> > photons_scSeed2x5TopBottomAsym  (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedSigmaIetaIphi     (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryEta            (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryPhi            (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryIeta           (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryIphi           (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryX              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryY              (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryIx             (new vector<float>);
    unique_ptr<vector<float> > photons_scSeedCryIy             (new vector<float>);
    unique_ptr<vector<float> > photons_clusterMaxDR            (new vector<float>);
    unique_ptr<vector<float> > photons_clusterMaxDRDPhi        (new vector<float>);
    unique_ptr<vector<float> > photons_clusterMaxDRDEta        (new vector<float>);
    unique_ptr<vector<float> > photons_clustersMeanDRToSeed    (new vector<float>);
    unique_ptr<vector<float> > photons_clustersMeanDEtaToSeed  (new vector<float>);
    unique_ptr<vector<float> > photons_clustersMeanDPhiToSeed  (new vector<float>);
    unique_ptr<vector<float> > photons_clusterMaxDRRawEnergy   (new vector<float>);
    unique_ptr<vector<float> > photons_clustersMeanRawEnergy   (new vector<float>);
    unique_ptr<vector<float> > photons_clustersRMSRawEnergy    (new vector<float>);

    unique_ptr<vector<vector<int> > > photons_clusterInMustache       (new vector<vector<int > >);
    unique_ptr<vector<vector<int> > > photons_clusterInDynDPhi        (new vector<vector<int > >);

    unique_ptr<vector<vector<float> > > photons_clusterRawEnergy      (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_clusterCalibEnergy    (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_clusterEta            (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_clusterPhi            (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_clusterDPhiToSeed     (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_clusterDEtaToSeed     (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_clusterDPhiToCentroid (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_clusterDEtaToCentroid (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_psClusterRawEnergy    (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_psClusterEta          (new vector<vector<float > >);
    unique_ptr<vector<vector<float> > > photons_psClusterPhi          (new vector<vector<float > >);
 
    ///////////////////// 
    // Get the photons //
    /////////////////////
    Handle<View<pat::Photon> > photons_h;
    iEvent.getByToken(photonsToken, photons_h);

    // // Get tools to get cluster shape information
  
    if (clusterTools_) delete clusterTools_;
    clusterTools_ = new EcalClusterLazyTools( iEvent, iSetup,
					      ebReducedRecHitCollection,
					      eeReducedRecHitCollection,
					      esReducedRecHitCollection );

    //loop over photon collection
    size_t photonsIndex = 0;
    unsigned int photonsIndexCMS3 = -1;
    View<pat::Photon>::const_iterator photon;
    for(photon = photons_h->begin(); photon != photons_h->end(); photon++, photonsIndex++) {
	// throw out photons below minEt
	if (photon->et() < minEt_)
            //instead of photon et, use sc et for alignment purposes (?)
            continue;
	photonsIndexCMS3++; // this index is the one for CMS3 variables. Increments with the push_backs below

	// Get photon and track objects
	const edm::RefToBase<pat::Photon> photonRef = photons_h->refAt(photonsIndex);
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
	photons_e1x5    		   ->push_back( photon->e1x5()					 );
	photons_e3x3               ->push_back( photon->e3x3()                   );
	photons_e5x5               ->push_back( photon->e5x5()                   );
	photons_e2x5Max            ->push_back( photon->e2x5()                   );
	photons_sigmaEtaEta        ->push_back( photon->sigmaEtaEta()          	 );

	photons_full5x5_e1x5    	   ->push_back( photon->full5x5_e1x5()			 );
	photons_full3x3_e3x3               ->push_back( photon->full5x5_e3x3()                   );
	photons_full5x5_e5x5               ->push_back( photon->full5x5_e5x5()                   );
	photons_full5x5_e2x5Max            ->push_back( photon->full5x5_e2x5()                   );
	photons_full5x5_sigmaEtaEta        ->push_back( photon->full5x5_sigmaEtaEta()          	 );
	

	// Isolation  (all 0.3 cone size)
	photons_ecalIso03          ->push_back(	photon->ecalRecHitSumEtConeDR03() );
	photons_hcalIso03          ->push_back(	photon->hcalTowerSumEtConeDR03()  );	
	photons_tkIsoSolid03       ->push_back(	photon->trkSumPtSolidConeDR03()	  );
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


	// Added for 53x //
	photons_hcalTowerSumEtBcConeDR04       ->push_back( photon->hcalTowerSumEtBcConeDR04()       );
	photons_hcalDepth1TowerSumEtBcConeDR04 ->push_back( photon->hcalDepth1TowerSumEtBcConeDR04() );
	photons_hcalDepth2TowerSumEtBcConeDR04 ->push_back( photon->hcalDepth2TowerSumEtBcConeDR04() );
	photons_hcalTowerSumEtBcConeDR03       ->push_back( photon->hcalTowerSumEtBcConeDR03()       );
	photons_hcalDepth1TowerSumEtBcConeDR03 ->push_back( photon->hcalDepth1TowerSumEtBcConeDR03() );
	photons_hcalDepth2TowerSumEtBcConeDR03 ->push_back( photon->hcalDepth2TowerSumEtBcConeDR03() );



	//This is a fix for accessing SC information in reminiAOD_V2
	int numberOfClusters =  photon->superCluster()->clusters().size();
	bool missing_clusters = false;
	if( numberOfClusters > 0 ) missing_clusters = !photon->superCluster()->clusters()[numberOfClusters-1].isAvailable();
	  
	int numberOfPSClusters =  photon->superCluster()->preshowerClusters().size();
	bool missing_PSclusters = false;
	if( numberOfPSClusters > 0 ) missing_PSclusters = !photon->superCluster()->preshowerClusters()[numberOfPSClusters-1].isAvailable();

	if( !(missing_clusters || missing_PSclusters) && (photon->r9()>0.8 || photon->chargedHadronIso()<20 || photon->chargedHadronIso()<0.3*photon->p4().pt()) ){

            const int N_ECAL = photon->superCluster()->clustersEnd() - photon->superCluster()->clustersBegin();
            const int N_PS   = photon->superCluster()->preshowerClustersEnd() - photon->superCluster()->preshowerClustersBegin();

            photons_N_ECALClusters         ->push_back(std::max(0,N_ECAL-1)); // minus 1 because of seed
            photons_N_PSClusters           ->push_back(N_PS);

            // The commented ones are already available above! Keeping it here for reference
	  
            //	photons_scRawEnergy             = photon->superCluster()->rawEnergy();
            //	photons_scCalibratedEnergy      = photon->superCluster()->energy();
            //	photons_scPreshowerEnergy       = photon->superCluster()->preshowerEnergy();
            //	photons_scEta                   = photon->superCluster()->position().Eta();
            //	photons_scPhi                   = photon->superCluster()->position().Phi();
            //    photons_scSeedR9                = photons_scSeedE3x3->at(photonsIndex)/photons_eSCRaw->at(photonsIndex));
            //	photons_scSeedE2x5max           = clusterTools_->e2x5Max(*(photon->superCluster()->seed()));
            //    photons_scSeedSigmaIetaIeta     = see;
            //    photons_scSeedE5x5              = clusterTools_->e5x5(*(photon->superCluster()->seed()));
            //    photons_isEcalDriven            = photon->ecalDriven();
            //    photons_isTrackerDriven         = photon->trackerDrivenSeed();

            photons_scPreshowerEnergyPlane1->push_back(photon->superCluster()->preshowerEnergyPlane1());
            photons_scPreshowerEnergyPlane2->push_back(photon->superCluster()->preshowerEnergyPlane2());
            photons_scIsEB                 ->push_back((photon->superCluster()->seed()->hitsAndFractions().at(0).first.subdetId()==EcalBarrel));
            photons_scR                    ->push_back(photon->superCluster()->position().R());

            photons_scSeedEta              ->push_back(photon->superCluster()->seed()->eta());
            photons_scSeedPhi              ->push_back(photon->superCluster()->seed()->phi());
            photons_scSeedSize             ->push_back(photon->superCluster()->seed()->hitsAndFractions().size());
            photons_scSeedE3x3             ->push_back(clusterTools_->e3x3(*(photon->superCluster()->seed())));
            photons_scSeedEmax             ->push_back(clusterTools_->eMax(*(photon->superCluster()->seed())));
            photons_scSeedE2nd             ->push_back(clusterTools_->e2nd(*(photon->superCluster()->seed())));

            photons_scSeedELeft            ->push_back(clusterTools_->eLeft(*(photon->superCluster()->seed())));
            photons_scSeedERight           ->push_back(clusterTools_->eRight(*(photon->superCluster()->seed())));
            photons_scSeedETop             ->push_back(clusterTools_->eTop(*(photon->superCluster()->seed())));
            photons_scSeedEBottom          ->push_back(clusterTools_->eBottom(*(photon->superCluster()->seed())));
            photons_scSeedE2x5Left         ->push_back(clusterTools_->e2x5Left(*(photon->superCluster()->seed())));
            photons_scSeedE2x5Right        ->push_back(clusterTools_->e2x5Right(*(photon->superCluster()->seed())));
            photons_scSeedE2x5Top          ->push_back(clusterTools_->e2x5Top(*(photon->superCluster()->seed())));
            photons_scSeedE2x5Bottom       ->push_back(clusterTools_->e2x5Bottom(*(photon->superCluster()->seed())));
            photons_scSeedLeftRightAsym    ->push_back((photons_scSeedELeft->at(photonsIndexCMS3)+photons_scSeedERight->at(photonsIndexCMS3)!=0. ? (photons_scSeedELeft->at(photonsIndexCMS3)-photons_scSeedERight->at(photonsIndexCMS3))/(photons_scSeedELeft->at(photonsIndexCMS3)+photons_scSeedERight->at(photonsIndexCMS3)) : 0.));
            photons_scSeedTopBottomAsym    ->push_back((photons_scSeedETop->at(photonsIndexCMS3)+photons_scSeedEBottom->at(photonsIndexCMS3)!=0. ? (photons_scSeedETop->at(photonsIndexCMS3)-photons_scSeedEBottom->at(photonsIndexCMS3))/(photons_scSeedETop->at(photonsIndexCMS3)+photons_scSeedEBottom->at(photonsIndexCMS3)) : 0.));
            photons_scSeed2x5LeftRightAsym ->push_back((photons_scSeedE2x5Left->at(photonsIndexCMS3)+photons_scSeedE2x5Right->at(photonsIndexCMS3)!=0. ? (photons_scSeedE2x5Left->at(photonsIndexCMS3)-photons_scSeedE2x5Right->at(photonsIndexCMS3))/(photons_scSeedE2x5Left->at(photonsIndexCMS3)+photons_scSeedE2x5Right->at(photonsIndexCMS3)) : 0.));
            photons_scSeed2x5TopBottomAsym ->push_back((photons_scSeedE2x5Top->at(photonsIndexCMS3)+photons_scSeedE2x5Bottom->at(photonsIndexCMS3)!=0. ? (photons_scSeedE2x5Top->at(photonsIndexCMS3)-photons_scSeedE2x5Bottom->at(photonsIndexCMS3))/(photons_scSeedE2x5Top->at(photonsIndexCMS3)+photons_scSeedE2x5Bottom->at(photonsIndexCMS3)) : 0.));

            std::vector<float> vCov  = clusterTools_->localCovariances(*(photon->superCluster()->seed()));
	  
            double see = (isnan(vCov[0]) ? 0. : sqrt(vCov[0]));
            double spp = (isnan(vCov[2]) ? 0. : sqrt(vCov[2]));
            double sep = 0.;
            if (see*spp > 0)
                sep = vCov[1] / (see * spp);
            else if (vCov[1] > 0)
                sep = 1.0;
            else
                sep = -1.0;
	  
            photons_scSeedSigmaIetaIphi    ->push_back(sep); 

            // Those are saved in ElectronMaker but not here, so we add them.
            photons_scPhiWidth             ->push_back(photon->superCluster()->phiWidth());
            photons_scEtaWidth             ->push_back(photon->superCluster()->etaWidth());
            photons_scSeedRawEnergy        ->push_back(photon->superCluster()->seed()->energy());
            photons_scSeedCalibratedEnergy ->push_back(photon->superCluster()->seed()->energy());
            photons_scSeedSigmaIphiIphi    ->push_back(spp); 

            ///////////////////////////////////////////////////////
            // Get crystal info that is not stored in the object //
            ///////////////////////////////////////////////////////
            edm::ESHandle<CaloGeometry> pG;
            iSetup.get<CaloGeometryRecord>().get(pG);
            if(photon->superCluster()->seed()->hitsAndFractions().at(0).first.subdetId()==EcalBarrel) {
                float cryPhi, cryEta, thetatilt, phitilt;
                int ieta, iphi;
                egammaTools::localEcalClusterCoordsEB(*(photon->superCluster()->seed()), *pG, cryEta, cryPhi, ieta, iphi, thetatilt, phitilt);
                photons_scSeedCryEta         ->push_back(cryEta);
                photons_scSeedCryPhi         ->push_back(cryPhi);
                photons_scSeedCryIeta        ->push_back(ieta);
                photons_scSeedCryIphi        ->push_back(iphi);
                photons_scSeedCryX           ->push_back(0);
                photons_scSeedCryY           ->push_back(0);
                photons_scSeedCryIx          ->push_back(0);
                photons_scSeedCryIy          ->push_back(0);
            } else {
                float cryX, cryY, thetatilt, phitilt;
                int ix, iy;
                egammaTools::localEcalClusterCoordsEE(*(photon->superCluster()->seed()), *pG, cryX, cryY, ix, iy, thetatilt, phitilt);
                photons_scSeedCryX           ->push_back(cryX);
                photons_scSeedCryY           ->push_back(cryY);
                photons_scSeedCryIx          ->push_back(ix);
                photons_scSeedCryIy          ->push_back(iy);
                photons_scSeedCryEta         ->push_back(0);
                photons_scSeedCryPhi         ->push_back(0);
                photons_scSeedCryIeta        ->push_back(0);
                photons_scSeedCryIphi        ->push_back(0);
            }

            ///////////////////////////////////
            // Information about subclusters //
            ///////////////////////////////////
	  
            // Containers for partial sums
            float partialsum_photons_clusterMaxDR            = 999.;
            float partialsum_photons_clusterMaxDRDPhi        = 999.;
            float partialsum_photons_clusterMaxDRDEta        = 999.;
            float partialsum_photons_clustersMeanDRToSeed    = 999.;
            float partialsum_photons_clustersMeanDEtaToSeed  = 999.;
            float partialsum_photons_clustersMeanDPhiToSeed  = 999.;
            float partialsum_photons_clusterMaxDRRawEnergy   = 0.;
            float partialsum_photons_clustersMeanRawEnergy   = 0.;
            float partialsum_photons_clustersRMSRawEnergy    = 0.;
            float partialsum_photons_clustersMeanSquareRawEnergy = 0.;
            float partialsum_photons_subclustersRawEnergy = 0.;
	  
            vector<float> partial_photons_clusterRawEnergy;
            vector<float> partial_photons_clusterCalibEnergy;
            vector<float> partial_photons_clusterEta;
            vector<float> partial_photons_clusterPhi;
            vector<float> partial_photons_clusterDPhiToSeed;
            vector<float> partial_photons_clusterDEtaToSeed;
            vector<float> partial_photons_clusterDPhiToCentroid;
            vector<float> partial_photons_clusterDEtaToCentroid;

            vector<int>  partial_photons_clusterInMustache;
            vector<int>  partial_photons_clusterInDynDPhi;

            size_t iclus = 0;
            float maxDR = 0;
            for( auto clus = photon->superCluster()->clustersBegin(); clus != photon->superCluster()->clustersEnd(); ++clus ) {
                partialsum_photons_clustersMeanRawEnergy += (*clus)->energy();
                partialsum_photons_clustersMeanSquareRawEnergy += ((*clus)->energy()*(*clus)->energy());

                if( photon->superCluster()->seed() == (*clus) ) continue;
                partial_photons_clusterRawEnergy.push_back((*clus)->energy());
                partial_photons_clusterCalibEnergy.push_back((*clus)->energy());
                partial_photons_clusterEta.push_back((*clus)->eta());
                partial_photons_clusterPhi.push_back((*clus)->phi());
                partial_photons_clusterDPhiToSeed.push_back(TVector2::Phi_mpi_pi((*clus)->phi() - photon->superCluster()->seed()->phi()));
                partial_photons_clusterDEtaToSeed.push_back((*clus)->eta() - photon->superCluster()->seed()->eta());
                partial_photons_clusterDPhiToCentroid.push_back(TVector2::Phi_mpi_pi((*clus)->phi() - photon->superCluster()->phi()));
                partial_photons_clusterDEtaToCentroid.push_back((*clus)->eta() - photon->superCluster()->eta());
                // find cluster with max dR
                if(reco::deltaR(*(*clus), *(photon->superCluster()->seed())) > maxDR) {
                    maxDR = reco::deltaR(*(*clus), *(photon->superCluster()->seed()));
                    partialsum_photons_clusterMaxDR = maxDR;
                    partialsum_photons_clusterMaxDRDPhi = partial_photons_clusterDPhiToSeed[iclus];
                    partialsum_photons_clusterMaxDRDEta = partial_photons_clusterDEtaToSeed[iclus];
                    partialsum_photons_clusterMaxDRRawEnergy = partial_photons_clusterRawEnergy[iclus];
                }

                partialsum_photons_subclustersRawEnergy += (*clus)->energy();
                partialsum_photons_clustersMeanDRToSeed   = reco::deltaR(*(*clus), *(photon->superCluster()->seed()))*(*clus)->energy();
                partialsum_photons_clustersMeanDEtaToSeed = ((*clus)->eta() - photon->superCluster()->seed()->eta())*(*clus)->energy();
                partialsum_photons_clustersMeanDPhiToSeed = TVector2::Phi_mpi_pi((*clus)->phi() - photon->superCluster()->seed()->phi())*(*clus)->energy();
	  
                partial_photons_clusterInMustache.push_back((int) reco::MustacheKernel::inMustache(photon->superCluster()->seed()->eta(),photon->superCluster()->seed()->phi(),(*clus)->energy(),(*clus)->eta(),(*clus)->phi()));
                partial_photons_clusterInDynDPhi.push_back((int) reco::MustacheKernel::inDynamicDPhiWindow(photon->superCluster()->seed()->hitsAndFractions().at(0).first.subdetId()==EcalBarrel,photon->superCluster()->seed()->phi(),(*clus)->energy(),(*clus)->eta(),(*clus)->phi()));
                ++iclus;
            }

            partialsum_photons_clustersMeanRawEnergy /= (double)(photons_N_ECALClusters->at(photonsIndexCMS3)+1);
            partialsum_photons_clustersMeanSquareRawEnergy /= (double)(photons_N_ECALClusters->at(photonsIndexCMS3)+1);
            partialsum_photons_clustersRMSRawEnergy = sqrt(partialsum_photons_clustersMeanSquareRawEnergy - partialsum_photons_clustersMeanRawEnergy*partialsum_photons_clustersMeanRawEnergy);

            if(partialsum_photons_subclustersRawEnergy>0.) {
                partialsum_photons_clustersMeanDRToSeed /= partialsum_photons_subclustersRawEnergy;
                partialsum_photons_clustersMeanDEtaToSeed /= partialsum_photons_subclustersRawEnergy;
                partialsum_photons_clustersMeanDPhiToSeed /= partialsum_photons_subclustersRawEnergy;
            }

            // saves the information
            photons_clusterMaxDR->push_back(partialsum_photons_clusterMaxDR);
            photons_clusterMaxDRDPhi->push_back(partialsum_photons_clusterMaxDRDPhi);
            photons_clusterMaxDRDEta->push_back(partialsum_photons_clusterMaxDRDEta);
            photons_clustersMeanDRToSeed->push_back(partialsum_photons_clustersMeanDRToSeed);
            photons_clustersMeanDEtaToSeed->push_back(partialsum_photons_clustersMeanDEtaToSeed);
            photons_clustersMeanDPhiToSeed->push_back(partialsum_photons_clustersMeanDPhiToSeed);
            photons_clusterMaxDRRawEnergy->push_back(partialsum_photons_clusterMaxDRRawEnergy);
            photons_clustersMeanRawEnergy->push_back(partialsum_photons_clustersMeanRawEnergy);
            photons_clustersRMSRawEnergy->push_back(partialsum_photons_clustersRMSRawEnergy);

            photons_clusterInMustache->push_back(partial_photons_clusterInMustache); 
            photons_clusterInDynDPhi->push_back(partial_photons_clusterInDynDPhi);   

            photons_clusterRawEnergy->push_back(partial_photons_clusterRawEnergy);
            photons_clusterCalibEnergy->push_back(partial_photons_clusterCalibEnergy);
            photons_clusterEta->push_back(partial_photons_clusterEta);
            photons_clusterPhi->push_back(partial_photons_clusterPhi);
            photons_clusterDPhiToSeed->push_back(partial_photons_clusterDPhiToSeed);
            photons_clusterDEtaToSeed->push_back(partial_photons_clusterDEtaToSeed);
            photons_clusterDPhiToCentroid->push_back(partial_photons_clusterDPhiToCentroid);
            photons_clusterDEtaToCentroid->push_back(partial_photons_clusterDEtaToCentroid);

            vector<float> partial_photons_psClusterRawEnergy;
            vector<float> partial_photons_psClusterEta;
            vector<float> partial_photons_psClusterPhi;
    
            // loop over all preshower clusters 
            size_t ipsclus = 0;
            for( auto psclus = photon->superCluster()->preshowerClustersBegin(); psclus != photon->superCluster()->preshowerClustersEnd(); ++psclus )  {
                partial_photons_psClusterRawEnergy.push_back((*psclus)->energy());
                partial_photons_psClusterEta.push_back((*psclus)->eta());
                partial_photons_psClusterPhi.push_back((*psclus)->phi());
                ++ipsclus;
            }

            photons_psClusterRawEnergy->push_back(partial_photons_psClusterRawEnergy);
            photons_psClusterEta->push_back(partial_photons_psClusterEta);
            photons_psClusterPhi->push_back(partial_photons_psClusterPhi);
    
            photons_isEB->push_back(photon->isEB());
	} else {

            photons_N_ECALClusters         ->push_back(-999);
            photons_N_PSClusters           ->push_back(-999);
            photons_scPreshowerEnergyPlane1->push_back(-999.);
            photons_scPreshowerEnergyPlane2->push_back(-999.);
            photons_scIsEB                 ->push_back(-999);
            photons_scR                    ->push_back(-999.);
            photons_scSeedEta              ->push_back(-999.);
            photons_scSeedPhi              ->push_back(-999.);
            photons_scSeedSize             ->push_back(-999.);
            photons_scSeedE3x3             ->push_back(-999.);
            photons_scSeedEmax             ->push_back(-999.);
            photons_scSeedE2nd             ->push_back(-999.);
            photons_scSeedELeft            ->push_back(-999.);
            photons_scSeedERight           ->push_back(-999.);
            photons_scSeedETop             ->push_back(-999.);
            photons_scSeedEBottom          ->push_back(-999.);
            photons_scSeedE2x5Left         ->push_back(-999.);
            photons_scSeedE2x5Right        ->push_back(-999.);
            photons_scSeedE2x5Top          ->push_back(-999.);
            photons_scSeedE2x5Bottom       ->push_back(-999.);
            photons_scSeedLeftRightAsym    ->push_back(-999.);
            photons_scSeedTopBottomAsym    ->push_back(-999.);
            photons_scSeed2x5LeftRightAsym ->push_back(-999.);
            photons_scSeed2x5TopBottomAsym ->push_back(-999.);
            photons_scSeedSigmaIetaIphi    ->push_back(-999.);
            photons_scPhiWidth             ->push_back(-999.);
            photons_scEtaWidth             ->push_back(-999.);
            photons_scSeedRawEnergy        ->push_back(-999.);
            photons_scSeedCalibratedEnergy ->push_back(-999.);
            photons_scSeedSigmaIphiIphi    ->push_back(-999.);
            photons_scSeedCryEta           ->push_back(-999.);
            photons_scSeedCryPhi           ->push_back(-999.);
            photons_scSeedCryIeta          ->push_back(-999.);
            photons_scSeedCryIphi          ->push_back(-999.);
            photons_scSeedCryX             ->push_back(-999.);
            photons_scSeedCryY             ->push_back(-999.);
            photons_scSeedCryIx            ->push_back(-999.);
            photons_scSeedCryIy            ->push_back(-999.);

            photons_clusterMaxDR           ->push_back(-999.);
            photons_clusterMaxDRDPhi       ->push_back(-999.);
            photons_clusterMaxDRDEta       ->push_back(-999.);
            photons_clustersMeanDRToSeed   ->push_back(-999.);
            photons_clustersMeanDEtaToSeed ->push_back(-999.);
            photons_clustersMeanDPhiToSeed ->push_back(-999.);
            photons_clusterMaxDRRawEnergy  ->push_back(-999.);
            photons_clustersMeanRawEnergy  ->push_back(-999.);
            photons_clustersRMSRawEnergy   ->push_back(-999.);

            photons_clusterInMustache      ->push_back(vector<int>{-999});
            photons_clusterInDynDPhi       ->push_back(vector<int>{-999});

            photons_clusterRawEnergy       ->push_back(vector<float>{-999.});
            photons_clusterCalibEnergy     ->push_back(vector<float>{-999.});
            photons_clusterEta             ->push_back(vector<float>{-999.});
            photons_clusterPhi             ->push_back(vector<float>{-999.});
            photons_clusterDPhiToSeed      ->push_back(vector<float>{-999.});
            photons_clusterDEtaToSeed      ->push_back(vector<float>{-999.});
            photons_clusterDPhiToCentroid  ->push_back(vector<float>{-999.});
            photons_clusterDEtaToCentroid  ->push_back(vector<float>{-999.});

            photons_psClusterRawEnergy     ->push_back(vector<float>{-999.});
            photons_psClusterEta           ->push_back(vector<float>{-999.});
            photons_psClusterPhi           ->push_back(vector<float>{-999.});

            photons_isEB                   ->push_back(-999);
	}

  
    }
 
    // Put the results into the event
    std::string branchprefix = aliasprefix_;
    if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

    //
    iEvent.put(std::move( photons_mass           ), branchprefix+"mass"            );
    iEvent.put(std::move( photons_fiduciality    ), branchprefix+"fiduciality"     );

    iEvent.put(std::move(photons_etaSC       ), "photonsetaSC"       );
    iEvent.put(std::move(photons_phiSC       ), "photonsphiSC"       );
    iEvent.put(std::move(photons_eSC         ), "photonseSC"         );
    iEvent.put(std::move(photons_eSCRaw      ), "photonseSCRaw"      );
    iEvent.put(std::move(photons_eSCPresh    ), "photonseSCPresh"    );

    iEvent.put(std::move( photons_e1x5           ), branchprefix+"e1x5"            );
    iEvent.put(std::move( photons_e3x3           ), branchprefix+"e3x3"            );
    iEvent.put(std::move( photons_e5x5           ), branchprefix+"e5x5"            );
    iEvent.put(std::move( photons_e2x5Max        ), branchprefix+"e2x5Max"         );
    iEvent.put(std::move( photons_sigmaEtaEta    ), branchprefix+"sigmaEtaEta"     );

    iEvent.put(std::move( photons_full5x5_e1x5           ), branchprefix+"full5x5e1x5"            );
    iEvent.put(std::move( photons_full3x3_e3x3           ), branchprefix+"full3x3e3x3"            );
    iEvent.put(std::move( photons_full5x5_e5x5           ), branchprefix+"full5x5e5x5"            );
    iEvent.put(std::move( photons_full5x5_e2x5Max        ), branchprefix+"full5x5e2x5Max"         );
    iEvent.put(std::move( photons_full5x5_sigmaEtaEta    ), branchprefix+"full5x5sigmaEtaEta"     );

    iEvent.put(std::move( photons_tkIsoSolid03   ), branchprefix+"tkIsoSolid03"    );      
    iEvent.put(std::move( photons_ntkIsoSolid03  ), branchprefix+"ntkIsoSolid03"   );      
    iEvent.put(std::move( photons_ecalIso03      ), branchprefix+"ecalIso03"       );
    iEvent.put(std::move( photons_hcalIso03      ), branchprefix+"hcalIso03"       );
    iEvent.put(std::move( photons_tkIsoHollow04  ), branchprefix+"tkIsoHollow04"   );
    iEvent.put(std::move( photons_tkIsoSolid04   ), branchprefix+"tkIsoSolid04"    );      
    iEvent.put(std::move( photons_ntkIsoHollow04 ), branchprefix+"ntkIsoHollow04"  );
    iEvent.put(std::move( photons_ntkIsoSolid04  ), branchprefix+"ntkIsoSolid04"   );      
    iEvent.put(std::move( photons_ecalIso04      ), branchprefix+"ecalIso04"       );
    iEvent.put(std::move( photons_hcalIso04      ), branchprefix+"hcalIso04"       );  
    iEvent.put(std::move( photons_chargedHadronIso), branchprefix+"chargedHadronIso");  
    iEvent.put(std::move( photons_neutralHadronIso), branchprefix+"neutralHadronIso");  
    iEvent.put(std::move( photons_photonIso       ), branchprefix+"photonIso"       );  

    iEvent.put(std::move( photons_hcalTowerSumEtBcConeDR04       ), branchprefix + "hcalTowerSumEtBcConeDR04"      );// Added for 53x //
    iEvent.put(std::move( photons_hcalDepth1TowerSumEtBcConeDR04 ), branchprefix + "hcalDepth1TowerSumEtBcConeDR04");// Added for 53x //
    iEvent.put(std::move( photons_hcalDepth2TowerSumEtBcConeDR04 ), branchprefix + "hcalDepth2TowerSumEtBcConeDR04");// Added for 53x //
    iEvent.put(std::move( photons_hcalTowerSumEtBcConeDR03       ), branchprefix + "hcalTowerSumEtBcConeDR03"      );// Added for 53x //
    iEvent.put(std::move( photons_hcalDepth1TowerSumEtBcConeDR03 ), branchprefix + "hcalDepth1TowerSumEtBcConeDR03");// Added for 53x //
    iEvent.put(std::move( photons_hcalDepth2TowerSumEtBcConeDR03 ), branchprefix + "hcalDepth2TowerSumEtBcConeDR03");// Added for 53x //


    // iEvent.put(std::move( photons_scindex        ), branchprefix+"scindex"         );
    // iEvent.put(std::move( photons_swissSeed      ), branchprefix+"swissSeed"       );

    ////////////////////////////
    // For calibration in 7_X //
    ////////////////////////////

    iEvent.put(std::move(photons_N_ECALClusters), branchprefix + "NECALClusters");
    iEvent.put(std::move(photons_N_PSClusters), branchprefix + "NPSClusters");
    iEvent.put(std::move(photons_scPreshowerEnergyPlane1), branchprefix + "scPreshowerEnergyPlane1");
    iEvent.put(std::move(photons_scPreshowerEnergyPlane2), branchprefix + "scPreshowerEnergyPlane2");
    iEvent.put(std::move(photons_scIsEB), branchprefix + "scIsEB");
    iEvent.put(std::move(photons_scR), branchprefix + "scR");
    iEvent.put(std::move(photons_scSeedEta), branchprefix + "scSeedEta");
    iEvent.put(std::move(photons_scSeedPhi), branchprefix + "scSeedPhi");
    iEvent.put(std::move(photons_scSeedSize), branchprefix + "scSeedSize");
    iEvent.put(std::move(photons_scSeedE3x3), branchprefix + "scSeedE3x3");
    iEvent.put(std::move(photons_scSeedEmax), branchprefix + "scSeedEmax");
    iEvent.put(std::move(photons_scSeedE2nd), branchprefix + "scSeedE2nd");
    iEvent.put(std::move(photons_scSeedELeft), branchprefix + "scSeedELeft");
    iEvent.put(std::move(photons_scSeedERight), branchprefix + "scSeedERight");
    iEvent.put(std::move(photons_scSeedETop), branchprefix + "scSeedETop");
    iEvent.put(std::move(photons_scSeedEBottom), branchprefix + "scSeedEBottom");
    iEvent.put(std::move(photons_scSeedE2x5Left), branchprefix + "scSeedE2x5Left");
    iEvent.put(std::move(photons_scSeedE2x5Right), branchprefix + "scSeedE2x5Right");
    iEvent.put(std::move(photons_scSeedE2x5Top), branchprefix + "scSeedE2x5Top");
    iEvent.put(std::move(photons_scSeedE2x5Bottom), branchprefix + "scSeedE2x5Bottom");
    iEvent.put(std::move(photons_scSeedLeftRightAsym), branchprefix + "scSeedLeftRightAsym");
    iEvent.put(std::move(photons_scSeedTopBottomAsym), branchprefix + "scSeedTopBottomAsym");
    iEvent.put(std::move(photons_scSeed2x5LeftRightAsym), branchprefix + "scSeed2x5LeftRightAsym");
    iEvent.put(std::move(photons_scSeed2x5TopBottomAsym), branchprefix + "scSeed2x5TopBottomAsym");
    iEvent.put(std::move(photons_scSeedSigmaIetaIphi), branchprefix + "scSeedSigmaIetaIphi");
    iEvent.put(std::move(photons_scPhiWidth), branchprefix + "scPhiWidth");
    iEvent.put(std::move(photons_scEtaWidth), branchprefix + "scEtaWidth");
    iEvent.put(std::move(photons_scSeedRawEnergy), branchprefix + "scSeedRawEnergy");
    iEvent.put(std::move(photons_scSeedCalibratedEnergy), branchprefix + "scSeedCalibratedEnergy");
    iEvent.put(std::move(photons_scSeedSigmaIphiIphi), branchprefix + "scSeedSigmaIphiIphi");
    iEvent.put(std::move(photons_scSeedCryEta), branchprefix + "scSeedCryEta");
    iEvent.put(std::move(photons_scSeedCryPhi), branchprefix + "scSeedCryPhi");
    iEvent.put(std::move(photons_scSeedCryIeta), branchprefix + "scSeedCryIeta");
    iEvent.put(std::move(photons_scSeedCryIphi), branchprefix + "scSeedCryIphi");
    iEvent.put(std::move(photons_scSeedCryX), branchprefix + "scSeedCryX");
    iEvent.put(std::move(photons_scSeedCryY), branchprefix + "scSeedCryY");
    iEvent.put(std::move(photons_scSeedCryIx), branchprefix + "scSeedCryIx");
    iEvent.put(std::move(photons_scSeedCryIy), branchprefix + "scSeedCryIy");

    iEvent.put(std::move(photons_clusterMaxDR), branchprefix + "clusterMaxDR");
    iEvent.put(std::move(photons_clusterMaxDRDPhi), branchprefix + "clusterMaxDRDPhi");
    iEvent.put(std::move(photons_clusterMaxDRDEta), branchprefix + "clusterMaxDRDEta");
    iEvent.put(std::move(photons_clustersMeanDRToSeed), branchprefix + "clustersMeanDRToSeed");
    iEvent.put(std::move(photons_clustersMeanDEtaToSeed), branchprefix + "clustersMeanDEtaToSeed");
    iEvent.put(std::move(photons_clustersMeanDPhiToSeed), branchprefix + "clustersMeanDPhiToSeed");
    iEvent.put(std::move(photons_clusterMaxDRRawEnergy), branchprefix + "clusterMaxDRRawEnergy");
    iEvent.put(std::move(photons_clustersMeanRawEnergy), branchprefix + "clustersMeanRawEnergy");
    iEvent.put(std::move(photons_clustersRMSRawEnergy), branchprefix + "clustersRMSRawEnergy");

    iEvent.put(std::move(photons_clusterInMustache), branchprefix + "clusterInMustache");
    iEvent.put(std::move(photons_clusterInDynDPhi), branchprefix + "clusterInDynDPhi");

    iEvent.put(std::move(photons_clusterRawEnergy), branchprefix + "clusterRawEnergy");
    iEvent.put(std::move(photons_clusterCalibEnergy), branchprefix + "clusterCalibEnergy");
    iEvent.put(std::move(photons_clusterEta), branchprefix + "clusterEta");
    iEvent.put(std::move(photons_clusterPhi), branchprefix + "clusterPhi");
    iEvent.put(std::move(photons_clusterDPhiToSeed), branchprefix + "clusterDPhiToSeed");
    iEvent.put(std::move(photons_clusterDEtaToSeed), branchprefix + "clusterDEtaToSeed");
    iEvent.put(std::move(photons_clusterDPhiToCentroid), branchprefix + "clusterDPhiToCentroid");
    iEvent.put(std::move(photons_clusterDEtaToCentroid), branchprefix + "clusterDEtaToCentroid");

    iEvent.put(std::move(photons_psClusterRawEnergy), branchprefix + "psClusterRawEnergy");
    iEvent.put(std::move(photons_psClusterEta), branchprefix + "psClusterEta");
    iEvent.put(std::move(photons_psClusterPhi), branchprefix + "psClusterPhi");
    
    iEvent.put(std::move(photons_isEB), branchprefix + "isEB");
 
}

//define this as a plug-in
DEFINE_FWK_MODULE(PhotonExtraMaker);
