import FWCore.ParameterSet.Config as cms
import CMS3.NtupleMaker.configProcessName as configProcessName

photonMaker = cms.EDProducer("PhotonMaker",
                             aliasPrefix = cms.untracked.string("photons"),
                             minEt       = cms.double(10.), #gev, min to keep
                             photonsInputTag = cms.InputTag("slimmedPhotons"),
                             )


photonBeforeGSFixMaker = photonMaker.clone()
photonBeforeGSFixMaker.aliasPrefix = cms.untracked.string("photonsBeforeGSFix")
photonBeforeGSFixMaker.electronsInputTag   = cms.InputTag("slimmedPhotonsBeforeGSFix")
photonBeforeGSFixMaker.useVID   = cms.bool(False)
photonBeforeGSFixMaker.ebReducedRecHitCollectionTag = cms.InputTag("reducedEgammaBeforeGSFix:reducedEBRecHits")
photonBeforeGSFixMaker.eeReducedRecHitCollectionTag = cms.InputTag("reducedEgammaBeforeGSFix:reducedEERecHits")
photonBeforeGSFixMaker.esReducedRecHitCollectionTag = cms.InputTag("reducedEgammaBeforeGSFix:reducedESRecHits")
