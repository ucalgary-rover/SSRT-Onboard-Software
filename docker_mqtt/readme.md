In order to get a working version of eclipse mosquitto, perform the following steps.

First we must run the following commands:
    - Sudo apt install
    - Sudo apt install docker-compose

These commands ensure that you have all the necessary packages installed, plus the docker compose which will allow us to create a docker image.

The following command will create the docker image, I chose to name mine mqtt for simplicity. I have included additional commands to run after this will ensure it is working properly.

    - Sudo docker-compose up -d
    - Docker restart <container_name_or_id>
    - Docker ps -a (lists all images created)
    - docker logs <container_name_or_id> (ensure it is working as intended)
    - I have additional steps if this does not work, please reach out to Rodrigo Amesty if that is the case.

When adding publishers and subscribers, run the following commands to get a simple test of how the publishers and subscribers would work. You need two terminals to run this.
    - Subscriber: mosquitto_sub -h localhost -t "test/topic" (run first)
Publisher:  mosquitto_pub -h localhost -t "test/topic" -m "Hello, MQTT"