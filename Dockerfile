# To make sure it runs on linux
FROM ubuntu:latest

RUN apt update && apt install -y g++ make

WORKDIR /app

COPY . .

RUN make linux

EXPOSE 3000

CMD ["./server"]
