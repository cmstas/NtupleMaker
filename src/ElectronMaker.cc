//-*- C++ -*- 
//
// Package:    ElectronMaker
// Class:      ElectronMaker
//
// Original Author:  Puneeth Kalavase
//         Created:  Fri Jun  6 11:07:38 CDT 2008
// $Id: ElectronMaker.cc,v 1.89 2012/08/16 00:00:27 slava77 Exp $


//System include files
#include <memory>
#include <math.h>

//User include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "CMS3/NtupleMaker/interface/ElectronMaker.h"
#include "CMS3/NtupleMaker/interface/MatchUtilities.h"
#include "CMS3/NtupleMaker/interface/MCUtilities.h"
#include "CMS3/NtupleMaker/interface/EgammaFiduciality.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"
#include "DataFormats/EgammaReco/interface/ClusterShapeFwd.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit1D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "Math/VectorUtil.h"
#include "TVector2.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/TrackingGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"

#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "RecoEgamma/EgammaTools/interface/EcalClusterLocal.h"
#include "RecoEcal/EgammaCoreTools/interface/Mustache.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "EgammaAnalysis/ElectronTools/interface/EGammaCutBasedEleId.h"

using namespace reco;
using namespace edm;
using namespace std;

typedef math::XYZPoint Point;
typedef Ref<edmNew::DetSetVector<SiStripCluster>,SiStripCluster > ClusterRef;
typedef Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster > pixel_ClusterRef;

