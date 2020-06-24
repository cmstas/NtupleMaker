// -*- C++ -*-
//
// Package:    MuonMaker
// Class:      MuonMaker
// 
/**\class MuonMaker MuonMaker.cc CMS2/MuonMaker/src/MuonMaker.cc

   Description: <one line class summary>

   Implementation:
   <Notes on implementation>
*/
//
// Original Author:  pts/4
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: MuonMaker.cc,v 1.68 2012/07/20 01:19:39 dbarge Exp $
//
//


// system include files
#include <memory>
#include <sstream>

// user include files
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonCocktails.h"
#include "DataFormats/MuonReco/interface/MuonCosmicCompatibility.h"
#include "DataFormats/MuonReco/interface/MuonPFIsolation.h"
#include "DataFormats/MuonReco/interface/MuonQuality.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonSimInfo.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackToTrackMap.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/PFIsolation.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Math/VectorUtil.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "CMS3/NtupleMaker/interface/MuonMaker.h"
#include "CMS3/NtupleMaker/interface/MatchUtilities.h"

#include "DataFormats/MuonReco/interface/MuonChamberMatch.h"
#include "DataFormats/MuonReco/interface/MuonShower.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"


//////////////
// typedefs //
//////////////

typedef math::XYZPoint Point;


////////////////
// namespaces //
////////////////

using namespace std;
using namespace reco;
using namespace edm;


/////////////////
// Constructor //
/////////////////

