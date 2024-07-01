#include <iostream>
#include <string>
#include "highSpeedTrain.h"

/**
 * Empty Constructor
 * Initializes attributes according to the default route, namely "Florence -> Rome in 90 mins.".
 **/
HighSpeedTrain::HighSpeedTrain(){
    // YOUR CODE HERE
    from = "Florence";
    to = "Rome";
    eta = 90;
}

/*
 * Constructor
 * Fills the attributes according to given values.
 *
 * @param from std::string starting location of the travel
 * @param to std::string destination of the travel
 * @param eta int estimated time of arrival in minutes.
 */
HighSpeedTrain::HighSpeedTrain(std::string fr, std::string t, int et){
    // YOUR CODE HERE
    from = fr;
    to = t;
    eta=et;
}

/*
 * Copy Constructor
 * Fills the attributes according to attributes of the given object with one exception.
 * Since it requires an extra time to replace a train, the eta of the train will be delayed 60 mins.
 *
 * @param h HighSpeedTrain train that is going to be replaced in the route.
 * @return this new train with updated eta.
 */
HighSpeedTrain::HighSpeedTrain(const HighSpeedTrain &h){
    // YOUR CODE HERE
    from=h.from;
    to=h.to;
    eta= h.eta+60;
    
}

/*
 * Stream Extraction Overload
 * Formats the output of a given train as "source -> destination in # mins."
 *
 * @param output std::ostream output stream
 * @param h HighSpeedTrain the train whose route is going to be summarized.
 */
std::ostream &operator<<(std::ostream &output, const HighSpeedTrain &h){
    // YOUR CODE HERE
    output<<h.from + " -> " + h.to + " in " + std::to_string(h.eta) + " mins.";
    
    return output;
}

/*
 * Addition Overload
 * This operator will be used to represent transfers.
 * If the destination of second train does not match with the source of the first one,
 * then it produces the string of "Transfer is not possible!" with NO NEW LINE AT THE END.
 * Otherwise it combines the routes and calculate the total eta and produces the string in
 * the form of "source1 -> destination1 -> destination2 in # mins."
 *
 * @param h1 HighSpeedTrain the first train
 * @param h2 HighSpeedTrain the second train
 * @return one of the strings as explained above.
 */
std::string operator+(const HighSpeedTrain &h1, const HighSpeedTrain &h2){
    // YOUR CODE HERE
    if(h2.from != h1.to)
    {
        return "Transfer is not possible!";
    }
    return h1.from + " -> " + h1.to + " -> " + h2.to +" in " + std::to_string(h1.eta+h2.eta) + " mins."; // ADJUST THIS AFTER YOUR IMPLEMENTATION IS DONE.
}

/*
 * Comparison Overloads
 * This operator will be used to order the summaries of train.
 * The priority for comparison is here:
 * 1. Compare the source of the trains alphabetically (ex. Florence < Milano)
 * 2. Compare the destination of the trains alphabetically (the same example above)
 * 3. Compare the eta's of the trains, the train with less eta will be smaller.
 *
 * @param h1, h2 HighSpeedTrain the trains to be compared.
 */
bool operator<(const HighSpeedTrain &h1, const HighSpeedTrain &h2){
    // YOUR CODE HERE
    if(std::string(h1.from)<std::string(h2.from))return true;
    else if(std::string(h1.from)==std::string(h2.from) && std::string(h1.to)<std::string(h2.to)) return true;
    else if(std::string(h1.from)==std::string(h2.from) && std::string(h1.to)==std::string(h2.to) && h1.eta<h2.eta) return true;
    else return false;
    // ADJUST THIS AFTER YOUR IMPLEMENTATION IS DONE.
}

bool operator>(const HighSpeedTrain &h1, const HighSpeedTrain &h2){
    // YOUR CODE HERE
    if(std::string(h1.from)>std::string(h2.from))return true;
    else if(std::string(h1.from)==std::string(h2.from) && std::string(h1.to)>std::string(h2.to)) return true;
    else if(std::string(h1.from)==std::string(h2.from) && std::string(h1.to)==std::string(h2.to) && h1.eta>h2.eta) return true;
    else return false;
  // ADJUST THIS AFTER YOUR IMPLEMENTATION IS DONE.
}
