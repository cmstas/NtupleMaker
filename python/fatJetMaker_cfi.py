import FWCore.ParameterSet.Config as cms

#ca10subJetMaker = cms.EDProducer("FatJetMaker",
#  aliasPrefix = cms.untracked.string("ca10pfchs"),
#  NjettinessPrefix = cms.untracked.string("NjettinessCA10CHS"),
#  LinksPrefix = cms.untracked.string("ca10PFJetsCHS"),
#  pfJetsInputTag                   = cms.InputTag("selectedPatJetsCA10PFCHS","","CMS3"),
#)
#ca12subJetMaker = cms.EDProducer("FatJetMaker",
#  aliasPrefix = cms.untracked.string("ca12pfchs"),
#  NjettinessPrefix = cms.untracked.string("NjettinessCA12CHS"),
#  LinksPrefix = cms.untracked.string("ca12PFJetsCHS"),
#  pfJetsInputTag                   = cms.InputTag("selectedPatJetsCA12PFCHS","","CMS3"),
#)
ak10subJetMaker = cms.EDProducer("FatJetMaker",
  aliasPrefix = cms.untracked.string("ak10pfchs"),
  NjettinessPrefix = cms.untracked.string("NjettinessAK10CHS"),
  LinksPrefix = cms.untracked.string("ak10PFJetsCHS"),
  pfJetsInputTag                   = cms.InputTag("selectedPatJetsAK10PFCHS","","CMS3"),
)
ak10subJetMakerPuppi = cms.EDProducer("FatJetMaker",
  aliasPrefix = cms.untracked.string("ak10pfpuppi"),
  NjettinessPrefix = cms.untracked.string("NjettinessAK10Puppi"),
  LinksPrefix = cms.untracked.string("ak10PFJetsPuppi"),
  pfJetsInputTag                   = cms.InputTag("selectedPatJetsAK10PFPuppi","","CMS3"),
)
#
#ak12subJetMaker = cms.EDProducer("FatJetMaker",
#  aliasPrefix = cms.untracked.string("ak12pfchs"),
#  NjettinessPrefix = cms.untracked.string("NjettinessAK12CHS"),
##  LinksPrefix = cms.untracked.string("ak12PFJetsCHS"),
#  pfJetsInputTag                   = cms.InputTag("selectedPatJetsAK12PFCHS","","CMS3"),
#)


