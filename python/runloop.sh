#!/bin/bash

pyFile="l1Ntuple2026_PbPb.py"
runNum=404549
lumiList=(102 627)   # <- replace with your lumi numbers
pdList=(1 2 3 4 5)             # <- replace with your pd numbers
Key="PhysicsHIPhysicsRawPrime"

runNumPart1=${runNum:0:3}
runNumPart2=${runNum:3:3}

for lumiNum in "${lumiList[@]}"; do
    for pdNum in "${pdList[@]}"; do
        jobStr="run${runNum}_ls${lumiNum}_pd${pdNum}"
        dirOut="/eos/cms/store/group/phys_heavyions/xirong/l1ntuple/Run2026/${Key}${pdNum}/${runNumPart1}/${runNumPart2}"
        echo "Out Directory: $dirOut"

        mkdir -p "$dirOut"

        fileOut=$dirOut/L1Ntuple_$jobStr.root
        echo "Processing File: /eos/cms/store/t0streamer/Data/${Key}${pdNum}/000/$runNumPart1/$runNumPart2"
        echo "$fileOut"

        # make a per-job copy of the original python cfg and edit that copy
        workingPy="${pyFile%.py}_run${runNum}_ls${lumiNum}_pd${pdNum}.py"
        cp "$pyFile" "$workingPy"

        sed -i "s@runNum = .*@runNum = $runNum@" "$workingPy"
        sed -i "s@lumiNum = .*@lumiNum = $lumiNum@" "$workingPy"
        sed -i "s@pdNum = .*@pdNum = $pdNum@" "$workingPy"
        sed -i "s@Key = .*@Key = $Key@" "$workingPy"

        pyFileDump=${workingPy%.py}_dump.py

        if [[ -f $fileOut ]]; then
                 echo "FILE '$fileOut' exists! rm if you wish to replace!!! sleep 5"
                 sleep 5
                 echo " continuing"
        fi

        edmConfigDump "$workingPy" > "$pyFileDump"

        sed -i "s@L1Ntuple.root@$fileOut@g" "$pyFileDump"
        sed -i "s@+process.l1AXOEmuTree@@g" "$pyFileDump"
        sed -i "s@process.l1uGTTestcrateTree+@@g" "$pyFileDump"
        sed -i "s@reportEvery = cms.untracked.int32(1)@reportEvery = cms.untracked.int32(1000)@g" "$pyFileDump"

        echo "Running $pyFileDump...."
        sleep 5
        cmsRun "$pyFileDump" > "log_$jobStr.txt" 2>&1 &

        #rm -f "$workingPy" "$pyFileDump"
    done
done
