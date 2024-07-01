module PE3 where

-- can use these if you want to...
import Data.List
import Data.Maybe

data Private = Private { idNumber :: Int, height :: Int, timeToSwap :: Int } deriving Show

data Cell = Empty | Full Private deriving Show

type Area = [[Cell]] 

---------------------------------------------------------------------------------------------
------------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
--------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
--------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
---------------------------------------------------------------------------------------------


-- Note: undefined is a value that causes an error when evaluated. Replace it with
-- a viable definition! Name your arguments as you like by changing the holes: _


--------------------------
-- Part I: Warming up with Abstractions

-- bubbleHumans: Applying a parallel bubble step on integers.
bubbleHumans :: [Int] -> [Int]
bubbleHumans xs = doing xs (length xs-1)
doing xs limit = uptoo xs 0
  where uptoo xs count | count < limit = swap xs
                    | otherwise = xs
                      where swap [x] = [x]
                            swap (x:y:xs) | x > y     = x : (uptoo (y:xs) (count +1))
                                          | otherwise = y : x:(uptoo xs (count +2))

--doonce (length xs-1) xs
 --   where doonce v (a:b:c) | v>0 = swap (a:b:c)
   --                        |otherwise = (a:b:c)
     --                      where swap [] = []
       --                          swap [x] = [x]
         --                        
           --                      swap (a:b:c) | b>a = (a:(b:(doonce (v-2) c)))
             --                                 |otherwise = (a:(doonce (v-1) (b:c)))
                           

-- bubblePrivates: The same thing, but on privates with a key function and an asc/desc option.
bubblePrivates :: (Private -> Int) -> Bool -> [Private] -> [Private]
firstvar (a,_) = a
secvar (_,a) = a
descending xs limit = uptoode xs 0
  where uptoode xs count | count < limit = swapde xs
                    | otherwise = xs
                      where swapde [x] = [x]
                            swapde (x:y:xs) |(firstvar x) > (firstvar y)    = x : (uptoode (y:xs) (count +1))
                                            |(firstvar x) == (firstvar y) = x : (uptoode (y:xs) (count +1))
                                          | otherwise = y : x:(uptoode xs (count +2))
ascending xs limit = uptooas xs 0
  where uptooas xs count | count < limit = swapas xs
                    | otherwise = xs
                      where swapas [x] = [x]
                            swapas (x:y:xs) |(firstvar x) < (firstvar y)    = x : (uptooas (y:xs) (count +1))
                                            |(firstvar x) == (firstvar y) = x : (uptooas (y:xs) (count +1))
                                          | otherwise = y : x:(uptooas xs (count +2))
bubblePrivates u tf lst = if (tf == True) then (descend u lst) else (ascend u lst)
makeitind crt a = zip [crt y| y<- a] [0..]

descend criterian lst = [lst !! (secvar x) | x <- (descending (makeitind criterian lst) (length (makeitind criterian lst)))]
ascend criterian lst = [lst !! (secvar x) | x <- (ascending (makeitind criterian lst) (length (makeitind criterian lst)))]
-- sortPrivatesByHeight: Full sort via multiple bubble steps, calculate the sorting time too!
sortPrivatesByHeight :: [Private] -> ([Private], Int)
a = 0
sortPrivatesByHeight xs = bubbling xs 0
bubbling xs time = deneti xs (length xs-1) 0
    where deneti xs limit time | limit >=0 = let fire = (bubblePrivates height True xs)
                                            in (deneti fire (limit-1) (time+(timebond fire xs 0)))
                                 | otherwise = (xs, time)
timebond [] [] time = time
timebond [a] [b] time = time
timebond (xs:bs:ds) (cs:fs:gs) time = if (idNumber xs == idNumber cs) then (timebond (bs:ds) (fs:gs) time) else (timebond ds gs (maximum [(timeToSwap xs),(timeToSwap bs), time]))




--------------------------
-- Part II: Squeezing your Brain Muscles

-- ceremonialFormation: Sorting again, but with multiple files. Don't forget the time!
ceremonialFormation :: Area -> (Area, Int)
ceremonialFormation army = undefined


-- swapPrivates: Swap two arbitrary privates by ID if they are in the area. Big ouch!
swapPrivates :: Int -> Int -> Area -> Area
swapPrivates _ _ _ = undefined 

-- Best of luck to you, friend and colleague!


























