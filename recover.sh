function env_set() {
    sed -i -e "/^export $2=.*/d" ~/.bashrc
    echo "export $2=`readlink -e $1`" >> ~/.bashrc
}

[ -z ${STUID} ]   && echo "STUID must be set (RTFM)"   && exit
[ -z ${STUNAME} ] && echo "STUNAME must be set (RTFM)" && exit
echo "uploading public key..."
curl -F "id=${STUID}" -F "name=${STUNAME}" -F "file=@${HOME}/.ssh/id_rsa.pub" https://dssl.cun.io/teach/api/uploads/pasetup
sleep 2
echo "recover from remote git..."
git remote add hustpa pa@dssl.cun.io:${STUID}-git
git fetch hustpa
git branch -d pa0
git branch -d pa1
git branch -d pa2
git branch -d pa3
git branch -d pa4
git branch -d pa5
git checkout hustpa/pa0
git checkout -b pa0  hustpa/pa0
git checkout hustpa/pa1
git checkout -b pa1  hustpa/pa1
git checkout hustpa/pa2
git checkout -b pa2  hustpa/pa2
git checkout hustpa/pa3
git checkout -b pa3  hustpa/pa3
git checkout hustpa/pa4
git checkout -b pa4  hustpa/pa4
git checkout hustpa/pa5
git checkout -b pa5  hustpa/pa5
echo "set ENV ..."
env_set nemu NEMU_HOME
env_set nexus-am AM_HOME
env_set navy-apps NAVY_HOME
source ~/.bashrc
echo "done!"

