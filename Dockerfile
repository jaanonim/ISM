FROM node:18-alpine as build-step

RUN npm install -g pnpm

WORKDIR /app
RUN mkdir ./client
RUN mkdir ./server

WORKDIR /app/client
COPY ./client/package.json ./client/pnpm-lock.yaml ./
RUN pnpm install
ADD ./client ./
RUN pnpm build

FROM python:3.10-alpine
WORKDIR /app
RUN mkdir ./client
RUN mkdir ./server

WORKDIR /app/server
COPY --from=build-step /app/client/dist /app/client/dist

COPY server/requirements.txt ./
RUN pip install --no-dependencies -r requirements.txt 
ADD ./server ./

EXPOSE 5000 2693
CMD ["python3", "app.py"]
