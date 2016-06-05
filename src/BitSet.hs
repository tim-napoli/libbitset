{-# LANGUAGE ForeignFunctionInterface #-}

import Control.Applicative ((<$>), (<*>))
import Foreign.Storable

data BitSet = BitSet (Int, Int, Ptr Int64)

