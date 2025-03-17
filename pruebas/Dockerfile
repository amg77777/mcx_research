FROM ubuntu:22.04
RUN apt-get update
RUN apt-get install -y    build-essential
RUN apt-get install -y    git
RUN apt-get install -y    bash
RUN apt-get install -y    pkg-config
RUN apt-get install -y    openssl
RUN apt-get install -y    autoconf
RUN apt-get install -y    automake
RUN apt-get install -y    libtool
RUN apt-get install -y    libglib2.0-dev
RUN apt-get install -y    libsctp-dev
RUN apt-get install -y    libxml2-dev
RUN apt-get install -y    libssl-dev
RUN apt-get install -y    libortp-dev
RUN apt-get install -y    pulseaudio
RUN apt-get install -y    libpulse-dev
RUN apt-get install -y    alsa-utils
RUN apt-get install -y    libsdl1.2-dev
RUN apt-get install -y    libasound2-dev
RUN apt-get install -y    libsndfile1-dev

RUN git clone https://github.com/freeswitch/sofia-sip.git sofia-sip
RUN cd sofia-sip \
    && bash autogen.sh  \
    && ./configure  \
    && make  \
    && make install

RUN git clone https://github.com/BelledonneCommunications/ortp.git oRTP
#RUN     chmod +x /oRTP/autogen.sh 
#RUN     chmod +x /oRTP/configure
#RUN    cd /oRTP
#RUN    bash autogen.sh
#RUN    ./configure
#RUN    make
#RUN    make install

#RUN useradd -m user
#USER user

VOLUME ["/tmp", "/home/user/"]

#WORKDIR /app
#COPY . /app

EXPOSE 5060 5004 5005

CMD ["./run_sip_audio"]