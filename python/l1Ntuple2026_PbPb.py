# L1Ntuple for ZDC studies with 2026 data
# Hannah Bossi, <hannah.bossi@cern.ch>
# 5/21/2026
import FWCore.ParameterSet.Config as cms
import os
import sys

from Configuration.Eras.Era_Run3_pp_on_PbPb_approxSiStripClusters_2026_cff import Run3_pp_on_PbPb_approxSiStripClusters_2026
process = cms.Process('RAW2DIGI', Run3_pp_on_PbPb_approxSiStripClusters_2026)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_DataMapper_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '161X_dataRun3_Prompt_v1', '')

# To change the number of events, change this part
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(5000),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
# in case of dat files - read it like this

# functions for getting lumisections from filename
'''
import re as RegEx
def get_lumisection(fileName):
    match = RegEx.match(r"run(\d+)_ls(\d+)_stream([A-Za-z0-9]+)_StorageManager.dat",fileName)
    print("Run Number ", match.group(1),  "LS ", match.group(2), "DataSet ", match.group(3),)
    return(int(match.group(2)))

validLumiNums = {
    "399499" : [[100,150],]
}

filedir = '/eos/cms/store/t0streamer/Data/PhysicsHIForward0/000/399/499'
infile    = cms.untracked.vstring()
for f in reversed(os.listdir(filedir)):
   validLumi = False
   LumiSection = get_lumisection(f)
   for lumiNum in validLumiNums["399499"]:
      if (LumiSection >= lumiNum[0] and LumiSection <= lumiNum[1]) : validLumi = True
   if validLumi == False: continue
   if f[-4:] == '.dat' :
       infile.append('file:'+filedir+'/'+f)
print(infile)


process.source = cms.Source("NewEventStreamFileReader",
                            fileNames = infile,
)
'''

process.source = cms.Source("PoolSource",
     fileNames = cms.untracked.vstring(
         '/store/hidata/HIRun2025A/HIEphemeralZeroBias0/RAW/v1/000/399/925/00000/f44192ce-9c5b-445d-aaf3-f844dc5c6294.root'
     )
)

#process.source.lumisToProcess = cms.untracked.VLuminosityBlockRange('399499:100-399499:150')




process.options = cms.untracked.PSet(
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Output definition
from Configuration.Applications.ConfigBuilder import MassReplaceInputTag

# Additional output definition

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.endjob_step = cms.EndPath(process.endOfProcess)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step, process.endjob_step)


from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)


# customisation of the process.

# Automatic addition of the customisation function from L1Trigger.Configuration.customiseReEmul
from L1Trigger.Configuration.customiseReEmul import L1TReEmulFromRAW 

#call to customisation function L1TReEmulFromRAW imported from L1Trigger.Configuration.customiseReEmul
process = L1TReEmulFromRAW(process)

# Automatic addition of the customisation function from L1Trigger.L1TNtuples.customiseL1Ntuple
from L1Trigger.L1TNtuples.customiseL1Ntuple import L1NtupleRAWEMU 

#call to customisation function L1NtupleRAWEMU imported from L1Trigger.L1TNtuples.customiseL1Ntuple
process = L1NtupleRAWEMU(process)

# Automatic addition of the customisation function from L1Trigger.Configuration.customiseSettings
from L1Trigger.Configuration.customiseSettings import Tight_ZS_iEta_26_27_28_masked #
process = Tight_ZS_iEta_26_27_28_masked(process) 

# Automatic addition of the customisation function from L1Trigger.Configuration.customiseUtils
from L1Trigger.Configuration.customiseUtils import L1TGlobalMenuXML 

#call to customisation function L1TGlobalMenuXML imported from L1Trigger.Configuration.customiseUtils
process = L1TGlobalMenuXML(process)

# End of customisation functions


# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion


#########################
# ZDC RecHit Producer && Analyzer
#########################
# to prevent crash related to HcalSeverityLevelComputerRcd record
process.load("RecoLocalCalo.HcalRecAlgos.hcalRecAlgoESProd_cfi")
process.load('HeavyIonsAnalysis.ZDCAnalysis.ZDCAnalyzersPbPb_cff')
process.load('HeavyIonsAnalysis.ZDCAnalysis.FSCAnalyzers_cff')
process.zdcreco_step = cms.Path(process.zdcrecoRun3)
process.zdcanalyzer_step = cms.Path(process.zdcanalyzer)
process.fscanalyzer_step = cms.Path(process.fscanalyzer)
process.schedule.append(process.zdcreco_step)
process.schedule.append(process.zdcanalyzer_step)
process.schedule.append(process.fscanalyzer_step)

#=======================================================================


# ======================================================================
# ======================== add in the emulator =========================
# unpacked etsums
process.l1UpgradeTree.sumZDCTag = cms.untracked.InputTag("gtStage2Digis","EtSumZDC") 
process.l1UpgradeTree.sumZDCToken = cms.untracked.InputTag("gtStage2Digis","EtSumZDC")

# l1 emulator sums
process.l1UpgradeEmuTree.sumZDCTag = cms.untracked.InputTag("etSumZdcProducer")
process.l1UpgradeEmuTree.sumZDCToken = cms.untracked.InputTag("etSumZdcProducer")

# do not change these settings
process.etSumZdcProducer = cms.EDProducer('L1TZDCEtSumsProducer',
                                          hcalTPDigis = cms.InputTag("simHcalTriggerPrimitiveDigis"),
#                                          hcalTPDigis = cms.InputTag("hcalDigis"),
                                          bxFirst = cms.int32(-2),
                                          bxLast = cms.int32(3)
)

#Via Hannah, for the simHcal collection
process.simHcalTriggerPrimitiveDigis.inputLabel = cms.VInputTag("hcalDigis", "hcalDigis:ZDC")
process.simHcalTriggerPrimitiveDigis.inputUpgradeLabel = cms.VInputTag("hcalDigis", "hcalDigis:ZDC")


process.etSumZdc = cms.Path(process.etSumZdcProducer)
process.schedule.append(process.etSumZdc)
#======================================================================


#UNCOMMENT HERE TO WORK WITH THE LATEST GREATEST
#MassReplaceInputTag(process, new="rawDataRepacker", old="rawDataCollector")
#MassReplaceInputTag(process, new="rawDataMapperByLabel", old="rawDataCollector")
