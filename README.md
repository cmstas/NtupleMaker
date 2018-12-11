# NtupleMaker

### Installing
1. `curl https://raw.githubusercontent.com/cmstas/NtupleMaker/master/install.sh > install.sh`.
2. Specify the CMSSW_release and CMS3Tag (branch name or tag name) you want to use at the top of `install.sh`.
3. `source install.sh` will check out the CMSSW release and NtupleMaker repository, and build everything.

### Running

1. If you're not there already, `cd $CMSSW_BASE/src/CMS3/NtupleMaker/test/`.
2. The main pset is, unsuprisingly, `main_pset.py`, and it supports running on data (PromptReco or a Re-Reco), MC (FullSim or FastSim) with various options.
3. Check out the last block of code in the pset to see what parameters you need to modify. Typically, the parameters to modify are:
```
process.GlobalTag.globaltag = "80X_dataRun2_2016SeptRepro_v7"
process.out.fileName = cms.untracked.string('ntuple.root') # output
process.source.fileNames = cms.untracked.vstring('file:DataDoubleEG2016C.root') # input
process.eventMaker.CMS3tag = cms.string('V08-00-18') # doesn't affect ntupling, only for bookkeeping later on
process.eventMaker.datasetName = cms.string('/DoubleEG/Run2016C-03Feb2017-v1/MINIAOD') # doesn't affect ntupling, only for bookkeeping later on
process.maxEvents.input = cms.untracked.int32(3000) # max number of events; note that crab overrides this to -1
```
4. Finally, 
  * `cmsRun main_pset.py data=True prompt=True` to run on _prompt_ data
  * `cmsRun main_pset.py data=False year=2017` to run on _FullSim_ MC
  * `cmsRun main_pset.py fastsim=True year=2017` to run on _FastSim_ MC


### Some quickstart parameters
In `install.sh`, use `CMS3Tag=CMS4_V09-04-20` and `CMSSW_release=CMSSW_9_4_9` to run on the RunII 2017 re-reco sample for `/ttHToNonbb_M125_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/MINIAODSIM`

And paste the following at the end of `main_pset.py`.

```
process.GlobalTag.globaltag = "94X_mc2017_realistic_v14"
process.out.fileName = cms.untracked.string('ntuple.root') # output
process.source.fileNames = cms.untracked.vstring(['/store/mc/RunIIFall17MiniAODv2/ttHToNonbb_M125_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/PU2017_12Apr2018_new_pmx_94X_mc2017_realistic_v14-v1/50000/00436CBC-6B70-E811-850C-00259075D70C.root']) # input
process.eventMaker.CMS3tag = cms.string('dummy') # doesn't affect ntupling, only used for bookkeeping later on
process.eventMaker.datasetName = cms.string('dummy') # doesn't affect ntupling, only for bookkeeping later on
process.maxEvents.input = cms.untracked.int32(3000) # max number of events; note that batch stuff overrides this to -1
```

Run it with `cmsRun main_pset.py data=True setup=2017`.

### ProjectMetis details
To make a tarfile for use with Metis, the current monster string looks something like this
```bash
# include stuff for DeepAK8, 
# drop some unnecessary matrix element stuff, 
# drop tons of hella-large EGamma weight files...
# use xz with best compression, 
mtarfile lib_CMS4_V09-04-20_949.tar.xz -e $CMSSW_BASE/external/$SCRAM_ARCH/lib/libmxnet_predict.so $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/mxnet_predict.xml  \
        --xz --xz_level 9 -x "ZZMatrixElement/MELA/data/Pdfdata" "*ZZMatrixElement/MELA/data/*.root" "*Identification/data/MVA/*.gz" "_puinfo_"
```
