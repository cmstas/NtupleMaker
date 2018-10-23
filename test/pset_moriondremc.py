import FWCore.ParameterSet.Config as cms

# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideAboutPythonConfigFile#VarParsing_Documentation
# Allow command line options like
#     cmsRun main_pset.py data=True prompt=True   # prompt data
#     cmsRun main_pset.py data=False               # MC
#     cmsRun main_pset.py fastsim=True             # fastsim
import FWCore.ParameterSet.VarParsing as VarParsing
opts = VarParsing.VarParsing('python')
vpbool = VarParsing.VarParsing.varType.bool
opts.register('data'    , False , mytype=vpbool)
opts.register('prompt'  , False , mytype=vpbool)
opts.register('fastsim' , False , mytype=vpbool)
opts.register('relval'  , False , mytype=vpbool)
opts.register('triginfo'  , False , mytype=vpbool)
opts.parseArguments()
# be smart. if fastsim, it's obviously MC
# if it's MC, it's obviously not prompt
if opts.fastsim: opts.data = False
if not opts.data: opts.prompt = False
print """PSet is assuming:
   data?     : {}
   prompt?   : {}
   fastsim?  : {}
   relval?   : {}
   triginfo? : {}
""".format(bool(opts.data), bool(opts.prompt), bool(opts.fastsim), bool(opts.relval), bool(opts.triginfo))

import CMS3.NtupleMaker.configProcessName as configProcessName
configProcessName.name="PAT"
if opts.data and opts.prompt:
    configProcessName.name="RECO"

configProcessName.name2="RECO"

if opts.relval:
    configProcessName.name="reRECO"
    configProcessName.name2="reRECO"

if opts.fastsim:
    configProcessName.fastSimName="HLT"
    configProcessName.name2=configProcessName.fastSimName
configProcessName.isFastSim=opts.fastsim

# CMS3
process = cms.Process("CMS3")

# Version Control For Python Configuration Files
process.configurationMetadata = cms.untracked.PSet(
        version    = cms.untracked.string('$Revision: 1.11 $'),
        annotation = cms.untracked.string('CMS3'),
        name       = cms.untracked.string('CMS3 test configuration')
)

from Configuration.EventContent.EventContent_cff   import *

# load event level configurations
process.load('Configuration/EventContent/EventContent_cff')
process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration.Geometry.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")

# services
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.GlobalTag.globaltag = "80X_mcRun2_asymptotic_2016_miniAODv2_v0"
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.MessageLogger.cerr.threshold  = ''
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( allowUnscheduled = cms.untracked.bool(True),SkipEvent = cms.untracked.vstring('ProductNotFound') )

process.out = cms.OutputModule("PoolOutputModule",
                               fileName     = cms.untracked.string('ntuple.root'),
                               dropMetaData = cms.untracked.string("ALL"),
                               basketSize = cms.untracked.int32(16384*23)
)



#load cff and third party tools
from JetMETCorrections.Configuration.DefaultJEC_cff import *
from JetMETCorrections.Configuration.JetCorrectionServices_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducersDefault_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducers_cff import *
from JetMETCorrections.Configuration.CorrectedJetProducersAllAlgos_cff import *
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

#Electron Identification for PHYS 14
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *  
from PhysicsTools.SelectorUtils.centralIDRegistry import central_id_registry
process.load("RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cfi")
process.load("RecoEgamma.ElectronIdentification.ElectronMVAValueMapProducer_cfi")
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectrons',"",configProcessName.name)
process.electronMVAValueMapProducer.srcMiniAOD = cms.InputTag('slimmedElectrons',"",configProcessName.name)
process.egmGsfElectronIDSequence = cms.Sequence(process.electronMVAValueMapProducer * process.egmGsfElectronIDs)
my_id_modules = [
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring15_25ns_nonTrig_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring15_25ns_Trig_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_HZZ_V1_cff',
                 ]
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

### -------------------------------------------------------------------
### Setup puppi AK8 jets as input to the DeepAK8 tagger with puppi
### -------------------------------------------------------------------
# ---------------------------------------------------------
# set up TransientTrackBuilder
process.TransientTrackBuilderESProducer = cms.ESProducer("TransientTrackBuilderESProducer",
    ComponentName=cms.string('TransientTrackBuilder')
)
# ---------------------------------------------------------
# recluster Puppi jets
bTagDiscriminators = [
    'pfCombinedInclusiveSecondaryVertexV2BJetTags',
    'pfBoostedDoubleSecondaryVertexAK8BJetTags'
]
JETCorrLevels = ['L2Relative', 'L3Absolute']

