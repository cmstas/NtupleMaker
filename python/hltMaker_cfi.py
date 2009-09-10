import FWCore.ParameterSet.Config as cms

hltMaker8e29 = cms.EDProducer("HLTMaker",
    processName = cms.untracked.string("HLT8E29"),
    fillTriggerObjects = cms.untracked.bool(True),
    prunedTriggerNames = cms.untracked.vstring(
        # wildcards
        "*Mu*",
        "*Ele*",
        # jets
        "HLT_Jet50U",
        "HLT_DiJetAve15U",
        "HLT_DiJetAve30U",
        "HLT_FwdJet20U",
        "HLT_QuadJet15U",
        # met
        "HLT_MET45",
        "HLT_MET100",
        # ht
        "HLT_HT100U",
        # muons
        #"HLT_L1Mu20",
        #"HLT_L2Mu9",
        #"HLT_Mu3",
        #"HLT_Mu9",
        #"HLT_DoubleMu0",
        #"HLT_DoubleMu3",
        #"HLT_L1DoubleMuOpen",
        #"HLT_IsoMu3",
        # electrons
        #"HLT_Ele10_LW_L1R",
        #"HLT_Ele15_LW_L1R",
        #"HLT_DoubleEle5_SW_L1R",
        # photons
        "HLT_Photon15_L1R",
        "HLT_DoublePhoton5_eeRes_L1R",
        "HLT_DoublePhoton10_L1R",
        # taus
        "HLT_SingleLooseIsoTau20",
        "HLT_DoubleLooseIsoTau15",
    )
)

hltMaker1e31 = cms.EDProducer("HLTMaker",
    processName = cms.untracked.string("HLT"),
    fillTriggerObjects = cms.untracked.bool(True),
    prunedTriggerNames = cms.untracked.vstring(
        # wildcards
        "*Mu*",
        "*Ele*",
        # jets
        "HLT_Jet110",
        "HLT_DiJetAve15U",
        "HLT_DiJetAve30U",
        "HLT_DiJetAve50U",
        "HLT_DiJetAve70U",
        "HLT_DiJetAve130U",
        "HLT_FwdJet40",
        "HLT_QuadJet30",
        # met
        "HLT_MET60",
        "HLT_MET100",
        # ht
        "HLT_HT200",
        "HLT_HT300_MHT100",
        # muons
        #"HLT_L1Mu30",
        #"HLT_L2Mu11",
        #"HLT_Mu9",
        #"HLT_Mu15",
        #"HLT_DoubleMu0",
        #"HLT_DoubleMu3",
        #"HLT_IsoMu9",
        # electrons
        #"HLT_Ele15_SW_LooseTrackIso_L1R",
        #"HLT_Ele15_SW_EleId_L1R",
        #"HLT_Ele20_SW_L1R",
        #"HLT_DoubleEle10_SW_L1R",
        # photons
        "HLT_Photon25_L1R",
        "HLT_DoublePhoton15_L1R",
        "HLT_DoublePhoton15_VeryLooseEcalIso_L1R",
        # taus
        "HLT_SingleIsoTau30_Trk5",
        "HLT_DoubleLooseIsoTau15_Trk5",
    )
)