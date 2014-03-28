// -*- C++ -*-
//
// Package:    BCMaker
// Class:      BCMaker
// 
/**\class BCMaker BCMaker.cc CMS2/BCMaker/src/BCMaker.cc

Description: <one line class summary>

Implementation:
<Notes on implementation>
 */
//
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

#include "CMS2/NtupleMaker/interface/BCMaker.h"

#include "DataFormats/CaloRecHit/interface/CaloID.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/Common/interface/Ref.h"

#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"

typedef math::XYZTLorentzVectorF LorentzVector;
typedef math::XYZPoint Point;

//
// class decleration
//

//
// constructors and destructor
//
BCMaker::BCMaker(const edm::ParameterSet& iConfig) {

  aliasprefix_            = iConfig.getUntrackedParameter<std::string>("aliasPrefix");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

	// number of superclusters in the event
	produces<unsigned int>("evtnbcs").setBranchAlias("evt_nbcs");

	// number of basicclusters and crystals
	produces<std::vector<float> >(branchprefix+"crystalsSize").setBranchAlias(aliasprefix_+"_crystalsSize");

	// energies
	produces<std::vector<float> >(branchprefix+"energy").setBranchAlias(aliasprefix_+"_energy");

	// positions
	produces<std::vector<LorentzVector> >(branchprefix+"p4").setBranchAlias(aliasprefix_+"_p4");
	produces<std::vector<LorentzVector> >(branchprefix+"vtxp4").setBranchAlias(aliasprefix_+"_vtx_p4");
	produces<std::vector<LorentzVector> >(branchprefix+"posp4").setBranchAlias(aliasprefix_+"_pos_p4");
	produces<std::vector<float> >(branchprefix+"eta").setBranchAlias(aliasprefix_+"_eta");
	produces<std::vector<float> >(branchprefix+"phi").setBranchAlias(aliasprefix_+"_phi");

	// shape variables for seed basiccluster
	// see
	// RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
	// revision=1.7

	produces<std::vector<int> >(branchprefix+"severitySeed").setBranchAlias(aliasprefix_+"_severitySeed");

	produces<std::vector<float> >(branchprefix+"eSeed").setBranchAlias(aliasprefix_+"_eSeed");
	produces<std::vector<float> >(branchprefix+"eMax").setBranchAlias(aliasprefix_+"_eMax");
	produces<std::vector<float> >(branchprefix+"e2nd").setBranchAlias(aliasprefix_+"_e2nd");

	produces<std::vector<float> >(branchprefix+"e1x3").setBranchAlias(aliasprefix_+"_e1x3");
	produces<std::vector<float> >(branchprefix+"e3x1").setBranchAlias(aliasprefix_+"_e3x1"); 
	produces<std::vector<float> >(branchprefix+"e1x5").setBranchAlias(aliasprefix_+"_e1x5");
	produces<std::vector<float> >(branchprefix+"e2x2").setBranchAlias(aliasprefix_+"_e2x2"); 
	produces<std::vector<float> >(branchprefix+"e3x2").setBranchAlias(aliasprefix_+"_e3x2"); 
	produces<std::vector<float> >(branchprefix+"e3x3").setBranchAlias(aliasprefix_+"_e3x3"); 
	produces<std::vector<float> >(branchprefix+"e4x4").setBranchAlias(aliasprefix_+"_e4x4"); 
	produces<std::vector<float> >(branchprefix+"e5x5").setBranchAlias(aliasprefix_+"_e5x5"); 
	produces<std::vector<float> >(branchprefix+"e2x5Max").setBranchAlias(aliasprefix_+"_e2x5Max");
	// covariances
	produces<std::vector<float> >(branchprefix+"sigmaEtaEta").setBranchAlias(aliasprefix_+"_sigmaEtaEta");
	produces<std::vector<float> >(branchprefix+"sigmaEtaPhi").setBranchAlias(aliasprefix_+"_sigmaEtaPhi");
	produces<std::vector<float> >(branchprefix+"sigmaPhiPhi").setBranchAlias(aliasprefix_+"_sigmaPhiPhi");
	produces<std::vector<float> >(branchprefix+"sigmaIEtaIEta").setBranchAlias(aliasprefix_+"_sigmaIEtaIEta");
	produces<std::vector<float> >(branchprefix+"sigmaIEtaIPhi").setBranchAlias(aliasprefix_+"_sigmaIEtaIPhi");
	produces<std::vector<float> >(branchprefix+"sigmaIPhiIPhi").setBranchAlias(aliasprefix_+"_sigmaIPhiIPhi");

	// match to electrons

	// add superclusters to the ntuple if they have ET > scEtMin_
	scEtMin_ = iConfig.getParameter<double>("scEtMin");

	// hcal depth isolation
	//isoExtRadius_ = iConfig.getParameter<double> ("isoExtRadius");
	//isoIntRadius_ = iConfig.getParameter<double> ("isoIntRadius");
	//isoEtMin_ = iConfig.getParameter<double> ("isoEtMin");

	// input tags for superclusters
	scInputTag_EE_ = iConfig.getParameter<edm::InputTag>("scInputTag_EE");
	scInputTag_EB_ = iConfig.getParameter<edm::InputTag>("scInputTag_EB");
	ecalRecHitsInputTag_EE_ = iConfig.getParameter<edm::InputTag>("ecalRecHitsInputTag_EE");
	ecalRecHitsInputTag_EB_ = iConfig.getParameter<edm::InputTag>("ecalRecHitsInputTag_EB");

	scInputTags_.clear();
	scInputTags_.push_back(scInputTag_EE_);
	scInputTags_.push_back(scInputTag_EB_);

	hitInputTags_.clear();
	hitInputTags_.push_back(ecalRecHitsInputTag_EE_);
	hitInputTags_.push_back(ecalRecHitsInputTag_EB_);

}

