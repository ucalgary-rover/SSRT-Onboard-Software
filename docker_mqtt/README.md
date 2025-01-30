# Eclipse mosquitto setup 

## Docker installation
Unfamiliar with Docker and Docker setup? See here: https://github.com/ucalgary-rover/SSRT-Docs-Resources/blob/main/resources/HelloDocker.md

### In order to get a working version of eclipse mosquitto, perform the following steps.

#### First we must run the following commands:
Install docker-compose
```bash
sudo apt install docker-compose
```
Install tools for test publisher and subscriber
```bash
sudo apt install mosquitto-clients
```

These commands ensure that you have all the necessary packages installed, plus the docker compose which will allow us to create a docker image.

The following command will create the docker image, I chose to name mine mqtt for simplicity. I have included additional commands to run after this will ensure it is working properly.

Build the image
```bash
Sudo docker-compose up -d
```
Restart the container
```bash
Docker restart <container_name_or_id>
```
Lists all images created
```bash
Docker ps -a
```
Ensure it is working as intended
```bash
docker logs <container_name_or_id> 
```

I have additional steps if this does not work, please reach out to Rodrigo Amesty if that is the case.

When adding publishers and subscribers, run the following commands to get a simple test of how the publishers and subscribers would work. 

You need two terminals to run this.

Run this first in a terminal.
```bash
mosquitto_sub -h localhost -t "test/topic"
```

Then this.
```bash
mosquitto_pub -h localhost -t "test/topic" -m "Hello, MQTT"
```

You should now see the response in subscriber terminal.