MuonMaker::MuonMaker( const ParameterSet& iConfig ) {

    /////////////////////////////
    // Branch & Alias prefixes //
    /////////////////////////////

    aliasprefix_        = iConfig.getUntrackedParameter<string>("aliasPrefix");
    branchprefix_       = aliasprefix_; if( branchprefix_.find("_") != string::npos ) branchprefix_.replace( branchprefix_.find("_"), 1, "" );


    //////////////////////
    // Input Parameters //
    //////////////////////

    muonsToken    = consumes<View<pat::Muon> >(iConfig.getParameter<InputTag> ("muonsInputTag"   ));
    pfCandsToken  = consumes<pat::PackedCandidateCollection>(iConfig.getParameter<InputTag> ("pfCandsInputTag" ));
    pfJetsToken = consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("pfJetsInputTag"));
    vtxToken         = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vtxInputTag"));
    tevMuonsName     = iConfig.getParameter<string>   ("tevMuonsName"    );

    miniIsoChgValueMapToken_   = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("miniIsoChgValueMap"));
    miniIsoAllValueMapToken_   = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("miniIsoAllValueMap"));

    ////////////
    // Global //
    ////////////

    produces<vector<float> >          ( branchprefix_ + "gfitchi2"                  ).setBranchAlias( aliasprefix_ + "_gfit_chi2"          ); // chi2 of the global muon fit 
    produces<vector<int> >            ( branchprefix_ + "gfitndof"                  ).setBranchAlias( aliasprefix_ + "_gfit_ndof"          ); // number of degree of freedom of the global muon fit 
    produces<vector<int> >            ( branchprefix_ + "gfitvalidSTAHits"          ).setBranchAlias( aliasprefix_ + "_gfit_validSTAHits"  ); // number of hits in the stand alone fit that made it into the gfit
    produces<vector<LorentzVector> >  ( branchprefix_ + "gfitp4"                    ).setBranchAlias( aliasprefix_ + "_gfit_p4"            ); // global fit p4, if global fit exists
    produces<vector<int> >            ( branchprefix_ + "gfitalgo"                  ).setBranchAlias( aliasprefix_ + "_gfit_algo"          );
    produces<vector<float> >          ( branchprefix_ + "gfitptErr"                 ).setBranchAlias( aliasprefix_ + "_gfit_ptErr"         );
  
    ////////////
    // Best   //
    ////////////

    produces<vector<LorentzVector> >  ( branchprefix_ + "bfitp4"                    ).setBranchAlias( aliasprefix_ + "_bfit_p4"            ); // global fit p4, if global fit exists
    produces<vector<int> >            ( branchprefix_ + "bfitalgo"                  ).setBranchAlias( aliasprefix_ + "_bfit_algo"          );
    produces<vector<float> >          ( branchprefix_ + "bfitptErr"                 ).setBranchAlias( aliasprefix_ + "_bfit_ptErr"         );

    /////////////
    // Quality //
    /////////////

    produces<vector<float> >          ( branchprefix_ + "trkKink"                   ).setBranchAlias( aliasprefix_ + "_trkKink"             );  // Muon Quality - trkKink
    produces<vector<float> >          ( branchprefix_ + "chi2LocalPosition"         ).setBranchAlias( aliasprefix_ + "_chi2LocalPosition"   );  // Muon Quality - chi2LocalPositions
    produces<vector<float> >          ( branchprefix_ + "chi2LocalMomentum"         ).setBranchAlias( aliasprefix_ + "_chi2LocalMomentum"   );  // Muon Quality - chi2LocalMomentum
                                    
    //////////
    // Muon //
    //////////

    produces<vector<int> >            ( branchprefix_ + "type"                      ).setBranchAlias( aliasprefix_ + "_type"                   ); // type
    produces<vector<int> >            ( branchprefix_ + "charge"                    ).setBranchAlias( aliasprefix_ + "_charge"                 ); // charge from muon object             
    produces<vector<float> >          ( branchprefix_ + "caloCompatibility"         ).setBranchAlias( aliasprefix_ + "_caloCompatibility"      ); // calo compatibility variable
    produces<vector<float> >          ( branchprefix_ + "segmCompatibility"         ).setBranchAlias( aliasprefix_ + "_segmCompatibility"      );
    produces<vector<LorentzVector> >  ( branchprefix_ + "p4"                        ).setBranchAlias( aliasprefix_ + "_p4"                     ); // candidate p4->this can either be gfit p4, tracker p4 or STA p4 (only for STA muoons)     
    produces<vector<int> >            ( branchprefix_ + "numberOfMatchedStations"   ).setBranchAlias( aliasprefix_ + "_numberOfMatchedStations"); // number of muon stations with muon segements used in the fit

    ////////
    // ID //
    ////////

    produces<vector<int> > ( branchprefix_ + "pidTMLastStationLoose"     ).setBranchAlias( aliasprefix_ + "_pid_TMLastStationLoose"     ); // loose tracker muon identification based on muon/hadron penetration depth difference       
    produces<vector<int> > ( branchprefix_ + "pidTMLastStationTight"     ).setBranchAlias( aliasprefix_ + "_pid_TMLastStationTight"     ); // tight tracker muon identification based on muon/hadron penetration depth difference       
    produces<vector<int> > ( branchprefix_ + "pidTM2DCompatibilityLoose" ).setBranchAlias( aliasprefix_ + "_pid_TM2DCompatibilityLoose" ); // loose tracker muon likelihood identification based on muon matches and calo depositions   
    produces<vector<int> > ( branchprefix_ + "pidTM2DCompatibilityTight" ).setBranchAlias( aliasprefix_ + "_pid_TM2DCompatibilityTight" ); // tight tracker muon likelihood identification based on muon matches and calo depositions
    produces<vector<int> > ( branchprefix_ + "pidTMOneStationTight"      ).setBranchAlias( aliasprefix_ + "_pid_TMOneStationTight"      ); //  
    produces<vector<int> > ( branchprefix_ + "pidPFMuon"                 ).setBranchAlias( aliasprefix_ + "_pid_PFMuon"                 ); // is particle flow muon

    ////////////
    // Energy //
    ////////////

    produces<vector<float> >          ( branchprefix_ + "ecaltime"      ).setBranchAlias( aliasprefix_ + "_ecal_time"       ); 
    produces<vector<float> >          ( branchprefix_ + "hcaltime"      ).setBranchAlias( aliasprefix_ + "_hcal_time"       ); 

    ///////////////
    // Isolation //
    ///////////////
                                    
    produces<vector<float> >          ( branchprefix_ + "isotrckvetoDep"            ).setBranchAlias( aliasprefix_ + "_iso_trckvetoDep"     ); // sumPt in the veto cone, tracker
    produces<vector<float> >          ( branchprefix_ + "isoecalvetoDep"            ).setBranchAlias( aliasprefix_ + "_iso_ecalvetoDep"     ); // sumEt in the veto cone, ecal
    produces<vector<float> >          ( branchprefix_ + "isohcalvetoDep"            ).setBranchAlias( aliasprefix_ + "_iso_hcalvetoDep"     ); // sumPt in the veto cone, hcal
    produces<vector<float> >          ( branchprefix_ + "isohovetoDep"              ).setBranchAlias( aliasprefix_ + "_iso_hovetoDep"       ); // sumPt in the veto cone, ho
    produces<vector<float> >          ( branchprefix_ + "iso03sumPt"                ).setBranchAlias( aliasprefix_ + "_iso03_sumPt"         ); // sum of track Pt for cone of 0.3 
    produces<vector<float> >          ( branchprefix_ + "iso03emEt"                 ).setBranchAlias( aliasprefix_ + "_iso03_emEt"          ); // sum of ecal Et for cone of 0.3 
    produces<vector<float> >          ( branchprefix_ + "iso03hadEt"                ).setBranchAlias( aliasprefix_ + "_iso03_hadEt"         ); // sum of hcal Et for cone of 0.3 
    produces<vector<int> >            ( branchprefix_ + "iso03ntrk"                 ).setBranchAlias( aliasprefix_ + "_iso03_ntrk"          ); // number of tracks in the cone of 0.3 

    ////////////
    // Tracks //
    ////////////

    produces<vector<LorentzVector> > ( branchprefix_ + "trkp4"             ).setBranchAlias( aliasprefix_ + "_trk_p4"            ); // track p4            
    produces<vector<int> >           ( branchprefix_ + "validHits"         ).setBranchAlias( aliasprefix_ + "_validHits"         ); // number of used hits in the sitracker fit      
    produces<vector<int> >           ( branchprefix_ + "lostHits"          ).setBranchAlias( aliasprefix_ + "_lostHits"          ); // number of lost hits in the sitracker fit      
    produces<vector<float> >         ( branchprefix_ + "d0Err"             ).setBranchAlias( aliasprefix_ + "_d0Err"             ); // error on the impact parameter, si track fit      
    produces<vector<float> >         ( branchprefix_ + "z0Err"             ).setBranchAlias( aliasprefix_ + "_z0Err"             ); // error on z position of the point of closest approach, si track fit  
    produces<vector<float> >         ( branchprefix_ + "ptErr"             ).setBranchAlias( aliasprefix_ + "_ptErr"             ); // si track Pt error          
    produces<vector<int> >           ( branchprefix_ + "algo"              ).setBranchAlias( aliasprefix_ + "_algo"              );
    produces<vector<int> >           ( branchprefix_ + "algoOrig"          ).setBranchAlias( aliasprefix_ + "_algoOrig"          );
    produces<vector<int> >           ( branchprefix_ + "nlayers"           ).setBranchAlias( aliasprefix_ + "_nlayers"           );
    produces<vector<int> >           ( branchprefix_ + "validPixelHits"    ).setBranchAlias( aliasprefix_ + "_validPixelHits"    );
    produces<vector<int> >           ( branchprefix_ + "expinnerlayers"    ).setBranchAlias( aliasprefix_ + "_exp_innerlayers"   );
    produces<vector<int> >           ( branchprefix_ + "expouterlayers"    ).setBranchAlias( aliasprefix_ + "_exp_outerlayers"   );
    produces<vector<float> >         ( branchprefix_ + "dxyPV"             ).setBranchAlias( aliasprefix_ + "_dxyPV"             );
    produces<vector<float> >         ( branchprefix_ + "dzPV"              ).setBranchAlias( aliasprefix_ + "_dzPV"              );
    produces<vector<float> >         (branchprefix_  + "dzfirstPV"        ).    setBranchAlias( aliasprefix_ + "_dz_firstPV" );
    produces<vector<float> >         (branchprefix_  + "dxyfirstPV"        ).    setBranchAlias( aliasprefix_ + "_dxy_firstPV" );

    ////////
    // PF //
    ////////

    produces<vector< int> >           ( branchprefix_ + "pfcharge"                  ).setBranchAlias( aliasprefix_ + "_pfcharge"         );
    produces<vector< int> >           ( branchprefix_ + "pfidx"                     ).setBranchAlias( aliasprefix_ + "_pfidx"            );
    produces<vector< int> >           ( branchprefix_ + "pfparticleId"              ).setBranchAlias( aliasprefix_ + "_pfparticleId"     );
    produces<vector< LorentzVector> > ( branchprefix_ + "pfp4"                      ).setBranchAlias( aliasprefix_ + "_pfp4"                );

    produces<vector<float> >          ( branchprefix_ + "isoR03pfChargedHadronPt"   ).setBranchAlias( aliasprefix_ + "_isoR03_pf_ChargedHadronPt"   );
    produces<vector<float> >          ( branchprefix_ + "isoR03pfChargedParticlePt" ).setBranchAlias( aliasprefix_ + "_isoR03_pf_ChargedParticlePt" );
    produces<vector<float> >          ( branchprefix_ + "isoR03pfNeutralHadronEt"   ).setBranchAlias( aliasprefix_ + "_isoR03_pf_NeutralHadronEt"   );
    produces<vector<float> >          ( branchprefix_ + "isoR03pfPhotonEt"          ).setBranchAlias( aliasprefix_ + "_isoR03_pf_PhotonEt"          );
    produces<vector<float> >          ( branchprefix_ + "isoR03pfNeutralHadronEtHighThreshold").setBranchAlias( aliasprefix_ + "_isoR03_pf_NeutralHadronEtHighThreshold");
    produces<vector<float> >          ( branchprefix_ + "isoR03pfPhotonEtHighThreshold"       ).setBranchAlias( aliasprefix_ + "_isoR03_pf_PhotonEtHighThreshold"       );
    produces<vector<float> >          ( branchprefix_ + "isoR03pfPUPt"              ).setBranchAlias( aliasprefix_ + "_isoR03_pf_PUPt"              );

    produces<vector<float> >          ( branchprefix_ + "isoR04pfChargedHadronPt"   ).setBranchAlias( aliasprefix_ + "_isoR04_pf_ChargedHadronPt"   );
    produces<vector<float> >          ( branchprefix_ + "isoR04pfChargedParticlePt" ).setBranchAlias( aliasprefix_ + "_isoR04_pf_ChargedParticlePt" );
    produces<vector<float> >          ( branchprefix_ + "isoR04pfNeutralHadronEt"   ).setBranchAlias( aliasprefix_ + "_isoR04_pf_NeutralHadronEt"   );
    produces<vector<float> >          ( branchprefix_ + "isoR04pfPhotonEt"          ).setBranchAlias( aliasprefix_ + "_isoR04_pf_PhotonEt"          );
    produces<vector<float> >          ( branchprefix_ + "isoR04pfNeutralHadronEtHighThreshold").setBranchAlias( aliasprefix_ + "_isoR04_pf_NeutralHadronEtHighThreshold");
    produces<vector<float> >          ( branchprefix_ + "isoR04pfPhotonEtHighThreshold"       ).setBranchAlias( aliasprefix_ + "_isoR04_pf_PhotonEtHighThreshold"       );
    produces<vector<float> >          ( branchprefix_ + "isoR04pfPUPt"              ).setBranchAlias( aliasprefix_ + "_isoR04_pf_PUPt"              );

    ///////////
    // IP 3D //
    ///////////
  
    produces<vector<float> >          ( branchprefix_ + "ip3d"                      ).setBranchAlias( aliasprefix_ + "_ip3d"                ); 
    produces<vector<float> >          ( branchprefix_ + "ip3derr"                   ).setBranchAlias( aliasprefix_ + "_ip3derr"             ); 
    produces<vector<float> >          ( branchprefix_ + "ip2d"                      ).setBranchAlias( aliasprefix_ + "_ip2d"                ); 
    produces<vector<float> >          ( branchprefix_ + "ip2derr"                   ).setBranchAlias( aliasprefix_ + "_ip2derr"             ); 

    //////////////////////
    // genMatch miniAOD //
    //////////////////////

    produces<vector<int>           >("musmcpatMatchid"    ).setBranchAlias("mus_mc_patMatch_id" ); 
    produces<vector<LorentzVector> >("musmcpatMatchp4"    ).setBranchAlias("mus_mc_patMatch_p4" );
    produces<vector<float>         >("musmcpatMatchdr"    ).setBranchAlias("mus_mc_patMatch_dr" );
    produces<vector<float>         >("musminiIsouncor"    ).setBranchAlias("mus_miniIso_uncor"  );
    produces<vector<float>         >("musminiIsoch"       ).setBranchAlias("mus_miniIso_ch"     );
    produces<vector<float>         >("musminiIsonh"       ).setBranchAlias("mus_miniIso_nh"     );
    produces<vector<float>         >("musminiIsoem"       ).setBranchAlias("mus_miniIso_em"     );
    produces<vector<float>         >("musminiIsodb"       ).setBranchAlias("mus_miniIso_db"     );

    produces<vector<float>         >("musminiRelIsochg"       ).setBranchAlias("mus_miniRelIso_chg"                       	);
    produces<vector<float>         >("musminiRelIsoall"       ).setBranchAlias("mus_miniRelIso_all"                       	);

    produces<vector<unsigned int>         >("musselectors"       ).setBranchAlias("mus_selectors"     );
    produces<vector<int>         >("mussimType"       ).setBranchAlias("mus_simType"     );
    produces<vector<int>         >("mussimExtType"       ).setBranchAlias("mus_simExtType"     );

    produces<vector<int>         >("musjetNDauChargedMVASel"         	).setBranchAlias("mus_jetNDauChargedMVASel"                        	);
    produces<vector<float>         >("musptRatio"         	).setBranchAlias("mus_ptRatio"                        	);
    produces<vector<float>         >("musptRel"         	).setBranchAlias("mus_ptRel"                        	);
    produces<vector<float>         >("musjetBTagCSV"         	).setBranchAlias("mus_jetBTagCSV"                        	);

    // dimuon vertex stuff
    produces<float> ("musvtxchi2"   ).setBranchAlias("mus_vtx_chi2"   );
    produces<float> ("musvtxndof"   ).setBranchAlias("mus_vtx_ndof"   );
    produces<float> ("musvtxx"      ).setBranchAlias("mus_vtx_x"      );
    produces<float> ("musvtxy"      ).setBranchAlias("mus_vtx_y"      );
    produces<float> ("musvtxz"      ).setBranchAlias("mus_vtx_z"      );
    produces<float> ("musvtxxError" ).setBranchAlias("mus_vtx_xError" );
    produces<float> ("musvtxyError" ).setBranchAlias("mus_vtx_yError" );
    produces<float> ("musvtxzError" ).setBranchAlias("mus_vtx_zError" );

} // end Constructor

