
import FWCore.ParameterSet.Config as cms
import CMS3.NtupleMaker.configProcessName as configProcessName

pfmetMaker = cms.EDProducer("PFMETMaker",
                            aliasPrefix = cms.untracked.string("evt_old"), # this is superceded by Modified MET below
                            pfMetInputTag_ = cms.InputTag("slimmedMETs","",configProcessName.name),
                            onlySaveTwoVector   = cms.bool(False),
                            doUncertainties   = cms.bool(True)
)

pfmetMakerModifiedMET = cms.EDProducer("PFMETMaker",
                            aliasPrefix = cms.untracked.string("evt"),
                            pfMetInputTag_ = cms.InputTag("slimmedMETsModifiedMET","","CMS3"),
                            onlySaveTwoVector   = cms.bool(False),
                            doUncertainties   = cms.bool(True)
)

pfmetpuppiMaker = cms.EDProducer("PFMETMaker",
                            aliasPrefix = cms.untracked.string("evt_puppi"),
                            pfMetInputTag_ = cms.InputTag("slimmedMETsPuppi","",configProcessName.name),
                            onlySaveTwoVector   = cms.bool(False),
                            doUncertainties   = cms.bool(True)
)


