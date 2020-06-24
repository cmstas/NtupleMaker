#!/usr/bin/env sh


# # gtag=94X_dataRun2_v11
# gtag=102X_dataRun2_Sep2018Rereco_v1
# # gtag=102X_dataRun2_Prompt_v11
# inputs=root://cmsxrootd.fnal.gov//store/data/Run2018C/EGamma/MINIAOD/17Sep2018-v1/110000/16D0608A-36CE-7543-93A4-DD42EA7A417B.root
# cmsRun main_pset.py nevents=1000 output=output.root inputs=$inputs globaltag=$gtag triginfo=True data=True

# gtag=102X_upgrade2018_realistic_v12
# inputs=root://cmsxrootd.fnal.gov//store/mc/RunIIAutumn18MiniAOD/DYJetsToLL_M-5to50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v2/260000/F63C20A7-4E63-304B-84F3-725E152D9410.root
# cmsRun -n4 main_pset.py nevents=100000 output=output.root inputs=$inputs globaltag=$gtag triginfo=True data=False
# # >>> Events.Scan("genps_p4.pt():genps_id:genps_id_mother:genps_status","abs(genps_id_mother)<=6 && (abs(genps_id)==13) && genps_status<=3 && genps_isPromptFinalState")
# # /DYJetsToLL_M-5to50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM

gtag=102X_upgrade2018_realistic_v12
inputs=file:/home/users/namin/2019/scouting/repo/batch/Scouting/NtupleMaker/test/output_miniaod.root
cmsRun -n4 main_pset.py nevents=100000 output=output.root inputs=$inputs globaltag=$gtag triginfo=True data=False year=2018
