set -e

[ -z ${STUID} ]   && echo "STUID must be set (RTFM)"   && exit
[ -z ${STUNAME} ] && echo "STUNAME must be set (RTFM)" && exit
echo -n "The task to be submitted > "
read task

cwd=${PWD##*/}
echo "${STUID} (${STUNAME}) submitting ${task}..."

wiki='https://dssl.cun.io/teach/api/uploads/'

tarball=$(mktemp -q).tar.bz2
bash -c "cd .. && tar cj ${cwd} > ${tarball}"
curl -F "task=${task}" -F "id=${STUID}" -F "name=${STUNAME}" -F "submission=@${tarball}" ${wiki}upload
