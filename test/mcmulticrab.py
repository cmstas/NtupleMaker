import os
import sys
import multiprocessing
from CRABAPI.RawCommand import crabCommand
from CRABClient.UserUtilities import setConsoleLogLevel, config

import glob
from pprint import pprint
from metis.CrabManager import CrabManager

def get_proxy_file():
    return "/tmp/x509up_u{0}".format(os.getuid())

def get_config(dataset, shortname, gtag, version):


    # https://twiki.cern.ch/twiki/bin/view/CMSPublic/CRAB3ConfigurationFile
    cfg = config()

    cfg.General.requestName = '{}trigskim_{}'.format(
            shortname,
            version,
            )
    cfg.Data.inputDataset = dataset

    cfg.General.workArea = 'crab'
    cfg.General.transferLogs = True

    cfg.JobType.pluginName = 'Analysis'
    cfg.JobType.psetName = 'main_pset.py'

    cfg.JobType.pyCfgParams=["globaltag={}".format(gtag),"triginfo=True","data=False",]

    cfg.Data.splitting = 'EventAwareLumiBased'
    cfg.Data.unitsPerJob = int(750000)
    # cfg.Data.unitsPerJob = int(1e6)
    # cfg.Data.lumiMask = "Cert_2017-2018_10percentbyrun_JSON.txt"

    cfg.Data.publication = False
    cfg.Site.storageSite = "T2_US_UCSD"
    return cfg

def do_submit(q, config, proxy):
    extra = dict(config=config)
    if proxy: extra["proxy"] = proxy
    out = crabCommand('submit', **extra)
    q.put(out)

if __name__ == "__main__":

    version = "v1"
    proxy = get_proxy_file()
    taskdirs = []
    for shortname, gtag,dataset in [
            ["","102X_upgrade2018_realistic_v12","/DYJetsToLL_M-5to50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v2/MINIAODSIM"],
            ["dy1to4","102X_upgrade2018_realistic_v12","/DYJetsToLL_M-1to4_HT-70to100_TuneCP5_13TeV-madgraphMLM-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"],
            ["qcdb100to200","102X_upgrade2018_realistic_v12","/QCD_bEnriched_HT100to200_TuneCP5_13TeV-madgraph-pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"],
            ["qcdmu15to20","102X_upgrade2018_realistic_v12","/QCD_Pt-15to20_MuEnrichedPt5_TuneCP5_13TeV_pythia8/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v3/MINIAODSIM"],
            ["bjpsimumu","102X_upgrade2018_realistic_v12","/BToJpsi_ToMuMu_BMuonFilter_HardQCD_TuneCP5_13TeV-pythia8-evtgen/RunIIAutumn18MiniAOD-102X_upgrade2018_realistic_v15-v1/MINIAODSIM"],
            ]:
        cfg = get_config(dataset=dataset, gtag=gtag, version=version, shortname=shortname)
        taskdir = "{}/crab_{}/".format(cfg.General.workArea, cfg.General.requestName)
        taskdirs.append(taskdir)
        if os.path.exists(taskdir):
            print("Task dir {} already exists.".format(taskdir))
            continue
        # need to spawn a new process or else crab complains that a config has already been cached :(
        mpq = multiprocessing.Queue()
        mpp = multiprocessing.Process(target=do_submit, args=(mpq, cfg, proxy))
        mpp.start()
        mpp.join()
        out = mpq.get()
        print(out)

    statuses = {}
    for taskdir in taskdirs:
        print("\n\n----- {} -----".format(taskdir))
        cm = CrabManager(request_name=taskdir)

        # js = cm.crab_status(cacheseconds=1*60*60)
        # js = cm.crab_status(cacheseconds=10*60)
        js = cm.crab_status(cacheseconds=1*60)
        js.pop("job_info")
        print(js)
        # pprint(js)

        statuses[taskdir] = js

        # has_failures = js["job_breakdown"].get("failed",0) > 0
        # if has_failures:
        #     try:
        #         js = cm.crab_resubmit()
        #         pprint(js)
        #     except:
        #         pass

    src = """
<html>
<head>
<style>
.progress {
  width: 60%;
}
.container {
  width: 50%;
  margin-top: 10px;
}
</style>
<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js" crossorigin="anonymous"></script>
<link href="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css" rel="stylesheet" crossorigin="anonymous">
</head>
<body>
    """
    for taskdir,info in statuses.items():
        job_breakdown = info["job_breakdown"]
        totjobs = sum(job_breakdown.values())
        if totjobs == 0: continue
        ndone = (job_breakdown["finished"])
        nrunning = (job_breakdown["running"]+job_breakdown["transferring"]+job_breakdown["transferred"])
        nfailed = (job_breakdown["failed"])
        nidle = (job_breakdown["unsubmitted"]+job_breakdown["idle"]+job_breakdown["cooloff"])
        div = """
<div class="container">
  <div class="row">
    <span class="col-sm-4">{taskdir}</span>
    <div class="progress" class="col-sm-8">
      <div class="progress-bar bg-success" role="progressbar" style="width:{pctdone}%"> {ndone} done </div>
      <div class="progress-bar bg-info" role="progressbar" style="width:{pctrunning}%"> {nrunning} running </div>
      <div class="progress-bar bg-danger" role="progressbar" style="width:{pctfailed}%"> {nfailed} failed </div>
      <div class="progress-bar bg-warning" role="progressbar" style="width:{pctidle}%"> {nidle} idle </div>
    </div>
  </div>
</div>
        """.format(
                ndone=ndone,
                nrunning=nrunning,
                nfailed=nfailed,
                nidle=nidle,
                pctdone=1.0*ndone/totjobs*100.,
                pctrunning=1.0*nrunning/totjobs*100.,
                pctfailed=1.0*nfailed/totjobs*100.,
                pctidle=1.0*nidle/totjobs*100.,
                taskdir=taskdir,
                )
        src += div

    src += "</body></html>"

    user = os.getenv("USER").strip()
    outdir = "/home/users/{}/public_html/dump/".format(user)
    if not os.path.exists(outdir):
        os.system("mkdir -p {}".format(outdir))
    with open("{}/test.html".format(outdir),"w") as fh:
        fh.write(src)


