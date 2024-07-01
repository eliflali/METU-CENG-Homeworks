module PE1 where

import Text.Printf
import Data.List

-- Type synonyms --
type Point = (Double, Double)
type Signal = (Double, Double, Double, Double)

-- This function takes a Double and rounds it to 2 decimal places as requested in the PE --
getRounded :: Double -> Double
getRounded x = read s :: Double
               where s = printf "%.2f" x

-------------------------------------------------------------------------------------------
----------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------

getDistance :: Point -> Point -> Double
absolute a = if (a<0) then -a else a
getDistance (x1, y1) (x2, y2) = getRounded(sqrt((absolute(x1-x2))^2 + (absolute(y1-y2))^2))

-------------------------------------------------------------------------------------------
findAllDistances :: Point -> [Point] -> [Double]
findAllDistances b l = [getDistance b a | a<-l]
-------------------------------------------------------------------------------------------
myzip (a:b) (c:d) = (a,c) : (myzip b d)
position x xs = [i|(y,i) <- (myzip xs [0..n-1]),y == x]
    where n = length xs
findExtremes :: Point -> [Point] -> (Point, Point)
findExtremes b l = ((l!!((position (minimum(findAllDistances b l)) (findAllDistances b l) )!!0)), (l!!((position (maximum(findAllDistances b l)) (findAllDistances b l) )!!0)))

-------------------------------------------------------------------------------------------
north (a, _,_,_) = a
south(_, _, a,_) = a
east(_, a, _, _) =a 
west(_, _,_,a) = a
getSingleAction :: Signal -> String
getSingleAction signal
    | ((north signal == south signal) && (east signal == west signal)) = "Stay"
    | ((north signal == south signal) && (east signal > west signal)) = "East"
    | ((north signal == south signal) && (east signal < west signal)) = "West"
    | ((north signal > south signal) && (east signal == west signal)) = "North"
    | ((north signal < south signal) && (east signal == west signal)) = "South"
    | ((north signal < south signal) && (east signal < west signal)) = "SouthWest"
    | ((north signal > south signal) && (east signal < west signal)) = "NorthWest"
    | ((north signal < south signal) && (east signal > west signal)) = "SouthEast"
    | ((north signal > south signal) && (east signal > west signal)) = "NorthEast"

-------------------------------------------------------------------------------------------

getAllActions :: [Signal] -> [String]
getAllActions signals = [getSingleAction a|a<-signals]

-------------------------------------------------------------------------------------------
counter [] given = 0
counter (a:b) given = if (a==given) then 1+(counter b given) else (counter b given) 
numberOfGivenAction :: Num a => [Signal] -> String -> a
numberOfGivenAction signals action = counter (getAllActions signals) action
