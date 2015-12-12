import FWCore.ParameterSet.Config as cms
from Configuration.EventContent.EventContent_cff   import *

import CMS3.NtupleMaker.configProcessName as configProcessName
configProcessName.name="PAT"

# CMS3
process = cms.Process("CMS3")

# Version Control For Python Configuration Files
process.configurationMetadata = cms.untracked.PSet(
        version    = cms.untracked.string('$Revision: 1.11 $'),
        annotation = cms.untracked.string('CMS3'),
        name       = cms.untracked.string('CMS3 test configuration')
)

# load event level configurations
process.load('Configuration/EventContent/EventContent_cff')
process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")

# services
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.GlobalTag.globaltag = "MCRUN2_74_V9A"
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.cerr.threshold  = ''
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( allowUnscheduled = cms.untracked.bool(True),SkipEvent = cms.untracked.vstring('ProductNotFound') )

process.out = cms.OutputModule("PoolOutputModule",
  fileName     = cms.untracked.string('ntuple.root'),
  dropMetaData = cms.untracked.string("NONE")
)
process.outpath = cms.EndPath(process.out)

#load cff and third party tools
from JetMETCorrections.Configuration.DefaultJEC_cff import *
from JetMETCorrections.Configuration.JetCorrectionServices_cff import *
from JMEAnalysis.JetToolbox.jetToolbox_cff import *
#from JetMETCorrections.Configuration.JetCorrectionProducers_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducersDefault_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducers_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducersAllAlgos_cff import *
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
from RecoJets.JetProducers.fixedGridRhoProducerFastjet_cfi import *
process.fixedGridRhoFastjetAll = fixedGridRhoFastjetAll.clone(pfCandidatesTag = 'packedPFCandidates')

#Electron Identification for PHYS 14
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *  
from PhysicsTools.SelectorUtils.centralIDRegistry import central_id_registry
process.load("RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cfi")
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectrons',"",configProcessName.name)
process.egmGsfElectronIDSequence = cms.Sequence(process.egmGsfElectronIDs)
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff']
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

#process.globalPixelSeeds.OrderedHitsFactoryPSet.maxElement = cms.uint32(100000)
#process.gsfElectrons.MaxElePtForOnlyMVA = cms.double(50.0)

# Load Ntuple producer cff
process.load("CMS3.NtupleMaker.cms3CoreSequences_cff")
process.load("CMS3.NtupleMaker.cms3GENSequence_cff")
process.load("CMS3.NtupleMaker.cms3PFSequence_cff")

# Hypothesis cuts
process.hypDilepMaker.TightLepton_PtCut  = cms.double(10.0)
process.hypDilepMaker.LooseLepton_PtCut  = cms.double(10.0)

#Options for Input
process.source = cms.Source("PoolSource",
                            #fileNames = cms.untracked.vstring('file:///home/users/gzevi/ntupling/CMSSW_7_4_1/src/CMS3/NtupleMaker/QCD_Pt_1400to1800_Asympt50ns_MCRUN2_74_V9A-v1.root')
                            # fileNames = cms.untracked.vstring('file:/hadoop/cms/phedex/store/mc/RunIISpring15DR74/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v1/00000/20AD8065-31FD-E411-9D75-00259073E2F2.root')
                            fileNames = cms.untracked.vstring(
                                'file:/nfs-7/userdata/cwelke/TTTo2L2Nu_13TeV-powheg_MINIAODSIM_Asympt50ns_MCRUN2_74_V9A-v2/04C26C81-A30C-E511-8A0D-B083FED76C6C.root',
                                "file:/hadoop/cms/phedex/store/mc/RunIISpring15DR74/TTTo2L2Nu_13TeV-powheg/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v2/60000/00EFF2EE-E60B-E511-B422-B499BAAC0A22.root",
                                "file:/hadoop/cms/phedex/store/mc/RunIISpring15DR74/TTTo2L2Nu_13TeV-powheg/MINIAODSIM/Asympt50ns_MCRUN2_74_V9A-v2/60000/02C1F466-EE0B-E511-877D-00074305CC91.root",
                            )
)
process.source.noEventSort = cms.untracked.bool( True )

#Max Events
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

#Branches 
process.out.outputCommands = cms.untracked.vstring( 'keep *' )

#Run corrected MET maker

#configurable options =======================================================================
runOnData=False #data/MC switch
usePrivateSQlite=False #use external JECs (sqlite file)
useHFCandidates=False #create an additionnal NoHF slimmed MET collection if the option is set to false
applyResiduals=False #application of residual corrections. Have to be set to True once the 13 TeV residual corrections are available. False to be kept meanwhile. Can be kept to False later for private tests or for analysis checks and developments (not the official recommendation!).
#===================================================================

if usePrivateSQlite:
    from CondCore.DBCommon.CondDBSetup_cfi import *
    import os
    era="Summer15_50nsV4_MC"
    process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
                               connect = cms.string( "sqlite_file:"+era+".db" ),
                               toGet =  cms.VPSet(
            cms.PSet(
                record = cms.string("JetCorrectionsRecord"),
                tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK4PF"),
                label= cms.untracked.string("AK4PF")
                ),
            cms.PSet(
                record = cms.string("JetCorrectionsRecord"),
                tag = cms.string("JetCorrectorParametersCollection_"+era+"_AK4PFchs"),
                label= cms.untracked.string("AK4PFchs")
                ),
            )
                               )
    process.es_prefer_jec = cms.ESPrefer("PoolDBESSource",'jec')

