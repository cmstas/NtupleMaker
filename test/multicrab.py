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

def get_config_for_era(era, version):

    # https://twiki.cern.ch/twiki/bin/view/CMSPublic/CRAB3ConfigurationFile
    cfg = config()

    dataset = {
            "2017B":"/SingleElectron/Run2017B-31Mar2018-v1/MINIAOD",
            "2017C":"/SingleElectron/Run2017C-31Mar2018-v1/MINIAOD",
            "2017D":"/SingleElectron/Run2017D-31Mar2018-v1/MINIAOD",
            "2017E":"/SingleElectron/Run2017E-31Mar2018-v1/MINIAOD",
            "2017F":"/SingleElectron/Run2017F-31Mar2018-v1/MINIAOD",
            "2018A":"/EGamma/Run2018A-17Sep2018-v2/MINIAOD",
            "2018B":"/EGamma/Run2018B-17Sep2018-v1/MINIAOD",
            "2018C":"/EGamma/Run2018C-17Sep2018-v1/MINIAOD",
            "2018D":"/EGamma/Run2018D-22Jan2019-v2/MINIAOD",
    }[era]


    gtag=None
    if "Run2017" in dataset: gtag = "94X_dataRun2_v11"
    elif "Run2018D" in dataset: gtag = "102X_dataRun2_Prompt_v11"
    elif "Run2018" in dataset: gtag = "102X_dataRun2_Sep2018Rereco_v1"
    else:
        raise Exception("Hmm, can't figure out globaltag")

    cfg.General.requestName = 'trigskim_{}_{}'.format(
            era,
            version,
            )
    cfg.Data.inputDataset = dataset

    cfg.General.workArea = 'crab'
    cfg.General.transferLogs = True

    cfg.JobType.pluginName = 'Analysis'
    cfg.JobType.psetName = 'main_pset.py'

    cfg.JobType.pyCfgParams=["globaltag={}".format(gtag),"triginfo=True","data=True",]

    cfg.Data.splitting = 'EventAwareLumiBased'
    cfg.Data.unitsPerJob = int(1e6)
    cfg.Data.lumiMask = "Cert_2017-2018_10percentbyrun_JSON.txt"

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
    for era in [
            "2017C",
            "2017D",
            "2017E",
            "2017F",
            "2018A",
            "2018B",
            "2018C",
            "2018D",
            ]:
        cfg = get_config_for_era(era=era, version=version)
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


