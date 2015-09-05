//-*- C++ -*-

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "CMS3/NtupleMaker/interface/FatJetMaker.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
//#include "DataFormats/BTauReco/interface/CATopJetTagInfo.h"

typedef math::XYZTLorentzVectorF LorentzVector;

// Constructor
FatJetMaker::FatJetMaker(const edm::ParameterSet& iConfig){
  using namespace std;
  using namespace edm;
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix");
  //subjetprefix_ = iConfig.getUntrackedParameter<std::string>("subjetPrefix");
  NjettinessPrefix_ = iConfig.getUntrackedParameter<std::string>("NjettinessPrefix");
  LinksPrefix_ = iConfig.getUntrackedParameter<std::string>("LinksPrefix");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

  // product of this EDProducer
  produces<vector<LorentzVector> > ( branchprefix+"p4"                               ).setBranchAlias( aliasprefix_+"_p4"                        );
  produces<vector<float> >         ( branchprefix+"mass"                             ).setBranchAlias( aliasprefix_+"_mass"                      );
  produces<vector<float> >         ( branchprefix+"area"                             ).setBranchAlias( aliasprefix_+"_area"                      );
  produces<vector<float> >         ( branchprefix+"undoJEC"                          ).setBranchAlias( aliasprefix_+"_undoJEC"                   );
  produces<vector<float> >         ( branchprefix+"pfcandIndicies"                   ).setBranchAlias( aliasprefix_+"_pfcandIndicies"            );
  produces<vector<float> >         ( branchprefix+"nJettinessTau1"                   ).setBranchAlias( aliasprefix_+"_nJettinessTau1"            );
  produces<vector<float> >         ( branchprefix+"nJettinessTau2"                   ).setBranchAlias( aliasprefix_+"_nJettinessTau2"            );
  produces<vector<float> >         ( branchprefix+"nJettinessTau3"                   ).setBranchAlias( aliasprefix_+"_nJettinessTau3"            );
  produces<vector<float> >         ( branchprefix+"nJettinessTau4"                   ).setBranchAlias( aliasprefix_+"_nJettinessTau4"            );
//  produces<vector<float> >         ( branchprefix+"nSubJets"                         ).setBranchAlias( aliasprefix_+"_nSubJets"                  );
//  produces<vector<float> >         ( branchprefix+"topMass"                          ).setBranchAlias( aliasprefix_+"_topMass"                   );
//  produces<vector<float> >         ( branchprefix+"minMass"                          ).setBranchAlias( aliasprefix_+"_minMass"                   );
  produces<vector<float> >         ( branchprefix+"prunedMass"                       ).setBranchAlias( aliasprefix_+"_prunedMass"                );
  produces<vector<float> >         ( branchprefix+"trimmedMass"                      ).setBranchAlias( aliasprefix_+"_trimmedMass"               );
  produces<vector<float> >         ( branchprefix+"filteredMass"                     ).setBranchAlias( aliasprefix_+"_filteredMass"              );
  produces<vector<float> >         ( branchprefix+"massDropFilteredMass"             ).setBranchAlias( aliasprefix_+"_massDropFilteredMass"      );

  // Embedded b-tagging information (miniAOD only)
  pfJetsInputTag_                   = iConfig.getParameter<InputTag>   ( "pfJetsInputTag"                   );
  //pfJetPtCut_                       = iConfig.getParameter<double>     ( "pfJetPtCut"                       );
}

// Destructor
FatJetMaker::~FatJetMaker(){}

// ------------ method called once each job just before starting event loop  ------------
void FatJetMaker::beginJob() {}

// ------------ method called once each job just after ending the event loop  ------------
void FatJetMaker::endJob() {}

