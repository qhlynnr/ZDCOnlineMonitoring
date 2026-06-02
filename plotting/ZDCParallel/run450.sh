#!/bin/bash

mkdir -p logs

LUMI=${1:-"0069"}

LUMI_GROUPS=(
    #"0450,0451,0452"
   # "0453,0454,0455"
   # "0456,0457,0458"
   # "0459,0460,0461"
   # "0462,0463,0464"
   # "0465,0466,0467"
   # "0468,0469,0470"
   "0471,0472,0473"
   "0474,0475,0476"
   "0477,0478"
   "0479,0480"
)
TXT="/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/inputFolders/Forward_359_450.txt"
KEY="PhysicsHIForward"
OUTPUTFOLDER="/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/output_HIForward_Run404359_450to475/"

for GROUP in "${LUMI_GROUPS[@]}"; do
    ./plotZDCEmuLoop_Parallel "$GROUP" "$TXT" "$KEY" "$OUTPUTFOLDER" \
        > "logs/log_${GROUP//,/_}.txt" 2>&1 &
done

wait
echo "All jobs finished"