void MuonMaker::beginJob () {}  // method called once each job just before starting event loop
void MuonMaker::endJob   () {}  // method called once each job just after ending the event loop


//////////////
// Producer //
//////////////

void MuonMaker::produce(Event& iEvent, const EventSetup& iSetup) {


    ////////////  
    // Global //
    ////////////

    unique_ptr<vector<float> >         vector_mus_gfit_chi2                   ( new vector<float>         );
    unique_ptr<vector<int> >           vector_mus_gfit_ndof                   ( new vector<int>           );
    unique_ptr<vector<int> >           vector_mus_gfit_validSTAHits           ( new vector<int>           );
    unique_ptr<vector<LorentzVector> > vector_mus_gfit_p4                     ( new vector<LorentzVector> );
    unique_ptr<vector<int> >           vector_mus_gfit_algo                   ( new vector<int>           );
    unique_ptr<vector<float> >         vector_mus_gfit_ptErr                  ( new vector<float>         );

    ////////////  
    // Best   //
    ////////////

    unique_ptr<vector<LorentzVector> > vector_mus_bfit_p4                     ( new vector<LorentzVector> );
    unique_ptr<vector<int> >           vector_mus_bfit_algo                   ( new vector<int>           );
    unique_ptr<vector<float> >         vector_mus_bfit_ptErr                  ( new vector<float>         );

    /////////////
    // Quality //
    /////////////

    unique_ptr<vector<float> >  vector_mus_trkKink             ( new vector<float>  );
    unique_ptr<vector<float> >  vector_mus_chi2LocalPosition   ( new vector<float>  );
    unique_ptr<vector<float> >  vector_mus_chi2LocalMomentum   ( new vector<float>  );

    ///////////
    // Muons //
    ///////////

    unique_ptr<vector<int> >           vector_mus_type                    ( new vector<int>           );        
    unique_ptr<vector<int> >           vector_mus_charge                  ( new vector<int>           );        
    unique_ptr<vector<float> >         vector_mus_caloCompatibility       ( new vector<float>         );
    unique_ptr<vector<float> >         vector_mus_segmCompatibility       ( new vector<float>         );
    unique_ptr<vector<LorentzVector> > vector_mus_p4                      ( new vector<LorentzVector> );
    unique_ptr<vector<int> >           vector_mus_numberOfMatchedStations ( new vector<int>           );

    ////////
    // ID //
    ////////

    unique_ptr<vector<int> >           vector_mus_pid_TMLastStationLoose      ( new vector<int>     );
    unique_ptr<vector<int> >           vector_mus_pid_TMLastStationTight      ( new vector<int>     );
    unique_ptr<vector<int> >           vector_mus_pid_TM2DCompatibilityLoose  ( new vector<int>     );
    unique_ptr<vector<int> >           vector_mus_pid_TM2DCompatibilityTight  ( new vector<int>     );
    unique_ptr<vector<int> >           vector_mus_pid_TMOneStationTight       ( new vector<int>     );
    unique_ptr<vector<int> >           vector_mus_pid_PFMuon                  ( new vector<int>     );

    ////////////
    // Energy //
    ////////////

    unique_ptr<vector<float> >         vector_mus_ecal_time           ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_hcal_time           ( new vector<float>          );

    ///////////////
    // Isolation //
    ///////////////

    unique_ptr<vector<float> >         vector_mus_iso_trckvetoDep     ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_iso_ecalvetoDep     ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_iso_hcalvetoDep     ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_iso_hovetoDep       ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_iso03_sumPt         ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_iso03_emEt          ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_iso03_hadEt         ( new vector<float>          );
    unique_ptr<vector<int> >           vector_mus_iso03_ntrk          ( new vector<int>            );

    ////////////
    // Tracks //
    ////////////

    unique_ptr<vector<LorentzVector> > vector_mus_trk_p4              ( new vector<LorentzVector>  );
    unique_ptr<vector<int> >           vector_mus_validHits           ( new vector<int>            );        
    unique_ptr<vector<int> >           vector_mus_lostHits            ( new vector<int>            );        
    unique_ptr<vector<float> >         vector_mus_d0Err               ( new vector<float>          );      
    unique_ptr<vector<float> >         vector_mus_z0Err               ( new vector<float>          );      
    unique_ptr<vector<float> >         vector_mus_ptErr               ( new vector<float>          );      
    unique_ptr<vector<int> >           vector_mus_algo                ( new vector<int>            );
    unique_ptr<vector<int> >           vector_mus_algoOrig            ( new vector<int>            );
    unique_ptr<vector<int> >           vector_mus_nlayers             ( new vector<int>            );
    unique_ptr<vector<int> >           vector_mus_validPixelHits      ( new vector<int>            );
    unique_ptr<vector<int> >           vector_mus_exp_innerlayers     ( new vector<int>            );
    unique_ptr<vector<int> >           vector_mus_exp_outerlayers     ( new vector<int>            );
    unique_ptr<vector<float> >         vector_mus_dxyPV               ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_dzPV                ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_dz_firstPV          ( new vector<float>          );
    unique_ptr<vector<float> >         vector_mus_dxy_firstPV         ( new vector<float>          );

    ////////
    // PF //
    ////////

    unique_ptr< vector< int> >           vector_mus_pfcharge              ( new vector<int>   );
    unique_ptr< vector< int> >           vector_mus_pfidx                 ( new vector<int>   );
    unique_ptr< vector< int> >           vector_mus_pfparticleId          ( new vector<int>   );
    unique_ptr< vector< LorentzVector> > vector_mus_pfp4                  ( new vector<LorentzVector> );

    unique_ptr< vector<float> >         vector_mus_isoR03_pf_ChargedHadronPt                 ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR03_pf_ChargedParticlePt               ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR03_pf_NeutralHadronEt                 ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR03_pf_PhotonEt                        ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR03_pf_sumNeutralHadronEtHighThreshold ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR03_pf_sumPhotonEtHighThreshold        ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR03_pf_PUPt                            ( new vector<float>   );

    unique_ptr< vector<float> >         vector_mus_isoR04_pf_ChargedHadronPt                 ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR04_pf_ChargedParticlePt               ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR04_pf_NeutralHadronEt                 ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR04_pf_PhotonEt                        ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR04_pf_sumNeutralHadronEtHighThreshold ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR04_pf_sumPhotonEtHighThreshold        ( new vector<float>   );
    unique_ptr< vector<float> >         vector_mus_isoR04_pf_PUPt                            ( new vector<float>   );

    ///////////
    // IP 3D //
    ///////////

    unique_ptr<vector<float> >         vector_mus_ip3d                        ( new vector<float>   );
    unique_ptr<vector<float> >         vector_mus_ip3derr                     ( new vector<float>   );
    unique_ptr<vector<float> >         vector_mus_ip2d                        ( new vector<float>   );
    unique_ptr<vector<float> >         vector_mus_ip2derr                     ( new vector<float>   );

    //////////////////////
    // genMatch miniAOD //
    //////////////////////
    unique_ptr<vector<int>           > mus_mc_patMatch_id ( new vector<int>           );
    unique_ptr<vector<LorentzVector> > mus_mc_patMatch_p4 ( new vector<LorentzVector> );
    unique_ptr<vector<float>         > mus_mc_patMatch_dr ( new vector<float>         );
    unique_ptr<vector<float>         > mus_miniIso_uncor  ( new vector<float>         );   
    unique_ptr<vector<float>         > mus_miniIso_ch     ( new vector<float>         );   
    unique_ptr<vector<float>         > mus_miniIso_nh     ( new vector<float>         );   
    unique_ptr<vector<float>         > mus_miniIso_em     ( new vector<float>         );   
    unique_ptr<vector<float>         > mus_miniIso_db     ( new vector<float>         );   

    unique_ptr<vector<float>   >       mus_miniRelIso_chg                  (new vector<float>        );  	
    unique_ptr<vector<float>   >       mus_miniRelIso_all                  (new vector<float>        );  	

    unique_ptr<vector<unsigned int>         > mus_selectors     ( new vector<unsigned int>         );   
    unique_ptr<vector<int>         > mus_simType     ( new vector<int>         );   
    unique_ptr<vector<int>         > mus_simExtType     ( new vector<int>         );   

    unique_ptr<vector<int>   >       mus_jetNDauChargedMVASel                   (new vector<int>        );
    unique_ptr<vector<float>   >       mus_ptRatio                   (new vector<float>        );
    unique_ptr<vector<float>   >       mus_ptRel                   (new vector<float>        );
    unique_ptr<vector<float>   >       mus_jetBTagCSV                   (new vector<float>        );


    ////////////////////////////
    // --- Fill Muon Data --- //
    ////////////////////////////

    ///////////////
    // Get Muons //
    ///////////////

    Handle<View<pat::Muon> > muon_h;
    iEvent.getByToken( muonsToken , muon_h );

    //////////////////
    // Get Vertices //
    //////////////////

    iEvent.getByToken( vtxToken , vertexHandle );

    ///////////////////////
    // Get PF Candidates //
    ///////////////////////

    iEvent.getByToken(pfCandsToken, packPfCand_h);
    pfCandidates  = packPfCand_h.product();

    // Jets
    Handle<View<pat::Jet> > pfJetsHandle;
    iEvent.getByToken(pfJetsToken, pfJetsHandle);

    // Corrected Isolation using NanoAOD
    edm::Handle<edm::ValueMap<float> > miniIsoChg_values;
    edm::Handle<edm::ValueMap<float> > miniIsoAll_values;
    iEvent.getByToken(miniIsoChgValueMapToken_,miniIsoChg_values);
    iEvent.getByToken(miniIsoAllValueMapToken_,miniIsoAll_values);


    // https://github.com/cms-sw/cmssw/blob/02d4198c0b6615287fd88e9a8ff650aea994412e/HeavyFlavorAnalysis/Onia2MuMu/src/Onia2MuMuPAT.cc
    edm::ESHandle<TransientTrackBuilder> theB;
    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theB);
    KalmanVertexFitter vtxFitter(true);
    std::vector<reco::TransientTrack> t_tks;
  
    ///////////
    // Muons // 
    ///////////
  
    unsigned int muonIndex = 0;
    View<pat::Muon>::const_iterator muons_end = muon_h->end();  // Iterator
    for ( View<pat::Muon>::const_iterator muon = muon_h->begin(); muon != muons_end; ++muon ) {

        // References
        const RefToBase<pat::Muon>    muonRef                 = muon_h->refAt(muonIndex); 
        const TrackRef                globalTrack             = muon->globalTrack();
        const TrackRef                siTrack                 = muon->innerTrack();
        const TrackRef                staTrack                = muon->outerTrack();
        const TrackRef                bestTrack               = muon->muonBestTrack();
        const MuonQuality             quality                 = muon->combinedQuality();
        const VertexCollection*       vertexCollection        = vertexHandle.product();

        const Ptr<pat::Muon> muPtr(muon_h, muon - muon_h->begin() );

        // Iterators
        VertexCollection::const_iterator firstGoodVertex = vertexCollection->end();
        int firstGoodVertexIdx = 0;
        for (VertexCollection::const_iterator vtx = vertexCollection->begin(); vtx != vertexCollection->end(); ++vtx, ++firstGoodVertexIdx) {
            if (  !vtx->isFake() && vtx->ndof()>=4. && vtx->position().Rho()<=2.0 && fabs(vtx->position().Z())<=24.0 ) {
                firstGoodVertex = vtx;
                break;
            }
        }

        if (muon->isMediumMuon() and muon->pt()>=5.) {
            auto trk = muon->track(); // tracker only
            t_tks.push_back(theB->build(trk));
        }


        // float isopt = muon->p4().pt();
        // mus_miniRelIso_chg->push_back((*miniIsoChg_values)[muPtr]/isopt);
        // mus_miniRelIso_all->push_back((*miniIsoAll_values)[muPtr]/isopt);

        mus_selectors->push_back( muon->selectors() ); // DataFormats/MuonReco/interface/Muon.h
        mus_simType->push_back( muon->simType() ); // DataFormats/MuonReco/interface/MuonSimInfo.h
        mus_simExtType->push_back( muon->simExtType() ); // DataFormats/MuonReco/interface/MuonSimInfo.h

        ////////////
        // Global //
        ////////////
        vector_mus_gfit_chi2         -> push_back( globalTrack.isNonnull() ? globalTrack->chi2()                                  : -9999. );
        vector_mus_gfit_ndof         -> push_back( globalTrack.isNonnull() ? globalTrack->ndof()                                  : -9999  );
        vector_mus_gfit_validSTAHits -> push_back( globalTrack.isNonnull() ? globalTrack->hitPattern().numberOfValidMuonHits()    : -9999  );
        vector_mus_gfit_p4           -> push_back( globalTrack.isNonnull() ? LorentzVector( globalTrack->px(), globalTrack->py(), globalTrack->pz(), globalTrack->p() ) : LorentzVector(0.0,0.0,0.0,0.0) );
        vector_mus_gfit_algo            -> push_back( globalTrack.isNonnull() ? globalTrack->algo()                                                                     : -9999  );
        vector_mus_gfit_ptErr           -> push_back( globalTrack.isNonnull() ? globalTrack->ptError()                                                                  : -9999. );

	//////////
        // Best //
        //////////
        vector_mus_bfit_p4           -> push_back( bestTrack.isNonnull() ? LorentzVector( bestTrack->px(), bestTrack->py(), bestTrack->pz(), bestTrack->p() ) : LorentzVector(0.0,0.0,0.0,0.0) );
        vector_mus_bfit_algo            -> push_back( bestTrack.isNonnull() ? bestTrack->algo()                                                                     : -9999. );
        vector_mus_bfit_ptErr           -> push_back( bestTrack.isNonnull() ? bestTrack->ptError()                                                                  : -9999. );

        //////////////////
        // Muon Quality //
        //////////////////
        vector_mus_trkKink             -> push_back( quality.trkKink             );
        vector_mus_chi2LocalPosition   -> push_back( quality.chi2LocalPosition   );
        vector_mus_chi2LocalMomentum   -> push_back( quality.chi2LocalMomentum   );

        //////////
        // Muon //
        //////////
    
        /////////////////////
        // Muon Quantities //
        /////////////////////

        vector_mus_type                    -> push_back( muon->type()                                              );
        vector_mus_charge                  -> push_back( muon->charge()                                            );
        vector_mus_caloCompatibility       -> push_back( muon->caloCompatibility()                                 );
        vector_mus_segmCompatibility       -> push_back( muon::segmentCompatibility(*muon)                         );
        vector_mus_numberOfMatchedStations ->push_back( muon->numberOfMatchedStations()                            );
        vector_mus_p4                      -> push_back( LorentzVector( muon->p4()                              )  );

        ////////
        // ID //
        ////////

        bool matchIsValid = muon->isMatchesValid();

        vector_mus_pid_TMLastStationLoose     -> push_back( matchIsValid ? muon::isGoodMuon( *muon, muon::TMLastStationLoose     ) : -9999  );
        vector_mus_pid_TMLastStationTight     -> push_back( matchIsValid ? muon::isGoodMuon( *muon, muon::TMLastStationTight     ) : -9999  );
        vector_mus_pid_TM2DCompatibilityLoose -> push_back( matchIsValid ? muon::isGoodMuon( *muon, muon::TM2DCompatibilityLoose ) : -9999  );
        vector_mus_pid_TM2DCompatibilityTight -> push_back( matchIsValid ? muon::isGoodMuon( *muon, muon::TM2DCompatibilityTight ) : -9999  );
        vector_mus_pid_TMOneStationTight      -> push_back( matchIsValid ? muon::isGoodMuon( *muon, muon::TMOneStationTight      ) : -9999  );
        vector_mus_pid_PFMuon                 -> push_back( muon->isPFMuon() );

        ////////////
        // Energy //
        ////////////

        bool energyIsValid = muon->isEnergyValid();

        vector_mus_ecal_time       -> push_back( energyIsValid ? muon->calEnergy().ecal_time                     : -9999. );
        vector_mus_hcal_time       -> push_back( energyIsValid ? muon->calEnergy().hcal_time                     : -9999. );

        ///////////////
        // Isolation //
        ///////////////

        vector_mus_iso_trckvetoDep    -> push_back( muon->isEnergyValid()    ? muon->isolationR03().trackerVetoPt  : -9999.        );
        vector_mus_iso_ecalvetoDep    -> push_back( muon->isEnergyValid()    ? muon->isolationR03().emVetoEt       : -9999.        );
        vector_mus_iso_hcalvetoDep    -> push_back( muon->isEnergyValid()    ? muon->isolationR03().hadVetoEt      : -9999.        );
        vector_mus_iso_hovetoDep      -> push_back( muon->isEnergyValid()    ? muon->isolationR03().hoVetoEt       : -9999.        );
        vector_mus_iso03_sumPt        -> push_back( muon->isIsolationValid() ? muon->isolationR03().sumPt          : -9999.        );
        vector_mus_iso03_emEt         -> push_back( muon->isIsolationValid() ? muon->isolationR03().emEt           : -9999.        );
        vector_mus_iso03_hadEt        -> push_back( muon->isIsolationValid() ? muon->isolationR03().hadEt          : -9999.        );
        vector_mus_iso03_ntrk         -> push_back( muon->isIsolationValid() ? muon->isolationR03().nTracks        : -9999         );

        ////////////
        // Tracks //
        ////////////

        vector_mus_trk_p4             -> push_back( siTrack.isNonnull()     ? LorentzVector( siTrack.get()->px() , siTrack.get()->py() , siTrack.get()->pz() , siTrack.get()->p() ) : LorentzVector(     0.0,     0.0,     0.0,     0.0) );
        vector_mus_validHits          -> push_back( siTrack.isNonnull()     ? siTrack->numberOfValidHits()                         : -9999         );
        vector_mus_lostHits           -> push_back( siTrack.isNonnull()     ? siTrack->numberOfLostHits()                          : -9999         );
        vector_mus_d0Err              -> push_back( siTrack.isNonnull()     ? siTrack->d0Error()                                   :  -9999.       );
        vector_mus_z0Err              -> push_back( siTrack.isNonnull()     ? siTrack->dzError()                                   :  -9999.       );
        vector_mus_ptErr              -> push_back( siTrack.isNonnull()     ? siTrack->ptError()                                   :  -9999.       );
        vector_mus_algo               -> push_back( siTrack.isNonnull()     ? siTrack->algo       ()                               : -9999.        );
        vector_mus_algoOrig               -> push_back( siTrack.isNonnull()     ? siTrack->originalAlgo       ()                               : -9999.        );
        vector_mus_nlayers            -> push_back( siTrack.isNonnull()     ? siTrack->hitPattern().trackerLayersWithMeasurement() :  -9999        );
        vector_mus_validPixelHits     -> push_back( siTrack.isNonnull()     ? siTrack->hitPattern().numberOfValidPixelHits()       :  -9999        );
        vector_mus_exp_innerlayers    -> push_back( siTrack.isNonnull()     ? siTrack->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS)   :  -9999        );
        vector_mus_exp_outerlayers    -> push_back( siTrack.isNonnull()     ? siTrack->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_OUTER_HITS)   :  -9999        );
        if (firstGoodVertex!=vertexCollection->end()) { 
            vector_mus_dxyPV        ->push_back( siTrack.isNonnull()     ? siTrack->dxy( firstGoodVertex->position() )           : -9999.        );
            vector_mus_dzPV         ->push_back( siTrack.isNonnull()     ? siTrack->dz(  firstGoodVertex->position() )           : -9999.        );
        }
        else {
            vector_mus_dxyPV       ->push_back( -999. );
            vector_mus_dzPV        ->push_back( -999. );
        }

        vector_mus_dz_firstPV->push_back(siTrack.isNonnull() ? siTrack->dz((vertexCollection->begin())->position()) : -999. );
        vector_mus_dxy_firstPV->push_back(siTrack.isNonnull() ? siTrack->dxy((vertexCollection->begin())->position()) : -999. );

        ////////
        // PF //
        ////////

        // PF Isolation
        MuonPFIsolation pfStructR03 = muon->pfIsolationR03();
        MuonPFIsolation pfStructR04 = muon->pfIsolationR04();

        vector_mus_isoR03_pf_ChargedHadronPt                 -> push_back( pfStructR03.sumChargedHadronPt              );
        vector_mus_isoR03_pf_ChargedParticlePt               -> push_back( pfStructR03.sumChargedParticlePt            );
        vector_mus_isoR03_pf_NeutralHadronEt                 -> push_back( pfStructR03.sumNeutralHadronEt              );
        vector_mus_isoR03_pf_PhotonEt                        -> push_back( pfStructR03.sumPhotonEt                     );
        vector_mus_isoR03_pf_sumNeutralHadronEtHighThreshold -> push_back( pfStructR03.sumNeutralHadronEtHighThreshold );
        vector_mus_isoR03_pf_sumPhotonEtHighThreshold        -> push_back( pfStructR03.sumPhotonEtHighThreshold        );
        vector_mus_isoR03_pf_PUPt                            -> push_back( pfStructR03.sumPUPt                         );

        vector_mus_isoR04_pf_ChargedHadronPt                 -> push_back( pfStructR04.sumChargedHadronPt              );
        vector_mus_isoR04_pf_ChargedParticlePt               -> push_back( pfStructR04.sumChargedParticlePt            );
        vector_mus_isoR04_pf_NeutralHadronEt                 -> push_back( pfStructR04.sumNeutralHadronEt              );
        vector_mus_isoR04_pf_PhotonEt                        -> push_back( pfStructR04.sumPhotonEt                     );
        vector_mus_isoR04_pf_sumNeutralHadronEtHighThreshold -> push_back( pfStructR04.sumNeutralHadronEtHighThreshold );
        vector_mus_isoR04_pf_sumPhotonEtHighThreshold        -> push_back( pfStructR04.sumPhotonEtHighThreshold        );
        vector_mus_isoR04_pf_PUPt                            -> push_back( pfStructR04.sumPUPt                         );

        // Other PF
        reco::CandidatePtr pfCandRef = muon->sourceCandidatePtr(0);

        if(pfCandRef.isNonnull()){
            
            //
            vector_mus_pfp4                  ->push_back( LorentzVector( pfCandRef->p4() )                                        );
            vector_mus_pfcharge              ->push_back( pfCandRef->charge()                                                     );
            vector_mus_pfparticleId          ->push_back( pfCandRef->pdgId()                                                      );
            vector_mus_pfidx                 ->push_back( pfCandRef.key()                                                         );
        }
        else {
            
            //
            vector_mus_pfcharge              ->push_back( -9999.0 );
            vector_mus_pfidx                 ->push_back( -9999.0 );
            vector_mus_pfparticleId          ->push_back( -9999.0 );
        } //

        ///////////
        // IP 3D //
        ///////////

        vector_mus_ip3d         -> push_back( muon->dB(pat::Muon::PV3D) ); 
        vector_mus_ip3derr      -> push_back( muon->edB(pat::Muon::PV3D) );
        vector_mus_ip2d         -> push_back( muon->dB(pat::Muon::PV2D) ); 
        vector_mus_ip2derr      -> push_back( muon->edB(pat::Muon::PV2D) );
 
        //////////////////////
        // genMatch miniAOD //
        //////////////////////
    
        LorentzVector mc_p4(0,0,0,0);	 
        const reco::GenParticle * gen = muon->genParticle();
        if (gen != 0) {
            mc_p4 = gen->p4();
            mus_mc_patMatch_id      ->push_back( gen->pdgId()  );
            mus_mc_patMatch_p4      ->push_back( mc_p4         );
            mus_mc_patMatch_dr      ->push_back( ROOT::Math::VectorUtil::DeltaR(gen->p4(), muon->p4())  );
        }
        else {
            mus_mc_patMatch_id      ->push_back( -999   );
            mus_mc_patMatch_p4      ->push_back( mc_p4  );
            mus_mc_patMatch_dr      ->push_back( -999.  );
        }

        /////////////////////////////
        // LeptonMVA jet daughters //
        /////////////////////////////

        // const auto & pv = (*vertexHandle)[0];
        // auto infotuple = MatchUtilities::getLepMVAInfo(muPtr, pfJetsHandle, pv);
        // float ptRatio = std::get<0>(infotuple);
        // float ptRel = std::get<1>(infotuple);
        // int jetNDauChargedMVASel = std::get<2>(infotuple);
        // float jetBTagCSV = std::get<3>(infotuple);
        // mus_jetNDauChargedMVASel->push_back(jetNDauChargedMVASel);
        // mus_ptRatio->push_back(ptRatio);
        // mus_ptRel->push_back(ptRel);
        // mus_jetBTagCSV->push_back(jetBTagCSV);

        //////////////////////
        // mini-isolation   //
        //////////////////////
    
        // float minichiso     = 0.;
        // float mininhiso     = 0.;
        // float miniemiso     = 0.;
        // float minidbiso     = 0.;
        // muMiniIso(muon, true, 0.5, minichiso, mininhiso, miniemiso, minidbiso);
        // mus_miniIso_uncor   ->push_back( minichiso + mininhiso + miniemiso );
        // mus_miniIso_ch      ->push_back( minichiso );
        // mus_miniIso_nh      ->push_back( mininhiso );
        // mus_miniIso_em      ->push_back( miniemiso );
        // mus_miniIso_db      ->push_back( minidbiso );

        auto mu2 = muon->clone();
        auto miniiso = mu2->miniPFIsolation();
        mus_miniIso_uncor ->push_back(miniiso.chargedHadronIso() + miniiso.neutralHadronIso() + miniiso.photonIso());
        mus_miniIso_ch    ->push_back(miniiso.chargedHadronIso()); 
        mus_miniIso_nh    ->push_back(miniiso.neutralHadronIso()); 
        mus_miniIso_em    ->push_back(miniiso.photonIso()); 
        mus_miniIso_db    ->push_back(miniiso.puChargedHadronIso()); 
        delete mu2;
    
        muonIndex++;

    } // end loop on muons

    unique_ptr<float> vchi2(new float);
    unique_ptr<float> vndof(new float);
    unique_ptr<float> vx(new float);
    unique_ptr<float> vy(new float);
    unique_ptr<float> vz(new float);
    unique_ptr<float> vxe(new float);
    unique_ptr<float> vye(new float);
    unique_ptr<float> vze(new float);
    *vchi2 = -999.;
    *vndof  = -999.;
    *vx = -999.;
    *vy = -999.;
    *vz = -999.;
    *vxe = -999.;
    *vye = -999.;
    *vze = -999.;
    if (t_tks.size() >= 2) {
        TransientVertex vertex = vtxFitter.vertex(t_tks);
        if (vertex.isValid()) {
            *vchi2 = vertex.totalChiSquared();
            *vndof  = vertex.degreesOfFreedom();
            *vx = vertex.position().x();
            *vy = vertex.position().y();
            *vz = vertex.position().z();
            *vxe = vertex.positionError().cxx();
            *vye = vertex.positionError().cyy();
            *vze = vertex.positionError().cyy();
            // std::cout <<  " vx: " << vx <<  " vy: " << vy <<  " vz: " << vz <<  std::endl;
        }
    }

    iEvent.put(std::move(vchi2), branchprefix_+"vtxchi2"   );
    iEvent.put(std::move(vndof), branchprefix_+"vtxndof"   );
    iEvent.put(std::move(vx)   , branchprefix_+"vtxx"      );
    iEvent.put(std::move(vy)   , branchprefix_+"vtxy"      );
    iEvent.put(std::move(vz)   , branchprefix_+"vtxz"      );
    iEvent.put(std::move(vxe)  , branchprefix_+"vtxxError" );
    iEvent.put(std::move(vye)  , branchprefix_+"vtxyError" );
    iEvent.put(std::move(vze)  , branchprefix_+"vtxzError" );

    ////////////                                                                       
    // Global //
    /////////////

    iEvent.put(std::move( vector_mus_gfit_chi2                    ), branchprefix_ + "gfitchi2"           );
    iEvent.put(std::move( vector_mus_gfit_ndof                    ), branchprefix_ + "gfitndof"           );
    iEvent.put(std::move( vector_mus_gfit_validSTAHits            ), branchprefix_ + "gfitvalidSTAHits"   );
    iEvent.put(std::move( vector_mus_gfit_p4                      ), branchprefix_ + "gfitp4"             );
    iEvent.put(std::move( vector_mus_gfit_algo             	      ), branchprefix_ + "gfitalgo"            );
    iEvent.put(std::move( vector_mus_gfit_ptErr           	      ), branchprefix_ + "gfitptErr"           );

    ////////////                                                                       
    // Best   //
    ////////////

    iEvent.put(std::move( vector_mus_bfit_p4                      ), branchprefix_ + "bfitp4"             );
    iEvent.put(std::move( vector_mus_bfit_algo             	      ), branchprefix_ + "bfitalgo"            );
    iEvent.put(std::move( vector_mus_bfit_ptErr           	      ), branchprefix_ + "bfitptErr"           );

    //////////////////
    // Muon Quality //
    //////////////////

    iEvent.put(std::move( vector_mus_trkKink            ), branchprefix_ + "trkKink"            );
    iEvent.put(std::move( vector_mus_chi2LocalPosition  ), branchprefix_ + "chi2LocalPosition"  );
    iEvent.put(std::move( vector_mus_chi2LocalMomentum  ), branchprefix_ + "chi2LocalMomentum"  );

    ///////////
    // Muons //
    ///////////

    iEvent.put(std::move( vector_mus_type                    ), branchprefix_ + "type"                    );
    iEvent.put(std::move( vector_mus_charge                  ), branchprefix_ + "charge"                  );
    iEvent.put(std::move( vector_mus_caloCompatibility       ), branchprefix_ + "caloCompatibility"       );
    iEvent.put(std::move( vector_mus_segmCompatibility       ), branchprefix_ + "segmCompatibility"       );
    iEvent.put(std::move( vector_mus_p4                      ), branchprefix_ + "p4"                      );
    iEvent.put(std::move( vector_mus_numberOfMatchedStations ), branchprefix_ + "numberOfMatchedStations" );

    ////////
    // ID //
    ////////

    iEvent.put(std::move( vector_mus_pid_TMLastStationLoose       ), branchprefix_ + "pidTMLastStationLoose"    );
    iEvent.put(std::move( vector_mus_pid_TMLastStationTight       ), branchprefix_ + "pidTMLastStationTight"    );
    iEvent.put(std::move( vector_mus_pid_TM2DCompatibilityLoose   ), branchprefix_ + "pidTM2DCompatibilityLoose");
    iEvent.put(std::move( vector_mus_pid_TM2DCompatibilityTight   ), branchprefix_ + "pidTM2DCompatibilityTight");
    iEvent.put(std::move( vector_mus_pid_TMOneStationTight        ), branchprefix_ + "pidTMOneStationTight");
    iEvent.put(std::move( vector_mus_pid_PFMuon                   ), branchprefix_ + "pidPFMuon");

    ////////////
    // Energy //
    ////////////

    iEvent.put(std::move( vector_mus_ecal_time          ), branchprefix_ + "ecaltime"           );
    iEvent.put(std::move( vector_mus_hcal_time          ), branchprefix_ + "hcaltime"           );
  
    ///////////////
    // Isolation //
    ///////////////

    iEvent.put(std::move( vector_mus_iso_trckvetoDep    ), branchprefix_ + "isotrckvetoDep"     );
    iEvent.put(std::move( vector_mus_iso_ecalvetoDep    ), branchprefix_ + "isoecalvetoDep"     );
    iEvent.put(std::move( vector_mus_iso_hcalvetoDep    ), branchprefix_ + "isohcalvetoDep"     );
    iEvent.put(std::move( vector_mus_iso_hovetoDep      ), branchprefix_ + "isohovetoDep"       );
    iEvent.put(std::move( vector_mus_iso03_sumPt        ), branchprefix_ + "iso03sumPt"         );
    iEvent.put(std::move( vector_mus_iso03_emEt         ), branchprefix_ + "iso03emEt"          );
    iEvent.put(std::move( vector_mus_iso03_hadEt        ), branchprefix_ + "iso03hadEt"         );
    iEvent.put(std::move( vector_mus_iso03_ntrk         ), branchprefix_ + "iso03ntrk"          );

    iEvent.put(std::move(mus_miniRelIso_chg       ), "musminiRelIsochg"    );
    iEvent.put(std::move(mus_miniRelIso_all       ), "musminiRelIsoall"    );

    ////////////
    // Tracks //
    ////////////

    iEvent.put(std::move( vector_mus_trk_p4             ), branchprefix_ + "trkp4"              );
    iEvent.put(std::move( vector_mus_validHits          ), branchprefix_ + "validHits"          );
    iEvent.put(std::move( vector_mus_lostHits           ), branchprefix_ + "lostHits"           );
    iEvent.put(std::move( vector_mus_d0Err              ), branchprefix_ + "d0Err"              );
    iEvent.put(std::move( vector_mus_z0Err              ), branchprefix_ + "z0Err"              );
    iEvent.put(std::move( vector_mus_ptErr              ), branchprefix_ + "ptErr"              );
    iEvent.put(std::move( vector_mus_algo               ), branchprefix_ + "algo"           	   );
    iEvent.put(std::move( vector_mus_algoOrig           ), branchprefix_ + "algoOrig"           	   );
    iEvent.put(std::move( vector_mus_nlayers            ), branchprefix_ + "nlayers"        	   );
    iEvent.put(std::move( vector_mus_validPixelHits     ), branchprefix_ + "validPixelHits" 	   );
    iEvent.put(std::move( vector_mus_exp_innerlayers    ), branchprefix_ + "expinnerlayers"	   );
    iEvent.put(std::move( vector_mus_exp_outerlayers    ), branchprefix_ + "expouterlayers"     );
    iEvent.put(std::move( vector_mus_dxyPV              ), branchprefix_ + "dxyPV"      	   );
    iEvent.put(std::move( vector_mus_dzPV               ), branchprefix_ + "dzPV"      	   );
    iEvent.put(std::move( vector_mus_dz_firstPV         ), branchprefix_ +     "dzfirstPV"     );
    iEvent.put(std::move( vector_mus_dxy_firstPV        ), branchprefix_ +   "dxyfirstPV"    );

    
  
    ////////                                  
    // PF //
    ////////

    iEvent.put(std::move( vector_mus_pfcharge              ), branchprefix_ + "pfcharge"             );
    iEvent.put(std::move( vector_mus_pfidx                 ), branchprefix_ + "pfidx"            );
    iEvent.put(std::move( vector_mus_pfparticleId          ), branchprefix_ + "pfparticleId"         );
    iEvent.put(std::move( vector_mus_pfp4                  ), branchprefix_ + "pfp4"                 );

    iEvent.put(std::move( vector_mus_isoR03_pf_ChargedHadronPt                ), branchprefix_ + "isoR03pfChargedHadronPt"             );
    iEvent.put(std::move( vector_mus_isoR03_pf_ChargedParticlePt              ), branchprefix_ + "isoR03pfChargedParticlePt"           );
    iEvent.put(std::move( vector_mus_isoR03_pf_NeutralHadronEt                ), branchprefix_ + "isoR03pfNeutralHadronEt"             );
    iEvent.put(std::move( vector_mus_isoR03_pf_PhotonEt                       ), branchprefix_ + "isoR03pfPhotonEt"                    );
    iEvent.put(std::move( vector_mus_isoR03_pf_sumNeutralHadronEtHighThreshold), branchprefix_ + "isoR03pfNeutralHadronEtHighThreshold");
    iEvent.put(std::move( vector_mus_isoR03_pf_sumPhotonEtHighThreshold       ), branchprefix_ + "isoR03pfPhotonEtHighThreshold"       );
    iEvent.put(std::move( vector_mus_isoR03_pf_PUPt                           ), branchprefix_ + "isoR03pfPUPt"                        );

    iEvent.put(std::move( vector_mus_isoR04_pf_ChargedHadronPt                ), branchprefix_ + "isoR04pfChargedHadronPt"             );
    iEvent.put(std::move( vector_mus_isoR04_pf_ChargedParticlePt              ), branchprefix_ + "isoR04pfChargedParticlePt"           );
    iEvent.put(std::move( vector_mus_isoR04_pf_NeutralHadronEt                ), branchprefix_ + "isoR04pfNeutralHadronEt"             );
    iEvent.put(std::move( vector_mus_isoR04_pf_PhotonEt                       ), branchprefix_ + "isoR04pfPhotonEt"                    );
    iEvent.put(std::move( vector_mus_isoR04_pf_sumNeutralHadronEtHighThreshold), branchprefix_ + "isoR04pfNeutralHadronEtHighThreshold");
    iEvent.put(std::move( vector_mus_isoR04_pf_sumPhotonEtHighThreshold       ), branchprefix_ + "isoR04pfPhotonEtHighThreshold"       );
    iEvent.put(std::move( vector_mus_isoR04_pf_PUPt                           ), branchprefix_ + "isoR04pfPUPt"                        );

    ///////////
    // IP 3D //
    ///////////

    iEvent.put(std::move( vector_mus_ip3d                         ), branchprefix_ + "ip3d"               );
    iEvent.put(std::move( vector_mus_ip3derr                      ), branchprefix_ + "ip3derr"            );
    iEvent.put(std::move( vector_mus_ip2d                         ), branchprefix_ + "ip2d"               );
    iEvent.put(std::move( vector_mus_ip2derr                      ), branchprefix_ + "ip2derr"            );

    // genParticle matching from miniAOD
    iEvent.put(std::move( mus_mc_patMatch_id          		),"musmcpatMatchid"          	);
    iEvent.put(std::move( mus_mc_patMatch_p4           		),"musmcpatMatchp4"          	);
    iEvent.put(std::move( mus_mc_patMatch_dr          		),"musmcpatMatchdr"          	);

    iEvent.put(std::move(mus_miniIso_uncor       ), branchprefix_ + "miniIsouncor"    );
    iEvent.put(std::move(mus_miniIso_ch       ), "musminiIsoch"    );
    iEvent.put(std::move(mus_miniIso_nh       ), "musminiIsonh"    );
    iEvent.put(std::move(mus_miniIso_em       ), "musminiIsoem"    );
    iEvent.put(std::move(mus_miniIso_db       ), "musminiIsodb"    );

    iEvent.put(std::move(mus_selectors       ), "musselectors"    );
    iEvent.put(std::move(mus_simType       ), "mussimType"    );
    iEvent.put(std::move(mus_simExtType       ), "mussimExtType"    );

    iEvent.put(std::move(mus_jetNDauChargedMVASel        ), "musjetNDauChargedMVASel"       ); 
    iEvent.put(std::move(mus_ptRatio        ), "musptRatio"       ); 
    iEvent.put(std::move(mus_ptRel        ), "musptRel"       ); 
    iEvent.put(std::move(mus_jetBTagCSV        ), "musjetBTagCSV"       ); 

} //


