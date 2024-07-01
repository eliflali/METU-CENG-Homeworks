#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <utility> // For std::pair
#include <string> // For std::string
#include <fstream>
#include <sstream>
#include <dirent.h>

#include "Car.h"
#include "CrossRoad.h"
#include "Ferry.h"
#include "NarrowBridge.h"
#include "RoadConnector.h"
#include "WriteOutput.h"
#include "helper.h"

// Car thread function
void* car_thread(void* arg) {
    CarData* data = static_cast<CarData*>(arg);
    Car* car = data->car;
    std::vector<RoadConnector*>& connectors = data->connectors;

    for (const auto& step : car->path) {
        std::string connector_id_str = step.first;

        std::pair<int, int> directions = step.second;

        char connector_type = connector_id_str[0];
        int connector_id = std::stoi(connector_id_str.substr(1));

        WriteOutput(car->id, connector_type, connector_id, TRAVEL);
        sleep_milli(car->travel_time);
        WriteOutput(car->id, connector_type, connector_id, ARRIVE);
        RoadConnector* connector = connectors[connector_id];
        connector->pass( car->id, step.second.first); // Pass through the connector with 'from_direction'
    }
    return NULL;
}


// Read simulation data from standard input
void read_simulation_data(std::vector<NarrowBridge>& narrowBridges, std::vector<Ferry>& ferries,std::vector<Ferry>& reverse_ferries, std::vector<Crossroad>& crossroads, std::vector<Car>& cars) {
    int NN, FN, CN, CAN;
    //The first line contains the number of narrow bridges (NN )
    std::cin >> NN;

    // narrowbridges
    for (int i = 0; i < NN; ++i) {
        int NT, NM;
        //NT is an integer representing the travel time of the narrow bridge in milliseconds.
        //NM is an integer representing the maximum wait time of the narrow bridge in milliseconds.
        std::cin >> NT >> NM;

        narrowBridges.emplace_back('N', i, NT, NM);
    }


    // next line contains the number of ferries (FN )
    std::cin >> FN;

    // ferries
    for (int i = 0; i < FN; ++i) {
        //FT is an integer representing the travel time of the ferry in milliseconds.
        //FM is an integer representing the maximum wait time of the ferry in milliseconds. •
        //FC is an integer representing the capacity of the ferry.
        int FT, FM, FC;
        std::cin >> FT >> FM >> FC;

        ferries.emplace_back('F' ,i, FC, FT, FM);
        reverse_ferries.emplace_back('F' ,i, FC, FT, FM);
    }
    std::cin >> CN;
    // crossroads
    for (int i = 0; i < CN; ++i) {
        //CT is an integer representing the travel time of the crossroad in milliseconds.
        //CM is an integer representing the maximum wait time of the crossroad in milliseconds.
        int CT, CM;
        std::cin >> CT >> CM;
        crossroads.emplace_back('C', i,  4, CT, CM); // Assuming 4 directions by default
    }

    //next line contains the number of cars (CAN )
    std::cin >> CAN;

    // cars
    for (int i = 0; i < CAN; ++i) {
        //CAT is an integer representing the travel time of the car between connectors in milliseconds.
        //CAP is an integer representing the path length
        int CAT, CAP;
        std::cin >> CAT >> CAP;

        std::vector<std::pair<std::string, std::pair<int, int> > > path;
        for (int j = 0; j < CAP; ++j) {
            //PCM is the string representing the Mth connector
            std::string PCM;
            int FC, TC;
            //FCM is the from direction.
            //For ferry and narrow bridges, it can be either one or zero. For crossroads can be valued in [0, 3].
            //TCM is the to direction.
            //For ferry and narrow bridges, it can be either one or zero. For crossroads can be valued in [0, 3].
            //From and to directions can be the same.
            std::cin >> PCM >> FC >> TC;
            path.emplace_back(PCM, std::make_pair(FC, TC));
        }
        cars.emplace_back(i, CAT, path);


    }
    std::vector<int> car_depart;
    for(int i = 0; i< CAN; i++)
    {
        car_depart.emplace_back(0);

    }
    for(int i = 0; i<ferries.size(); i++)
    {
        ferries[i].set_departed_cars(car_depart);
        reverse_ferries[i].set_departed_cars(car_depart);
    }

}

RoadConnector* connector_retriever(std::string connector_name, std::vector<NarrowBridge>& narrowBridges, std::vector<Ferry>& ferries,std::vector<Ferry>& reverse_ferries, std::vector<Crossroad>& crossroads, std::vector<Car>& cars, int direction)
{
    RoadConnector* connector;

    int connector_id = std::stoi(connector_name.substr(1));
    switch(connector_name[0])
    {
        case 'N':
        {
            connector = &narrowBridges[connector_id];
            break;

        }
        case 'F':
        {
            switch(direction)
            {
                case 0:
                {
                    connector = &ferries[connector_id];
                    break;
                }
                case 1:
                {
                    connector = &reverse_ferries[connector_id];
                    break;
                }
            }
            break;
        }
        case 'C':
        {
            connector = &crossroads[connector_id];
            break;
        }
        default:
        {
            std::cout<< "error given connector is wrong: " << connector_name << std::endl;
        }
    }
    return connector;

}
int main() {

    std::vector<NarrowBridge> narrowBridges;
    std::vector<Ferry> ferries;
    std::vector<Crossroad> crossroads;
    std::vector<Car> cars;
    std::vector<Ferry> reverse_ferries;
    // call parser
    read_simulation_data(narrowBridges, ferries, reverse_ferries, crossroads, cars);

    // hold threads in vectors
    std::vector<pthread_t> threads(cars.size());
    std::vector<CarData> carData(cars.size());

    std::vector<std::vector<RoadConnector*> > connectors;  // This should be set up correctly

    //creating connectors list:
    for (int i = 0; i< cars.size(); i++)
    {
        std::vector<RoadConnector*> connector;
        std::vector<std::pair<std::string, std::pair<int, int> > > car_path = cars[i].path;

        for(int j = 0; j<car_path.size(); j++)
        {
            std::pair<std::string, std::pair<int, int> > cur_pair = car_path[j];
            std::string cur_connector = cur_pair.first;
            int direction = cur_pair.second.first;
            RoadConnector* cur_ptr = connector_retriever(cur_connector, narrowBridges, ferries, reverse_ferries,crossroads, cars, direction);
            connector.push_back(cur_ptr);
        }
        connectors.push_back(connector);

    }
    InitWriteOutput();
    // creating car threads:
    for (size_t i = 0; i < cars.size(); ++i) {
        carData[i].car = &cars[i];

        carData[i].connectors = connectors[i];
        pthread_create(&threads[i], NULL, car_thread, &carData[i]);
    }
    // waiting threads:
    for (auto& thread : threads) {
        pthread_join(thread, NULL);
    }
    return 0;
}

