import FWCore.ParameterSet.Config as cms

fixedGridRhoAllMaker = cms.EDProducer("EnergyDensityMaker",
                                      input = cms.InputTag("fixedGridRhoAll","", "HLT"),
                                      alias = cms.untracked.string("evt_fixgrid_all_rho"))

fixedGridRhoFastJetAllMaker = cms.EDProducer("EnergyDensityMaker",
                                             input = cms.InputTag("fixedGridRhoFastjetAll","", "HLT"),
                                             alias = cms.untracked.string("evt_fixgridfastjet_all_rho"))

fixedGridRhoFastJetAllCaloMaker = cms.EDProducer("EnergyDensityMaker",
                                             input = cms.InputTag("fixedGridRhoFastjetAllCalo","", "HLT"),
                                             alias = cms.untracked.string("evt_fixgridfastjet_allcalo_rho"))

fixedGridRhoFastJetCentralCaloMaker = cms.EDProducer("EnergyDensityMaker",
                                             input = cms.InputTag("fixedGridRhoFastjetCentralCalo","", "HLT"),
                                             alias = cms.untracked.string("evt_fixgridfastjet_centralcalo_rho"))
fixedGridRhoFastJetCentralChargedPileUpMaker = cms.EDProducer("EnergyDensityMaker",
                                             input = cms.InputTag("fixedGridRhoFastjetCentralChargedPileUp","", "HLT"),
                                             alias = cms.untracked.string("evt_fixgridfastjet_centralchargedpileup_rho"))
fixedGridRhoFastJetCentralNeutralMaker = cms.EDProducer("EnergyDensityMaker",
                                             input = cms.InputTag("fixedGridRhoFastjetCentralNeutral","", "HLT"),
                                             alias = cms.untracked.string("evt_fixgridfastjet_centralneutral_rho"))
