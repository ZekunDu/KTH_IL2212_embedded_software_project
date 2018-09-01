# IL2212 Specification Model of the Application

This package contains the executable specification of the image processing application used for the IL2212 Embedded Software lab project, provided as a ForSyDe-Haskell model. 

## Installing

This Haskell project uses the [Cabal](https://www.haskell.org/cabal/) package manager to generate a binary file, a library and a HTML documentation. You can install this package on the provided virtual machine or on your own host machine, depending on your preferences. While for larger projects we would advise installing packages using [Stack](https://docs.haskellstack.org/en/stable/README/) or [sandboxes](http://coldwa.st/e/blog/2013-08-20-Cabal-sandbox.html), for the scope of this lab a global `cabal` installation suffices. 

The following guide focuses on installing on the lab VM (Ubuntu Linux OS), where most of the dependencies are pre-installed. The procedure is similar for other OS, provided you have installed the [Haskell Platform](https://www.haskell.org/platform/). The lab VM has most of the dependencies pre-installed.

First, open a terminal (`Ctrl`+`T`), and copy/paste the following lines:

    echo "export PATH=\$PATH:~/.cabal/bin" >> ~/.bashrc
    source ~/.bashrc 

Provided you have set up the Git repository properly, `cd` into the `spec-model` folder and type in:

    cd <repo_root>/spec-model
	cabal update
	cabal install
	cabal build
	
## Generating the documentation
	
In order to generate de HTML documentation for the (rather old) Cabal version installed on the VM, use the provided `Makefile` script:

    cabal install hscolour   # only the first time
    make doc

On a newer version of [Haskell Platform](https://www.haskell.org/platform/), one would instead run `cabal haddock`. A HTML page will be generated under the `dist` folder, and its path will be printed out on the terminal. Open it using a browser of your choice, and study it. E.g.:

    firefox dist/doc/html/il2212/index.html
	
## Running the testbench binary
	
To run the generated executable program, type in:

    process-image path/to/test-image-folder
	
For example:

    student@il2212:~/il2212-project/spec-model$ process-image images/flag_tiny_mix/
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
    UUUUUUz--zUUUUUUUUUUUU
    UUUUUz+..+zUUUUUUUUUUU
                          
    UUUUUz+..+zUUUUUUUUUUU
    UUUUUUz--zUUUUUUUUUUUU
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
    UUUUUUz--zUUUUUUUUUUUU
    UUUUUz+..+zUUUUUUUUUUU
                          
    UUUUUz+..+zUUUUUUUUUUU
    UUUUUUz--zUUUUUUUUUUUU
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
                          
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
    UUUUUUz--zUUUUUUUUUUUU
    UUUUUz+..+zUUUUUUUUUUU
                          
    UUUUUz+..+zUUUUUUUUUUU
    UUUUUUz--zUUUUUUUUUUUU
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
         -t=-t=           
         -t=-t=           
         -t=-t=           
         -t--t=           
    ttttttt--/tttttttttttt
    ttttt/=..=/ttttttttttt
                          
    ttttt/=..=/ttttttttttt
    tttttt/-:/tttttttttttt
         -t--t=           
         -t=-t=           
         -t=-t=           
         -t=-t=           
         -t=-t=           
         -t=-t=           
         -t=-t=           
         -t--t=           
    ttttttt--/tttttttttttt
    ttttt/=..=/ttttttttttt
                          
    ttttt/=..=/ttttttttttt
    tttttt/-:/tttttttttttt
         -t--t=           
         -t=-t=           
         -t=-t=           
         -t=-t=           
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=           
    UUUUUUz--zUUUUUUUUUUUU
    UUUUUz+..+zUUUUUUUUUUU
                          
    UUUUUz+..+zUUUUUUUUUUU
    UUUUUUz--zUUUUUUUUUUUU
         +U==U=           
         +U==U=           
         +U==U=           
         +U==U=  

You can make your own picture test suite, by collecting PPM images into a folder. Make sure that the `.ppm` files do not contain comment lines (i.e. starting with `#`), and that all images in one folder have the same dimensions.

## Testing functions in an interpreter

Finally, in order to test the individual functions of the library, you need to open an interpreter session and load the needed modules. The GHCi interpreter is called with:

    ghci
	
You can always exit the session with `Ctrl`+`D`.

When inside a GHCi session, you can run any Haskell function and get the result immediately, making it an ideal environment for testing and learning. Get started by testing Haskell code snippets. For understanding Haskell syntax and the usage of GHCi refer to (at least) chapters 1 - 4, 6 and the first section of chapter 7 of [this great online book](http://learnyouahaskell.com/chapters), ideally testing in the interpreter while reading. Excerpt from an interpreter session:

    student@il2212:~/il2212-project/spec-model$ ghci
    GHCi, version 7.6.3: http://www.haskell.org/ghc/  :? for help
    Prelude> 1 + 2                 -- basic syntax, Ch.2
    3
    Prelude> :t 1                  -- types and type classes, Ch.3
    1 :: Num p => p
    Prelude> :t (+)                -- types and functions, Ch.3 and 4
    (+) :: Num a => a -> a -> a
    Prelude> :t (+ 1)              -- partial application/curried functions, Ch.6
    (+ 1) :: Num a => a -> a
    Prelude> let a = 1 :: Int      -- 'let' notation, Ch.4
    Prelude> :t a
    a :: Int
    Prelude> a + 2
    3
    Prelude> :t a + 2
    a + 2 :: Int
	Prelude> let x = [1..5]        -- lists intro, Ch.2
    Prelude> x
    [1,2,3,4,5]
    Prelude> :t x
    x :: (Num a, Enum a) => [a]
    Prelude> map (+1) x            -- higher-order functions, Ch.6
    [2,3,4,5,6]
    Prelude> :t map (+1) x
    map (+1) x :: (Enum b, Num b) => [b]
    Prelude> :info Int             -- info command. types and type classes, Ch.3
    data Int = GHC.Types.I# GHC.Prim.Int# 	-- Defined in ‘GHC.Types’
    instance Eq Int -- Defined in ‘GHC.Classes’
    instance Ord Int -- Defined in ‘GHC.Classes’
    instance Show Int -- Defined in ‘GHC.Show’
    instance Read Int -- Defined in ‘GHC.Read’
    instance Enum Int -- Defined in ‘GHC.Enum’
    instance Num Int -- Defined in ‘GHC.Num’
    instance Real Int -- Defined in ‘GHC.Real’
    instance Bounded Int -- Defined in ‘GHC.Enum’
    instance Integral Int -- Defined in ‘GHC.Real’
    
Your job will be to clearly understand the application specification, and a good practice to do that, apart from going through the code and through the documentation, is to test snippets and functions in the interpreter directly. As such, you would load in the interpreter either source files or, in this case, import library modules and test directly the functions or parts of the functions exported. 

For example, to test the `resize` functions exported by the `IL2212.ImageProcessing` module, you would roughly perform the following steps (consulting the generated documentation at the same time):

    Prelude> import IL2212.ImageProcessing as Img          -- load functions exported by this module
    Prelude Img> import IL2212.Utilities as U              -- the 'as' notation is to give a short alias to the module
    Prelude Img U> ppm <- readFile "images/flag_tiny_good/flag_tiny_01.ppm"  -- the '<-' assignment is used for non-pure I/O functions
    Prelude Img U> let img = grayscale $ ppm2img ppm       -- the 'let' assignment is used for pure functions
    Prelude Img U> printImage $ mapMatrix truncate img     -- notation 'f $ g x' is equivalent to 'f (g x)'
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <78,78,78,78,78,78,78,78,78,78,126,196,194,194,172,77,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78>
    <198,198,198,198,198,198,198,198,198,198,197,193,194,194,195,199,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198>
    <194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194>
    <194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194,194>
    <198,198,198,198,198,198,198,198,198,198,197,193,194,194,195,199,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198>
    <78,78,78,78,78,78,78,78,78,78,126,196,194,194,172,77,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,80,80,80,80,80,130,196,194,194,173,79,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80>
    Prelude Img U> let smallImg = resize img
    Prelude Img U> printImage $ mapMatrix truncate smallImg 
    <80,80,80,80,80,163,194,126,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,163,194,126,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,163,194,126,80,80,80,80,80,80,80,80,80>
    <79,79,79,79,79,162,194,125,79,79,79,79,79,79,79,79,79>
    <196,196,196,196,196,194,194,195,196,196,196,196,196,196,196,196,196>
    <196,196,196,196,196,194,194,195,196,196,196,196,196,196,196,196,196>
    <79,79,79,79,79,162,194,125,79,79,79,79,79,79,79,79,79>
    <80,80,80,80,80,163,194,126,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,163,194,126,80,80,80,80,80,80,80,80,80>
    <80,80,80,80,80,163,194,126,80,80,80,80,80,80,80,80,80>
    
Another example: say you do not understand the construction of the function `resize`. In this case you would reconstruct it step-by-step, as seen in the source code:

    resize = mapMatrix (/ 4) . sumRows . sumCols
      where
        sumCols = mapV (mapV (reduceV (+)) . groupV 2)
        sumRows = mapV (reduceV (zipWithV (+))) . groupV 2

As an exercise, let us reconstruct what is happening in internal function `sumCols`, by using a dummy matrix as image:

	Prelude Img U V> import ForSyDe.Shallow.Vector as V
    Prelude Img U V> let v = vector [1..9]
    Prelude Img U V> let dummy = vector [v,v,v,v]
    Prelude Img U V> dummy
    <<1,2,3,4,5,6,7,8,9>,<1,2,3,4,5,6,7,8,9>,<1,2,3,4,5,6,7,8,9>,<1,2,3,4,5,6,7,8,9>>
    Prelude Img U V> printImage dummy 
    <1,2,3,4,5,6,7,8,9>
    <1,2,3,4,5,6,7,8,9>
    <1,2,3,4,5,6,7,8,9>
    <1,2,3,4,5,6,7,8,9>
    Prelude Img U V> :t groupV 
    groupV :: Int -> Vector a -> Vector (Vector a)
    Prelude Img U V> let f1 = mapV (groupV 2)
    Prelude Img U V> :t f1
    f1 :: Vector (Vector a) -> Vector (Vector (Vector a))
    Prelude Img U V> printImage $ f1 dummy 
    <<1,2>,<3,4>,<5,6>,<7,8>>
    <<1,2>,<3,4>,<5,6>,<7,8>>
    <<1,2>,<3,4>,<5,6>,<7,8>>
    <<1,2>,<3,4>,<5,6>,<7,8>>
    Prelude Img U V> let f2 = mapV (reduceV (+))
    Prelude Img U V> :t f2
    f2 :: Num b => Vector (Vector b) -> Vector b
    Prelude Img U V>  f2 dummy 
    <45,45,45,45>
    Prelude Img U V> let f3 = mapV (mapV (reduceV (+)) . groupV 2)
    Prelude Img U V> :t f3
    f3 :: Num b => Vector (Vector b) -> Vector (Vector b)
    Prelude Img U V> printImage $ f3 dummy 
    <3,7,11,15>
    <3,7,11,15>
    <3,7,11,15>
    <3,7,11,15>
    
Hopefully the above exercises served as an appetizer to get your hands dirty with this ForSyDe-Haskell model. Understanding the system specification at such an abstraction level, albeit far from a specific implementation, enlarges the possible design space and opens up potential ways for alternative mappings and for exploiting various platforms. 

Good materials for helping with language or library related issues, which you should consult at any time:

* [the suggested book](http://learnyouahaskell.com/), or any beginners' manual;
* the generated API documentation;
* the [ForSyDe-Shallow](https://hackage.haskell.org/package/forsyde-shallow) API documentation;
* (especially) [Hoogle](https://www.haskell.org/hoogle/). 

Additional material on the core concepts will be given during the lectures. 
