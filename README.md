# Hiredis-example

This repository contains examples of using Redis for storing and retrieving dict-type values using `GET/SET`, as well as examples of publishing and subscribing to messages, images, and videos. The examples include event-driven code and basic blocking code for pub/sub functionality.

---
## Docker Setup

1. Build the Redis image using the following command:
    ```bash
    docker build -t redis-image .
    ```

2. Run the Redis container using the provided `docker_run.sh` script:
    ```bash
    sh docker_run.sh
    ```

---
## Usage

0. 먼저 `redis-server`를 실행시킨다
    ```bash
    redis-server &
    ```

1. Run `redis-cli` in the background after starting the Redis container:
    ```bash
    redis-cli &
    ```

2. Subscribe to a specific channel using the `subscribe` command:
    ```bash
    subscribe <channel name>
    ```

    If you want to subscribe to multiple channels, you can specify them as follows:
    ```bash
    subscribe <channel name 1> <channel name 2> ...
    ```

    This will allow you to receive all messages published to each channel.

    For more details on the Redis pub/sub functionality, you can refer to the [source](https://inpa.tistory.com/entry/REDIS-%F0%9F%93%9A-PUBSUB-%EA%B8%B0%EB%8A%A5-%EC%86%8C%EA%B0%9C-%EC%B1%84%ED%8C%85-%EA%B5%AC%EB%8F%85-%EC%95%8C%EB%A6%BC).

3. Optionally, you can use the `redis-cli monitor` command to monitor the overall activity of Redis. However, note that using `monitor` may slow down the overall performance of the pub/sub system.
