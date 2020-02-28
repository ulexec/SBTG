from ubuntu:19.04


RUN dpkg --add-architecture i386
RUN apt-get update -y
RUN apt-get install build-essential -y 
RUN apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 multiarch-support gcc-multilib -y
RUN mkdir ./SBTG
COPY ./ ./SBTG
WORKDIR ./SBTG
RUN make
RUN ./sbtg ./test
RUN ./output.elf 
RUN sleep 1000

