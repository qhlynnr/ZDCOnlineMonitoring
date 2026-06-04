#!/bin/bash

pyFile="l1Ntuple2026_PbPb.py"
runNum=404549
lumiNum=101
pdNum=1
Key="PhysicsHIPhysicsRawPrime"

runNumPart1=${runNum:0:3}
runNumPart2=${runNum:3:3}

sed -i "s@runNum = .*@runNum = $runNum@" "$pyFile"
sed -i "s@lumiNum = .*@lumiNum = $lumiNum@" "$pyFile"
sed -i "s@pdNum = .*@pdNum = $pdNum@" "$pyFile"
sed -i "s@Key = .*@Key = $Key@" "$pyFile"

jobStr="run${runNum}_ls${lumiNum}"
dirOut="/eos/cms/store/group/phys_heavyions/xirong/l1ntuple/Run2026/${Key}${pdNum}/${runNumPart1}/${runNumPart2}"

mkdir -p "$dirOut"

echo "Out Directory: $dirOut"
fileOut=$dirOut/L1Ntuple_$jobStr.root
echo "Processing File: /eos/cms/store/t0streamer/Data/${Key}${pdNum}/000/$runNumPart1/$runNumPart2"
echo "$fileOut"

#%chops off .py below
pyFileDump=${pyFile%.py}_dump.py

if [[ -f $fileOut ]]
then
     echo "FILE '$fileOut' exists! rm if you wish to replace!!! sleep 5"
     sleep 5
     echo " continuing"
fi

edmConfigDump $pyFile > $pyFileDump

sed -i "s@L1Ntuple.root@$fileOut@g" $pyFileDump
sed -i "s@+process.l1AXOEmuTree@@g" $pyFileDump
sed -i "s@process.l1uGTTestcrateTree+@@g" $pyFileDump
sed -i "s@reportEvery = cms.untracked.int32(1)@reportEvery = cms.untracked.int32(1000)@g" $pyFileDump

echo "Running $pyFileDump...."
sleep 5
#cmsRun $pyFileDump


