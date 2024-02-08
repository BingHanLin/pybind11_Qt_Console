# pybind11 Qt Console

## Overview

This project embeds a Python console into Qt using the pybind11 package. The Python console includes simple auto-completion capabilities and integrates with an undo/redo framework.

![](./asset/image.png)

## Features

* Python Console Integration: Incorporate a Python console into your Qt application, enabling direct interaction with Python code.

* Auto-Completion: Provide context-aware suggestions as the user types.

* Undo/Redo Framework: Support reverting and restoring changes.

* MIT License for easy integration into your own projects.

## Structure

The core code resides in the **pythonEmbedding** folder, which is an independent target in the CMakeLists.txt script.

The communication bridge between Python scripts and C++ codes is built in the **pythonCommands.cpp** file. It defines the available Python modules and methods using pybind11 embedding functions, facilitating easy interaction between Python and C++ functionalities.

## Usage
TODO.

## License
This project is licensed under the MIT License - see the [LICENSE](https://opensource.org/license/mit/) file for details.

## Contributions
Contributions to this project are welcome. Please feel free to submit issues or pull requests to improve the functionality or fix any bugs.

## Reference
1. [Qt Completer](https://doc.qt.io/qt-5/qtwidgets-tools-customcompleter-example.html)
1. [avogadro pythonterminal.cpp](https://github.com/cryos/avogadro/blob/master/libavogadro/src/extensions/pythonterminal.cpp)
1. [pybind11: Embedding the interpreter](https://pybind11.readthedocs.io/en/latest/advanced/embedding.html)
