import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.ak5JetID_cfi import ak5JetID

prunedUncorrectedCMS2Jets = cms.EDProducer("JetCollectionPruner",
                                           inputUncorrectedJPTJetCollection = cms.InputTag("JetPlusTrackZSPCorJetAntiKt5"),
                                           inputUncorrectedPFJetCollection  = cms.InputTag("ak5PFJets"),
                                           inputUncorrectedPFCHSJetCollection  = cms.InputTag("ak5PFJetsCHS"),
                                           inputUncorrectedTrkJetCollection = cms.InputTag("ak5TrackJets"),
                                           uncorrectedJPTJetPtCut           = cms.double(8.0), ##cut on uncorrected JPT jets!!!!!
                                           uncorrectedTrkJetPtCut           = cms.double(5.0),       
										 # set to 0 for MVA Met
                                           uncorrectedPFJetPtCut            = cms.double(0.0),
                                           uncorrectedPFCHSJetPtCut            = cms.double(0.0)
)                                         
