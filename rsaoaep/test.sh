#!/bin/bash - 
#===============================================================================
#
#          FILE: test.sh
# 
#         USAGE: ./test.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: BOSS14420 (), 
#  ORGANIZATION: 
#       CREATED: 09/09/2014 08:34
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

OAEP_HOME="$(dirname $0)"
OAEP="$OAEP_HOME/oaeptest"

keysize=6144

#$OAEP_HOME/rsa_genkey $keysize tmpkey
publickey=tmpkey.publickey
secretkey=tmpkey.secretkey

python2 -c "print '|' + '-'*17 + '|' + '-'*12 + '|' + '-'*10 + \
                '|' + '-'*11 + '|' + '-'*15 + '|' + '-'*10 + '|' + '-'*9 + '|'"
printf "| %-15s | %-10s | %-8s | %-9s | %-13s | %-8s | %-6s |\n" \
    "File" "Dung lượng" "File mã " "Tỉ lệ    " "Thời gian    " "Giải mã " "K/t MD5"
python2 -c "print '|' + '-'*17 + '|' + '-'*12 + '|' + '-'*10 + \
                '|' + '-'*11 + '|' + '-'*15 + '|' + '-'*10 + '|' + '-'*9 + '|'"

for file in "$@"
do
    filename=$(basename "$file")
    hsz=$(/bin/ls -lh "$file" | cut -d" " -f5)iB
    printf "| %-15s | %-10s |" "$filename" $hsz

    ctime=$(/usr/bin/time -f "%e" sh -c "cat $file | $OAEP e $publickey > "$file".enc" 2>&1)
    hcsz=$(/bin/ls -lh "$file".enc | cut -d" " -f5)iB
    sz=$(stat --printf "%s" "$file")
    csz=$(stat --printf "%s" "$file".enc)
    ratio=$(echo "$csz / $sz" | bc -l)
    ratio=$(printf "%.03f" $ratio)
    printf " %-8s | %-09s | %-13s |" $hcsz $ratio "$ctime"s

    xtime=$(/usr/bin/time -f "%e" sh -c "cat $file.enc | $OAEP d $secretkey > "$file".dec" 2>&1)
    printf " %-8s |" "$xtime"s
    #printf "| %-15s | %-10s | %-8s | %-09s | %-13s | %-8s |\n" \
    #    "$filename" $hsz $hcsz $ratio "$ctime"s "$xtime"s

    md51=$(md5sum "$file" | cut -d" " -f1)
    md52=$(md5sum "$file".dec | cut -d" " -f1)
    if [[ $md51 != $md52 ]]; then
        printf " %-7s |\n" "Lỗi"
    else
        printf " %-7s |\n" "OK"
        rm "$file".enc "$file".dec
    fi
done

python2 -c "print '|' + '-'*17 + '|' + '-'*12 + '|' + '-'*10 + \
                '|' + '-'*11 + '|' + '-'*15 + '|' + '-'*10 + '|' + '-'*9 + '|'"
