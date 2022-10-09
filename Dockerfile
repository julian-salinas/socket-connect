FROM gcc:11.2.0

WORKDIR /home/c-connect

RUN apt -y update \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN make && make install