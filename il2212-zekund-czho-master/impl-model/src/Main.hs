-----------------------------------------------------------------------------
-- |
-- Module      :  Main
-- Copyright   :  (c) George Ungureanu, 2018
-- License     :  BSD-style (see the file LICENSE)
-- 
-- Maintainer  :  ugeorge@kth.se
-- Stability   :  stable
-- Portability :  portable
--
-- Contains the main runner calling the testbench of this project.
-----------------------------------------------------------------------------
module Main where

import IL2212.ImplModel.TestBench
import System.Environment
import System.Directory
import Data.List (isSuffixOf)

-- | The main function, calling the testbench for the
-- 'ImageProcessing.imageProcessing' process network.
--
-- Usage in an interpreter session:
--
-- > :main path/to/image-collection
main = do
  args  <- getArgs
  files <- listDirectory (head args)
  let ppmFiles = filter (isSuffixOf ".ppm") files
      ppmFilePaths = map (\f->head args ++ "/" ++ f) ppmFiles
  if null ppmFilePaths
    then error "the directory does not contain PPM files"
    else testBench ppmFilePaths