// constructors and destructor
ElectronMaker::ElectronMaker(const ParameterSet& iConfig) {

    //get setup parameters
    electronVetoIdMapToken_   = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronVetoIdMap"));
    electronLooseIdMapToken_  = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronLooseIdMap"));
    electronMediumIdMapToken_ = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronMediumIdMap"));
    electronTightIdMapToken_  = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronTightIdMap"));
    electronHEEPIdMapToken_                = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronHEEPIdMap"));
    electronVIDNonTrigMvaWP80IdMapToken_   = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronVIDNonTrigMvaWP80IdMap"));
    electronVIDNonTrigMvaWP90IdMapToken_   = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronVIDNonTrigMvaWP90IdMap"));
    electronVIDTrigMvaWP80IdMapToken_      = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronVIDTrigMvaWP80IdMap"));
    electronVIDTrigMvaWP90IdMapToken_      = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("electronVIDTrigMvaWP90IdMap"));   
    electronVIDNonTrigMvaValueMapToken_    = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDNonTrigMvaValueMap"));
    electronVIDNonTrigMvaCatMapToken_      = consumes<edm::ValueMap<int> >(iConfig.getParameter<edm::InputTag>("electronVIDNonTrigMvaCatMap"));
    electronVIDTrigMvaValueMapToken_       = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDTrigMvaValueMap"));
    electronVIDTrigMvaCatMapToken_         = consumes<edm::ValueMap<int> >(iConfig.getParameter<edm::InputTag>("electronVIDTrigMvaCatMap"));

    electronVIDSpring16GPMvaValueMapToken_  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDSpring16GPMvaValueMap"));
    electronVIDSpring16GPMvaCatMapToken_    = consumes<edm::ValueMap<int>   >(iConfig.getParameter<edm::InputTag>("electronVIDSpring16GPMvaCatMap"));
    electronVIDSpring16HZZMvaValueMapToken_  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDSpring16HZZMvaValueMap"));
    electronVIDSpring16HZZMvaCatMapToken_    = consumes<edm::ValueMap<int>   >(iConfig.getParameter<edm::InputTag>("electronVIDSpring16HZZMvaCatMap"));

    electronVIDFall17NoIsoMvaValueMapToken_  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDFall17NoIsoMvaValueMap"));
    electronVIDFall17NoIsoMvaCatMapToken_    = consumes<edm::ValueMap<int>   >(iConfig.getParameter<edm::InputTag>("electronVIDFall17NoIsoMvaCatMap"));
    electronVIDFall17IsoMvaValueMapToken_  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDFall17IsoMvaValueMap"));
    electronVIDFall17IsoMvaCatMapToken_    = consumes<edm::ValueMap<int>   >(iConfig.getParameter<edm::InputTag>("electronVIDFall17IsoMvaCatMap"));

    electronVIDFall17V2NoIsoMvaValueMapToken_  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDFall17V2NoIsoMvaValueMap"));
    electronVIDFall17V2NoIsoMvaCatMapToken_    = consumes<edm::ValueMap<int>   >(iConfig.getParameter<edm::InputTag>("electronVIDFall17V2NoIsoMvaCatMap"));
    electronVIDFall17V2IsoMvaValueMapToken_  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("electronVIDFall17V2IsoMvaValueMap"));
    electronVIDFall17V2IsoMvaCatMapToken_    = consumes<edm::ValueMap<int>   >(iConfig.getParameter<edm::InputTag>("electronVIDFall17V2IsoMvaCatMap"));

    electronsToken  = consumes<edm::View<pat::Electron>  >(iConfig.getParameter<edm::InputTag>("electronsInputTag"));
    vtxToken  = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vtxInputTag"));
    pfCandsToken  = consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCandsInputTag"));
    pfJetsToken = consumes<edm::View<pat::Jet> >(iConfig.getParameter<edm::InputTag>("pfJetsInputTag"));
    bFieldToken  = consumes<float>(iConfig.getParameter<edm::InputTag>("bFieldInputTag"));
    beamSpotToken  = consumes<LorentzVector>(iConfig.getParameter<edm::InputTag>("beamSpotInputTag"));
    trksInputTag_                = iConfig.getParameter<edm::InputTag> ("trksInputTag"                 );
    gsftracksInputTag_           = iConfig.getParameter<edm::InputTag> ("gsftracksInputTag"            );
    cms2scsseeddetidInputTag_    = iConfig.getParameter<edm::InputTag> ("cms2scsseeddetidInputTag"     );
    eidLHTag_                    = iConfig.getParameter<edm::InputTag> ("eidLHTag"                     );
    ebReducedRecHitCollectionTag = iConfig.getParameter<edm::InputTag> ("ebReducedRecHitCollectionTag" );
    eeReducedRecHitCollectionTag = iConfig.getParameter<edm::InputTag> ("eeReducedRecHitCollectionTag" );
    esReducedRecHitCollectionTag = iConfig.getParameter<edm::InputTag> ("esReducedRecHitCollectionTag" );
    // pfIsoCharged03InputTag    = iConfig.getParameter<edm::InputTag> ("pfIsoCharged03InputTag"   );
    // pfIsoGamma03InputTag      = iConfig.getParameter<edm::InputTag> ("pfIsoGamma03InputTag"     );
    // pfIsoNeutral03InputTag    = iConfig.getParameter<edm::InputTag> ("pfIsoNeutral03InputTag"   );
    // pfIsoCharged04InputTag    = iConfig.getParameter<edm::InputTag> ("pfIsoCharged04InputTag"   );
    // pfIsoGamma04InputTag      = iConfig.getParameter<edm::InputTag> ("pfIsoGamma04InputTag"     );
    // pfIsoNeutral04InputTag    = iConfig.getParameter<edm::InputTag> ("pfIsoNeutral04InputTag"   );
    
    miniIsoChgValueMapToken_   = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("miniIsoChgValueMap"));
    miniIsoAllValueMapToken_   = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("miniIsoAllValueMap"));

    ebReducedRecHitCollection = mayConsume<EcalRecHitCollection>(ebReducedRecHitCollectionTag);
    eeReducedRecHitCollection = mayConsume<EcalRecHitCollection>(eeReducedRecHitCollectionTag);
    esReducedRecHitCollection = mayConsume<EcalRecHitCollection>(esReducedRecHitCollectionTag);

    recoConversionToken = consumes<reco::ConversionCollection>(iConfig.getParameter<edm::InputTag>("recoConversionInputTag"));
    rhoInputTag_              = iConfig.getParameter<edm::InputTag> ("rhoInputTag"              );
    beamSpot_tag_             = iConfig.getParameter<edm::InputTag> ("beamSpotTag"              );

    minAbsDist_               = iConfig.getParameter<double>          ("minAbsDist"              );
    minAbsDcot_               = iConfig.getParameter<double>          ("minAbsDcot"              );
    minSharedFractionOfHits_  = iConfig.getParameter<double>          ("minSharedFractionOfHits" );
    aliasprefix_              = iConfig.getUntrackedParameter<string> ("aliasPrefix"             );

 

    produces<unsigned int>       ("evtnels"                    ).setBranchAlias("evt_nels"                   ); //number of electrons in event

    // ECAL related (superCluster) variables
    produces<vector<float> >     ("elseSC"                     ).setBranchAlias("els_eSC"                    );
    produces<vector<float> >     ("elsetaSC"                   ).setBranchAlias("els_etaSC"                  );
    produces<vector<float> >     ("elsphiSC"                   ).setBranchAlias("els_phiSC"                  );
    produces<vector<float> >     ("elseSCRaw"                  ).setBranchAlias("els_eSCRaw"                 );
    produces<vector<float> >     ("elseSCPresh"                ).setBranchAlias("els_eSCPresh"               );
    produces<vector<float> >     ("elsetaSCwidth"              ).setBranchAlias("els_etaSCwidth"             );
    produces<vector<float> >     ("elsphiSCwidth"              ).setBranchAlias("els_phiSCwidth"             );

    // Corrections and uncertainties
    //
    produces<vector<float> >     ("elsecalEnergy"              ).setBranchAlias("els_ecalEnergy"             );
    produces<vector<float> >     ("elsecalEnergyError"         ).setBranchAlias("els_ecalEnergyError"        );
    produces<vector<float> >     ("elstrackMomentumError"      ).setBranchAlias("els_trackMomentumError"     );

    // ID variables
    //

    produces<vector<float> >     ("elsdEtaIn"                  ).setBranchAlias("els_dEtaIn"                 );
    produces<vector<float> >     ("elsdEtaOut"                 ).setBranchAlias("els_dEtaOut"                );
    produces<vector<float> >     ("elsdPhiIn"                  ).setBranchAlias("els_dPhiIn"                 );
    produces<vector<float> >     ("elsdPhiOut"                 ).setBranchAlias("els_dPhiOut"                );
    produces<vector<float> >     ("elsfbrem"                   ).setBranchAlias("els_fbrem"                  );
    produces<vector<float> >     ("elseOverPIn"                ).setBranchAlias("els_eOverPIn"               );
    produces<vector<float> >     ("elseOverPOut"               ).setBranchAlias("els_eOverPOut"              );

    produces<vector<float> >     ("elshOverE"                  ).setBranchAlias("els_hOverE"                 );
    produces<vector<float> >     ("elsfull5x5hOverE"           ).setBranchAlias("els_full5x5_hOverE"                 );

    produces<vector<float> >     ("elssigmaIPhiIPhi"           ).setBranchAlias("els_sigmaIPhiIPhi"          );
    produces<vector<float> >     ("elssigmaIEtaIEta"           ).setBranchAlias("els_sigmaIEtaIEta"          );


    produces<vector<bool > >     ("elsisGsfCtfScPixChargeConsistent").setBranchAlias("els_isGsfCtfScPixChargeConsistent");

    produces<vector<int> >       ("elstype"                    ).setBranchAlias("els_type"                   );
    produces<vector<float> >     ("elseSeed"                   ).setBranchAlias("els_eSeed"                  );

    // predefined ID decisions
    // http://cmslxr.fnal.gov/lxr/source/DataFormats/EgammaCandidates/interface/GsfElectron.h

    // Spring 15 predefined ID decisions
    // produces<vector<int> >       ("passVetoId"                 ).setBranchAlias("els_passVetoId"                 );
    // produces<vector<int> >       ("passLooseId"                ).setBranchAlias("els_passLooseId"                );
    // produces<vector<int> >       ("passMediumId"               ).setBranchAlias("els_passMediumId"               );
    // produces<vector<int> >       ("passTightId"                ).setBranchAlias("els_passTightId"                );
    // produces<vector<int> >       ("passHEEPId"                 ).setBranchAlias("els_passHEEPId"                 );
    produces<vector<int> >       ("passVIDNonTrigMvaWP80Id"    ).setBranchAlias("els_passVIDNonTrigMvaWP80Id"    );
    produces<vector<int> >       ("passVIDNonTrigMvaWP90Id"    ).setBranchAlias("els_passVIDNonTrigMvaWP90Id"    );
    produces<vector<int> >       ("passVIDTrigMvaWP80Id"       ).setBranchAlias("els_passVIDTrigMvaWP80Id"       );
    produces<vector<int> >       ("passVIDTrigMvaWP90Id"       ).setBranchAlias("els_passVIDTrigMvaWP90Id"       );
    produces<vector<float> >     ("VIDNonTrigMvaValue"         ).setBranchAlias("els_VIDNonTrigMvaValue"         );
    produces<vector<float> >     ("VIDTrigMvaValue"            ).setBranchAlias("els_VIDTrigMvaValue"            );
    produces<vector<int> >       ("VIDNonTrigMvaCat"           ).setBranchAlias("els_VIDNonTrigMvaCat"           );
    produces<vector<int> >       ("VIDTrigMvaCat"              ).setBranchAlias("els_VIDTrigMvaCat"              );
    produces<vector<float> >     ("VIDSpring16GPMvaValue"      ).setBranchAlias("els_VIDSpring16GPMvaValue"         );
    produces<vector<int> >       ("VIDSpring16GPMvaCat"        ).setBranchAlias("els_VIDSpring16GPMvaCat"           );
    produces<vector<float> >     ("VIDSpring16HZZMvaValue"     ).setBranchAlias("els_VIDSpring16HZZMvaValue"         );
    produces<vector<int> >       ("VIDSpring16HZZMvaCat"       ).setBranchAlias("els_VIDSpring16HZZMvaCat"           );
    produces<vector<float> >     ("VIDFall17NoIsoMvaValue"     ).setBranchAlias("els_VIDFall17NoIsoMvaValue"         );
    produces<vector<int> >       ("VIDFall17NoIsoMvaCat"       ).setBranchAlias("els_VIDFall17NoIsoMvaCat"           );
    produces<vector<float> >     ("VIDFall17IsoMvaValue"     ).setBranchAlias("els_VIDFall17IsoMvaValue"         );
    produces<vector<int> >       ("VIDFall17IsoMvaCat"       ).setBranchAlias("els_VIDFall17IsoMvaCat"           );
    produces<vector<float> >     ("VIDFall17V2NoIsoMvaValue"     ).setBranchAlias("els_VIDFall17V2NoIsoMvaValue"         );
    produces<vector<int> >       ("VIDFall17V2NoIsoMvaCat"       ).setBranchAlias("els_VIDFall17V2NoIsoMvaCat"           );
    produces<vector<float> >     ("VIDFall17V2IsoMvaValue"     ).setBranchAlias("els_VIDFall17V2IsoMvaValue"         );
    produces<vector<int> >       ("VIDFall17V2IsoMvaCat"       ).setBranchAlias("els_VIDFall17V2IsoMvaCat"           );

    // for the ID definitions, see https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideElectronID
    // the decisions should be the SAME as the els_pat_*id branches made by PATElectronMaker
    produces<vector<int> >       ("elscategory"                ).setBranchAlias("els_category"               );

    // isolation variables
    //
    produces<vector<float> >     ("elshcalDepth1TowerSumEt"   ).setBranchAlias("els_hcalDepth1TowerSumEt"   );
    produces<vector<float> >     ("elstkIso"                  ).setBranchAlias("els_tkIso"                  );
    produces<vector<float> >     ("elsecalIso"                ).setBranchAlias("els_ecalIso"                );
    produces<vector<float> >     ("elshcalIso"                ).setBranchAlias("els_hcalIso"                );

    produces<vector<float> >     ("elstkIso04"                ).setBranchAlias("els_tkIso04"                );
    produces<vector<float> >     ("elsecalIso04"              ).setBranchAlias("els_ecalIso04"              );
    produces<vector<float> >     ("elshcalIso04"              ).setBranchAlias("els_hcalIso04"              );



    // pf isolation variables
    produces<vector<float> >     ("elspfChargedHadronIso").setBranchAlias("els_pfChargedHadronIso");
    produces<vector<float> >     ("elspfNeutralHadronIso").setBranchAlias("els_pfNeutralHadronIso");
    produces<vector<float> >     ("elspfPhotonIso"       ).setBranchAlias("els_pfPhotonIso"       );
    produces<vector<float> >     ("elspfPUIso"           ).setBranchAlias("els_pfPUIso"           );


    // track variables
    //
    produces<vector<int> >       ("elscharge"        ).setBranchAlias("els_charge"         ); //candidate charge
    produces<vector<int> >       ("elssccharge"      ).setBranchAlias("els_sccharge"       );
    produces<vector<int> >       ("elstrkcharge"     ).setBranchAlias("els_trk_charge"     );
    produces<vector<float> >     ("elschi2"          ).setBranchAlias("els_chi2"           );
    produces<vector<float> >     ("elsndof"          ).setBranchAlias("els_ndof"           );
    produces<vector<float> >     ("elsckfchi2"       ).setBranchAlias("els_ckf_chi2"       );
    produces<vector<float> >     ("elsckfndof"       ).setBranchAlias("els_ckf_ndof"       );
    produces<vector<int> >       ("elsvalidHits"     ).setBranchAlias("els_validHits"      ); //number of used hits in fit
    produces<vector<int> >       ("elslostHits"      ).setBranchAlias("els_lostHits"       ); //number of lost hits in fit
    produces<vector<float> >     ("elsdxyPV"         ).setBranchAlias("els_dxyPV"          );
    produces<vector<float> >     ("elsdzPV"          ).setBranchAlias("els_dzPV"           );
    produces<vector<float> >     ("elsd0Err"         ).setBranchAlias("els_d0Err"          );
    produces<vector<float> >     ("elsz0Err"         ).setBranchAlias("els_z0Err"          );
    produces<vector<float> >     ("elsptErr"         ).setBranchAlias("els_ptErr"          );
    produces<vector<float> >     ("elsptErrGsf"      ).setBranchAlias("els_ptErrGsf"       );
    produces<vector<float> >     ("elsip3d"          ).setBranchAlias("els_ip3d"           ); // Ip3d from normal vertex
    produces<vector<float> >     ("elsip3derr"       ).setBranchAlias("els_ip3derr"        ); // Ip3d error from normal vertex
    produces<vector<float> >     ("elsip2d"          ).setBranchAlias("els_ip2d"           ); // Ip2d from normal vertex
    produces<vector<float> >     ("elsip2derr"       ).setBranchAlias("els_ip2derr"        ); // Ip2d error from normal vertex
    produces<vector<int> >       ("elsckflaywithmeas").setBranchAlias("els_ckf_laywithmeas");
    produces<vector<int> >       ("elsckfcharge"     ).setBranchAlias("els_ckf_charge"     );

    // LorentzVectors
    //
    produces<vector<LorentzVector> >  ("elsp4"    ).setBranchAlias("els_p4"     );
    produces<vector<LorentzVector> >  ("elstrkp4" ).setBranchAlias("els_trk_p4" );
    produces<vector<LorentzVector> >  ("elsp4In"  ).setBranchAlias("els_p4In"   );
    produces<vector<LorentzVector> >  ("elsp4Out" ).setBranchAlias("els_p4Out"  );

    // Vertex
    //
    produces<vector<LorentzVector> >  ("elsvertexp4").setBranchAlias("els_vertex_p4");
    produces<vector<LorentzVector> >  ("elstrkvertexp4").setBranchAlias("els_trk_vertex_p4");

    //Hit Pattern information
    //
    produces<vector<int> >            ("elsvalidpixelhits" ).setBranchAlias("els_valid_pixelhits" );
    produces<vector<int> >            ("elslostpixelhits"  ).setBranchAlias("els_lost_pixelhits"  );
    produces<vector<int> >            ("elsnlayers"        ).setBranchAlias("els_nlayers"         );
    produces<vector<int> >            ("elsnlayers3D"      ).setBranchAlias("els_nlayers3D"       );
    produces<vector<int> >            ("elsnlayersLost"    ).setBranchAlias("els_nlayersLost"     );
    produces<vector<int> >            ("elsexpinnerlayers" ).setBranchAlias("els_exp_innerlayers" );
    produces<vector<int> >            ("elsexpouterlayers" ).setBranchAlias("els_exp_outerlayers" );   

    //CTF track matching stuff
    produces<vector<float>  >    ("elstrkshFrac" ).setBranchAlias("els_trkshFrac" );
    produces<vector<float>  >    ("elstrkdr"     ).setBranchAlias("els_trkdr"     );

    produces<vector<bool>     >       ("elsconvvtxflag"        ).setBranchAlias("els_conv_vtx_flag"        );
    produces<vector<float>    >       ("elsconvvtxprob"        ).setBranchAlias("els_conv_vtx_prob"        );
 
    // predefined 2012 ID decisions
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaCutBasedIdentification
    // http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/UserCode/EGamma/EGammaAnalysisTools/interface/EGammaCutBasedEleId.h?revision=1.5&view=markup
    //

    //////////////////////
    // genMatch miniAOD //
    //////////////////////

    produces<vector<int>           >("elsmcpatMatchid"            	).setBranchAlias("els_mc_patMatch_id"          		); 
    produces<vector<LorentzVector> >("elsmcpatMatchp4"            	).setBranchAlias("els_mc_patMatch_p4"          		);
    produces<vector<float>         >("elsmcpatMatchdr"            	).setBranchAlias("els_mc_patMatch_dr"                  	);

    produces<vector<float>         >("elssigmaIPhiIPhifull5x5"   	).setBranchAlias("els_sigmaIPhiIPhi_full5x5"                  	);
    produces<vector<float>         >("elssigmaEtaEtafull5x5"     	).setBranchAlias("els_sigmaEtaEta_full5x5"                    	);
    produces<vector<float>         >("elssigmaIEtaIEtafull5x5"   	).setBranchAlias("els_sigmaIEtaIEta_full5x5"                  	);
    produces<vector<float>         >("elsr9full5x5"              	).setBranchAlias("els_r9_full5x5"                             	);
    produces<vector<float> >  ("elsr9"                      ).setBranchAlias("els_r9"                      );
    produces<vector<float>         >("else1x5full5x5"            	).setBranchAlias("els_e1x5_full5x5"                           	);
    produces<vector<float>         >("else5x5full5x5"            	).setBranchAlias("els_e5x5_full5x5"                           	);
    produces<vector<float>         >("else2x5Maxfull5x5"         	).setBranchAlias("els_e2x5Max_full5x5"                        	);

    produces<vector<int>         >("elsjetNDauChargedMVASel"         	).setBranchAlias("els_jetNDauChargedMVASel"                        	);
    produces<vector<float>         >("elsptRatio"         	).setBranchAlias("els_ptRatio"                        	);
    produces<vector<float>         >("elsptRel"         	).setBranchAlias("els_ptRel"                        	);
    produces<vector<float>         >("elsjetBTagCSV"         	).setBranchAlias("els_jetBTagCSV"                        	);

    produces<vector<float>         >("elsminiIsouncor"       ).setBranchAlias("els_miniIso_uncor"                       	);
    produces<vector<float>         >("elsminiIsoch"       ).setBranchAlias("els_miniIso_ch"                       	);
    produces<vector<float>         >("elsminiIsonh"       ).setBranchAlias("els_miniIso_nh"                       	);
    produces<vector<float>         >("elsminiIsoem"       ).setBranchAlias("els_miniIso_em"                       	);
    produces<vector<float>         >("elsminiIsodb"       ).setBranchAlias("els_miniIso_db"                       	);

    produces<vector<float>         >("elsminiRelIsochg"       ).setBranchAlias("els_miniRelIso_chg"                       	);
    produces<vector<float>         >("elsminiRelIsoall"       ).setBranchAlias("els_miniRelIso_all"                       	);


    produces<vector<float>         >("elsecalPFClusterIso"       ).setBranchAlias("els_ecalPFClusterIso"                       	);
    produces<vector<float>         >("elshcalPFClusterIso"       ).setBranchAlias("els_hcalPFClusterIso"                       	);

    ////////////////////////////
    // For calibration in 7_X //
    ////////////////////////////
    produces<vector<int> > ("elsisEcalDriven").setBranchAlias("els_isEcalDriven");
    produces<vector<int> > ("elsisTrackerDriven").setBranchAlias("els_isTrackerDriven");
    produces<vector<int> > ("elsisEB").setBranchAlias("els_isEB");

    produces<vector<float> > ("elsscSeedEta").setBranchAlias("els_scSeedEta");



}

