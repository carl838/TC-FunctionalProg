import Data.Time.Clock (getCurrentTime, diffUTCTime)
import Control.DeepSeq (NFData, deepseq)
import GHC.Generics (Generic)

data Tree a = Empty
            | Node a (Tree a) (Tree a)
            deriving (Show, Generic)

instance NFData a => NFData (Tree a)

insert :: (Ord a) => a -> Tree a -> Tree a
insert x Empty = Node x Empty Empty
insert x (Node y left right)
    | x < y     = Node y (insert x left) right
    | x > y     = Node y left (insert x right)
    | otherwise = Node y left right 


lookupValue :: (Ord a) => a -> Tree a -> Bool
lookupValue _ Empty = False
lookupValue x (Node y left right)
    | x == y    = True
    | x < y     = lookupValue x left
    | otherwise = lookupValue x right

inOrderTraversal :: Tree a -> [a]
inOrderTraversal Empty = []
inOrderTraversal (Node x left right) =
    inOrderTraversal left ++ [x] ++ inOrderTraversal right

estimateMemoryUsage :: Tree a -> Int -> Int -> Int
estimateMemoryUsage Empty elementSize overhead = 0
estimateMemoryUsage (Node _ left right) elementSize overhead =
    overhead + elementSize + estimateMemoryUsage left elementSize overhead + estimateMemoryUsage right elementSize overhead

forceEvaluation :: NFData a => a -> IO ()
forceEvaluation x = x `deepseq` return ()

measureTime :: String -> IO a -> IO a
measureTime description action = do
    start <- getCurrentTime
    result <- action
    end <- getCurrentTime
    putStrLn $ description ++ " took " ++ show (diffUTCTime end start)
    return result

testTreeOperations :: Int -> IO ()
testTreeOperations numElements = do
    let elementSize = 8
    let overhead = 32 
    putStrLn $ "\nTesting " ++ show numElements ++ " elements:"

    measureTime "Tree insertion" $ do
        let tree = foldl (flip insert) Empty [1..numElements]
        forceEvaluation tree
        let memoryUsage = estimateMemoryUsage tree elementSize overhead
        putStrLn $ "Memory usage after inserting " ++ show numElements ++ " elements: " ++ show memoryUsage ++ " bytes"

    measureTime "Tree lookup" $ do
        let tree = foldl (flip insert) Empty [1..numElements]
        forceEvaluation $ lookupValue numElements tree

main :: IO ()
main = do
    testTreeOperations 1000
    testTreeOperations 2000
    testTreeOperations 3000