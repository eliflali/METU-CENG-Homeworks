//
// Created by Elif Lale on 28.04.2024.
//

#ifndef OS_HOMEWORK2_ROADCONNECTOR_H
#define OS_HOMEWORK2_ROADCONNECTOR_H

// pass will be overwritten by C F N
class RoadConnector {
public:
    virtual void pass(int car_id, int direction) = 0; // Virtual function
    virtual char get_name() = 0; // Returns the type of connector ('N', 'F', 'C')
};
#endif //OS_HOMEWORK2_ROADCONNECTOR_H