ElectronMaker::~ElectronMaker()
{
}

void  ElectronMaker::beginRun(const edm::Run&, const EventSetup& es) {
  
}

void ElectronMaker::beginJob() {
}

void ElectronMaker::endJob() {
}

// ------------ method called to produce the data  ------------
void ElectronMaker::produce(Event& iEvent, const EventSetup& iSetup) {

    // Define vectors to be filled
    unique_ptr<unsigned int>   evt_nels(new unsigned int) ;

    // ECAL related (superCluster) variables
    unique_ptr<vector<float> > els_etaSC       (new vector<float> );
    unique_ptr<vector<float> > els_phiSC       (new vector<float> );
    unique_ptr<vector<float> > els_eSC         (new vector<float> );
    unique_ptr<vector<float> > els_eSCRaw      (new vector<float> );
    unique_ptr<vector<float> > els_eSCPresh    (new vector<float> );
    unique_ptr<vector<float> > els_etaSCwidth  (new vector<float> );
    unique_ptr<vector<float> > els_phiSCwidth  (new vector<float> );
    unique_ptr<vector<int> >   els_type        (new vector<int>   );

    // uncertainties and corrections
    // somewhat complicated: see 
    // http://cms-service-sdtweb.web.cern.ch/cms-service-sdtweb/doxygen/CMSSW_3_1_2/doc/html/d5/d4b/GsfElectron_8h-source.html
    // note that if ecalEnergy == eSC depends on if further ecal corrections have been applied to the electron
    // after its construction
    unique_ptr<vector<float> > els_ecalEnergy            (new vector<float>);
    unique_ptr<vector<float> > els_ecalEnergyError       (new vector<float>);
    unique_ptr<vector<float> > els_trackMomentumError    (new vector<float>);
    unique_ptr<vector<float> > els_electronMomentumError (new vector<float>);
  
    // ID variables
    //
    unique_ptr<vector<float> > els_eSeed                         (new vector<float> );
    unique_ptr<vector<float> > els_dEtaIn                        (new vector<float> );
    unique_ptr<vector<float> > els_dEtaOut                       (new vector<float> );
    unique_ptr<vector<float> > els_dPhiIn                        (new vector<float> );
    unique_ptr<vector<float> > els_dPhiOut                       (new vector<float> );
    unique_ptr<vector<float> > els_fbrem                         (new vector<float> );
    unique_ptr<vector<float> > els_eOverPIn                      (new vector<float> );
    unique_ptr<vector<float> > els_eOverPOut                     (new vector<float> );
    unique_ptr<vector<bool > > els_isGsfCtfScPixChargeConsistent (new vector<bool > );
                             
    unique_ptr<vector<float> > els_hOverE                        (new vector<float> );
    unique_ptr<vector<float> > els_full5x5_hOverE                (new vector<float> );
                             
    unique_ptr<vector<float> > els_sigmaIPhiIPhi                 (new vector<float> );
    unique_ptr<vector<float> > els_sigmaIEtaIEta                 (new vector<float> );
                             

    // predefined ID decisions
    //
    unique_ptr<vector<int> > els_category (new vector<int>);

    // unique_ptr<vector<int> > passVetoId     (new vector<int>);
    // unique_ptr<vector<int> > passLooseId    (new vector<int>);
    // unique_ptr<vector<int> > passMediumId   (new vector<int>);
    // unique_ptr<vector<int> > passTightId    (new vector<int>);
    // unique_ptr<vector<int> > passHEEPId                  (new vector<int>);
    unique_ptr<vector<int> > passVIDNonTrigMvaWP80Id     (new vector<int>);
    unique_ptr<vector<int> > passVIDNonTrigMvaWP90Id     (new vector<int>);
    unique_ptr<vector<int> > passVIDTrigMvaWP80Id        (new vector<int>);
    unique_ptr<vector<int> > passVIDTrigMvaWP90Id        (new vector<int>);
    unique_ptr<vector<float> > VIDNonTrigMvaValue        (new vector<float>);
    unique_ptr<vector<float> > VIDTrigMvaValue           (new vector<float>);
    unique_ptr<vector<int> > VIDNonTrigMvaCat            (new vector<int>   );
    unique_ptr<vector<int> > VIDTrigMvaCat               (new vector<int>   );
    unique_ptr<vector<float> > VIDSpring16GPMvaValue     (new vector<float>);
    unique_ptr<vector<int> >   VIDSpring16GPMvaCat       (new vector<int>   );
    unique_ptr<vector<float> > VIDSpring16HZZMvaValue    (new vector<float>);
    unique_ptr<vector<int> >   VIDSpring16HZZMvaCat      (new vector<int>   );

    unique_ptr<vector<float> > VIDFall17NoIsoMvaValue    (new vector<float>);
    unique_ptr<vector<int> >   VIDFall17NoIsoMvaCat      (new vector<int>   );
    unique_ptr<vector<float> > VIDFall17IsoMvaValue    (new vector<float>);
    unique_ptr<vector<int> >   VIDFall17IsoMvaCat      (new vector<int>   );

    unique_ptr<vector<float> > VIDFall17V2NoIsoMvaValue    (new vector<float>);
    unique_ptr<vector<int> >   VIDFall17V2NoIsoMvaCat      (new vector<int>   );
    unique_ptr<vector<float> > VIDFall17V2IsoMvaValue    (new vector<float>);
    unique_ptr<vector<int> >   VIDFall17V2IsoMvaCat      (new vector<int>   );

    // isolation variables
    //
    unique_ptr<vector<float> > els_hcalDepth1TowerSumEt   (new vector<float> );
    unique_ptr<vector<float> > els_tkIso                  (new vector<float> );
    unique_ptr<vector<float> > els_ecalIso                (new vector<float> );
    unique_ptr<vector<float> > els_hcalIso                (new vector<float> );
                             
    unique_ptr<vector<float> > els_tkIso04                (new vector<float> );
    unique_ptr<vector<float> > els_ecalIso04              (new vector<float> );
    unique_ptr<vector<float> > els_hcalIso04              (new vector<float> );



    unique_ptr<vector<float> > els_pfChargedHadronIso     (new vector<float> );
    unique_ptr<vector<float> > els_pfNeutralHadronIso     (new vector<float> );
    unique_ptr<vector<float> > els_pfPhotonIso            (new vector<float> );
    unique_ptr<vector<float> > els_pfPUIso                (new vector<float> );

    // track variables
    //
    unique_ptr<vector<int> >   els_charge     (new vector<int>   );
    unique_ptr<vector<int> >   els_trk_charge (new vector<int>   );
    unique_ptr<vector<int> >   els_sccharge   (new vector<int>   );
    unique_ptr<vector<float> > els_chi2       (new vector<float> );
    unique_ptr<vector<float> > els_ndof       (new vector<float> );
    unique_ptr<vector<float> > els_ckf_chi2   (new vector<float> );
    unique_ptr<vector<float> > els_ckf_ndof   (new vector<float> );
    unique_ptr<vector<int> >   els_validHits  (new vector<int>   );
    unique_ptr<vector<int> >   els_lostHits   (new vector<int>   );
    unique_ptr<vector<float> > els_dxyPV      (new vector<float> );
    unique_ptr<vector<float> > els_dzPV       (new vector<float> );
    unique_ptr<vector<float> > els_d0Err      (new vector<float> );
    unique_ptr<vector<float> > els_z0Err      (new vector<float> );
    unique_ptr<vector<float> > els_ptErr      (new vector<float> );
    unique_ptr<vector<float> > els_ptErrGsf   (new vector<float> );
    unique_ptr<vector<float> > els_ip3d       (new vector<float> );
    unique_ptr<vector<float> > els_ip3derr    (new vector<float> );
    unique_ptr<vector<float> > els_ip2d       (new vector<float> );
    unique_ptr<vector<float> > els_ip2derr    (new vector<float> );

  
    // LorentzVectors
    //
    unique_ptr<vector<LorentzVector> > els_p4     (new vector<LorentzVector>);
    unique_ptr<vector<LorentzVector> > els_trk_p4 (new vector<LorentzVector>);
    unique_ptr<vector<LorentzVector> > els_p4In   (new vector<LorentzVector>);
    unique_ptr<vector<LorentzVector> > els_p4Out  (new vector<LorentzVector>);

    // Vertex
    //
    unique_ptr<vector<LorentzVector> > els_vertex_p4 (new vector<LorentzVector>);
    unique_ptr<vector<LorentzVector> > els_trk_vertex_p4 (new vector<LorentzVector>);

    //HitPattern information
    //
    unique_ptr<vector<int> >                    els_valid_pixelhits      (new vector<int>           ); 
    unique_ptr<vector<int> >                    els_lost_pixelhits       (new vector<int>           ); 
    unique_ptr<vector<int> >                    els_nlayers              (new vector<int>           ); 
    unique_ptr<vector<int> >                    els_nlayers3D            (new vector<int>           ); 
    unique_ptr<vector<int> >                    els_nlayersLost          (new vector<int>           ); 
    unique_ptr<vector<int> >                    els_exp_innerlayers      (new vector<int>           ); 
    unique_ptr<vector<int> >                    els_exp_outerlayers      (new vector<int>           ); 
    unique_ptr<vector<int> >                    els_ckf_laywithmeas      (new vector<int>           );
    unique_ptr<vector<int> >                    els_ckf_charge           (new vector<int>           );

    unique_ptr<vector<float> >                  els_trkshFrac            (new vector<float>         );
    unique_ptr<vector<float> >                  els_trkdr                (new vector<float>         );

    unique_ptr<vector<bool> >                   els_conv_vtx_flag        (new vector<bool>           );
    unique_ptr<vector<float> >                  els_conv_vtx_prob        (new vector<float>          );

    //////////////////////
    // Added miniAOD    //
    //////////////////////
    unique_ptr<vector<int>           >       els_mc_patMatch_id          (new vector<int>          );
    unique_ptr<vector<LorentzVector> >       els_mc_patMatch_p4          (new vector<LorentzVector>);
    unique_ptr<vector<float>         >       els_mc_patMatch_dr          (new vector<float>        );

    unique_ptr<vector<float>   >       els_sigmaIPhiIPhi_full5x5             (new vector<float>        );
    unique_ptr<vector<float>   >       els_sigmaEtaEta_full5x5               (new vector<float>        );
    unique_ptr<vector<float>   >       els_sigmaIEtaIEta_full5x5             (new vector<float>        );
    unique_ptr<vector<float>   >       els_r9_full5x5                        (new vector<float>        );
    unique_ptr<vector<float> >  els_r9                       ( new vector<float> );
    unique_ptr<vector<float>   >       els_e1x5_full5x5                      (new vector<float>        );
    unique_ptr<vector<float>   >       els_e5x5_full5x5                      (new vector<float>        );
    unique_ptr<vector<float>   >       els_e2x5Max_full5x5                   (new vector<float>        );

    unique_ptr<vector<float>   >       els_miniIso_uncor                  (new vector<float>        );  	
    unique_ptr<vector<float>   >       els_miniIso_ch                  (new vector<float>        );  	
    unique_ptr<vector<float>   >       els_miniIso_nh                  (new vector<float>        );  	
    unique_ptr<vector<float>   >       els_miniIso_em                  (new vector<float>        );  	
    unique_ptr<vector<float>   >       els_miniIso_db                  (new vector<float>        );  	

    unique_ptr<vector<float>   >       els_miniRelIso_chg                  (new vector<float>        );  	
    unique_ptr<vector<float>   >       els_miniRelIso_all                  (new vector<float>        );  	

    unique_ptr<vector<float>   >       els_ecalPFClusterIso                (new vector<float>        );  	
    unique_ptr<vector<float>   >       els_hcalPFClusterIso                (new vector<float>        );  	

    unique_ptr<vector<int>   >       els_jetNDauChargedMVASel                   (new vector<int>        );
    unique_ptr<vector<float>   >       els_ptRatio                   (new vector<float>        );
    unique_ptr<vector<float>   >       els_ptRel                   (new vector<float>        );
    unique_ptr<vector<float>   >       els_jetBTagCSV                   (new vector<float>        );

    ///////////////////////////////
    // Added for 7_X calibration //
    ///////////////////////////////
    unique_ptr<vector<int> > els_isEcalDriven              (new vector<int>);
    unique_ptr<vector<int> > els_isTrackerDriven           (new vector<int>);
    unique_ptr<vector<int> > els_isEB                      (new vector<int>);

    unique_ptr<vector<float> > els_scSeedEta               (new vector<float>);




    // --- Get Input Collections --- //

    ///////////////
    // Electrons //
    ///////////////

    Handle<View<pat::Electron> > els_h;
    iEvent.getByToken(electronsToken, els_h);
    if( !els_h.isValid() ) {
        throw cms::Exception("ElectronMaker::produce: error getting electron collection from Event!");
    }


    // Jets
    Handle<View<pat::Jet> > pfJetsHandle;
    iEvent.getByToken(pfJetsToken, pfJetsHandle);

//    Handle<GsfElectronCollection> els_coll_h;
//    iEvent.getByLabel(electronsInputTag_, els_coll_h);    

    //////////////
    // PF Cands //
    //////////////

    iEvent.getByToken(pfCandsToken, packPfCand_h);
    if( !packPfCand_h.isValid() ) {
        throw cms::Exception("ElectronMaker::produce: error getting packed pfcands from Event!");
    }
    pfCandidates  = packPfCand_h.product();


  
    ////////////
    // Vertex //
    ////////////

    iEvent.getByToken(vtxToken, vertexHandle);
    if( !vertexHandle.isValid() ) {
        throw cms::Exception("ElectronMaker::produce: error getting vertex collection from Event!");
    }

    /////////////////
    // Conversions //
    /////////////////
    
    iEvent.getByToken(recoConversionToken, convs_h);
    if( !convs_h.isValid() ) {
        throw cms::Exception("ElectronMaker::produce: error getting conversion collection");
    }


    ///////////////////////////
    // TransientTrackBuilder //
    ///////////////////////////
//    ESHandle<TransientTrackBuilder> theTTBuilder;
//    iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",theTTBuilder);


    ////////////////////////////////////////////////
    // Get tools to get cluster shape information //
    ////////////////////////////////////////////////



    //////////////
    // Beamspot //
    //////////////

    Handle<LorentzVector> beamSpotH;
    iEvent.getByToken(beamSpotToken, beamSpotH);
    const Point beamSpot = beamSpotH.isValid() ? Point(beamSpotH->x(), beamSpotH->y(), beamSpotH->z()) : Point(0,0,0);

    //Handle<reco::BeamSpot> beamspot_h;
    //iEvent.getByLabel(beamSpot_tag_, beamspot_h);
    //const reco::BeamSpot &beamSpotreco = *(beamspot_h.product()); 
    //if ( beamSpotreco.x0() == 1234567 ) ; // Avoid "unused variable" error while the function using this variable is inactive


    ///////////////////////
    // rho for isolation //
    ///////////////////////

//    edm::Handle<float> rhoIso_h;
//    iEvent.getByLabel(rhoInputTag_, rhoIso_h);
//    float rhoIso = *(rhoIso_h.product());

    /////////////////////////////////////////////////////////////
    // Get the value maps for the Egamma electron ID decisions //
    /////////////////////////////////////////////////////////////

//    const ValueMap<float>&  eidLHMap = getValueMap<float>(iEvent, eidLHTag_);

    // edm::Handle<edm::ValueMap<bool> > veto_id_decisions;
    // edm::Handle<edm::ValueMap<bool> > loose_id_decisions;
    // edm::Handle<edm::ValueMap<bool> > medium_id_decisions;
    // edm::Handle<edm::ValueMap<bool> > tight_id_decisions;
    // edm::Handle<edm::ValueMap<bool> > HEEP_id_decisions;
    edm::Handle<edm::ValueMap<bool> > VIDNonTrigMvaWP80_id_decisions;
    edm::Handle<edm::ValueMap<bool> > VIDNonTrigMvaWP90_id_decisions;
    edm::Handle<edm::ValueMap<bool> > VIDTrigMvaWP80_id_decisions;
    edm::Handle<edm::ValueMap<bool> > VIDTrigMvaWP90_id_decisions;
    edm::Handle<edm::ValueMap<float> > VIDNonTrigMva_values;
    edm::Handle<edm::ValueMap<float> > VIDTrigMva_values;
    edm::Handle<edm::ValueMap<int> >  VIDNonTrigMva_cats;
    edm::Handle<edm::ValueMap<int> >  VIDTrigMva_cats;
    edm::Handle<edm::ValueMap<float> > VIDSpring16GPMva_values;
    edm::Handle<edm::ValueMap<int> >  VIDSpring16GPMva_cats;
    edm::Handle<edm::ValueMap<float> > VIDSpring16HZZMva_values;
    edm::Handle<edm::ValueMap<int> >  VIDSpring16HZZMva_cats;
    edm::Handle<edm::ValueMap<float> > VIDFall17NoIsoMva_values;
    edm::Handle<edm::ValueMap<int> >   VIDFall17NoIsoMva_cats;
    edm::Handle<edm::ValueMap<float> > VIDFall17IsoMva_values;
    edm::Handle<edm::ValueMap<int> >   VIDFall17IsoMva_cats;
    edm::Handle<edm::ValueMap<float> > VIDFall17V2NoIsoMva_values;
    edm::Handle<edm::ValueMap<int> >   VIDFall17V2NoIsoMva_cats;
    edm::Handle<edm::ValueMap<float> > VIDFall17V2IsoMva_values;
    edm::Handle<edm::ValueMap<int> >   VIDFall17V2IsoMva_cats;

    edm::Handle<edm::ValueMap<float> > miniIsoChg_values;
    edm::Handle<edm::ValueMap<float> > miniIsoAll_values;

    // iEvent.getByToken(electronVetoIdMapToken_,veto_id_decisions);
    // iEvent.getByToken(electronLooseIdMapToken_,loose_id_decisions);
    // iEvent.getByToken(electronMediumIdMapToken_,medium_id_decisions);
    // iEvent.getByToken(electronTightIdMapToken_,tight_id_decisions);
    // iEvent.getByToken(electronHEEPIdMapToken_,HEEP_id_decisions);
    // iEvent.getByToken(electronVIDNonTrigMvaWP80IdMapToken_,VIDNonTrigMvaWP80_id_decisions);
    // iEvent.getByToken(electronVIDNonTrigMvaWP90IdMapToken_,VIDNonTrigMvaWP90_id_decisions);
    // iEvent.getByToken(electronVIDTrigMvaWP80IdMapToken_,VIDTrigMvaWP80_id_decisions);
    // iEvent.getByToken(electronVIDTrigMvaWP90IdMapToken_,VIDTrigMvaWP90_id_decisions);
    // iEvent.getByToken(electronVIDNonTrigMvaValueMapToken_,VIDNonTrigMva_values);
    // iEvent.getByToken(electronVIDTrigMvaValueMapToken_,VIDTrigMva_values);
    // iEvent.getByToken(electronVIDNonTrigMvaCatMapToken_,VIDNonTrigMva_cats);
    // iEvent.getByToken(electronVIDTrigMvaCatMapToken_,VIDTrigMva_cats);
    iEvent.getByToken(electronVIDSpring16GPMvaValueMapToken_,VIDSpring16GPMva_values);
    iEvent.getByToken(electronVIDSpring16GPMvaCatMapToken_,VIDSpring16GPMva_cats);
    iEvent.getByToken(electronVIDSpring16HZZMvaValueMapToken_,VIDSpring16HZZMva_values);
    iEvent.getByToken(electronVIDSpring16HZZMvaCatMapToken_,VIDSpring16HZZMva_cats);
    iEvent.getByToken(electronVIDFall17NoIsoMvaValueMapToken_,VIDFall17NoIsoMva_values);
    iEvent.getByToken(electronVIDFall17NoIsoMvaCatMapToken_,  VIDFall17NoIsoMva_cats);
    iEvent.getByToken(electronVIDFall17IsoMvaValueMapToken_,VIDFall17IsoMva_values);
    iEvent.getByToken(electronVIDFall17IsoMvaCatMapToken_,  VIDFall17IsoMva_cats);
    iEvent.getByToken(electronVIDFall17V2NoIsoMvaValueMapToken_,VIDFall17V2NoIsoMva_values);
    iEvent.getByToken(electronVIDFall17V2NoIsoMvaCatMapToken_,  VIDFall17V2NoIsoMva_cats);
    iEvent.getByToken(electronVIDFall17V2IsoMvaValueMapToken_,VIDFall17V2IsoMva_values);
    iEvent.getByToken(electronVIDFall17V2IsoMvaCatMapToken_,  VIDFall17V2IsoMva_cats);

    // Corrected Isolation using NanoAOD
    iEvent.getByToken(miniIsoChgValueMapToken_,miniIsoChg_values);
    iEvent.getByToken(miniIsoAllValueMapToken_,miniIsoAll_values);

    //////////////////////////
    // get cms2scsseeddetid //
    //////////////////////////

//    InputTag cms2scsseeddetid_tag(cms2scsseeddetidInputTag_.label(),"scsdetIdSeed");
//    Handle<vector<int> > cms2scsseeddetid_h;
//    iEvent.getByLabel(cms2scsseeddetid_tag, cms2scsseeddetid_h); 
//    const vector<int> *cms2scsseeddetid = cms2scsseeddetid_h.product();

    //////////////////////////////
    // Get the ele<->PFCand map //
    //////////////////////////////

//    edm::Handle<edm::ValueMap<std::vector<reco::PFCandidateRef > > > eleToParticleBasedIsoMapHandle;
//    InputTag particleBase(string("particleBasedIsolation"),string("gedGsfElectrons"));  
//    iEvent.getByLabel(particleBase, eleToParticleBasedIsoMapHandle);    
//    edm::ValueMap<std::vector<reco::PFCandidateRef > >   eleToParticleBasedIsoMap =  *(eleToParticleBasedIsoMapHandle.product());
    
    // --- Fill --- //

    /////////////////////////
    // Loop Over Electrons //
    /////////////////////////

    *evt_nels       = els_h->size();
    double mass     = 0.000510998918;
    size_t elsIndex = 0;
    for( View<pat::Electron>::const_iterator el = els_h->begin(); el != els_h->end(); el++, elsIndex++ ) {


        ////////////////
        // References //
        ////////////////
        const GsfTrackRef            el_track         = el->gsfTrack(); // Embedded GSF Track for miniAOD
        const RefToBase<pat::Electron> gsfElRef         = els_h->refAt(elsIndex);    
        const TrackRef               ctfTkRef         = el->closestCtfTrackRef(); // Embedded CTF Track for miniAOD 

/*
  const Track*                 el_track         = (const Track*)(el->gsfTrack().get());
  const RefToBase<pat::Electron> gsfElRef         = els_h->refAt(elsIndex);    

  //const TrackRef               ctfTkRef         = el->closestCtfTrackRef();
  const GsfTrackRef            gsfTkRef         = el->gsfTrack();

*/

        ////////////
        // Vertex //
        ////////////
        const VertexCollection*      vertexCollection = vertexHandle.product();
        VertexCollection::const_iterator firstGoodVertex = vertexCollection->end();
        int firstGoodVertexIdx = 0;
        for (VertexCollection::const_iterator vtx = vertexCollection->begin(); vtx != vertexCollection->end(); ++vtx, ++firstGoodVertexIdx) {
            // Replace isFake() for miniAOD because it requires tracks and miniAOD vertices don't have tracks:
            // Vertex.h: bool isFake() const {return (chi2_==0 && ndof_==0 && tracks_.empty());}
            if (  /*!vtx->isFake() &&*/ !(vtx->chi2()==0 && vtx->ndof()==0) &&  vtx->ndof()>=4. && vtx->position().Rho()<=2.0 && fabs(vtx->position().Z())<=24.0) {
                firstGoodVertex = vtx;
                break;
            }
        }

        //////////////////////
        // Fiduciality Mask //
        //////////////////////

        int fiducialityMask = 0;  // the enum is in interface/EgammaFiduciality.h
        if ( el->isEB()        ) fiducialityMask |= 1 << ISEB;
        if ( el->isEBEEGap()   ) fiducialityMask |= 1 << ISEBEEGAP;
        if ( el->isEE()        ) fiducialityMask |= 1 << ISEE;
        if ( el->isEEGap()     ) fiducialityMask |= 1 << ISEEGAP;
        if ( el->isEBEtaGap()  ) fiducialityMask |= 1 << ISEBETAGAP;
        if ( el->isEBPhiGap()  ) fiducialityMask |= 1 << ISEBPHIGAP;
        if ( el->isEEDeeGap()  ) fiducialityMask |= 1 << ISEEDEEGAP;
        if ( el->isEERingGap() ) fiducialityMask |= 1 << ISEERINGGAP;
        if ( el->isGap()       ) fiducialityMask |= 1 << ISGAP;

  
        ///////////////////////////
        // Corrections & Seeding //
        ///////////////////////////

        int electronTypeMask = 0;
        if ( el->isEcalEnergyCorrected()        ) electronTypeMask |= 1 << ISECALENERGYCORRECTED;
        if ( el->trackerDrivenSeed()            ) electronTypeMask |= 1 << ISTRACKERDRIVEN;
        if ( el->ecalDrivenSeed()               ) electronTypeMask |= 1 << ISECALDRIVEN;
        if ( el->passingCutBasedPreselection()  ) electronTypeMask |= 1 << ISCUTPRESELECTED;
        if ( el->passingMvaPreselection()       ) electronTypeMask |= 1 << ISMVAPRESELECTED;
        //if ( el->isMomentumCorrected() ) electronTypeMask |= 1 << ISMOMENTUMCORRECTED; // Depricated in CMSSW_4_2x ( DataFormats/EgammaCandidates/interface/GsfElectron.h )

        /////////////////////
        // Lorentz Vectors //
        /////////////////////

        LorentzVector    p4In;
        LorentzVector    p4Out;
        LorentzVector    trk_p4( el_track->px(), el_track->py(), el_track->pz(), el_track->p() );
        math::XYZVectorF p3In  = el->trackMomentumAtVtx();
        math::XYZVectorF p3Out = el->trackMomentumOut();
        p4In.SetXYZT (   p3In.x() , p3In.y() , p3In.z() , sqrt( mass*mass + p3In.R() *p3In.R()  ) );
        p4Out.SetXYZT(   p3Out.x(), p3Out.y(), p3Out.z(), sqrt( mass*mass + p3Out.R()*p3Out.R() ) );


        ///////////////////
        // Predifined ID //
        ///////////////////

        els_category           ->push_back( classify(gsfElRef)    ); // this is the sani classification

        const Ptr<pat::Electron> elPtr(els_h, el - els_h->begin() );
  
        // passVetoId  ->push_back( (*veto_id_decisions)[ elPtr ] );
        // passLooseId ->push_back( (*loose_id_decisions)[ elPtr ] );
        // passMediumId->push_back( (*medium_id_decisions)[ elPtr ] );
        // passTightId ->push_back( (*tight_id_decisions)[ elPtr ] );
        // passHEEPId               ->push_back( (*HEEP_id_decisions)[ elPtr ] );
        // passVIDNonTrigMvaWP80Id  ->push_back( (*VIDNonTrigMvaWP80_id_decisions)[ elPtr ] );
        // passVIDNonTrigMvaWP90Id  ->push_back( (*VIDNonTrigMvaWP90_id_decisions)[ elPtr ] );
        // passVIDTrigMvaWP80Id     ->push_back( (*VIDTrigMvaWP80_id_decisions)[ elPtr ] );
        // passVIDTrigMvaWP90Id     ->push_back( (*VIDTrigMvaWP90_id_decisions)[ elPtr ] );
        // VIDNonTrigMvaValue       ->push_back( (*VIDNonTrigMva_values)[ elPtr ] );
        // VIDTrigMvaValue          ->push_back( (*VIDTrigMva_values)[ elPtr ] );
        // VIDNonTrigMvaCat         ->push_back( (*VIDNonTrigMva_cats)[ elPtr ] );
        // VIDTrigMvaCat            ->push_back( (*VIDTrigMva_cats)[ elPtr ] );
        VIDSpring16GPMvaValue    ->push_back( (*VIDSpring16GPMva_values)[ elPtr ] );
        VIDSpring16GPMvaCat      ->push_back( (*VIDSpring16GPMva_cats)[ elPtr ] );
        VIDSpring16HZZMvaValue   ->push_back( (*VIDSpring16HZZMva_values)[ elPtr ] );
        VIDSpring16HZZMvaCat     ->push_back( (*VIDSpring16HZZMva_cats)[ elPtr ] );
        VIDFall17NoIsoMvaValue   ->push_back( (*VIDFall17NoIsoMva_values)[ elPtr ] );
        VIDFall17NoIsoMvaCat     ->push_back( (*VIDFall17NoIsoMva_cats)[ elPtr ] );
        VIDFall17IsoMvaValue   ->push_back( (*VIDFall17IsoMva_values)[ elPtr ] );
        VIDFall17IsoMvaCat     ->push_back( (*VIDFall17IsoMva_cats)[ elPtr ] );
        VIDFall17V2NoIsoMvaValue   ->push_back( (*VIDFall17V2NoIsoMva_values)[ elPtr ] );
        VIDFall17V2NoIsoMvaCat     ->push_back( (*VIDFall17V2NoIsoMva_cats)[ elPtr ] );
        VIDFall17V2IsoMvaValue   ->push_back( (*VIDFall17V2IsoMva_values)[ elPtr ] );
        VIDFall17V2IsoMvaCat     ->push_back( (*VIDFall17V2IsoMva_cats)[ elPtr ] );

        // float isopt = el->p4().pt();
        // els_miniRelIso_chg->push_back((*miniIsoChg_values)[elPtr]/isopt);
        // els_miniRelIso_all->push_back((*miniIsoAll_values)[elPtr]/isopt);

        //////////////
        // Electron //
        //////////////

        //els_ecalEnergy         ->push_back( el->ecalEnergy()                                );  // energy corrections and uncertainties
        //els_ecalEnergyError    ->push_back( el->ecalEnergyError()                           );
        els_ecalEnergy         ->push_back( el->correctedEcalEnergy()                       );  // energy corrections and uncertainties
        els_ecalEnergyError    ->push_back( el->correctedEcalEnergyError()                  );
        els_trackMomentumError ->push_back( el->trackMomentumError()                        );
        els_p4                 ->push_back( LorentzVector( el->p4() )                       );
        els_trk_p4             ->push_back( trk_p4                                          );
        els_p4In               ->push_back( p4In                                            );
        els_p4Out              ->push_back( p4Out                                           );
        els_vertex_p4          ->push_back( LorentzVector(el->vx(), el->vy(), el->vz(), 0.) );
        els_trk_vertex_p4      ->push_back( LorentzVector(el_track->vx(), el_track->vy(), el_track->vz(), 0.) );
        els_type               ->push_back( electronTypeMask                                );


        ///////////////
        // Isolation //
        ///////////////

        els_hcalDepth1TowerSumEt  ->push_back( el->dr03HcalDepth1TowerSumEt()             );
        els_ecalIso               ->push_back( el->dr03EcalRecHitSumEt()                  );
        els_hcalIso               ->push_back( el->dr03HcalTowerSumEt()                   );
        els_tkIso                 ->push_back( el->dr03TkSumPt()                          );

        els_ecalIso04             ->push_back( el->dr04EcalRecHitSumEt()                  );
        els_hcalIso04             ->push_back( el->dr04HcalTowerSumEt()                   );
        els_tkIso04               ->push_back( el->dr04TkSumPt()                          );

    
        //////////////////
        // PF Isolation //
        //////////////////

        GsfElectron::PflowIsolationVariables pfIso = el->pfIsolationVariables();

        els_pfChargedHadronIso -> push_back( pfIso.sumChargedHadronPt );
        els_pfNeutralHadronIso -> push_back( pfIso.sumNeutralHadronEt );
        els_pfPhotonIso        -> push_back( pfIso.sumPhotonEt        );
        els_pfPUIso            -> push_back( pfIso.sumPUPt            );


        //////////////////
        // Supercluster //
        //////////////////

        els_etaSC         ->push_back( el->superCluster()->eta()             );
        els_phiSC         ->push_back( el->superCluster()->phi()             );
	els_eSC           ->push_back( el->superCluster()->energy()          );
        els_eSCRaw        ->push_back( el->superCluster()->rawEnergy()       );
        els_eSCPresh      ->push_back( el->superCluster()->preshowerEnergy() );
        els_sigmaIEtaIEta ->push_back( el->sigmaIetaIeta()                   );
        els_etaSCwidth    ->push_back( el->superCluster()->etaWidth()        );
        els_phiSCwidth    ->push_back( el->superCluster()->phiWidth()        );
	
	// We used to make these using the cluster tools, but now we can take them directly from RECO electron
	els_sigmaIPhiIPhi         ->push_back( el->sigmaIphiIphi()           );

	// Take these directly from the PAT electron of the miniAOD
	els_sigmaIPhiIPhi_full5x5  ->push_back( el->full5x5_sigmaIphiIphi()   ); 
	els_sigmaEtaEta_full5x5    ->push_back( el->full5x5_sigmaEtaEta()     );
	els_sigmaIEtaIEta_full5x5  ->push_back( el->full5x5_sigmaIetaIeta()   );
	els_r9_full5x5             ->push_back( el->full5x5_r9()              );
    els_r9                      ->push_back( el->r9()                       );
	els_e1x5_full5x5           ->push_back( el->full5x5_e1x5()            );
	els_e5x5_full5x5           ->push_back( el->full5x5_e5x5()            );  
	els_e2x5Max_full5x5        ->push_back( el->full5x5_e2x5Max()         );

    /////////////////////////////
    // LeptonMVA jet daughters //
    /////////////////////////////

    // const auto & pv = (*vertexHandle)[0];
    // auto infotuple = MatchUtilities::getLepMVAInfo(elPtr, pfJetsHandle, pv);
    // float ptRatio = std::get<0>(infotuple);
    // float ptRel = std::get<1>(infotuple);
    // int jetNDauChargedMVASel = std::get<2>(infotuple);
    // float jetBTagCSV = std::get<3>(infotuple);
    // els_jetNDauChargedMVASel->push_back(jetNDauChargedMVASel);
    // els_ptRatio->push_back(ptRatio);
    // els_ptRel->push_back(ptRel);
    // els_jetBTagCSV->push_back(jetBTagCSV);

	///////////////////////////////////////////////////////
	// Get cluster info that is not stored in the object //
	///////////////////////////////////////////////////////


	//This is a fix for accessing SC information in reminiAOD_V2
	int numberOfClusters =  el->superCluster()->clusters().size();
	bool missing_clusters = false;
	if( numberOfClusters > 0 ) missing_clusters = !el->superCluster()->clusters()[numberOfClusters-1].isAvailable();
	  
	int numberOfPSClusters =  el->superCluster()->preshowerClusters().size();
	bool missing_PSclusters = false;
	if( numberOfPSClusters > 0 ) missing_PSclusters = !el->superCluster()->preshowerClusters()[numberOfPSClusters-1].isAvailable();

	if( !(missing_clusters || missing_PSclusters) && (el->p4().pt() > 5) ){



            // The commented ones are already available above! Keeping it here for reference
	  
            //	els_scRawEnergy             = el->superCluster()->rawEnergy();
            //	els_scCalibratedEnergy      = el->superCluster()->energy();
            //	els_scPreshowerEnergy       = el->superCluster()->preshowerEnergy();
            //	els_scEta                   = el->superCluster()->position().Eta();
            //	els_scPhi                   = el->superCluster()->position().Phi();
            //	els_scPhiWidth              = el->superCluster()->phiWidth();
            //	els_scEtaWidth              = el->superCluster()->etaWidth();
            //	els_scSeedRawEnergy         = el->superCluster()->seed()->energy();
            //	els_scSeedCalibratedEnergy  = el->superCluster()->seed()->energy();
            //    els_scSeedE5x5             ->push_back(clusterTools_->e5x5(*(el->superCluster()->seed())));
            //	els_scSeedE2x5max          ->push_back(clusterTools_->e2x5Max(*(el->superCluster()->seed())));
            //    els_scSeedSigmaIetaIeta    ->push_back(see);
            //    els_scSeedSigmaIphiIphi    ->push_back(spp); 
	  
	
            // The one below is kept for historical reasons

            els_eSeed                  ->push_back(el->superCluster()->seed()->energy());
	  
            els_scSeedEta              ->push_back(el->superCluster()->seed()->eta());

            ///////////////////////////////////
            // Information about subclusters //
            ///////////////////////////////////
	  
            /////////////////////////////
            // Electron classification //
            /////////////////////////////

            //	els_classification = -1;   
            els_isEcalDriven->push_back(el->ecalDriven());
            els_isTrackerDriven->push_back(el->trackerDrivenSeed());
            els_isEB->push_back(el->isEB());
	} else {

            els_scSeedEta              ->push_back(-999.);

            els_eSeed                  ->push_back(-999.);	  




            els_isEcalDriven           ->push_back(-999);
            els_isTrackerDriven        ->push_back(-999);
            els_isEB                   ->push_back(-999);
	}
//
//            //
//            const BasicCluster&  clRef              = *(el->superCluster()->seed());
//            const vector<float>& covs               = clusterTools_->covariances(clRef);                         // get the covariances computed in 5x5 around the seed
//            const vector<float>& lcovs              = clusterTools_->localCovariances(clRef);                    // get the local covariances computed in a 5x5 around the seed
//            const vector<float>  localCovariancesSC = clusterTools_->scLocalCovariances(*(el->superCluster()));  // get the local covariances computed using all crystals in the SC
//
//            //
//get from RECO            els_sigmaIPhiIPhi   ->push_back( isfinite(lcovs[2])              ? lcovs[2] > 0               ? sqrt(lcovs[2]) : -1 * sqrt(-1 * lcovs[2])                             : -9999. );
//
//            //


        ////////
        // ID //
        ////////

        //els_hOverE                        ->push_back( el->hadronicOverEm()                 );
        els_hOverE                        ->push_back( el->hcalOverEcal()                   );
        els_full5x5_hOverE                ->push_back( el->full5x5_hcalOverEcal()           );
        els_eOverPIn                      ->push_back( el->eSuperClusterOverP()             );
        els_eOverPOut                     ->push_back( el->eEleClusterOverPout()            );
        els_fbrem                         ->push_back( el->fbrem()                          );

        els_dEtaIn                        ->push_back( el->deltaEtaSuperClusterTrackAtVtx() );
        els_dEtaOut                       ->push_back( el->deltaEtaSeedClusterTrackAtCalo() );
        els_dPhiIn                        ->push_back( el->deltaPhiSuperClusterTrackAtVtx() );
        els_dPhiOut                       ->push_back( el->deltaPhiSeedClusterTrackAtCalo() );
	els_isGsfCtfScPixChargeConsistent ->push_back( el->isGsfCtfScPixChargeConsistent()  );



        
        ////////////
        // Tracks //
        ////////////

        float pt       = el_track->pt();
        float p        = el_track->p();
        float q        = el_track->charge();
        float pz       = el_track->pz();
        float trkpterr = (el_track->charge()!=0) ? sqrt(pt*pt*p*p/pow(q, 2)*(el_track->covariance(0,0))+2*pt*p/q*pz*(el_track->covariance(0,1))+ pz*pz*(el_track->covariance(1,1) ) ) : -9999.;
            
        els_chi2                  ->push_back( el_track->chi2()                          );
        els_ndof                  ->push_back( el_track->ndof()                          );
        els_d0Err                 ->push_back( el_track->d0Error()                       );
        els_z0Err                 ->push_back( el_track->dzError()                       );
        els_ptErr                 ->push_back( trkpterr                                  );
        els_ptErrGsf              ->push_back( el_track->ptError()                       );
        els_validHits             ->push_back( el_track->numberOfValidHits()             );
        els_lostHits              ->push_back( el_track->numberOfLostHits()              );
        els_charge                ->push_back( el->charge()                              );
        els_trk_charge            ->push_back( el_track->charge()                        );
        els_sccharge              ->push_back( el->scPixCharge()                         );
	if (firstGoodVertex!=vertexCollection->end()) {
            els_dxyPV                 ->push_back( el_track->dxy( firstGoodVertex->position() )  );
            els_dzPV                  ->push_back( el_track->dz(  firstGoodVertex->position() )  );
	}
	else {
            els_dxyPV ->push_back( -999. );
            els_dzPV  ->push_back( -999. );
	}

        /////////
        // CTF //
        /////////

        if( ctfTkRef.isNonnull() ) {
            //els_trkshFrac -> push_back( static_cast<float>( el->shFracInnerHits() )                                  );
            els_trkshFrac -> push_back( static_cast<float>( el->ctfGsfOverlap() )                                    );
            els_trkdr     -> push_back( deltaR( el_track->eta(), el_track->phi(), ctfTkRef->eta(), ctfTkRef->phi() ) );
            els_ckf_chi2  -> push_back( ctfTkRef->chi2() );
            els_ckf_ndof  -> push_back( ctfTkRef->ndof() );
            els_ckf_laywithmeas -> push_back( ctfTkRef->hitPattern().trackerLayersWithMeasurement() );
	    els_ckf_charge-> push_back( ctfTkRef->charge() );
        } 
        else {
            els_trkshFrac -> push_back(-9999.);
            els_trkdr     -> push_back(-9999.);
            els_ckf_chi2  -> push_back(-9999.);
            els_ckf_ndof  -> push_back(-9999.);
            els_ckf_laywithmeas -> push_back(-9999.);
	    els_ckf_charge-> push_back( 9999 );
        }

        
//        ////////////////////
//        // Regular Vertex //
//        ////////////////////        
//        TransientTrack tt = theTTBuilder->build(el->gsfTrack());
//    
//        if ( firstGoodVertex!=vertexCollection->end() ) {
//            Measurement1D ip3D_regular = IPTools::absoluteImpactParameter3D(tt, *firstGoodVertex).second;
//            //
//            els_ip3d      -> push_back( ip3D_regular.value() );
//            els_ip3derr   -> push_back( ip3D_regular.error() );
//        } else {
//            //
//            els_ip3d      -> push_back( -999. );
//            els_ip3derr   -> push_back( -999. );
//        }

	
        //Impact Parameters
	els_ip3d   -> push_back( el->dB(pat::Electron::PV3D) ); 
	els_ip3derr-> push_back( el->edB(pat::Electron::PV3D) ); 
	els_ip2d   -> push_back( el->dB(pat::Electron::PV2D) ); 
	els_ip2derr-> push_back( el->edB(pat::Electron::PV2D) ); 
	//els_ip3d      -> push_back( el->ip3d() ); // miniAOD


        /////////////////
        // Hit Pattern //
        /////////////////

        //if( el_track->extra().isAvailable() ) {
        //} else {
        //}
    
	// Redesign according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/TrackingHitPatternRedesign
        const HitPattern& pattern = el_track->hitPattern();
        //const HitPattern& p_inner = el_track->trackerExpectedHitsInner(); 
        //const HitPattern& p_outer = el_track->trackerExpectedHitsOuter();

        els_exp_innerlayers -> push_back(pattern.numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS));
        els_exp_outerlayers -> push_back(pattern.numberOfLostHits(reco::HitPattern::MISSING_OUTER_HITS));
        els_valid_pixelhits -> push_back(pattern.numberOfValidPixelHits());
        els_lost_pixelhits  -> push_back(pattern.numberOfLostPixelHits(reco::HitPattern::TRACK_HITS)); // Not sure about this. Could be MISSING_INNER_HITS instead.
	els_nlayers         -> push_back(pattern.trackerLayersWithMeasurement());
	els_nlayers3D       -> push_back(pattern.pixelLayersWithMeasurement() + pattern.numberOfValidStripLayersWithMonoAndStereo());
	els_nlayersLost     -> push_back(pattern.trackerLayersWithoutMeasurement(reco::HitPattern::TRACK_HITS));

        /////////////////
        // Conversions //
        /////////////////

	const reco::Conversion* conv_ref = ConversionTools::matchedConversion(*el, *convs_h, beamSpot);
	float vertexFitProbability = -1.; 
	if(conv_ref) {
            const reco::Vertex &vtx = conv_ref->conversionVertex(); 
            if (vtx.isValid()) {
                vertexFitProbability = TMath::Prob( vtx.chi2(), vtx.ndof());
            } 
	}
	els_conv_vtx_prob         -> push_back( vertexFitProbability );
	//cout<<"Found electron with pt eta phi "<<el->p4().pt() <<" "<< el->p4().eta() <<" "<< el->p4().phi()<<" and vertexFitProbability "<<vertexFitProbability<<endl;


        //////////////////////////////
        // Flag For Vertex Fit Conversion Rejection //
        //////////////////////////////


        els_conv_vtx_flag        -> push_back( !el->passConversionVeto() ); // PAT variable: http://cmslxr.fnal.gov/lxr/source/PhysicsTools/PatAlgos/plugins/PATElectronProducer.cc#467

	//////////////////////
	// genMatch miniAOD //
	//////////////////////
	
	LorentzVector mc_p4(0,0,0,0);	 
	const reco::GenParticle * gen = el->genParticle();
	if (gen != 0) {
            mc_p4 = gen->p4();
            els_mc_patMatch_id      ->push_back( gen->pdgId()  );
            els_mc_patMatch_p4      ->push_back( mc_p4         );
            els_mc_patMatch_dr      ->push_back( ROOT::Math::VectorUtil::DeltaR(gen->p4(), el->p4())  );
	}
	else {
            els_mc_patMatch_id      ->push_back( -999   );
            els_mc_patMatch_p4      ->push_back( mc_p4  );
            els_mc_patMatch_dr      ->push_back( -999.  );
	}

	//////////////////////
	// mini-isolation   //
	//////////////////////

	// float minichiso     = 0.;
	// float mininhiso     = 0.;
	// float miniemiso     = 0.;
	// float minidbiso     = 0.;
	// elMiniIso(el, true, 0.0, minichiso, mininhiso, miniemiso, minidbiso);
	// els_miniIso_uncor   ->push_back( minichiso + mininhiso + miniemiso );
	// els_miniIso_ch      ->push_back( minichiso );
	// els_miniIso_nh      ->push_back( mininhiso );
	// els_miniIso_em      ->push_back( miniemiso );
	// els_miniIso_db      ->push_back( minidbiso );

        auto el2 = el->clone();
        auto miniiso = el2->miniPFIsolation();
        els_miniIso_uncor ->push_back(miniiso.chargedHadronIso() + miniiso.neutralHadronIso() + miniiso.photonIso());
        els_miniIso_ch    ->push_back(miniiso.chargedHadronIso()); 
        els_miniIso_nh    ->push_back(miniiso.neutralHadronIso()); 
        els_miniIso_em    ->push_back(miniiso.photonIso()); 
        els_miniIso_db    ->push_back(miniiso.puChargedHadronIso()); 
        delete el2;

	///////////////////////////
	// PFCluster isolation   //
	///////////////////////////

	els_ecalPFClusterIso ->push_back(   el->ecalPFClusterIso()  );
	els_hcalPFClusterIso ->push_back(   el->hcalPFClusterIso()  );



    } // end Loop on Electrons
  





    // Put the results into the event
    //
    iEvent.put(std::move(evt_nels), "evtnels");

    // Predefined ID descisions 
    //
    iEvent.put(std::move(els_category ), "elscategory" );
  
    // iEvent.put(std::move(passVetoId),   "passVetoId"   );
    // iEvent.put(std::move(passLooseId),  "passLooseId"  );
    // iEvent.put(std::move(passMediumId), "passMediumId" );
    // iEvent.put(std::move(passTightId),  "passTightId"  );
    // iEvent.put(std::move(passHEEPId),                "passHEEPId"  );
    iEvent.put(std::move(passVIDNonTrigMvaWP80Id),   "passVIDNonTrigMvaWP80Id"  );
    iEvent.put(std::move(passVIDNonTrigMvaWP90Id),   "passVIDNonTrigMvaWP90Id"  );
    iEvent.put(std::move(passVIDTrigMvaWP80Id),      "passVIDTrigMvaWP80Id"  );
    iEvent.put(std::move(passVIDTrigMvaWP90Id),      "passVIDTrigMvaWP90Id"  );
    iEvent.put(std::move(VIDNonTrigMvaValue),        "VIDNonTrigMvaValue"  );
    iEvent.put(std::move(VIDTrigMvaValue),           "VIDTrigMvaValue"  );
    iEvent.put(std::move(VIDNonTrigMvaCat),          "VIDNonTrigMvaCat"  );
    iEvent.put(std::move(VIDTrigMvaCat),             "VIDTrigMvaCat"  );
    iEvent.put(std::move(VIDSpring16GPMvaValue),     "VIDSpring16GPMvaValue"  );
    iEvent.put(std::move(VIDSpring16GPMvaCat),       "VIDSpring16GPMvaCat"  );
    iEvent.put(std::move(VIDSpring16HZZMvaValue),    "VIDSpring16HZZMvaValue"  );
    iEvent.put(std::move(VIDSpring16HZZMvaCat),      "VIDSpring16HZZMvaCat"  );
    iEvent.put(std::move(VIDFall17NoIsoMvaValue),    "VIDFall17NoIsoMvaValue"  );
    iEvent.put(std::move(VIDFall17NoIsoMvaCat),      "VIDFall17NoIsoMvaCat"  );
    iEvent.put(std::move(VIDFall17IsoMvaValue),    "VIDFall17IsoMvaValue"  );
    iEvent.put(std::move(VIDFall17IsoMvaCat),      "VIDFall17IsoMvaCat"  );
    iEvent.put(std::move(VIDFall17V2NoIsoMvaValue),    "VIDFall17V2NoIsoMvaValue"  );
    iEvent.put(std::move(VIDFall17V2NoIsoMvaCat),      "VIDFall17V2NoIsoMvaCat"  );
    iEvent.put(std::move(VIDFall17V2IsoMvaValue),    "VIDFall17V2IsoMvaValue"  );
    iEvent.put(std::move(VIDFall17V2IsoMvaCat),      "VIDFall17V2IsoMvaCat"  );

    // Track parameters
    //
    iEvent.put(std::move(els_dxyPV      ), "elsdxyPV"    );
    iEvent.put(std::move(els_dzPV       ), "elsdzPV"     );
    iEvent.put(std::move(els_chi2       ), "elschi2"      );
    iEvent.put(std::move(els_ndof       ), "elsndof"      );
    iEvent.put(std::move(els_d0Err      ), "elsd0Err"     );
    iEvent.put(std::move(els_z0Err      ), "elsz0Err"     );
    iEvent.put(std::move(els_ptErr      ), "elsptErr"     );
    iEvent.put(std::move(els_ptErrGsf   ), "elsptErrGsf"  );
    iEvent.put(std::move(els_ip3d       ), "elsip3d"      );
    iEvent.put(std::move(els_ip3derr    ), "elsip3derr"   );
    iEvent.put(std::move(els_ip2d       ), "elsip2d"      );
    iEvent.put(std::move(els_ip2derr    ), "elsip2derr"   );
  
    iEvent.put(std::move(els_validHits  ), "elsvalidHits" );
    iEvent.put(std::move(els_lostHits   ), "elslostHits"  );
    iEvent.put(std::move(els_charge     ), "elscharge"    );
    iEvent.put(std::move(els_trk_charge ), "elstrkcharge" );
    iEvent.put(std::move(els_sccharge   ), "elssccharge"  );

    // Supercluster parameters
    //
    iEvent.put(std::move(els_etaSC       ), "elsetaSC"       );
    iEvent.put(std::move(els_phiSC       ), "elsphiSC"       );
    iEvent.put(std::move(els_eSC         ), "elseSC"         );
    iEvent.put(std::move(els_eSCRaw      ), "elseSCRaw"      );
    iEvent.put(std::move(els_eSCPresh    ), "elseSCPresh"    );
    iEvent.put(std::move(els_etaSCwidth  ), "elsetaSCwidth"  );
    iEvent.put(std::move(els_phiSCwidth  ), "elsphiSCwidth"  );
    iEvent.put(std::move(els_eSeed       ), "elseSeed"       );
    iEvent.put(std::move(els_type        ), "elstype"        );

    // Corrections and uncertainties
    //
    iEvent.put(std::move(els_ecalEnergy         ), "elsecalEnergy"         );
    iEvent.put(std::move(els_ecalEnergyError    ), "elsecalEnergyError"    );
    iEvent.put(std::move(els_trackMomentumError ), "elstrackMomentumError" );

    // Electron ID
    //
    iEvent.put(std::move(els_sigmaIPhiIPhi      ), "elssigmaIPhiIPhi"      );
    iEvent.put(std::move(els_sigmaIEtaIEta      ), "elssigmaIEtaIEta"      );
    iEvent.put(std::move(els_hOverE             ), "elshOverE"             );
    iEvent.put(std::move(els_full5x5_hOverE      ), "elsfull5x5hOverE"      );

    iEvent.put(std::move(els_eOverPIn                      ), "elseOverPIn"                      );
    iEvent.put(std::move(els_eOverPOut                     ), "elseOverPOut"                     );
    iEvent.put(std::move(els_fbrem                         ), "elsfbrem"                         );
    iEvent.put(std::move(els_dEtaIn                        ), "elsdEtaIn"                        );
    iEvent.put(std::move(els_dEtaOut                       ), "elsdEtaOut"                       );
    iEvent.put(std::move(els_dPhiIn                        ), "elsdPhiIn"                        );
    iEvent.put(std::move(els_dPhiOut                       ), "elsdPhiOut"                       );
    iEvent.put(std::move(els_isGsfCtfScPixChargeConsistent ), "elsisGsfCtfScPixChargeConsistent" );

    // Lorentz vectors
    //
    iEvent.put(std::move(els_p4     ), "elsp4"    );
    iEvent.put(std::move(els_trk_p4 ), "elstrkp4" );
    iEvent.put(std::move(els_p4In   ), "elsp4In"  );
    iEvent.put(std::move(els_p4Out  ), "elsp4Out" );

    // Vertex
    //
    iEvent.put(std::move(els_vertex_p4), "elsvertexp4");
    iEvent.put(std::move(els_trk_vertex_p4), "elstrkvertexp4");

    // Isolation
    //
    iEvent.put(std::move(els_hcalDepth1TowerSumEt ), "elshcalDepth1TowerSumEt" );
    iEvent.put(std::move(els_tkIso                ), "elstkIso"                );
    iEvent.put(std::move(els_ecalIso              ), "elsecalIso"              );
    iEvent.put(std::move(els_hcalIso              ), "elshcalIso"              );

    iEvent.put(std::move(els_tkIso04                ), "elstkIso04"                );
    iEvent.put(std::move(els_ecalIso04              ), "elsecalIso04"              );
    iEvent.put(std::move(els_hcalIso04              ), "elshcalIso04"              );



    iEvent.put(std::move(els_pfChargedHadronIso ), "elspfChargedHadronIso" );
    iEvent.put(std::move(els_pfNeutralHadronIso ), "elspfNeutralHadronIso" );
    iEvent.put(std::move(els_pfPhotonIso        ), "elspfPhotonIso"        );
    iEvent.put(std::move(els_pfPUIso            ), "elspfPUIso"            );

    //Hit Pattern Information
    iEvent.put(std::move(els_valid_pixelhits ), "elsvalidpixelhits" );
    iEvent.put(std::move(els_lost_pixelhits  ), "elslostpixelhits"  );
    iEvent.put(std::move(els_nlayers         ), "elsnlayers"        );
    iEvent.put(std::move(els_nlayers3D       ), "elsnlayers3D"      );
    iEvent.put(std::move(els_nlayersLost     ), "elsnlayersLost"    );
    iEvent.put(std::move(els_exp_innerlayers ), "elsexpinnerlayers" );
    iEvent.put(std::move(els_exp_outerlayers ), "elsexpouterlayers" );

    //CTF track info
    //
    iEvent.put(std::move(els_trkdr           ), "elstrkdr"         );
    iEvent.put(std::move(els_trkshFrac       ), "elstrkshFrac"     );
    iEvent.put(std::move(els_ckf_chi2        ),"elsckfchi2"        );
    iEvent.put(std::move(els_ckf_ndof        ),"elsckfndof"        );
    iEvent.put(std::move(els_ckf_laywithmeas ),"elsckflaywithmeas" );
    iEvent.put(std::move(els_ckf_charge      ),"elsckfcharge"      );

    //conversion

    iEvent.put(std::move(els_conv_vtx_flag        ), "elsconvvtxflag"        );
    iEvent.put(std::move(els_conv_vtx_prob        ), "elsconvvtxprob"        );

    /////////////////////////
    // Added for miniAOD   //
    /////////////////////////

    // genParticle matching from miniAOD
    iEvent.put(std::move( els_mc_patMatch_id        ), "elsmcpatMatchid"        	);
    iEvent.put(std::move( els_mc_patMatch_p4        ), "elsmcpatMatchp4"         );
    iEvent.put(std::move( els_mc_patMatch_dr        ), "elsmcpatMatchdr"         );
    iEvent.put(std::move(els_sigmaIPhiIPhi_full5x5  ), "elssigmaIPhiIPhifull5x5" );
    iEvent.put(std::move(els_sigmaEtaEta_full5x5    ), "elssigmaEtaEtafull5x5"   );
    iEvent.put(std::move(els_sigmaIEtaIEta_full5x5  ), "elssigmaIEtaIEtafull5x5" );
    iEvent.put(std::move(els_r9_full5x5             ), "elsr9full5x5"            );
    iEvent.put(std::move(els_r9             ), "elsr9"            );
    iEvent.put(std::move(els_e1x5_full5x5           ), "else1x5full5x5"          );
    iEvent.put(std::move(els_e5x5_full5x5           ), "else5x5full5x5"          );
    iEvent.put(std::move(els_e2x5Max_full5x5        ), "else2x5Maxfull5x5"       ); 

    iEvent.put(std::move(els_jetNDauChargedMVASel        ), "elsjetNDauChargedMVASel"       ); 
    iEvent.put(std::move(els_ptRatio        ), "elsptRatio"       ); 
    iEvent.put(std::move(els_ptRel        ), "elsptRel"       ); 
    iEvent.put(std::move(els_jetBTagCSV        ), "elsjetBTagCSV"       ); 
    
    iEvent.put(std::move(els_miniIso_uncor       ), "elsminiIsouncor"    );
    iEvent.put(std::move(els_miniIso_ch       ), "elsminiIsoch"    );
    iEvent.put(std::move(els_miniIso_nh       ), "elsminiIsonh"    );
    iEvent.put(std::move(els_miniIso_em       ), "elsminiIsoem"    );
    iEvent.put(std::move(els_miniIso_db       ), "elsminiIsodb"    );

    iEvent.put(std::move(els_miniRelIso_chg       ), "elsminiRelIsochg"    );
    iEvent.put(std::move(els_miniRelIso_all       ), "elsminiRelIsoall"    );
    
    iEvent.put(std::move(els_ecalPFClusterIso       ), "elsecalPFClusterIso"    );
    iEvent.put(std::move(els_hcalPFClusterIso       ), "elshcalPFClusterIso"    );
    
    ///////////////////////////
    // Added for calibration //
    ///////////////////////////
    iEvent.put(std::move(els_isEcalDriven), "elsisEcalDriven");
    iEvent.put(std::move(els_isTrackerDriven), "elsisTrackerDriven");
    iEvent.put(std::move(els_isEB), "elsisEB");

    iEvent.put(std::move(els_scSeedEta), "elsscSeedEta");

}

