// -*- C++ -*-
//
// Package:    MuToElsAssMaker
// Class:      MuToElsAssMaker
// 
/**\class MuToElsAssMaker MuToElsAssMaker.cc CMS2/NtupleMaker/src/MuToElsAssMaker.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  pts/4
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: MuToElsAssMaker.cc,v 1.7 2010/03/18 02:13:08 kalavase Exp $
//
//


// user include files
#include "CMS2/NtupleMaker/interface/MuToElsAssMaker.h"


using std::vector;

MuToElsAssMaker::MuToElsAssMaker(const edm::ParameterSet& iConfig) {

  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");
     produces<vector<int>   >(branchprefix+"closestEle").setBranchAlias(aliasprefix_+"_closestEle");	// muon matched to electron
     produces<vector<float> >(branchprefix+"eledr"     ).setBranchAlias(aliasprefix_+"_eledr"     );
     
     m_minDR       =  iConfig.getParameter<double>("minDR");
     musToken_  =  consumes<std::vector<LorentzVector> >(iConfig.getParameter<edm::InputTag>("musInputTag"));
     elsToken_  =  consumes<std::vector<LorentzVector> >(iConfig.getParameter<edm::InputTag>("elsInputTag"));
}

void MuToElsAssMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
     using namespace edm;
     // make vectors to hold the information
     std::auto_ptr<vector<int>   > vector_mus_closestEle(new vector<int>  );
     std::auto_ptr<vector<float> > vector_mus_eledr     (new vector<float>);

     // get muons
     Handle<vector<LorentzVector> > mus_p4_h;
     iEvent.getByToken(musToken_, mus_p4_h);  

     // get electrons
     Handle<vector<LorentzVector> > els_p4_h;
     iEvent.getByToken(elsToken_, els_p4_h);
     
     //loop over muons and find the closest electron
     for(vector<LorentzVector>::const_iterator mus_it = mus_p4_h->begin(),
	 mus_end = mus_p4_h->end();
	 mus_it != mus_end; ++mus_it) {
       
       double mu_eta = mus_it->Eta();
       double mu_phi = mus_it->Phi();
       
       double minDR = m_minDR;
       unsigned int i = 0;
       int index      = -9999;
 
       for(vector<LorentzVector>::const_iterator els_it = els_p4_h->begin(),
	   els_end = els_p4_h->end();
	   els_it != els_end; ++els_it, ++i) {
	 
	 double el_eta = els_it->Eta();
	 double el_phi = els_it->Phi();
	 double dR = deltaR(mu_eta, mu_phi, el_eta, el_phi);

	 if(dR < minDR) {
	   minDR = dR;
	   index = i;
	 }
       }

       // fill vector
       vector_mus_closestEle->push_back(index);
       vector_mus_eledr     ->push_back(minDR);
     }

     // store vectors
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

     iEvent.put(vector_mus_closestEle, branchprefix+"closestEle");
     iEvent.put(vector_mus_eledr     , branchprefix+"eledr"     );
}

// ------------ method called once each job just before starting event loop  ------------
void 
MuToElsAssMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MuToElsAssMaker::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuToElsAssMaker);
