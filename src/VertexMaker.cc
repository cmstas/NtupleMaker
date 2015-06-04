// -*- C++ -*-
//
// Package:    VertexMaker
// Class:      VertexMaker
// 
/**\class VertexMaker VertexMaker.cc CMS2/VertexMaker/src/VertexMaker.cc

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

#include "CMS3/NtupleMaker/interface/VertexMaker.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Math/interface/LorentzVector.h"

typedef math::XYZTLorentzVectorF LorentzVector;

//
// class decleration
//

//
// constructors and destructor
//
VertexMaker::VertexMaker(const edm::ParameterSet& iConfig) {

  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

  produces<unsigned int>                      ("evtn"+branchprefix              ).setBranchAlias("evt_n"+aliasprefix_              );  // number of vertices in event
  produces<std::vector<LorentzVector> >       (branchprefix+"position"          ).setBranchAlias(aliasprefix_+"_position"          );  // position of vertices and associated errors
  produces<std::vector<float> >               (branchprefix+"xError"            ).setBranchAlias(aliasprefix_+"_xError"            );
  produces<std::vector<float> >               (branchprefix+"yError"            ).setBranchAlias(aliasprefix_+"_yError"            );
  produces<std::vector<float> >               (branchprefix+"zError"            ).setBranchAlias(aliasprefix_+"_zError"            );
  produces<std::vector<float> >               (branchprefix+"chi2"              ).setBranchAlias(aliasprefix_+"_chi2"              );   // chi2 and ndof. Tracks apparently can contribute with a weight so ndof may be non integral
  produces<std::vector<float> >               (branchprefix+"ndof"              ).setBranchAlias(aliasprefix_+"_ndof"              );
  produces<std::vector<float> >               (branchprefix+"score"             ).setBranchAlias(aliasprefix_+"_score"             );
  produces<std::vector<int>   >               (branchprefix+"isFake"            ).setBranchAlias(aliasprefix_+"_isFake"            );
  produces<std::vector<int>   >               (branchprefix+"isValid"           ).setBranchAlias(aliasprefix_+"_isValid"           );
  produces<std::vector<std::vector<float > > >(branchprefix+"covMatrix"         ).setBranchAlias(aliasprefix_+"_covMatrix"         );

  // vertex collection input tag
  primaryVertexInputTag_ = iConfig.getParameter<edm::InputTag>("primaryVertexInputTag");
}

void VertexMaker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  // get the primary vertices
//  std::cout << __LINE__ <<"vertex maker begin" <<std::endl;
  edm::Handle<reco::VertexCollection> vertexHandle;

//  try {
 //   iEvent.getByLabel(primaryVertexInputTag_, vertexHandle);
//  }
 // catch ( cms::Exception& ex ) {
  //  edm::LogError("VertexMakerError") << "Error! can't get the primary vertex";
 // }

    iEvent.getByLabel(primaryVertexInputTag_, vertexHandle);
  const reco::VertexCollection *vertexCollection = vertexHandle.product();
//  std::cout << __LINE__ <<"number of vertex" <<vertexCollection->size()<<std::endl;

  std::auto_ptr<unsigned int>                      evt_nvtxs                     (new unsigned int                     );
  std::auto_ptr<std::vector<LorentzVector> >       vector_vtxs_position          (new std::vector<LorentzVector>       );
  std::auto_ptr<std::vector<float> >               vector_vtxs_xError            (new std::vector<float>               );
  std::auto_ptr<std::vector<float> >               vector_vtxs_yError            (new std::vector<float>               );
  std::auto_ptr<std::vector<float> >               vector_vtxs_zError            (new std::vector<float>               );
  std::auto_ptr<std::vector<float> >               vector_vtxs_chi2              (new std::vector<float>               );
  std::auto_ptr<std::vector<float> >               vector_vtxs_ndof              (new std::vector<float>               );
  std::auto_ptr<std::vector<float> >               vector_vtxs_score             (new std::vector<float>               );
  std::auto_ptr<std::vector<int>   >               vector_vtxs_isFake            (new std::vector<int>                 );
  std::auto_ptr<std::vector<int>   >               vector_vtxs_isValid           (new std::vector<int>                 );
  std::auto_ptr<std::vector<std::vector<float> > > vector_vtxs_covMatrix         (new std::vector<std::vector<float> > );
     
  *evt_nvtxs = vertexCollection->size();

  edm::Handle<edm::ValueMap<float> > vertexScoreHandle;
  try {
    iEvent.getByLabel(primaryVertexInputTag_, vertexScoreHandle);
  }
  catch ( cms::Exception& ex ) {
    edm::LogError("VertexMakerError") << "Error! can't get the score of primary vertices";
  }

  unsigned int index = 0;
  const unsigned int covMatrix_dim = 3;

  for (reco::VertexCollection::const_iterator vtx = vertexCollection->begin(); vtx != vertexCollection->end(); ++vtx, ++index) {
    vector_vtxs_position         ->push_back( LorentzVector( vtx->position().x(), vtx->position().y(), vtx->position().z(), 0 ) );
    vector_vtxs_xError           ->push_back( vtx->xError()            );
    vector_vtxs_yError           ->push_back( vtx->yError()            );
    vector_vtxs_zError           ->push_back( vtx->zError()            );
    vector_vtxs_chi2             ->push_back( vtx->chi2()              );
    vector_vtxs_ndof             ->push_back( vtx->ndof()              );
    vector_vtxs_isFake           ->push_back( vtx->isFake()            );
    vector_vtxs_isValid          ->push_back( vtx->isValid()           );
    
    if (vertexScoreHandle.isValid()) {
      vector_vtxs_score             ->push_back( vertexScoreHandle->get(vertexHandle.id(),index) );
    } else { 
      vector_vtxs_score             ->push_back( -9999. );
    }

    std::vector<float> temp_vec;
    temp_vec.clear();

    for( unsigned int i = 0; i < covMatrix_dim; i++ ) {
      for( unsigned int j = 0; j < covMatrix_dim; j++ ) {
	temp_vec.push_back( vtx->covariance(i, j) );
      }
    }

    vector_vtxs_covMatrix->push_back( temp_vec );
  }

  // store into the event
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos) branchprefix.replace(branchprefix.find("_"),1,"");

  iEvent.put(evt_nvtxs,                     "evtn"+branchprefix              );
  iEvent.put(vector_vtxs_position,          branchprefix+"position"          );
  iEvent.put(vector_vtxs_xError,            branchprefix+"xError"            );
  iEvent.put(vector_vtxs_yError,            branchprefix+"yError"            );
  iEvent.put(vector_vtxs_zError,            branchprefix+"zError"            );
  iEvent.put(vector_vtxs_chi2,              branchprefix+"chi2"              );
  iEvent.put(vector_vtxs_ndof,              branchprefix+"ndof"              );
  iEvent.put(vector_vtxs_score,             branchprefix+"score"             );
  iEvent.put(vector_vtxs_isFake,            branchprefix+"isFake"            );
  iEvent.put(vector_vtxs_isValid,           branchprefix+"isValid"           );
  iEvent.put(vector_vtxs_covMatrix,         branchprefix+"covMatrix"         );
}

// ------------ method called once each job just before starting event loop  ------------
void VertexMaker::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void VertexMaker::endJob() 
{
}

//define this as a plug-in
DEFINE_FWK_MODULE(VertexMaker);

