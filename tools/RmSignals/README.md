## RmSignals -- *radiomon* Graphing Tool


This utility allows you to graph signals at various points in a chain. If is used in conjunction with the
*zmq-sample-pub* block. You can insert instances of this block at points in the chain with
each point opening a different IPC socket. Multiple instances of this utility can be run with each being a ZMQ subscriber to an instance.

Usage:
`python -m RmSignals -s <URL>`

where *\<URL\>* is an IPC file on the same host.

**NOTE:** TCP or other network schemes have not been tested yet.

Example
In the tools directory, enter:
`python -m RmSignals -s "ipc:///tmp/radiomon_sock-hilbert-test"`

This uses the system `/tmp` directory for the IPC file but you can use any directory in which you have read/write priviledges.

#### Other options:
`-h, --help` : Display the help and exit.
`-v, --version` : Display the version and exit.

### Installation

You will need Python version 3.12 or greater due to the requirments for PyQtGraph. The library requirements are in *requirements.txt* and can be installed in the usual way:

`pip install -r requrements.txt`

You'll need to decide if you want to install it in a Python **venv** (recommended) or globally. Global installation is usually blocked by the distribution by default since if it overwrites Python files that the OS needs, things could break. See [venv — Creation of virtual environments](https://docs.python.org/3/library/venv.html) for instructions. If you want to change *RmSignals* code and are using Visual Code, the Python plugin will set up a venv for you. By default it installs the venv in the project folder in a hidden directory (i.e., RmSignals/.venv).

### Features

RmSignals allow you to graph the incoming sample stream in the time or frequency domain. The GUI controls are self explanatory. Three types of graphs are available:

* **Curve** This is the fastest and most efficient method of graphing the samples. Individual sample points are not displayed, only the curve itself. This is enabled by default on start up.
* **Sample Curve** Similar to the *Curve* plot but it also plots the sample points. Hovering over a point will display the X and Y values.
* **Stem** This is good for viewing PAM like signals (e.g., post demodulation). Sample points are connected to the X axis to emphasize amplitude.

All three types are available in both time and frequency views. The frequency plot also has four different window functions available. A mouse left click and hold will allow you to pan the display. Using the mouse wheel, hold the mouse over the X axis to zoom in or out horizontally, or the Y axis to zoom in or out vertically.  Doing this over the graph will zoom in or out on both axes.

