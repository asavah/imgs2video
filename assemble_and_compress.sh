#!/bin/bash

if [[ $# -lt 2 ]]
then
    echo 'Usage: <input images dir> <outfile> [filter]'
    echo 'Options are configured in config file, pass filename as $IMGS2VIDEO_CFGFILE env var'
    exit 1
fi

if [[ -z $IMGS2VIDEO_CFGFILE ]]
then
    echo 'IMGS2VIDEO_CFGFILE env var must be available'
    exit 1
fi
if ! [[ -f $IMGS2VIDEO_CFGFILE ]]
then
    echo "Cfg file $IMGS2VIDEO_CFGFILE does not exist"
    exit 1
fi

source $IMGS2VIDEO_CFGFILE

if [ -z "$VIDEO_ENCODING_OPTS" ]; then
    filename=$(basename -- "$2")
    extension="${filename##*.}"
    if [ ! "x${extension}" = "xmp4" ] && [ ! "x${extension}" = "xwebm" ]; then
        echo "Invalid extension: ${extension}, only mp4 and webm are supported"
        exit 1
    fi
    VIDEO_ENCODING_OPTS=${VIDEO_ENCODING_OPTS_ARRAY[${extension}]}
fi

IMGDIR=`readlink -f $1`
OUTFILE=`readlink -f $2`

if [ -n "$3" ]; then
FILTER=$3
fi

if [ -z "$FILTER" ] || [ "x$FILTER" = "xnull" ]; then
FILTER=""
else
FILTER="${FILTER},"
fi

function cleanup {
    echo "Removing unfinished output file $OUTFILE"
    rm $OUTFILE
    exit 0
}

trap cleanup INT TERM QUIT

echo Making $OUTFILE of $IMGDIR
$FFMPEG \
        -err_detect explode \
        -f image2 \
        -ts_from_file 1 \
        -pattern_type glob \
        -i $IMGDIR'/*.jpg' \
        -vf "${FILTER}settb=1/1000,setpts=(PTS-STARTPTS)/$SPEEDUP" \
        $VIDEO_ENCODING_OPTS \
        -r $FRAMERATE \
        -an \
        -y \
        $OUTFILE
RET=$?
if [[ $RET != 0 ]]
then
    echo "ERROR: Making $OUTFILE of $IMGDIR failed"
    rm $OUTFILE
fi
exit $RET
