//
// Created by Elif Lale on 28.04.2024.
//

#ifndef OS_HOMEWORK2_CAR_H
#define OS_HOMEWORK2_CAR_H
#include <vector>
#include "RoadConnector.h"
// Define the car structure
struct Car {
    int id;
    int travel_time;
    //{string, {int, int}}
    // {PCM ,{FC, TC}}
    // PCM is the string representing the Mth connector on the path with the format:
    // {N or F or C for connector type}{ConnectorID}
    // FCM is the from direction.
    // For ferry and narrow bridges, it can be either one or zero.
    // For crossroads can be valued in [0, 3].
    //TCM is the to direction. For ferry and narrow bridges,
    // it can be either one or zero.
    // For crossroads can be valued in [0, 3].
    // From and to directions can be the same.
    std::vector<std::pair<std::string, std::pair<int, int> > > path;

    // Constructor for Car
    Car(int id, int travel_time, std::vector<std::pair<std::string, std::pair<int, int> > > path)
            : id(id), travel_time(travel_time), path(std::move(path)) {}
};

// Structure to pass to car_thread
struct CarData {
    Car* car; // Pointer to the Car object
    std::vector<RoadConnector*> connectors; // Vector of connectors
};
#endif //OS_HOMEWORK2_CAR_H