// ------------ method called to produce the data  ------------
void FatJetMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  using namespace std;
  using namespace edm;
  using namespace reco;
 
  // create containers
  auto_ptr<vector<LorentzVector> > pfjets_p4                        (new vector<LorentzVector>  );
  auto_ptr<vector<float> >         pfjets_mass                      (new vector<float>          );
  auto_ptr<vector<float> >         pfjets_undoJEC                   (new vector<float>          );
  auto_ptr<vector<vector<int> >  > pfjets_pfcandIndicies            (new vector<vector<int> >   );
  auto_ptr<vector<float> >         pfjets_area                      (new vector<float>          );  
  auto_ptr<vector<int> >           pfjets_partonFlavour             (new vector<int>            );  
  auto_ptr<vector<float> >         pfjets_nJettinessTau1           (new vector<float>          );  
  auto_ptr<vector<float> >         pfjets_nJettinessTau2           (new vector<float>          );  
  auto_ptr<vector<float> >         pfjets_nJettinessTau3           (new vector<float>          );  
  auto_ptr<vector<float> >         pfjets_nJettinessTau4           (new vector<float>          );  
 // auto_ptr<vector<float> >         pfjets_topJetMass               (new vector<float>          );  
  auto_ptr<vector<float> >         pfjets_prunedMass               (new vector<float>          );  
  auto_ptr<vector<float> >         pfjets_trimmedMass              (new vector<float>          );  
  auto_ptr<vector<float> >         pfjets_filteredMass             (new vector<float>          );  
  auto_ptr<vector<float> >         pfjets_massDropFilteredMass     (new vector<float>          );  

  Handle<View<pat::Jet> > pfJetsHandle;
  iEvent.getByLabel(pfJetsInputTag_, pfJetsHandle);
  for(View<pat::Jet>::const_iterator pfjet_it = pfJetsHandle->begin(); pfjet_it != pfJetsHandle->end(); pfjet_it++){
   if(pfjet_it->p4().pt()<100) continue;
   pfjets_p4                        ->push_back( LorentzVector( pfjet_it->p4() ) * pfjet_it->jecFactor("Uncorrected")     );
   pfjets_mass                      ->push_back( pfjet_it->mass()                     );
   pfjets_area                      ->push_back(pfjet_it->jetArea()                   );
   pfjets_undoJEC                   ->push_back( 1.0 / pfjet_it->jecFactor("Uncorrected")   );
//   pfjets_partonFlavour             ->push_back(pfjet_it->partonFlavour()             );
    float nJettinessTau1 = -999, nJettinessTau2 = -999, nJettinessTau3 = -999,nJettinessTau4 = -999;
//    float topMass = -999, minMass = -999, nSubJets = -999;
    float prunedMass = -999, trimmedMass = -999, filteredMass = -999, massDropFilteredMass = -999;  
    if ( pfjet_it->hasUserFloat(NjettinessPrefix_+":tau1") ) nJettinessTau1 = pfjet_it->userFloat(NjettinessPrefix_+":tau1");
    if ( pfjet_it->hasUserFloat(NjettinessPrefix_+":tau2") ) nJettinessTau2 = pfjet_it->userFloat(NjettinessPrefix_+":tau2");
    if ( pfjet_it->hasUserFloat(NjettinessPrefix_+":tau3") ) nJettinessTau3 = pfjet_it->userFloat(NjettinessPrefix_+":tau3");
    if ( pfjet_it->hasUserFloat(NjettinessPrefix_+":tau4") ) nJettinessTau4 = pfjet_it->userFloat(NjettinessPrefix_+":tau4");
    if ( pfjet_it->hasUserFloat(LinksPrefix_+"PrunedMass") ) prunedMass = pfjet_it->userFloat(LinksPrefix_+"PrunedMass");
    if ( pfjet_it->hasUserFloat(LinksPrefix_+"TrimmedMass") ) trimmedMass = pfjet_it->userFloat(LinksPrefix_+"TrimmedMass");
    if ( pfjet_it->hasUserFloat(LinksPrefix_+"FilteredMass") ) filteredMass = pfjet_it->userFloat(LinksPrefix_+"FilteredMass");
    if ( pfjet_it->hasUserFloat(LinksPrefix_+"MassDropFilteredMass") ) massDropFilteredMass = pfjet_it->userFloat(LinksPrefix_+"MassDropFilteredMass");
//    reco::CATopJetTagInfo const * tagInfo =  dynamic_cast<reco::CATopJetTagInfo const *>( pfjet_it->tagInfo("caTop"));
//    if (tagInfo) topMass = tagInfo->properties().topMass;
//    if (tagInfo) minMass = tagInfo->properties().minMass;
//    if (tagInfo) nSubJets = tagInfo->properties().nSubJets;

    pfjets_nJettinessTau1           ->push_back( nJettinessTau1                       );
    pfjets_nJettinessTau2           ->push_back( nJettinessTau2                       );
    pfjets_nJettinessTau3           ->push_back( nJettinessTau3                       );
    pfjets_nJettinessTau4           ->push_back( nJettinessTau4                       );
    pfjets_prunedMass               ->push_back( prunedMass                           );
    pfjets_trimmedMass              ->push_back( trimmedMass                          );
    pfjets_filteredMass             ->push_back( filteredMass                         );
    pfjets_massDropFilteredMass     ->push_back( massDropFilteredMass                 );
//    pfjets_topMass                  ->push_back( topMass                              );
 //   pfjets_minMass                  ->push_back( minMass                              );
 //   pfjets_nSubJets                 ->push_back( nSubJets                              );
/*
  int idx = pfjet_it - pfJetsHandle->begin();
  RefToBase < Jet > jetRef1( Ref < View < pat::Jet > > ( pfJetsHandle , idx ) );
  std::vector <reco::CandidatePtr> pfjet_cands = pfjet_it->daughterPtrVector(); 
  vector<int> pfcandIndicies;
  for(std::vector<reco::CandidatePtr>::const_iterator cand_it = pfjet_cands.begin(); cand_it != pfjet_cands.end(); cand_it++){
      pfcandIndicies.push_back(cand_it->key());
    } 
     pfjets_pfcandIndicies->push_back( pfcandIndicies );
*/
 }
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

  iEvent.put(pfjets_p4                                  , branchprefix+"p4"              );
  iEvent.put(pfjets_mass                                , branchprefix+"mass"            );
  iEvent.put(pfjets_area                                , branchprefix+"area"            );
//  iEvent.put(pfjets_partonFlavour                       , branchprefix+"partonFlavour"   );
 // iEvent.put(pfjets_pfcandIndicies                       , branchprefix+"pfcandIndicies " );
  iEvent.put(pfjets_nJettinessTau1                      , branchprefix+"nJettinessTau1"  );
  iEvent.put(pfjets_nJettinessTau2                      , branchprefix+"nJettinessTau2"  );
  iEvent.put(pfjets_nJettinessTau3                      , branchprefix+"nJettinessTau3"  );
  iEvent.put(pfjets_nJettinessTau4                      , branchprefix+"nJettinessTau4"  );
  iEvent.put(pfjets_prunedMass                          , branchprefix+"prunedMass"      );
  iEvent.put(pfjets_trimmedMass                         , branchprefix+"trimmedMass"     );
  iEvent.put(pfjets_filteredMass                        , branchprefix+"filteredMass"    );
  iEvent.put(pfjets_massDropFilteredMass                , branchprefix+"massDropFilteredMass");
}

//define this as a plug-in
DEFINE_FWK_MODULE(FatJetMaker);
