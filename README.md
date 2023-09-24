# ISM

My smart home project that base on Rasberry pi zero (as central point) and Esp8266 modules.

## Folders:

-   isma - Code for esp that control my aquarium
-   ismp - Code for esp that watering the plants
-   client - frontent code for main website (rasberry pi)
-   server - backend code for main website (rasberry pi)

## Docker

    docker buildx build --platform linux/amd64,linux/arm -t jaanonim/ism . --push

Set `network_mode: host` for Wake on Lan to work.
