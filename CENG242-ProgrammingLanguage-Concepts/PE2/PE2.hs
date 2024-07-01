module PE2 where

import Text.Printf

type Point = (Int, Int)
type Dimensions = (Int, Int)
type Vector = (Int, Int)

getRounded :: Double -> Double
getRounded x = read s :: Double
               where s = printf "%.2f" x

castIntToDouble x = (fromIntegral x) :: Double

-------------------------------------------------------------------------------------------
----------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------------------------
getVector :: String -> Vector
getVector d
        |(d == "North") = (0,1)
        |(d == "South") = (0,-1)
        |(d == "East") = (1, 0)
        |(d == "West") = (-1, 0)
        |(d == "NorthWest") = (-1, 1)
        |(d == "SouthWest") = (-1, -1)
        |(d == "NorthEast") = (1, 1)
        |(d == "SouthEast") = (1, -1)
        |(d == "Stay") = (0,0)

-------------------------------------------------------------------------------------------------------------------------------
getAllVectors :: [String] -> [Vector]
getAllVectors l = [getVector x| x<-l]

-------------------------------------------------------------------------------------------------------------------------------

producePath :: Point -> [String] -> [Point]
adder (a, b) (c,d) = (a+c, b+d)
helped x [] = [x]
helped x (a:b) = x : (helped (adder x a) b)
producePath initial actions = helped initial (getAllVectors actions)
-------------------------------------------------------------------------------------------------------------------------------
takePathInArea :: [Point] -> Dimensions -> [Point]
isAllowed (a, b) (c, d)
            |((a<0) || (b<0)) = False
            |((a>=c) && (b>=d)) = False
            |((a>=c) && (b<=d)) = False
            |((a<=c) && (b>=d)) = False
            |((a<c) && (b<d)) = True
helper (a:b) 0 (bx,by) = []
helper [] a (bx,by) = []
helper (a:b) 1 (bx, by) = if (isAllowed a (bx,by)) then a:(helper b 1 (bx, by)) else helper (a:b) 0 (bx, by)
takePathInArea path (bx, by) = helper path 1 (bx, by)


-------------------------------------------------------------------------------------------------------------------------------

remainingObjects :: [Point] -> Dimensions -> [Point] -> [Point]
isEqual (a, b) (c, d) = if ((a == c) && (b == d)) then True else False
isIncluded [] c = False
isIncluded (a:b) c = if (isEqual a c) then True else (isIncluded b c)
lasthelper path [] = []
lasthelper path (a:b) = if (isIncluded path a) then (lasthelper path b) else a:(lasthelper path b)
remainingObjects path border objects = lasthelper (takePathInArea path border) objects

-------------------------------------------------------------------------------------------------------------------------------
averageStepsInSuccess :: [[Point]] -> Dimensions -> [Point] -> Double
nothingRemained [] border c = []
nothingRemained (a:b) border c = if (length (remainingObjects a border c) == 0) then a:(nothingRemained b border c) else (nothingRemained b border c)
lengthcalculator [] = 0
lengthcalculator (a:b) = (length a) + (lengthcalculator b)
helperst (a:b) 0 (bx,by) = False
helperst [] a (bx,by) = True
helperst (a:b) 1 (bx, by) = if (isAllowed a (bx,by)) then helperst b 1 (bx, by) else helperst (a:b) 0 (bx, by)
isPathOkay paths border = [x| x<-paths, (helperst x 1 border)]
averageStepsInSuccess paths border objects = castIntToDouble ((lengthcalculator(nothingRemained (isPathOkay paths border) border objects))) / castIntToDouble(length (isPathOkay paths border))

























