# pybind11 Qt Console

## Overview

This project embeds a Python console into Qt using the pybind11 package. The Python console includes simple auto-completion capabilities and integrates with an undo/redo framework.

![](./asset/image.png)

## Features

* Python Console Integration: Incorporate a Python console into your Qt application, enabling direct interaction with Python code.

* Auto-Completion: Provide context-aware suggestions as the user types.

* Undo/Redo Framework: Support reverting and restoring changes.


* Operation Recording: Capture and record user interactions with the UI, enabling convenient playback and analysis of scripts.


* MIT License for easy integration into your own projects.

## Structure

The core code resides in the **pythonEmbedding** folder, which is an independent target in the CMakeLists.txt script.

The communication bridge between Python scripts and C++ codes is built in the **pythonCommands.cpp** file. It defines the available Python modules and methods using pybind11 embedding functions, facilitating easy interaction between Python and C++ functionalities.

## Usage
1. **Prepare pybind11 environment**

    Set up a Python virtual environment, naming it **pybind11-env**.
    ```bash
    python -m venv pybind11-env
    ```
    After the virtual environment is created, you can activate it. On Windows, you can activate it with:
    ```bash
    .\pybind11-env\Scripts\activate
    ```
    After activation, use the following command to install the necessary package:
    ```bash
    pip install -r requirements.txt
    ```

2. **Configure CMakePresets.json**
    Change the value of `Qt5_DIR` and `Qt5_BIN_DIR` in the **CMakePresets.json** to match your environment.

3. **Build project**
    Build the project with the following commands.
    ```bash
    cmake --preset MSVC_x64-release
    ```
    ```bash
    cmake --build --preset MSVC_x64-release
    ``` 

    ```bash
    cmake --install build/release/MSVC_x64-release
    ``` 
4. Find the executable under the **bin** folder.

## License
This project is licensed under the MIT License - see the [LICENSE](https://opensource.org/license/mit/) file for details.

## Contributions
Contributions to this project are welcome. Please feel free to submit issues or pull requests to improve the functionality or fix any bugs.

## Reference
1. [Qt Completer](https://doc.qt.io/qt-5/qtwidgets-tools-customcompleter-example.html)
1. [avogadro pythonterminal.cpp](https://github.com/cryos/avogadro/blob/master/libavogadro/src/extensions/pythonterminal.cpp)
1. [pybind11: Embedding the interpreter](https://pybind11.readthedocs.io/en/latest/advanced/embedding.html)
1. [QPyConsole](https://github.com/roozbehg/QPyConsole)
1. [Creating a Scripting Egnine with pybind11. Step by step.](https://github.com/aoloe/cpp-pybind11-playground)
1. [QtCpp-PythonConsole](https://github.com/kaiware0x/QtCpp-PythonConsole)
1. [QGIS/python/console](https://github.com/qgis/QGIS/tree/master/python/console)
1. [A Qt Console for IPython](https://ipython.org/ipython-doc/dev/interactive/qtconsole.html)
