#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <vector>
#include <hiredis/hiredis.h>
#include <opencv2/opencv.hpp>

using namespace std::literals;

const bool INFINITE_LOOP = true;
// pub 배속 관련한 변수
const int PUB_SPEED = 4; // 배속

void producerThread(redisContext* redis) {
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for the consumer thread to start first

    const char* channel = "nas:image:ir";
    const char* video_path = "ir_hannara.mp4";

    cv::VideoCapture video(video_path);
    if (!video.isOpened()) {
        std::cout << "Failed to open video file: " << video_path << std::endl;
        return;
    }
    
    double frameRate = video.get(cv::CAP_PROP_FPS);
    std::cout << "frameRate: " << frameRate << "FPS" << std::endl;
    float sleepDuration = 1000.0 / frameRate; // Calculate sleep duration in milliseconds

    // 배속 관련
    sleepDuration = sleepDuration / PUB_SPEED;

    std::cout << "sleepDuration: " << sleepDuration << "ms" << std::endl;

    std::chrono::high_resolution_clock::time_point startTime, endTime;

    while (true) {
        startTime = std::chrono::high_resolution_clock::now();

        cv::Mat frame;
        video >> frame;
        
        if (frame.empty()) {
            if (INFINITE_LOOP) {
                video.set(cv::CAP_PROP_POS_FRAMES, 0);
                continue;
            }
            else {
                std::cout << "End of video reached." << std::endl;
                break;
            }
        }

        // 16'은 'CV_8UC3'에 해당하며, 이는 3개의 채널이 있는 8비트 부호 없는 정수
        std::cout << "Frame data type: " << frame.type() << std::endl;
        std::cout << "Frame size: " << frame.size() << std::endl;
        std::cout << "Frame channels: " << frame.channels() << std::endl;

        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);

        // ERROR
        // std::vector<char> buffer(frame.data, frame.data + frame.total());

        if (buffer.empty()) {
            std::cout << "Buffer is empty. Failed to decode image." << std::endl;
            return;
        }

        std::cout << "buffer size: " << buffer.size() << std::endl;

        // Publish message
        redisReply* reply = (redisReply*)redisCommand(redis, "PUBLISH %s %b", channel, buffer.data(), buffer.size());
        if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
            std::cout << "Failed to publish message" << std::endl;
        }

        freeReplyObject(reply);

        // Save the image as RGB JPG

        // cv::Mat image = cv::imdecode(buffer, cv::IMREAD_COLOR);
        // if (image.empty()) {
        //     std::cout << "Failed to decode image" << std::endl;
        //     return;
        // }

        // std::string outputFilePath = "output.jpg";
        // cv::imwrite(outputFilePath, image);
        
        endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        std::cout << "Elapsed time: " << duration << " ms, File size: " << buffer.size() << std::endl;

        int sleepAdjustment = static_cast<int>(sleepDuration) - duration;
        if (sleepAdjustment > 0) {
            std::cout << "sleepAdjustment time: " << sleepAdjustment << "ms" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepAdjustment));
        }
    }

    video.release();
}

int main() {
    // Redis connection setup
    redisContext* redis = redisConnect("localhost", 6379);
    if (redis == nullptr || redis->err) {
        std::cout << "Failed to connect to Redis: " << (redis == nullptr ? "Memory allocation failed" : redis->errstr) << std::endl;
        return 1;
    }

    std::thread producer(producerThread, redis);

    producer.join();

    // Redis connection cleanup
    redisFree(redis);

    return 0;
}