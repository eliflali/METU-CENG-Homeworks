#ifndef OS_HOMEWORK2_CROSSROAD_H
#define OS_HOMEWORK2_CROSSROAD_H

#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include "RoadConnector.h"
#include "WriteOutput.h"
#include "helper.h"

// Crossroad class
class Crossroad : public RoadConnector {
    pthread_mutex_t lock; // Mutex for thread safety
    pthread_cond_t condition; // Condition variable for signaling
    char connector_name;
    int connector_id;
    int active_road; // Current active road
    int road_count; // Number of roads
    int max_wait_time; // Maximum wait time for other lanes
    int travel_time; // Time to cross the crossroad
    int car_count;
    std::vector<std::queue<int> > car_queue;
    std::queue<int> passing_lane;
    pthread_t pass_delay_thread;   // Thread for managing the wait limit
    pthread_t wait_timer_thread; // Timer thread for wait limit
    std::vector<int> road_track;
    bool timer_running; // Indicates if the timer is running
    bool wait_limit_reached; // Indicates if the maximum wait limit is reached

public:
    Crossroad(char connector_name, int connector_id, int road_count, int travel_time, int max_wait_time)
            : connector_name(connector_name), connector_id(connector_id), road_count(road_count), active_road(-1),
              travel_time(travel_time), max_wait_time(max_wait_time),
              timer_running(false), wait_limit_reached(false), car_count(0) {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&condition, NULL); // Initialize condition variable
        for(int i = 0; i<road_count; i++)
        {
            road_track.emplace_back(0);
        }
        std::queue<int> car_queue_dir1;
        std::queue<int> car_queue_dir2;
        std::queue<int> car_queue_dir3;
        std::queue<int> car_queue_dir4;
        car_queue.emplace_back(car_queue_dir1);
        car_queue.emplace_back(car_queue_dir2);
        car_queue.emplace_back(car_queue_dir3);
        car_queue.emplace_back(car_queue_dir4);

    }

    static void* wait_timer(void* arg) {
        Crossroad* crossroad = (Crossroad*)arg;
        sleep_milli(crossroad->max_wait_time); // Convert to microseconds

        pthread_mutex_lock(&crossroad->lock);
        crossroad->wait_limit_reached = true;
        pthread_cond_broadcast(&crossroad->condition); // Broadcast to all waiting threads
        pthread_mutex_unlock(&crossroad->lock);

        return NULL;
    }

    static void* pass_delay_checker(void* arg)
    {
        Crossroad* bridge = (Crossroad*)arg;
        sleep_milli(PASS_DELAY - 1);
        bridge->passing_lane.pop();
        return NULL;
    }

    void pass(int car_id, int direction) override {
        pthread_mutex_lock(&lock);
        car_queue[direction].push(car_id);

        if (!timer_running) {
            // Start the wait timer if it's not running
            timer_running = true;
            pthread_create(&wait_timer_thread, NULL, wait_timer, this);
        }

        // Wait if the bridge is occupied by the opposite direction
        while (active_road != -1 && active_road != direction) {
            road_track[direction] = 1;
            pthread_cond_wait(&condition, &lock);
        }

        active_road = direction;
        car_count++;  // Increment car count as it enters the bridge

        // Simulate passing delay for cars in the same direction
        /*if (car_count > 1) {
            sleep_milli(PASS_DELAY);
        }*/
        while(!passing_lane.empty()) {
            sleep_milli(1);
        }
        passing_lane.push(car_id);
        pthread_create(&pass_delay_thread, NULL, pass_delay_checker, this);

        int cur_car = car_queue[active_road].front();
        car_queue[active_road].pop();
        pthread_mutex_unlock(&lock);


        WriteOutput(cur_car, connector_name, connector_id, START_PASSING);

        sleep_milli(travel_time); // Simulate crossing time

        WriteOutput(cur_car, connector_name, connector_id, FINISH_PASSING);

        pthread_mutex_lock(&lock);

        car_count--;  // Decrement car count as it leaves the bridge
        if (car_count == 0) {
            road_track[active_road] = 0;
            int prev_active = active_road;
            //active_road = (active_road + 1) % road_count; // Cycle to the next road
            int cycle = 0;
            while(road_track[active_road] != 1 && cycle <= road_count)
            {
                active_road = (active_road + 1) % road_count;
                cycle++;
            }

            if(prev_active != active_road)
            {
                pthread_cond_broadcast(&condition); // Notify waiting threads for a direction change
            }

            if(cycle>road_count)
            {
                active_road = -1;
            }
            if (wait_limit_reached) {
                wait_limit_reached = false;
                timer_running = false;
                pthread_cond_broadcast(&condition); // Notify waiting threads for a direction change
            }
        }
        pthread_mutex_unlock(&lock);
    }

    char get_name() override {
        return connector_name; // Type identifier for Crossroad
    }

    ~Crossroad() {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&condition); // Clean up condition variable

        if (timer_running) {
            pthread_cancel(wait_timer_thread); // Ensure the timer stops
        }
    }
};

#endif //OS_HOMEWORK2_CROSSROAD_H