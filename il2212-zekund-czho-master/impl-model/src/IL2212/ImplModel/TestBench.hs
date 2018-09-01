-----------------------------------------------------------------------------
-- |
-- Module      :  IL2212.ImplModel.TestBench
-- Copyright   :  (c) George Ungureanu, 2018
-- License     :  BSD-style (see the file LICENSE)
-- 
-- Maintainer  :  ugeorge@kth.se
-- Stability   :  stable
-- Portability :  portable
--
-- Contains the testbench of this project.
-----------------------------------------------------------------------------
module IL2212.ImplModel.TestBench where

import Data.List (intercalate)
import ForSyDe.Shallow (signal, fromSignal)
import IL2212.ImplModel.ImageProcessing
import IL2212.Utilities

-- | Testbench function for the ForSyDe process network
-- 'IL2212.ImageProcessing.imageProcessing' . It reads a
-- <https://en.wikipedia.org/wiki/Netpbm_format P3 PPM image> as a
-- stream of pixels, replicates it @n@ times, feeds it into the
-- process network, collects the result stream and prints out the
-- output image(s).
testBench :: [FilePath] -- ^ path to the input PPM files.
          -> IO ()
testBench filePaths = do
  (dimX, dimY, imageStream) <- readAllPPM filePaths
  let -------testbench-------
      -- signalOut = your-process-network-instantiation 
      -------testbench-------
      imageOut = intercalate "\n" $ partition dimX1 dimY1 $ fromSignal signalOut
      dimX1 = (dimX `div` 2) - 2
      dimY1 = (dimY `div` 2) - 2

  putStrLn imageOut
