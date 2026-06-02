#!/bin/bash

mkdir -p logs

LUMI_GROUPS=(
   "0100,0101,0102"
   "0103,0104,0105"
   "0106,0107,0108"
   "0109,0110"
)
TXT="/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/inputFolders/RawPrime_359.txt"
KEY="PhysicsHIPhysicsRawPrime"
OUTPUTFOLDER="/afs/cern.ch/user/x/xirong/ZDCOnlineMonitoring/plotting/output_HIPhysicsRawPrime_Run404359_100to110/"

mkdir -p "$OUTPUTFOLDER"
for GROUP in "${LUMI_GROUPS[@]}"; do
    echo "Processing: $GROUP $TXT $KEY $OUTPUTFOLDER"
    ./plotZDCEmuLoop_Parallel "$GROUP" "$TXT" "$KEY" "$OUTPUTFOLDER" \
        > "logs/log_${KEY}_${GROUP//,/_}.txt" 2>&1 &
done

wait
echo "All jobs finished"