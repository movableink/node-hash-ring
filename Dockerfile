FROM node:12.18

ADD . /app

WORKDIR /app

RUN yarn install

CMD /bin/bash