from JMEAnalysis.JetToolbox.jetToolbox_cff import jetToolbox
jetToolbox(process, 'ak8', 'jetSequence', 'out', PUMethod='Puppi', JETCorrPayload='AK8PFPuppi', JETCorrLevels=JETCorrLevels, miniAOD=True, runOnMC=True,
           Cut='pt > 170.0 && abs(rapidity()) < 2.4', addNsub=True, maxTau=3,
           addSoftDrop=True, addSoftDropSubjets=True, subJETCorrPayload='AK4PFPuppi', subJETCorrLevels=JETCorrLevels,
           bTagDiscriminators=bTagDiscriminators)
# srcJets = cms.untracked.InputTag('selectedPatJetsAK8PFPuppi')
# srcSubjets = cms.untracked.InputTag('selectedPatJetsAK8PFPuppiSoftDropPacked')
# ---------------------------------------------------------

# Load Ntuple producer cff
process.load("CMS3.NtupleMaker.cms3CoreSequences_cff")
if not opts.data: process.load("CMS3.NtupleMaker.cms3GENSequence_cff")
process.load("CMS3.NtupleMaker.cms3PFSequence_cff")
process.eventMaker.isData = cms.bool(opts.data)
    
do_deepbtag = True
if do_deepbtag:
    from PhysicsTools.PatAlgos.tools.jetTools import *
    deep_discriminators = ["deepFlavourJetTags:probudsg", "deepFlavourJetTags:probb", "deepFlavourJetTags:probc", "deepFlavourJetTags:probbb", "deepFlavourJetTags:probcc" ]
    updateJetCollection(
        process,
        jetSource = cms.InputTag('slimmedJets'),
       jetCorrections = ('AK4PFchs', cms.vstring([]), 'None'),
        btagDiscriminators = deep_discriminators
    )
    updateJetCollection(
        process,
        labelName = 'Puppi',
        jetSource = cms.InputTag('slimmedJetsPuppi'),
       jetCorrections = ('AK4PFchs', cms.vstring([]), 'None'),
        btagDiscriminators = deep_discriminators
    )

    # Needed for the above updateJetCollection() calls
    process.pfJetMaker.pfJetsInputTag = cms.InputTag('selectedUpdatedPatJets')
    process.pfJetPUPPIMaker.pfJetsInputTag = cms.InputTag('selectedUpdatedPatJetsPuppi')

# Hypothesis cuts
process.hypDilepMaker.TightLepton_PtCut  = cms.double(10.0)
process.hypDilepMaker.LooseLepton_PtCut  = cms.double(10.0)

# Options for Input
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(
                                'file:/home/users/namin/2017/slimming/CMSSW_8_0_26_patch1/src/CMS3/NtupleMaker/test/TTJets_HT-1200to2500.root',
                            )
)
process.source.noEventSort = cms.untracked.bool( True )

# Max Events
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


# Run corrected MET maker

# ====================== configurable options ======================
usePrivateSQlite=False #use external JECs (sqlite file)
applyResiduals=opts.data #application of residual corrections. Have to be set to True once the 13 TeV residual corrections are available. False to be kept meanwhile. Can be kept to False later for private tests or for analysis checks and developments (not the official recommendation!).
#===================================================================

if usePrivateSQlite:
    from CondCore.DBCommon.CondDBSetup_cfi import *
    import os
    era="Summer15_25nsV5_MC"
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

### =================================================================================
### jets are rebuilt from those candidates by the tools, no need to do anything else
### =================================================================================

process.outpath = cms.EndPath(process.out)
process.out.outputCommands = cms.untracked.vstring( 'drop *' )

if not opts.data:
    from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
    #default configuration for miniAOD reprocessing, change the isData flag to run on data
    #for a full met computation, remove the pfCandColl input
    runMetCorAndUncFromMiniAOD(process,
                               isData=opts.data,
                               )

process.out.outputCommands = cms.untracked.vstring( 'drop *' )
process.out.outputCommands.extend(cms.untracked.vstring('keep *_*Maker*_*_CMS3*'))

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
### ------------------------------------------------------------------

# end Run corrected MET maker

