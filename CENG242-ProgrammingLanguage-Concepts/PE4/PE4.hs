module PE4 where

import Data.List
import Data.Maybe

data Room = SeedStorage Int
          | Nursery Int Int
          | QueensChambers
          | Tunnel
          | Empty
          deriving Show

data Nestree = Nestree Room [Nestree] deriving Show

---------------------------------------------------------------------------------------------
------------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
--------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
--------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
---------------------------------------------------------------------------------------------

-- Note: undefined is a value that causes an error when evaluated. Replace it with
-- a viable definition! Name your arguments as you like by changing the holes: _

---------------------------------------------------------------------------------------------

-- Q1: Calculate the nutrition value of a given nest.
nestNutritionValue :: Nestree -> Int
roomreturn (SeedStorage a) = 3*a
roomreturn (Nursery a b) = 7*a + 10*b
roomreturn QueensChambers = 0
roomreturn Tunnel = 0
roomreturn Empty = 0

tostart (Nestree b c) = c
tostart2 (Nestree b c) = b

traversal [] = []
traversal [Nestree a []] = [a]
--traversal [Nestree a (b:c)] = [a]++(traversal [b])++(traversal c)
traversal [Nestree a c] = [a]++(traversin c)

traversin [] = []
traversin (a:b) = (traversal [a])++(traversin b)

nowwedo [] = []
nowwedo a = if ((length a) > 1) then (traversal a) else (traversin a) 


totaling [] = 0
totaling (a:b) = (roomreturn a) + (totaling b)
--nestNutritionValue feedus = 0
nestNutritionValue feedus = totaling (nowwedo [feedus])
--nestNutritionValue feedus = (roomreturn (tostart2 feedus)) + (totaling (nowwedo (tostart feedus)))
--nestNutritionValue $ Nestree (Nursery 392 698) [Nestree Tunnel [Nestree QueensChambers [Nestree (Nursery 959 399) [Nestree Empty 
--[Nestree (SeedStorage 298) [], Nestree Tunnel [], Nestree Empty [], Nestree (SeedStorage 974) [], Nestree Tunnel [Nestree (Nursery 846 964) [], 
--Nestree Empty [Nestree (SeedStorage 874) [], Nestree (SeedStorage 913) [], Nestree Tunnel [], Nestree (SeedStorage 549) [], Nestree Tunnel [Nestree Empty [], 
--Nestree (SeedStorage 167) [Nestree (SeedStorage 814) [Nestree Empty [Nestree Empty [], Nestree (SeedStorage 229) [], Nestree Tunnel [], Nestree Tunnel [], Nestree Empty [], 
--Nestree Tunnel [Nestree (Nursery 157 869) [], Nestree Empty [], Nestree Tunnel [], Nestree (SeedStorage 308) [], Nestree (SeedStorage 300) [], Nestree (Nursery 979 905) [], Nestree (Nursery 863 881) [], Nestree Empty [], 
--Nestree (Nursery 58 771) [Nestree (SeedStorage 283) [], Nestree Empty [], Nestree Tunnel [], Nestree (SeedStorage 58) [Nestree (SeedStorage 659) [], 
--Nestree (SeedStorage 270) [], Nestree (SeedStorage 773) [], Nestree Empty [Nestree Empty [], Nestree (SeedStorage 757) [], Nestree Tunnel [Nestree Empty [], Nestree Empty [], Nestree (SeedStorage 629) []]]]]]]]]]]]]]]]]



-- Q2: Calculate the nutrition value of each root-to-leaf path.
pathNutritionValues :: Nestree -> [Int]
--pathNutritionValues feedus = [31]
pathNutritionValues feedus = travyersin [feedus] 0 
trav [] x = [x]
trav [Nestree a []] x = [x]
trav [Nestree a (b:c)] x = (travyersin [b] x)

travmid [Nestree a []] x = travyersin [] x
travmid [] x = travyersin [] x
travmid [Nestree a (b:c)] x = (travyersin c x)


travyersin [] x = []
travyersin (a:b) x = (trav [a] (x+(roomreturn (tostart2 a)))) ++(travyersin b x)++(travmid [a] (x+(roomreturn (tostart2 a))))




-- Q3: Find the depth of the shallowest tunnel, if you can find one.
isTunnel Tunnel = True
isTunnel _ = False


shallowestTunnel :: Nestree -> Maybe Int
shallowestTunnel (Nestree Tunnel _) = Just 1
shallowestTunnel (Nestree _ []) = Nothing
shallowestTunnel a | travyersinm [a] 0 == [] = Nothing
                   | otherwise = Just(minimum(travyersinm [a] 0))

travm [] x = []
travm [Nestree a []] x = if (isTunnel a) then (travma [] x) else []
travm [Nestree a (b:c)] x = if (isTunnel a) then (travma [] x) else (travyersinm [b] x)

