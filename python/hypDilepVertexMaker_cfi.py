import FWCore.ParameterSet.Config as cms

hypDilepVertexMaker = cms.EDProducer("HypDilepVertexMaker",
	aliasPrefix = cms.untracked.string("hyp_FVFit"),
                                   recomuonsInputTag = cms.InputTag("muons"),
                                   cms2muonsInputTag = cms.InputTag("muonMaker"),
                                   recoelectronsInputTag = cms.InputTag("gedGsfElectrons"),
                                   cms2electronsInputTag = cms.InputTag("electronMaker"),
                                   hypInputTag = cms.InputTag("hypDilepMaker")
                                   )