# process.p = cms.Path( 
#   process.metFilterMaker *
#   process.egmGsfElectronIDSequence *     
#   process.vertexMaker *
#   process.secondaryVertexMaker *
#   process.eventMaker *
#   process.pfCandidateMaker *
#   process.isoTrackMaker *
#   process.electronMaker *
#   process.muonMaker *
#   process.pfJetMaker *
#   process.pfJetPUPPIMaker *
#   process.subJetMaker *
#   process.pfmetMaker *
#   process.pfmetpuppiMaker *
#   process.hltMakerSequence *
#   process.pftauMaker *
#   process.photonMaker *
#   process.genMaker *
#   process.genJetMaker *
#   process.candToGenAssMaker * # requires electronMaker, muonMaker, pfJetMaker, photonMaker
#   process.pdfinfoMaker *
#   process.puSummaryInfoMaker *
#   process.hypDilepMaker
# )

if opts.fastsim:
    process.sParmMaker.vsparms = cms.untracked.vstring("mStop", "mLSP")

process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.Timing = cms.Service("Timing",
        summaryOnly = cms.untracked.bool(True)
        )


# for use with Valgrind. After enabling, can do
# $ valgrind --leak-check=yes  cmsRun main_pset.py >& log.txt
# $ valgrindMemcheckParser.pl --preset=prod,-prod1+ log.txt  > blah.html
# process.ProfilerService = cms.Service (
#         "ProfilerService",
#         firstEvent = cms.untracked.int32(2),
#         lastEvent = cms.untracked.int32(10),
#         paths = cms.untracked.vstring('p1')
# )


# process.GlobalTag.globaltag = "SUPPLY_GLOBAL_TAG"
# process.out.fileName = cms.untracked.string('SUPPLY_OUTPUT_FILE_NAME'),
# process.source.fileNames = cms.untracked.vstring('SUPPLY_INPUT_FILE_NAME')
# process.eventMaker.CMS3tag = cms.string('SUPPLY_CMS3_TAG')
# process.eventMaker.datasetName = cms.string('SUPPLY_DATASETNAME')
# process.maxEvents.input = cms.untracked.int32(SUPPLY_MAX_NEVENTS)

# process.GlobalTag.globaltag = "80X_dataRun2_2016SeptRepro_v7"
# process.out.fileName = cms.untracked.string('ntuple.root')
# process.source.fileNames = cms.untracked.vstring('file:DataDoubleEG2016C.root')
# process.eventMaker.CMS3tag = cms.string('V08-00-18')
# process.eventMaker.datasetName = cms.string('/DoubleEG/Run2016C-03Feb2017-v1/MINIAOD')
# process.maxEvents.input = cms.untracked.int32(3000)

# process.GlobalTag.globaltag = "80X_mcRun2_asymptotic_2016_miniAODv2_v0"
# process.out.fileName = cms.untracked.string('ntuple.root') # output
# process.source.fileNames = cms.untracked.vstring(['/store/mc/RunIISpring16MiniAODv2/SMS-T2tt_dM-10to80_genHT-160_genMET-80_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16Fast_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/80000/5EADC1A2-664B-E611-A66D-28924A33AF26.root']) # input
# process.eventMaker.CMS3tag = cms.string('CMS4_V00-00-02_test') # doesn't affect ntupling, only for bookkeeping later on
# process.eventMaker.datasetName = cms.string('/SMS-T2tt_dM-10to80_genHT-160_genMET-80_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16Fast_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/MINIAODSIM') # doesn't affect ntupling, only for bookkeeping later on
# process.maxEvents.input = cms.untracked.int32(3000) # max number of events; note that crab overrides this to -1

process.GlobalTag.globaltag = "80X_mcRun2_asymptotic_2016_TrancheIV_v8"
process.out.fileName = cms.untracked.string('ntuple.root') # output
process.source.fileNames = cms.untracked.vstring(['/store/mc/RunIISummer16MiniAODv2/ZprimeToTT_M-3000_W-30_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/D6D620EF-73BE-E611-8BFB-B499BAA67780.root']) # input
process.eventMaker.CMS3tag = cms.string('CMS4_V08-00-06') # doesn't affect ntupling, only for bookkeeping later on
process.eventMaker.datasetName = cms.string('ZprimeToTT_M-3000_W-30_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/MINIAODSIM') # doesn't affect ntupling, only for bookkeeping later on
process.maxEvents.input = cms.untracked.int32(3000) # max number of events; note that crab overrides this to -1