//----------------------------------------------------------------------------
// Electron Id classification function (a flag for the Sani type class)
//----------------------------------------------------------------------------
int ElectronMaker::classify(const RefToBase<pat::Electron> &electron) {

    double eOverP = electron->eSuperClusterOverP();
    double fbrem = electron->fbrem();
  
    int cat;
    if((electron->isEB() && fbrem<0.06) || (electron->isEE() && fbrem<0.1)) 
        cat=1;
    else if (eOverP < 1.2 && eOverP > 0.8) 
        cat=0;
    else 
        cat=2;
  
    return cat;

}

//little labour saving function to get the reference to the ValueMap
template<typename T> const ValueMap<T>& ElectronMaker::getValueMap(const Event& iEvent, InputTag& inputTag){
    Handle<ValueMap<T> > handle;
    iEvent.getByLabel(inputTag,handle);
    return *(handle.product());
}

double ElectronMaker::electronIsoValuePF(const GsfElectron& el, const Vertex& vtx, float coner, float minptn, float dzcut, float footprintdr, float gammastripveto, float elestripveto, int filterId){

    float pfciso = 0.;
    float pfniso = 0.;
    float pffootprint = 0.;
    float pfjurveto = 0.;
    float pfjurvetoq = 0.;

    TrackRef siTrack     = el.closestCtfTrackRef();
    // TrackRef siTrack     = el.closestTrack();
    GsfTrackRef gsfTrack = el.gsfTrack();

    if (gsfTrack.isNull() && siTrack.isNull()) return -9999.;

    float eldz = gsfTrack.isNonnull() ? gsfTrack->dz(vtx.position()) : siTrack->dz(vtx.position());
    float eleta = el.eta();

    for (PFCandidateCollection::const_iterator pf=pfCand_h->begin(); pf<pfCand_h->end(); ++pf){

        float pfeta = pf->eta();    
        float dR = deltaR(pfeta, pf->phi(), eleta, el.phi());
        if (dR>coner) continue;

        float deta = fabs(pfeta - eleta);
        int pfid = abs(pf->pdgId());
        float pfpt = pf->pt();

        if (filterId!=0 && filterId!=pfid) continue;

        if (pf->charge()==0) {
            //neutrals
            if (pfpt>minptn) {
                pfniso+=pfpt;
                if (dR<footprintdr && pfid==130) pffootprint+=pfpt;
                if (deta<gammastripveto && pfid==22)  pfjurveto+=pfpt;
            }
        } else {
            //charged  
            //avoid double counting of electron itself
            //if either the gsf or the ctf track are shared with the candidate, skip it
            const TrackRef pfTrack  = pf->trackRef();
            if (siTrack.isNonnull()  && pfTrack.isNonnull() && siTrack.key()==pfTrack.key()) continue;
            //below pfid==1 is commented out: in some cases the pfCand has a gsf even if it is not an electron... this is to improve the sync with MIT
            if (/*pfid==11 &&*/ pf->gsfTrackRef().isNonnull()) {
                if (gsfTrack.isNonnull() && gsfTrack.key()==pf->gsfTrackRef().key()) continue;
            } 
            //check electrons with gsf track
            if (pfid==11 && pf->gsfTrackRef().isNonnull()) {
                if(fabs(pf->gsfTrackRef()->dz(vtx.position()) - eldz )<dzcut) {//dz cut
                    pfciso+=pfpt;
                    if (deta<elestripveto && pfid==11) pfjurvetoq+=pfpt;
                }
                continue;//and avoid double counting
            }
            //then check anything that has a ctf track
            if (pfTrack.isNonnull()) {//charged (with a ctf track)
                if(fabs( pfTrack->dz(vtx.position()) - eldz )<dzcut) {//dz cut
                    pfciso+=pfpt;
                    if (deta<elestripveto && pfid==11) pfjurvetoq+=pfpt;
                }
            }
        } 
    }
    return pfciso+pfniso-pffootprint-pfjurveto-pfjurvetoq;
}

