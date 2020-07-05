from ubuntu:18.04


RUN dpkg --add-architecture i386
RUN apt-get update -y
RUN apt-get install build-essential -y 
RUN apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 multiarch-support gcc-multilib -y
RUN mkdir ./SBTG
COPY ./ ./SBTG
WORKDIR ./SBTG
ENTRYPOINT ["./docker-entrypoint.sh"]