travma [] x = [x]
travmidi [Nestree a []] x = travyersinm [] x
travmidi [] x = travyersinm [] x
travmidi [Nestree a (b:c)] x = (travyersinm c x)


travyersinm [] x = []
travyersinm (a:b) x = (travm [a] (x+1)) ++(travyersinm b x)++(travmidi [a] (x+1))



-- Q4: Find the path to the Queen's Chambers, if such a room exists.
pathToQueen :: Nestree -> Maybe [Room]
isQueen QueensChambers = True
isQueen _ = False

pathToQueen (Nestree QueensChambers _) = Just []
pathToQueen (Nestree _ []) = Nothing
pathToQueen a | (length (travyersinmi [a] [])) == 0 = Nothing
              | otherwise = Just (travyersinmi [a] [])

travmh [] x = []
travmh [Nestree a []] x = if (isQueen a) then (travmah [] x) else []
travmh [Nestree a (b:c)] x = if (isQueen a) then (travmah [] x) else (travyersinmi [b] x)

travmah [] x = x
travmidih [Nestree a []] x = travyersinmi [] x
travmidih [] x = travyersinmi [] x
travmidih [Nestree a (b:c)] x = (travyersinmi c x)


travyersinmi [] x = []
travyersinmi (a:b) x = if (isQueen (tostart2 a)) then (travmah [] x) else (travmh [a] (x++[(tostart2 a)])) ++(travyersinmi b x)++(travmidih [a] (x++[(tostart2 a)]))

-- Q5: Find the quickest depth to the Queen's Chambers, including tunnel-portation :)
quickQueenDepth :: Nestree -> Maybe Int
quickQueenDepth (Nestree QueensChambers _) = Just 1
quickQueenDepth (Nestree _ []) = Nothing
quickQueenDepth dx | (isNothing (pathToQueen dx)) = Nothing
                   | (isNothing (shallowestTunnel dx)) = Just(length(fromMaybe [] (pathToQueen dx))+1)
                   |(length (fromMaybe [] (pathToQueen dx)) < (fromMaybe 10000 (shallowestTunnel dx))) = Just (length (fromMaybe [] (pathToQueen dx))+1) 
                   | otherwise = Just (length(trial dx) +1 )
                   --tunnelelimination (pathToQueen dx)
-- (length (fromMaybe [] (pathToQueen dx)) < (fromMaybe 10000 (shallowestTunnel dx))) = Just (length (fromMaybe [] (pathToQueen dx)))
tunnelelimination [] x y = x
tunnelelimination (a:b) x y = if (isTunnel a) then (tunnelelimination b (x++[y]) (y+1)) else (tunnelelimination b x (y+1))
concour [] count y (g:gs) res = res ++ [g] ++ gs
concour _ count y [] res = res
concour (x:xs) count y  (g:gs) res | (((length xs)>0) && (count < x) && (y == True)) = concour (x:xs) (count+1) True gs (res++[g])
                                   |(((length xs)>0) && (count == x) && (y == True)) = concour xs (count+1) False gs (res++[g])
                                   |(((length xs)>0) && (count < x) && (y == False)) = concour (x:xs) (count+1) False gs res
                                   |(((length xs)>0) && (count == x) && (y == False)) = concour xs (count+1) True gs (res ++ [g])
                                   | ((length xs)<0) = res ++ [g] ++ gs
                                   
trial dx = concour (tunnelelimination(fromMaybe [] (pathToQueen dx)) [] 0) 0 True (fromMaybe [] (pathToQueen dx)) []




                     



-- Example nest given in the PDF.
exampleNest :: Nestree
exampleNest = 
  Nestree Tunnel [
    Nestree (SeedStorage 15) [
      Nestree (SeedStorage 81) []
    ],
    Nestree (Nursery 8 16) [
      Nestree Tunnel [
        Nestree QueensChambers [
          Nestree (Nursery 25 2) []
        ]
      ]
    ],
    Nestree Tunnel [
      Nestree Empty [],
      Nestree (SeedStorage 6) [
        Nestree Empty [],
        Nestree Empty []
      ]
    ]
  ]

-- Same example tree, with tunnels replaced by Empty
exampleNestNoTunnel :: Nestree
exampleNestNoTunnel = 
  Nestree Empty [
    Nestree (SeedStorage 15) [
      Nestree (SeedStorage 81) []
    ],
    Nestree (Nursery 8 16) [
      Nestree Empty [
        Nestree QueensChambers [
          Nestree (Nursery 25 2) []
        ]
      ]
    ],
    Nestree Empty [
      Nestree Empty [],
      Nestree (SeedStorage 6) [
        Nestree Empty [],
        Nestree Empty []
      ]
    ]
  ]
