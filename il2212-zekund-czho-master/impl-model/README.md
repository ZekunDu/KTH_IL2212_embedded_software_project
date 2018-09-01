# IL2212 Implementation Model of the Application

This is the frame for the **optional** task in IL2212 lab project. For this task you have to write the ForSyDe-Haskell model of the image processing application described by the given [specification](../spec-model), but reflecting the mapping decisions of your multi-processor SoC implementation. Thus each actor will merge the image processing functions mapped to one processor core. 

## Installing

This Haskell project uses the [Cabal](https://www.haskell.org/cabal/) package manager to generate a binary file, a library and a HTML documentation. You can install this package on the provided virtual machine or on your own host machine, depending on your preferences. While for larger projects we would advise installing packages using [Stack](https://docs.haskellstack.org/en/stable/README/) or [sandboxes](http://coldwa.st/e/blog/2013-08-20-Cabal-sandbox.html), for the scope of this lab a global `cabal` installation suffices. 

The following guide focuses on installing on the lab VM (Ubuntu Linux OS), where most of the dependencies are pre-installed. The procedure is similar for other OS, provided you have installed the [Haskell Platform](https://www.haskell.org/platform/). The lab VM has most of the dependencies pre-installed.

First, open a terminal (`Ctrl`+`T`), and copy/paste the following line:

    echo "PATH=\$PATH:~/.cabal/bin" >> .bashrc

This project depends on the libraries exported by the [specification model](../spec-model). This means that you can reuse the code from that project by properly importing the library modules. Also, this means that you have previously installed the [specification model](../spec-model) by following the instructions in the README file. Provided you have set up the Git repository properly, `cd` into the `impl-model` folder and type in:

    cd <repo_root>/impl-model
	cabal update
	cabal install
	cabal build
	
## Generating the documentation
	
In order to generate de HTML documentation:

    cabal haddock --hyperlink-source

A HTML page will be generated under the `dist` folder, and its path will be printed out on the terminal. Open it using a browser of your choice, and study it. E.g.:

    firefox dist/doc/html/il2212-impl/index.html
	