double MuonMaker::muonIsoValuePF(const Muon& mu, const Vertex& vtx, float coner, float minptn, float dzcut, int filterId){

    float pfciso = 0;
    float pfniso = 0;

    const TrackRef siTrack  = mu.innerTrack();

    float mudz = siTrack.isNonnull() ? siTrack->dz(vtx.position()) : mu.standAloneMuon()->dz(vtx.position());

    for (PFCandidateCollection::const_iterator pf=pfCand_h->begin(); pf<pfCand_h->end(); ++pf){

        float dR = deltaR(pf->eta(), pf->phi(), mu.eta(), mu.phi());
        if (dR>coner) continue;

        int pfid = abs(pf->pdgId());
        if (filterId!=0 && filterId!=pfid) continue;

        float pfpt = pf->pt();
        if (pf->charge()==0) {
            //neutrals
            if (pfpt>minptn) pfniso+=pfpt;
        } else {
            //charged
            //avoid double counting of muon itself
            const TrackRef pfTrack  = pf->trackRef();
            if (siTrack.isNonnull()  && pfTrack.isNonnull() && siTrack.key()==pfTrack.key()) continue;
            //first check electrons with gsf track
            if (abs(pf->pdgId())==11 && pf->gsfTrackRef().isNonnull()) {
                if(fabs(pf->gsfTrackRef()->dz(vtx.position()) - mudz )<dzcut) {//dz cut
                    pfciso+=pfpt;
                }
                continue;//and avoid double counting
            }
            //then check anything that has a ctf track
            if (pfTrack.isNonnull()) {//charged (with a ctf track)
                if(fabs( pfTrack->dz(vtx.position()) - mudz )<dzcut) {//dz cut
                    pfciso+=pfpt;
                }
            } 
        }
    } 
    return pfciso+pfniso;
}
void MuonMaker::muIsoCustomCone( edm::View<pat::Muon>::const_iterator& mu, float dr, bool useVetoCones, float ptthresh, float &chiso, float &nhiso, float &emiso, float & dbiso){
    chiso     = 0.;
    nhiso     = 0.;
    emiso     = 0.;
    dbiso     = 0.;
    float deadcone_ch = 0.0001;
    float deadcone_pu = 0.01;
    float deadcone_ph = 0.01;
    float deadcone_nh = 0.01;

  double phi = mu->p4().Phi();
  double eta = mu->p4().Eta();
  double pi = M_PI;

    for( pat::PackedCandidateCollection::const_iterator pf_it = pfCandidates->begin(); pf_it != pfCandidates->end(); pf_it++ ) {
        float id = pf_it->pdgId();
        if (fabs(id) != 211 && fabs(id) != 130 && fabs(id) != 22) continue;

        double deltaPhi = phi-pf_it->p4().Phi();
        if ( deltaPhi > pi ) deltaPhi -= 2.0*pi;
        else if ( deltaPhi <= -pi ) deltaPhi += 2.0*pi;
        deltaPhi = fabs(deltaPhi);
        if (deltaPhi > dr) continue;
        double deltaEta = fabs(pf_it->p4().Eta()-eta);
        if (deltaEta > dr) continue;
        double thisDR = sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);

        if ( thisDR>dr ) continue;  
        float pt = pf_it->p4().pt();
        if ( fabs(id)==211 ) {
            if (pf_it->fromPV() > 1 && (!useVetoCones || thisDR > deadcone_ch) ) chiso+=pt;
            else if ((pf_it->fromPV() <= 1) && (pt > ptthresh) && (!useVetoCones || thisDR > deadcone_pu)) dbiso+=pt;
        }
        if ( fabs(id)==130 && (pt > ptthresh) && (!useVetoCones || thisDR > deadcone_nh) ) nhiso+=pt;
        if ( fabs(id)==22 && (pt > ptthresh) && (!useVetoCones || thisDR > deadcone_ph) ) emiso+=pt;
    }
    return;
}

void MuonMaker::muMiniIso( edm::View<pat::Muon>::const_iterator& mu, bool useVetoCones, float ptthresh, float &chiso, float &nhiso, float &emiso, float & dbiso){

    float pt = mu->p4().pt();
    float dr = 0.2;
    if (pt>50) dr = 10./pt;
    if (pt>200) dr = 0.05;
    muIsoCustomCone(mu,dr,useVetoCones,ptthresh, chiso, nhiso, emiso, dbiso);
    return;
}


//define this as a plug-in
DEFINE_FWK_MODULE(MuonMaker);
