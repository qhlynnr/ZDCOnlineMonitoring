#!/bin/bash

mkdir -p logs

LUMI_GROUPS=(
    "0069,0070,0071,0072"
    "0073,0074,0075,0076"
    "0077,0078,0079,0080"
    "0081,0082,0083,0084"
    "0085,0086,0087,0088"
    "0089,0090,0091,0092"
    "0093,0094,0095,0096"
    "0097,0098,0099,0100"
    "0101,0102,0103,0104"
    "0105,0106,0107,0108"
    "0109,0110,0111,0112"
)
TXT=${2:-"/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/inputFolders/Forward_394.txt"}
KEY=${3:-"PhysicsHIForward"}
OUTPUTFOLDER=${4:-"/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/output_debug/"}

for GROUP in "${LUMI_GROUPS[@]}"; do
    ./plotZDCEmuLoop_Parallel "$GROUP" "$TXT" "$KEY" "$OUTPUTFOLDER" \
        > "logs/log_${GROUP//,/_}.txt" 2>&1 &
done

wait
echo "All jobs finished"