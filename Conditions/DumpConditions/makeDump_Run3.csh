#!/bin/csh
cmsenv

if ("$4" == "test") then
     echo "connectstring = sqlite_file:test.db"
     set connectstring = sqlite_file:test.db
else  if ( $#argv == 3 ) then
    echo "connectstring = frontier://FrontierProd/CMS_CONDITIONS"
    set connectstring = frontier://FrontierProd/CMS_CONDITIONS
else
    echo "connectstring = $4"
    set connectstring = $4
endif

cat >! temp_dump_cfg.py <<%


import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
process = cms.Process("DUMP",eras.Run3)


process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load('Configuration.StandardSequences.Services_cff')
'''
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond["phase1_2022_realistic"]
'''

# here you change the global tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '161X_dataRun3_Prompt_v1', '')

process.maxEvents = cms.untracked.PSet(

    input = cms.untracked.int32(1)
)

process.source = cms.Source("EmptySource",
    numberEventsInRun = cms.untracked.uint32(1),
    firstRun = cms.untracked.uint32($3)
)

process.load("CondCore.CondDB.CondDB_cfi")
process.es_pool = cms.ESSource("PoolDBESSource",
        #timetype = cms.string('runnumber'),
        #authenticationMethod = cms.untracked.uint32(0),
        connect =   cms.string("$connectstring"),  # cms.string("$4"),  
        toGet = cms.VPSet(cms.PSet(record = cms.string("Hcal$1Rcd"),
                                   tag = cms.string("$2")
                                  )                        
                         )
)
process.es_prefer_es_pool = cms.ESPrefer( "PoolDBESSource", "es_pool" )

process.dumpcond = cms.EDAnalyzer("HcalDumpConditions",
                                  dump = cms.untracked.vstring("$1")
                                  )

process.p = cms.Path(process.dumpcond)

%
cmsRun temp_dump_cfg.py
rm temp_dump_cfg.py