void BCMaker::beginRun(const edm::Run & iRun, const edm::EventSetup & iSetup )
{
	edm::ESHandle<EcalChannelStatus> chStatus;
	iSetup.get<EcalChannelStatusRcd>().get(chStatus);
	// where const EcalChannelStatusCode * channelStatus;
	channelStatus_ = chStatus.product();
}



void BCMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{


	// get the primary vertices
	//edm::Handle<reco::VertexCollection> vertexHandle;
	//try {
	//  iEvent.getByLabel(primaryVertexInputTag_, vertexHandle);
	//}
	//catch ( cms::Exception& ex ) {
	//  edm::LogError("BCMakerError") << "Error! can't get the primary vertex";
	//}
	//const reco::VertexCollection *vertexCollection = vertexHandle.product();
	Point pv(0.0, 0.0, 0.0);
	//if (vertexCollection->size() > 0) {
	//  pv = vertexCollection->at(0).position();
	//}


	// ecal cluster shape variables
	EcalClusterLazyTools lazyTools(iEvent, iSetup,
			ecalRecHitsInputTag_EB_, ecalRecHitsInputTag_EE_);


	std::auto_ptr<unsigned int> evt_nbcs (new unsigned int);
	std::auto_ptr<std::vector<LorentzVector> > vector_bcs_p4 (new std::vector<LorentzVector>);
	std::auto_ptr<std::vector<LorentzVector> > vector_bcs_pos_p4 (new std::vector<LorentzVector>);
	std::auto_ptr<std::vector<LorentzVector> > vector_bcs_vtx_p4 (new std::vector<LorentzVector>);
	std::auto_ptr<std::vector<float> > vector_bcs_eta (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_phi (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_crystalsSize (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_energy (new std::vector<float>);
	//std::auto_ptr<std::vector<float> > vector_bcs_hd1 (new std::vector<float>);
	//std::auto_ptr<std::vector<float> > vector_bcs_hd2 (new std::vector<float>);

	std::auto_ptr<std::vector<float> > vector_bcs_eMax (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e2nd (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_eSeed (new std::vector<float>);
	std::auto_ptr<std::vector<int> > vector_bcs_severitySeed (new std::vector<int>);

	std::auto_ptr<std::vector<float> > vector_bcs_e1x3 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e3x1 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e1x5 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e2x2 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e3x2 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e3x3 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e4x4 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e5x5 (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_e2x5Max (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_sigmaEtaEta (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_sigmaEtaPhi(new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_sigmaPhiPhi(new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_sigmaIEtaIEta (new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_sigmaIEtaIPhi(new std::vector<float>);
	std::auto_ptr<std::vector<float> > vector_bcs_sigmaIPhiIPhi(new std::vector<float>);

	*evt_nbcs = 0;
	// there are multiple supercluster collections. In the ntuple
	// these will become concatonated
	for (unsigned int i = 0; i < scInputTags_.size(); ++i)
	{

		// get superclusters
		edm::Handle<reco::BasicClusterCollection> scHandle;
		try {
			iEvent.getByLabel(scInputTags_[i], scHandle);
		}
		catch ( cms::Exception& ex ) {
			edm::LogError("BCMakerError") << "Error! can't get the SuperClusters";
		}
		const reco::BasicClusterCollection *scCollection = scHandle.product();

		// get hits
		edm::Handle<EcalRecHitCollection> rhcHandle;
		iEvent.getByLabel(hitInputTags_[i], rhcHandle);
		const EcalRecHitCollection *recHits = rhcHandle.product();	

		size_t scIndex = 0;
		for (reco::BasicClusterCollection::const_iterator sc = scCollection->begin();
				sc != scCollection->end(); ++sc, ++scIndex) {

			// do ET cut
			if ( (sc->energy()/cosh(sc->eta())) < scEtMin_) continue;


			LorentzVector p4 = initP4(pv, *sc, lazyTools.e3x3(*(sc)));
			vector_bcs_p4->push_back( p4 );
			vector_bcs_vtx_p4->push_back( LorentzVector(pv.x(), pv.y(), pv.z(), 0.) );
			vector_bcs_pos_p4->push_back( LorentzVector(sc->position().x(), sc->position().y(), sc->position().z(), 0.) );
			vector_bcs_eta->push_back( sc->eta() );
			vector_bcs_phi->push_back( sc->phi() );
			vector_bcs_energy->push_back( sc->energy() );
			//vector_bcs_hd1->push_back(egammaIsoD1.getTowerEtSum(&(*sc)) );
			//vector_bcs_hd2->push_back(egammaIsoD2.getTowerEtSum(&(*sc)) );


			DetId seedId = sc->seed();
			EcalRecHitCollection::const_iterator seedHit = recHits->find(seedId);
			vector_bcs_eSeed->push_back( seedHit->energy() );	
			//	vector_bcs_severitySeed->push_back ( EcalSeverityLevelAlgo::severityLevel(seedId, *recHits, *channelStatus_ ) );
			vector_bcs_severitySeed->push_back ( seedHit->recoFlag() );

			vector_bcs_eMax->push_back( lazyTools.eMax(*(sc)) );
			vector_bcs_e2nd->push_back( lazyTools.e2nd(*(sc)) );

			vector_bcs_e1x3->push_back( lazyTools.e1x3(*(sc)) );
			vector_bcs_e3x1->push_back( lazyTools.e3x1(*(sc)) );
			vector_bcs_e1x5->push_back( lazyTools.e1x5(*(sc)) );
			vector_bcs_e2x2->push_back( lazyTools.e2x2(*(sc)) );
			vector_bcs_e3x2->push_back( lazyTools.e3x2(*(sc)) );
			vector_bcs_e3x3->push_back( lazyTools.e3x3(*(sc)) );
			vector_bcs_e4x4->push_back( lazyTools.e4x4(*(sc)) );
			vector_bcs_e5x5->push_back( lazyTools.e5x5(*(sc)) );
			vector_bcs_e2x5Max->push_back( lazyTools.e2x5Max(*(sc)) );
			std::vector<float> covariances = lazyTools.covariances(*(sc));
			// if seed basic cluster is in the endcap then correct sigma eta eta
			// according to the super cluster eta
			if(fabs(sc->eta()) > 1.479) {
				covariances[0] -= 0.02*(fabs(sc->eta()) - 2.3);
			}
			vector_bcs_sigmaEtaEta->push_back( sqrt(covariances[0]) );
			vector_bcs_sigmaEtaPhi->push_back( sqrt(covariances[1]) );
			vector_bcs_sigmaPhiPhi->push_back( sqrt(covariances[2]) );
			std::vector<float> localCovariances = lazyTools.localCovariances(*(sc));
			vector_bcs_sigmaIEtaIEta->push_back( sqrt(localCovariances[0]) );
			vector_bcs_sigmaIEtaIPhi->push_back( sqrt(localCovariances[1]) );
			vector_bcs_sigmaIPhiIPhi->push_back( sqrt(localCovariances[2]) );
			const std::vector<std::pair<DetId, float > > detIds = sc->hitsAndFractions() ;
			vector_bcs_crystalsSize->push_back( detIds.size() );

		} // end loop on bcs

	} // end loop on sc input tags

	*evt_nbcs = vector_bcs_p4->size();

	// put results into the event
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

	iEvent.put(evt_nbcs, "evtnbcs");
	iEvent.put(vector_bcs_energy, branchprefix+"energy");
	iEvent.put(vector_bcs_p4, branchprefix+"p4");
	iEvent.put(vector_bcs_vtx_p4, branchprefix+"vtxp4");
	iEvent.put(vector_bcs_pos_p4, branchprefix+"posp4");
	iEvent.put(vector_bcs_eta, branchprefix+"eta");
	iEvent.put(vector_bcs_phi, branchprefix+"phi");
	//iEvent.put(vector_bcs_hd1, branchprefix+"hd1");
	//iEvent.put(vector_bcs_hd2, branchprefix+"hd2");

	iEvent.put(vector_bcs_eSeed, branchprefix+"eSeed");
	iEvent.put(vector_bcs_severitySeed, branchprefix+"severitySeed");
	iEvent.put(vector_bcs_e2nd, branchprefix+"e2nd");
	iEvent.put(vector_bcs_eMax, branchprefix+"eMax");

	iEvent.put(vector_bcs_e1x3, branchprefix+"e1x3");
	iEvent.put(vector_bcs_e3x1, branchprefix+"e3x1");
	iEvent.put(vector_bcs_e1x5, branchprefix+"e1x5");
	iEvent.put(vector_bcs_e2x2, branchprefix+"e2x2");
	iEvent.put(vector_bcs_e3x2, branchprefix+"e3x2");
	iEvent.put(vector_bcs_e3x3, branchprefix+"e3x3");
	iEvent.put(vector_bcs_e4x4, branchprefix+"e4x4");
	iEvent.put(vector_bcs_e5x5, branchprefix+"e5x5");
	iEvent.put(vector_bcs_e2x5Max, branchprefix+"e2x5Max");
	iEvent.put(vector_bcs_sigmaEtaEta, branchprefix+"sigmaEtaEta");
	iEvent.put(vector_bcs_sigmaEtaPhi, branchprefix+"sigmaEtaPhi");
	iEvent.put(vector_bcs_sigmaPhiPhi, branchprefix+"sigmaPhiPhi");
	iEvent.put(vector_bcs_sigmaIEtaIEta, branchprefix+"sigmaIEtaIEta");
	iEvent.put(vector_bcs_sigmaIEtaIPhi, branchprefix+"sigmaIEtaIPhi");
	iEvent.put(vector_bcs_sigmaIPhiIPhi, branchprefix+"sigmaIPhiIPhi");
	iEvent.put(vector_bcs_crystalsSize, branchprefix+"crystalsSize");

}

math::XYZTLorentzVectorF BCMaker::initP4(const math::XYZPoint &pvPos, 
		const reco::BasicCluster &sc, float e3x3)
{

	math::XYZVector scPos(sc.x(), sc.y(), sc.z());
	math::XYZVector pvPosVec(pvPos.x(), pvPos.y(), pvPos.z());
	math::XYZVector objPosition = scPos - pvPosVec;
	double scale = sc.energy() / objPosition.R();
	return math::XYZTLorentzVectorF(objPosition.x() * scale, 
			objPosition.y() * scale, 
			objPosition.z() * scale, 
			sc.energy());
}



// ------------ method called once each job just before starting event loop  ------------
	void 
BCMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
	void 
BCMaker::endJob() 
{
}

//define this as a plug-in
DEFINE_FWK_MODULE(BCMaker);

