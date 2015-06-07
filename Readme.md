## WeatherSpot

A weather reporting ecosystem consisting of a website, windows native application, and multiple weather sensors to fetch weather data.

### Development
1. Login to the WeatherSpot server using ```ssh root@WeatherSpot.us```.
2. Make sure the influx database is running using ```docker ps -a``` to see active containers. If it's not running, a ```docker start weather_db``` will get it up again.
3. Do whatever you need to do.

In the event the influxdb container was removed, the following will get it up again:
```bash
docker run -d -p 8083:8083 -p 8084:8084 -e PRE_CREATE_DB="weather" --name weather_db tutum/influxdb:latest
```

### Docker Installation
```bash
# Have to use this script since the official repo has an outdated (v1.5) docker.
wget -qO- https://get.docker.com/ | sh

# Unmasks the services.
systemctl unmask docker.service
systemctl unmask docker.socket

# And starts them.
systemctl start docker.service
systemctl start docker.socket
```
