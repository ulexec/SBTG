docker build -t sbtg . &
sleep 20
sudo docker cp $(sudo docker ps -l | awk -F 'CONTAINER' '{print $1}' | awk -F ' ' '{print $1}'):/SBTG/output.elf .
sudo docker exec -it $(sudo docker ps -l | awk -F 'CONTAINER' '{print $1}' | awk -F ' ' '{print $1}') make clean
