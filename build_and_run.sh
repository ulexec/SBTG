docker build -t sbtg .
docker run sbtg /bin/bash -c "sh docker-entrypoint.sh; /bin/bash"
sudo docker cp $(sudo docker ps -l | awk -F 'sbtg' '{print $1}' |  awk -F 'CONTAINER' '{print $1}' | awk -F ' ' '{print $1}'):/SBTG/output.elf .
sudo docker stop $(sudo docker ps -l | awk -F 'sbtg' '{print $1}' |  awk -F 'CONTAINER' '{print $1}' | awk -F ' ' '{print $1}')