### =====================================================================================================


### ---------------------------------------------------------------------------
### Removing the HF from the MET computation
### ---------------------------------------------------------------------------
if not useHFCandidates:
    process.noHFCands = cms.EDFilter("CandPtrSelector",
                                     src=cms.InputTag("packedPFCandidates"),
                                     cut=cms.string("abs(pdgId)!=1 && abs(pdgId)!=2 && abs(eta)<3.0")
                                     )

#jets are rebuilt from those candidates by the tools, no need to do anything else
### =================================================================================

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD


#default configuration for miniAOD reprocessing, change the isData flag to run on data
#for a full met computation, remove the pfCandColl input
runMetCorAndUncFromMiniAOD(process,
                           isData=runOnData,
                           )

if not useHFCandidates:
    runMetCorAndUncFromMiniAOD(process,
                               isData=runOnData,
                               pfCandColl=cms.InputTag("noHFCands"),
                               postfix="NoHF"
                               )

### -------------------------------------------------------------------
### the lines below remove the L2L3 residual corrections when processing data
### -------------------------------------------------------------------
if not applyResiduals:
    process.patPFMetT1T2Corr.jetCorrLabelRes = cms.InputTag("L3Absolute")
    process.patPFMetT1T2SmearCorr.jetCorrLabelRes = cms.InputTag("L3Absolute")
    process.patPFMetT2Corr.jetCorrLabelRes = cms.InputTag("L3Absolute")
    process.patPFMetT2SmearCorr.jetCorrLabelRes = cms.InputTag("L3Absolute")
    process.shiftedPatJetEnDown.jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3Corrector")
    process.shiftedPatJetEnUp.jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3Corrector")

    if not useHFCandidates:
          process.patPFMetT1T2CorrNoHF.jetCorrLabelRes = cms.InputTag("L3Absolute")
          process.patPFMetT1T2SmearCorrNoHF.jetCorrLabelRes = cms.InputTag("L3Absolute")
          process.patPFMetT2CorrNoHF.jetCorrLabelRes = cms.InputTag("L3Absolute")
          process.patPFMetT2SmearCorrNoHF.jetCorrLabelRes = cms.InputTag("L3Absolute")
          process.shiftedPatJetEnDownNoHF.jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3Corrector")
          process.shiftedPatJetEnUpNoHF.jetCorrLabelUpToL3Res = cms.InputTag("ak4PFCHSL1FastL2L3Corrector")
### ------------------------------------------------------------------

# end Run corrected MET maker

if not runOnData:
   process.load("CMS3.NtupleMaker.genJetMaker_cfi")
# #Run jet tool box


process.out.outputCommands = cms.untracked.vstring( 'drop *' )
process.out.outputCommands.extend(cms.untracked.vstring('keep *_*Maker*_*_CMS3*'))
process.out.outputCommands.extend(cms.untracked.vstring('drop *_cms2towerMaker*_*_CMS3*'))
process.out.outputCommands.extend(cms.untracked.vstring('drop CaloTowers*_*_*_CMS3*'))
jetToolbox( process, 'ak10', 'ak10JetSubs', 'out', 
            PUMethod='CHS',
            Cut = 'pt > 100',
            #runOnMC=not runOnData,
            addPrunedSubjets=True, 
            addSoftDropSubjets=True,
            addTrimming=True,
            addPruning=True,
            addSoftDrop=True,
            addFiltering=True,
            addNsub=True,
            miniAOD=True,
            JETCorrLevels=['None'] ) 
jetToolbox( process, 'ak10', 'ak10JetSubs', 'out', 
            PUMethod='Puppi',
            Cut = 'pt > 100',
            #runOnMC=not runOnData,
            addPrunedSubjets=True, 
            addSoftDropSubjets=True,
            addTrimming=True,
            addPruning=True,
            addSoftDrop=True,
            addFiltering=True,
            addNsub=True,
            miniAOD=True,
            JETCorrLevels=['None'] ) 



process.p = cms.Path( 
  process.metFilterMaker *
  process.hcalNoiseSummaryMaker *
  process.egmGsfElectronIDSequence *     
  process.beamSpotMaker *
  process.vertexMaker *
  process.secondaryVertexMaker *
  process.eventMaker *
  process.pfCandidateMaker *
  process.isoTrackMaker *
  process.recoConversionMaker *
  process.electronMaker *
  process.muonMaker *
  process.pfJetMaker *
  process.pfJetPUPPIMaker *
  process.METToolboxJetMaker *
  process.subJetMaker *
  process.ak10subJetMaker *
  process.ak10subJetMakerPuppi *
  process.genak10JetMaker*
  process.genak10JetMakerPuppi*
  process.pfmetMaker *
  process.T1pfmetMaker *
  process.T1pfmetNoHFMaker *
  process.hltMakerSequence *
  process.pftauMaker *
  process.photonMaker *
  process.genMaker *
  process.genJetMaker *
  process.muToTrigAssMaker *  # requires muonMaker
  process.elToTrigAssMaker *  # requires electronMaker
  process.candToGenAssMaker * # requires electronMaker, muonMaker, pfJetMaker, photonMaker
  process.pdfinfoMaker *
  process.puSummaryInfoMaker *
  process.recoConversionMaker *
  process.miniAODrhoSequence *
  process.hypDilepMaker
)
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.eventMaker.isData                        = cms.bool(False)
#process.luminosityMaker.isData                   = process.eventMaker.isData

