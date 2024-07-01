#ifndef OS_HOMEWORK2_NARROWBRIDGE_H
#define OS_HOMEWORK2_NARROWBRIDGE_H

#include "RoadConnector.h"
#include <pthread.h>
#include <unistd.h>
#include "WriteOutput.h"
#include "helper.h"

class NarrowBridge : public RoadConnector {
    pthread_mutex_t lock;
    pthread_cond_t condition; // Used for direction change notification
    int active_direction;     // The current active direction of the bridge
    int car_count;            // Number of cars currently passing
    int max_wait_time;
    bool on_pass;             // if there is a car on pass delay
    std::vector<std::queue<int> > car_queue;
    int travel_time;
    char connector_name;
    int connector_id;
    pthread_t timer_thread;   // Thread for managing the wait limit
    pthread_t pass_delay_thread;   // Thread for managing the wait limit
    std::queue<int> passing_lane;
    bool timer_running;       // Indicates if the timer thread is running
    bool wait_limit_reached;  // Indicates if the max wait limit is reached

public:
    NarrowBridge(char connector_name, int connector_id, int travel_time, int max_wait_time)
            : connector_name(connector_name), connector_id(connector_id),
              active_direction(-1), car_count(0), max_wait_time(max_wait_time), travel_time(travel_time),
              timer_running(false), wait_limit_reached(false), on_pass(false) {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&condition, NULL);
        std::queue<int> dir1;
        std::queue<int> dir2;
        car_queue.emplace_back(dir1);
        car_queue.emplace_back(dir2);
    }

    static void* wait_limit_checker(void* arg) {
        NarrowBridge* bridge = (NarrowBridge*)arg;
        sleep_milli(bridge->max_wait_time);

        pthread_mutex_lock(&bridge->lock);
        bridge->wait_limit_reached = true;
        pthread_cond_broadcast(&bridge->condition); //notify waiting threads
        pthread_mutex_unlock(&bridge->lock);
        return NULL;
    }

    static void* pass_delay_checker(void* arg)
    {
        NarrowBridge* bridge = (NarrowBridge*)arg;
        sleep_milli(PASS_DELAY-1);
        bridge->passing_lane.pop();
        return NULL;
    }

    void pass(int car_id, int direction) override {
        pthread_mutex_lock(&lock);
        car_queue[direction].push(car_id);
        // timer thread not running -> start
        if (!timer_running) {
            timer_running = true;
            pthread_create(&timer_thread, NULL, wait_limit_checker, this);
        }

        // if opposite direction active
        while(active_direction != -1 && active_direction != direction) {
            pthread_cond_wait(&condition, &lock);
        }

        active_direction = direction; // assume active_direction = -1
        car_count++;  // car count incremented -> one car entered to bridge
        // passing delay for incoming cars
        on_pass = true;
        while(!passing_lane.empty()) {
            sleep_milli(1);
        }
        passing_lane.push(car_id);
        pthread_create(&pass_delay_thread, NULL, pass_delay_checker, this);
        //sleep_milli(PASS_DELAY);
        //passing_lane.pop();

        int cur_car = car_queue[active_direction].front();
        car_queue[active_direction].pop();
        pthread_mutex_unlock(&lock);

        WriteOutput(cur_car, connector_name, connector_id, START_PASSING);
        on_pass = false;
        sleep_milli(travel_time);
        WriteOutput(cur_car, connector_name, connector_id, FINISH_PASSING);

        pthread_mutex_lock(&lock);

        car_count--;  // Decrement car count as it leaves the bridge

        if (car_count == 0) {
            active_direction = -1; // Reset direction if no cars are passing (is it correct?)

            if (wait_limit_reached) {
                active_direction = -1; // Reset direction if no cars are passing (is it correct?)
                wait_limit_reached = false;
                timer_running = false;
            }
            pthread_cond_broadcast(&condition); // Notify waiting threads for a direction change

        }



        pthread_mutex_unlock(&lock);
    }

    char get_name() override
    {
        return connector_name;
    }

    ~NarrowBridge() {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&condition);
        if (timer_running) {
            pthread_cancel(timer_thread); // Cancel the timer thread
            pthread_join(timer_thread, NULL); // Wait for the timer thread to finish
        }
    }
};

#endif //OS_HOMEWORK2_NARROWBRIDGE_H