-----------------------------------------------------------------------------
-- |
-- Module      :  IL2212.Utilities
-- Copyright   :  (c) George Ungureanu, 2018
-- License     :  BSD-style (see the file LICENSE)
-- 
-- Maintainer  :  ugeorge@kth.se
-- Stability   :  stable
-- Portability :  portable
--
-- Contains utility functions and additional vector skeletons.
-----------------------------------------------------------------------------
module IL2212.Utilities (
  -- * Matrix skeletons

  -- | These skeletons (patterns of parallel computation and
  -- communication) are not defined in the @ForSyDe.Shallow@ library
  -- and are provided as local functions.
  mapMatrix, zipWithMatrix, reduceMatrix, rotateMatrix,
  lengthX, lengthY,

  -- * Image utilities

  -- | Collection of functions for reading, printing or converting
  -- between image formats.
  toImage, fromImage, wrapImageF, printImage,
  readPPM, readAllPPM, ppm2img, partition
  ) where

import Data.List (intercalate)
import ForSyDe.Shallow.Core.Vector

-- | the /map (farm) parallel pattern/ with one input, on matrices.
mapMatrix     = mapV . mapV

-- | the /map (farm) parallel pattern/ with two inputs, on matrices.
zipWithMatrix = zipWithV . zipWithV

-- | the /reduce parallel pattern/ on matrices. 
reduceMatrix f = reduceV f . mapV (reduceV f)

lengthX :: Vector (Vector a) -> Int
lengthX = lengthV . headV

lengthY :: Vector (Vector a) -> Int
lengthY = lengthV


-- | Parallel communication (data access) pattern which "rotates" a
-- matrix. The rotation is controled with the /x/ and /y/ index
-- arguments as following:
--
-- * @(> 0)@ : rotates the matrix right/down with the corresponding
-- number of positions.
-- 
-- * @(= 0)@ : does not modify the position for that axis.
-- 
-- * @(< 0)@ : rotates the matrix left/up with the corresponding
-- number of positions.
rotateMatrix :: Int -- ^ index on X axis
             -> Int -- ^ index on Y axis
             -> Vector (Vector a)
             -> Vector (Vector a)
rotateMatrix x y = rotateV y . mapV (rotateV x)

-- | Partitions a list of values into a list of list of values,
-- similar to a matrix. See 'toImage'.
partition :: Int -> Int -> [a] -> [[a]]
partition x y list
  | y - length image > 1 = error "image dimention Y mismatch"
  | otherwise = image
  where
    image = groupEvery x list

groupEvery n [] = []
groupEvery n l | length l < n = error "input P3 PPM image is ill-formed"
               | otherwise    = take n l : groupEvery n (drop n l)

-- | Converts a list intro an 'ImageProcessing.Image' (i.e. matrix).
toImage :: Int               -- ^ number of columns (X dimension)
        -> Int               -- ^ number of rows (Y dimension)
        -> [a]               -- ^ list of pixels
        -> Vector (Vector a) -- ^ 'ImageProcessing.Image' of pixels
toImage x y = vector . map vector . partition x y

-- | Converts an 'ImageProcessing.Image' back to a list. See
-- 'toImage'.
fromImage =  concatMap fromVector . fromVector

-- | Applies a function of images on a list. It basically performs
--
-- fromImage . function . toImage x y
wrapImageF :: Int -- ^ X dimension
           -> Int -- ^ Y dimension
           -> (Vector (Vector a) -> Vector (Vector b))
           -- ^ function of images
           -> [a] -- ^ Input list
           -> [b] -- ^ Output list
wrapImageF x y f = fromImage . f . toImage x y

-- | Reads the contents of a
-- <https://en.wikipedia.org/wiki/Netpbm_format P3 PPM file> and
-- returns a list with the pixel values, along with the dimensions of
-- the image.
readPPM :: String            -- ^ file content
        -> (Int, Int, [Int]) -- ^ (X dimension, Y dimension, list of pixel values)
readPPM str = (dimX, dimY, image)
  where
    imageData = map read $ tail $ words str :: [Int]
    (dimX, dimY, _, image)  = getHeader imageData
    getHeader (x:y:max:img) = (x, y, max, img)

-- | IO function which takes a list of paths to P3 PPM images, reads
-- the files with 'readPPM', checks if all images have the same
-- dimensions, concatenates all images and returns the obtained list
-- of pixel values along with the picture dimensions.
readAllPPM :: [FilePath]           -- ^ list of paths to PPM images
           -> IO (Int, Int, [Int])
           -- ^ IO (X dimension, Y dimension, list of pixel values from all images)
readAllPPM paths = do
  imageFiles <- mapM readFile paths
  let (xs,ys,images) = unzip3 $ map readPPM imageFiles
      xdim = head xs
      ydim = head ys
      imageSigs
        | not (all (==xdim) xs) = error "not all images have the same X dimension"
        | not (all (==ydim) ys) = error "not all images have the same Y dimension"
        | otherwise = foldl1 (++) images
  return (xdim, ydim, imageSigs)


-- | Reads the contents of a
-- <https://en.wikipedia.org/wiki/Netpbm_format P3 PPM file> and
-- returns a ready image. Useful for testing the image processing
-- functions from the "ImageProcessing" module.
ppm2img str = let (x, y, ppm) = readPPM str
              in  toImage x y ppm

-- | Prints out the contents of an image.
printImage :: Show a => Vector (Vector a) -> IO ()
printImage = putStrLn . intercalate "\n" . fromVector . mapV show

