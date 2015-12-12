import FWCore.ParameterSet.Config as cms

genJetMaker = cms.EDProducer("GenJetMaker", 
   genJetsInputTag = cms.InputTag("slimmedGenJets"),
   genJetMinPtCut  = cms.double(10.0),
   aliasPostfix = cms.untracked.string("NoMuNoNu")
)

genak10JetMaker = cms.EDProducer("GenJetMaker", 
   genJetsInputTag = cms.InputTag("selectedPatJetsAK10PFCHS","genJets","CMS3"),
   genJetMinPtCut  = cms.double(10.0),
   aliasPostfix = cms.untracked.string("ak10")
)
genak10JetMakerPuppi = cms.EDProducer("GenJetMaker", 
   genJetsInputTag = cms.InputTag("selectedPatJetsAK10PFPuppi","genJets","CMS3"),
   genJetMinPtCut  = cms.double(10.0),
   aliasPostfix = cms.untracked.string("ak10puppi")
)
#
#genak12JetMaker = cms.EDProducer("GenJetMaker", 
#   genJetsInputTag = cms.InputTag("selectedPatJetsAK12PFCHS","genJets","CMS3"),
#   genJetMinPtCut  = cms.double(10.0),
#   aliasPostfix = cms.untracked.string("ak12")
#)
#genca10JetMaker = cms.EDProducer("GenJetMaker", 
#   genJetsInputTag = cms.InputTag("selectedPatJetsCA10PFCHS","genJets","CMS3"),
#   genJetMinPtCut  = cms.double(10.0),
#   aliasPostfix = cms.untracked.string("ca10")
#)
#genca12JetMaker = cms.EDProducer("GenJetMaker", 
#   genJetsInputTag = cms.InputTag("selectedPatJetsCA12PFCHS","genJets","CMS3"),
#   genJetMinPtCut  = cms.double(10.0),
#   aliasPostfix = cms.untracked.string("ca12")
#)

