#ifndef OS_HOMEWORK2_FERRY_H
#define OS_HOMEWORK2_FERRY_H

#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include "RoadConnector.h"
#include "WriteOutput.h"
#include "helper.h"

class Ferry : public RoadConnector {
    pthread_mutex_t lock;
    pthread_cond_t condition;
    pthread_mutex_t already_departed_lock;
    pthread_cond_t already_departed_condition;

    int capacity;
    int count;
    int travel_time;
    int max_wait_time;
    pthread_t timer_thread;
    bool timer_running;
    bool on_way;

    bool wait_limit_reached;
    std::queue<int> car_queue;
    std::queue<int> car_finish_queue;
    std::vector<int> departed_cars;
    char connector_name;
    int connector_id;

public:
    Ferry(char connector_name, int connector_id, int capacity, int travel_time, int max_wait_time)
            : connector_name(connector_name), connector_id(connector_id), capacity(capacity),
              travel_time(travel_time), max_wait_time(max_wait_time), count(0),
              timer_running(false), wait_limit_reached(false), on_way(false) {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&condition, NULL);
        pthread_mutex_init(&already_departed_lock, NULL);
        pthread_cond_init(&already_departed_condition, NULL);
    }

    static void* wait_timer(void* arg) {
        Ferry* ferry = (Ferry*)arg;
        sleep_milli(ferry->max_wait_time);
        pthread_mutex_lock(&ferry->lock);
        ferry->wait_limit_reached = true;
        pthread_cond_broadcast(&ferry->condition);
        pthread_mutex_unlock(&ferry->lock);
        return NULL;
    }

    void pass(int car_id, int direction) override {
        pthread_mutex_lock(&lock);
        car_queue.push(car_id);

        count++;

        if (!timer_running) {
            timer_running = true;
            pthread_create(&timer_thread, NULL, wait_timer, this);
        }

        if (count == capacity || wait_limit_reached) {
            while (!car_queue.empty()) {
                int car = car_queue.front();
                car_queue.pop();
                car_finish_queue.push(car);
                departed_cars[car]=1;
                on_way = true;
                WriteOutput(car, connector_name, connector_id, START_PASSING);
            }
            sleep_milli(travel_time);
            while(!car_finish_queue.empty())
            {
                int car = car_finish_queue.front();
                car_finish_queue.pop();
                WriteOutput(car, connector_name, connector_id, FINISH_PASSING);

            }
            count = 0;
            on_way = false;
            wait_limit_reached = false;
            timer_running = false;
            pthread_cond_broadcast(&condition); // Notify any waiting threads
            departed_cars[car_id]=0;

        } else {
            pthread_cond_wait(&condition, &lock);
            pthread_cond_broadcast(&condition); // Notify any waiting threads
            while (departed_cars[car_id] == 0) {
                pthread_cond_wait(&condition, &lock);
            }
        }

        pthread_mutex_unlock(&lock);
    }

    void set_departed_cars(std::vector<int> dep)
    {
        departed_cars = dep;

    }

    char get_name() override
    {
        return connector_name;
    }
    ~Ferry() {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&condition);
        if (timer_running) {
            pthread_cancel(timer_thread);
            pthread_join(timer_thread, NULL);
        }
    }
};

#endif //OS_HOMEWORK2_FERRY_H