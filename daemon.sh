#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Usage: $0 <config file>"
    echo "Config file should be derived from config-sample.i2v"
    exit 1
fi
export IMGS2VIDEO_CFGFILE=$1 # exporting for routines in sub-shells
if ! [[ -f $IMGS2VIDEO_CFGFILE ]]
then
    echo "Cfg file $IMGS2VIDEO_CFGFILE does not exist"
    exit 1
fi
source $IMGS2VIDEO_CFGFILE

mkdir -p $IMGSDIR
mkdir -p $VIDEODIR
mkdir -p $DAILY_VIDEO_DIR
mkdir -p $LOG_DIR

function hourly {
# args:
# $1 dir with images for this hour
    DIR=$1
    HOUR=`basename $DIR`
    DATE=`basename \`dirname $DIR\``
    `dirname $0`/remove_old.sh
    echo Gonna remove zero-sized downloaded images, if any
    find $DIR -maxdepth 1 -type f -size 0 -exec rm -vf {} \;

    echo Gonna assemble $DIR
    if [[ -z "`ls $DIR`" ]]
    then
        echo This dir is empty, removing it and skipping assembling
        rmdir $DIR
    else
        ASSEMBLE_LOGFILE=$LOG_DIR/assemble__${DATE}_${HOUR}.log
        `dirname $0`/make_hourly_video.sh $DIR &> $ASSEMBLE_LOGFILE
        if [[ $? -ne 0 ]]
        then
            if [[ -n "$NOTIF_EMAILS" ]]
            then
                echo "ERROR: make_hourly_video.sh $DIR failed"
                cat $ASSEMBLE_LOGFILE | mail -s "Hourly video assembling failed on $NAME" -a $ASSEMBLE_LOGFILE $NOTIF_EMAILS
            fi
        fi
    fi

    if [[ "$NOCAT" == "yes" ]]
    then
        echo Configured to omit concatenation, skipping
    else
        CAT_LOGFILE=$LOG_DIR/cat__${DATE}_${HOUR}.log
        `dirname $0`/cat_lastday.sh &> $CAT_LOGFILE
        if [[ $? != 0 ]]
        then
            echo "ERROR: cat_lastday.sh failed (DATE=$DATE, HOUR=$HOUR)"
            if [[ -n "$NOTIF_EMAILS" ]]
            then
              cat $CAT_LOGFILE | mail -s "Video concatenation failed on $NAME" -a $CAT_LOGFILE $NOTIF_EMAILS
            fi
        fi
        echo "Concatenation succeed."
        `dirname $0`/cat_all_days.sh # cat's accidentally skipped daily videos
    fi

    $AFTER_HOUR_PROC_HOOK
    if [[ $HOUR == 23 ]]
    then
        $DAILY_HOOK
    fi
    echo "Hourly job succeed."
}

PREV_LAP_HOUR='unknown'
PREV_LAP_DAY='unknown'

if [[ "$HV_AND_LOG_SYNC" == "yes" ]]
then
    echo "Starting infinite hourly_video/ and log/ sync process in this daemon instance"
    source `dirname $0`/hv_and_log_sync.sh
    hv_and_log_sync &
fi

while true
do
    DATE=`date +'%F %H %M%S'`
    DAY=`echo $DATE | awk '{ printf $1 }'`
    HOUR=`echo $DATE | awk '{ printf $2 }'`
    MINSEC=`echo $DATE | awk '{ printf $3 }'`
    mkdir -p $IMGSDIR/$DAY/$HOUR
    FILENAME=$IMGSDIR/$DAY/$HOUR/${MINSEC}.jpg
    echo filename is $FILENAME
    wget --connect-timeout=2 --read-timeout=5 $URL -O $FILENAME 2>&1
    if [[ $? -ne 0 ]]
    then
        echo "wget $FILENAME failed, surviving" >&2
        rm $FILENAME
    fi

    if [[ $PREV_LAP_HOUR != 'unknown' ]] && [[ $PREV_LAP_HOUR != $HOUR ]]
    then
        echo "Hour has ticked from $PREV_LAP_HOUR to $HOUR, launching video assembling"
        HOURLY_LOGFILE=$LOG_DIR/hourly__${DAY}_${HOUR}.log
        hourly $IMGSDIR/$PREV_LAP_DAY/$PREV_LAP_HOUR &> $HOURLY_LOGFILE &
    fi
    $AFTER_GET_IMAGE_HOOK
    PREV_LAP_HOUR=$HOUR
    PREV_LAP_DAY=$DAY
done