void ElectronMaker::elIsoCustomCone(edm::View<pat::Electron>::const_iterator& el, float dr, bool useVetoCones, float ptthresh, float &chiso, float &nhiso, float &emiso, float & dbiso){
    chiso     = 0.;
    nhiso     = 0.;
    emiso     = 0.;
    dbiso     = 0.;
    float deadcone_ch = 0.;
    float deadcone_pu = 0.;
    float deadcone_ph = 0.;

    double phi = el->p4().Phi();
    double eta = el->p4().Eta();
    double pi = M_PI;

    // veto cones only in the endcap for electrons
    if (useVetoCones && fabs(el->superCluster()->eta()) > 1.479) { 
        deadcone_ch = 0.015;
        deadcone_pu = 0.015;
        deadcone_ph = 0.08;
    }
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
        if ( fabs(id)==130 && (pt > ptthresh) ) nhiso+=pt;
        if ( fabs(id)==22 && (pt > ptthresh) && (!useVetoCones || thisDR > deadcone_ph) ) emiso+=pt;
    }

    return;
}

void ElectronMaker::elMiniIso(edm::View<pat::Electron>::const_iterator& el, bool useVetoCones, float ptthresh, float &chiso, float &nhiso, float &emiso, float &dbiso){

    float pt = el->p4().pt();
    float dr = 0.2;
    if (pt>50) dr = 10./pt;
    if (pt>200) dr = 0.05;
    elIsoCustomCone(el,dr,useVetoCones,ptthresh, chiso, nhiso, emiso, dbiso);
    return;
}

//define this as a plug-in
DEFINE_FWK_MODULE(ElectronMaker);
