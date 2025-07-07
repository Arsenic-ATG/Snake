# SNAKE
SNAKE game clone created in C++ and SDL3, The project is also designed to be compile with emscripten to webasm and be deployed online.

| [Start a new game](https://arsenic-atg.github.io/Snake/) |
| --------------------------------------- |

<!---TODO: Add screen shots here --->


## controls

| key                | Action                   |
|--------------------|--------------------------|
| `w`                | move up                  |
| `a`                | move left                |
| `s`                | move down                |
| `d`                | move left                |
| `esc`              | exit window              |

## Dependencies

- [Git](https://git-scm.com): for version control
- [Standard C++ compiler](https://en.wikipedia.org/wiki/List_of_compilers#cite_ref-15) conforming to C++17 and above standards: to build the project locally (tested on gcc15.1 and clang 20.1)
- [SDL3.0 library](https://github.com/libsdl-org/SDL/blob/main/INSTALL.md) :for rendering stuff on screen.
- (Optional) [Emscripten](https://emscripten.org) : to compile the program to WebAsm

## Build instructions
( This section or README.md is still under construction, you may help in improving this by opening a PR/issue regarding the same )

perform the following steps to build the project on your native machine :

1. Clone/download the repository

2. Navigate to `src` subdirectory of repository

3. Now compile the program  (depending on whether you want to build the online deployable version or native build, follow one of the following steps )

   **caveat:** currently the repository doesn't have a build system/script to automate the build process, so sadly for now, manual build is the only way to get the project running on the system (If you are interested in setting up one for the project then feel free to open an issue discussing the same)

    3.1. **WebAsm build using emscripten** :


    To build the online deployable version of the game, we would be using `emscripten` to compile the program to webasm.

    - Executing the following command is I think all you need to compile the project ( do note that Emscripten tend to take relatively long time than your average C++ compiler to build the project, and compilling for the first time would almost always take much longer to build than subsequent builds )
    ``` shell
    $ em++ main.cpp snake.cpp -lSDL3 -o index.html
    ```
    emscripten compiler driver (`em++`) workes in almost same way as that of gcc, so in most cases all you might need is to just replace `g++` with `em++` when passing build command

    - The build files should be in the same folder (change `index.html` to the path you want the output build files to be placed )
    - You can simply use `emrun .` to run the HTML pages via a locally launched web server. ( or alternatively you can use your own webserver of your liking )

    ``` shell
    $ emrun index.html
    ```

    FWIW: you may also like to visit [emscripten documentation](https://emscripten.org/docs/compiling/Building-Projects.html#building-projects) regarding more info about building a project with emscripten if you are interesting to know more about the same.

    3.2. **Native build using native C++ compiler** :

    - Just use your native C++ compiler to build and executable from all the source files (order in which they should be provided to the command should not matter in most cases )

    - Make sure to link `SDL3` library properly.

    ```shell
     $ g++ main.cpp snake.cpp -o snake
    ```

    - run the built executable.

    ```shell
     $ ./snake
    ```
    
These instructions are meant to be understood by developers/users of every level, so if you are unable to understand anything or face any difficulty in building the project then make sure to complaint about the same by opening an issue or in discuss section.

## For Hacktoberfest

This repository is participating in hacktoberfest, so any contribution done on this project during the month of october should be counted towards hacktoberfest.

## Contribution

Contribution of all kinds ( code, non-code, low-code) are appriciated. 

<!--- - If you are looking to contribute to the project then make sure to read [contribution guidelines](./CONTRIBUTING.md) before making a new pull request. --->

- If you want to report a bug, or want to share ideas/contribute to the project then feel free to open an issue/PR regarding the same

Also feel free to open an issue for asking queries ( no questions are stupid questions ).

**If you liked the project then make sure to support the developers by giving it a star 🤩.**

## License

[![License](https://img.shields.io/:license-GPL3-blue.svg?style=flat-square)](https://www.gnu.org/licenses/gpl-3.0.en.html)

- **[GNU GPL-3.0](./LICENSE)**
