# FiwareWeatherSensor

This project sets up an IoT data processing and visualization platform using Docker containers. It includes components for data ingestion, storage, processing, and visualization.

## Overview
This project sets up an IoT data processing and visualization platform using Docker containers. It includes components for data ingestion, storage, processing, and visualization. The platform utilizes FIWARE components such as Orion-LD for context management, QuantumLeap for time-series data storage, and Grafana for visualization. Additionally, it includes a Python script for subscribing to MQTT messages, processing them, and updating context data in Orion-LD.

## Components
- **Orion-LD:** Context broker for managing context data in NGSI-LD format.
- **QuantumLeap:** Stores time-series data received from Orion-LD.
- **Grafana:** Dashboard visualization tool for analyzing data stored in QuantumLeap.
- **MongoDB:** Database for storing non-time-series data.
- **CrateDB:** Distributed SQL database for storing large volumes of time-series data efficiently.
- **Python Script:** Subscribes to MQTT messages, processes them, and updates context data in Orion-LD.

## Installation Guide
Make sure you have sensor connected and ino script running (Important: Update SSID and WiFiPassword).

Details ino script:
- Board: TTGO T1
- 115200 baud
- Port: COM7

1. Start Docker containers:
    ```
    docker-compose up -d
    ```

    **Note:** If crate-db does not start as expected, check the logs for `max virtual memory areas vm.max_map_count [65530] is too low, increase to at least [262144]`.<br>
    In this case, stop and delete all new containers and run these commands:
    ```
    wsl -d docker-desktop
    sysctl -w vm.max_map_count=262144
    ```
    c.f.: [https://support.axway.com/kb/181815](https://support.axway.com/kb/181815)

    then try to start the containers again using:
    ```
    docker-compose up -d
    ```

2. Change directory to `src`:
    ```
    cd src
    ```

3. Run the Python script:
    ```
    python.exe script2.py
    ```

4. Setting up Grafana:
   - Open Grafana in your web browser [localhost:3000](http://localhost:3000).
     - The default credentials for grafana are
       - username: admin
       - password: admin
   - Add a new PostgreSQL data source:
     - Name: CrateDB
     - Host: crate-db:5432 (Important: Do not put localhost)
     - Database: doc
     - User: crate
     - Password: leave it empty
     - SSL mode: disable
   - Build a Dashboard
     - **Query for the line-chart query:**<br>SELECT time_index, temperature, humidity FROM etsensorreading ORDER BY time_index DESC LIMIT 10000
     - **Query for the temperature gauge:**<br>SELECT temperature FROM etsensorreading ORDER BY time_index DESC LIMIT 1
     - **Query for the humidity gauge:**<br>SELECT humidity FROM etsensorreading ORDER BY time_index DESC LIMIT 1
    
   - For more detailed instructions use [this tutorial](https://cratedb.com/blog/visualizing-time-series-data-with-grafana-and-cratedb).

## Pictures

![Picture 01](/docs/pic01.png)

Sensor-board: BME280

![Picture 02](/docs/pic02.png)

Screenshots of Grafana:

![Picture 02](/docs/pic03.png)
![Picture 02](/docs/pic04.png)

## Dependencies
- Docker Engine
- Docker Compose
- Python (for the Python script)

