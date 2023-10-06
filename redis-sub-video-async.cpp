#include <iostream>
#include <hiredis/async.h>
#include <hiredis/adapters/libevent.h>
#include <event2/event.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

// Event base for the event loop
event_base* eventBase;

// Callback function to handle subscribed messages
void handleMessage(redisAsyncContext* context, void* reply, void* privateData) {
    redisReply* rp =  static_cast<redisReply*>(reply);
	if(!rp){
		printf("Invalid parameter.\n");
		return;
	}

    cv::String windowName = "Redis Frames";

	if(rp->type == REDIS_REPLY_ARRAY && rp->elements == 3){
		if(strcmp(rp->element[0]->str, "subscribe") != 0){
            // std::cout << std::unitbuf;  // Set std::cout to be unbuffered
            // std::cout << "Received message: " << rp->element[2]->str << std::endl;
			printf("Received [%s] channel %s, length of data: %ld\n", (char*)privateData, rp->element[1]->str, rp->element[2]->len);

            std::vector<char> imageBytes(rp->element[2]->str, rp->element[2]->str + rp->element[2]->len);
            // Decode the image bytes using OpenCV
            cv::Mat image = cv::imdecode(imageBytes, cv::IMREAD_COLOR);
            if (image.empty()) {
                std::cout << "Failed to decode image" << std::endl;
                return;
            }

            // Save the image as RGB JPG
            // std::string outputFilePath = "output.jpg";
            // cv::imwrite(outputFilePath, image);

            // std::cout << "Image saved as " << outputFilePath << std::endl;

            // Create a named window
            cv::namedWindow(windowName, cv::WINDOW_NORMAL);

            // Set the window size to Full HD
            cv::resizeWindow(windowName, 1920, 1080);
            cv::imshow(windowName, image);
            cv::waitKey(1); // Process OpenCV events (required for the window to update)
        }   
	}
}

// Callback function for keyboard input event
void handleKeyboardInput(evutil_socket_t fd, short event, void* arg) {
    std::cout << "Received handleKeyboardInput" << std::endl;
    char key;
    std::cin >> key;

    // press 'q' and 'enter'
    if (key == 'q') {
        // Stop the event loop
        std::cout << "Stop the Subscribe event loop" << std::endl;
        event_base_loopbreak(eventBase);
    }
}

void connect_callback(const redisAsyncContext* ctx, int status)
{
	if(status != REDIS_OK){
		printf("Error: %s\n", ctx->errstr);
		return;
	}
	printf("Connected...\n");
}

void disconnect_callback(const redisAsyncContext* ctx, int status)
{
    printf("Disconnected...\n");

	if(status != REDIS_OK){
		printf("Error: %s\n", ctx->errstr);
		return;
	}
}

void redisSubscriptionThread() {
    // Create a Redis async context
    redisAsyncContext* rc = redisAsyncConnect("localhost", 6379);
    if (rc->err) {
        std::cout << "Error connecting to Redis: " << rc->errstr << std::endl;
        return;
    }

    // Attach the Redis context to the event loop
    redisLibeventAttach(rc, eventBase);

    redisAsyncSetConnectCallback(rc, connect_callback);
    redisAsyncSetDisconnectCallback(rc, disconnect_callback);

    // Subscribe to a Redis channel
    const char* channelName = "channel_name";
    redisAsyncCommand(rc, handleMessage, NULL, "SUBSCRIBE %s", channelName);

    // Start the event loop
    event_base_dispatch(eventBase);

    // Clean up
    if (rc) {
        redisAsyncFree(rc);
    }
}

int main() {
    // Create an event base for the event loop
    eventBase = event_base_new();

    // Create an event for keyboard input
    struct event* keyboardEvent = event_new(eventBase, STDIN_FILENO, EV_READ | EV_PERSIST, handleKeyboardInput, nullptr);
    event_add(keyboardEvent, nullptr);

    std::cout << "Start!\n";

    // Start Redis subscription in a separate thread
    std::thread redisThread(redisSubscriptionThread);

    // Wait for the Redis thread to finish
    redisThread.join();

    std::cout << "Clean up!\n";
    // Clean up
    if (eventBase) 
        event_base_free(eventBase);

    return 0;
}