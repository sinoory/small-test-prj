cid=`sudo docker ps -a |grep fundebug |awk '{print $1}'`
sudo docker stop $cid
sudo docker rm $cid
dimg=`sudo docker images |grep none |awk '{print $3}' |tr "\n" " "`
sudo docker rmi $dimg
sudo docker build -t fundebug/nodejs .
sudo docker run -d --net=host --name=hello-fundebug fundebug/nodejs

