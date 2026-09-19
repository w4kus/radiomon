# Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
#
#  Licensed under the MIT License - see LICENSE file for details.

import sys
from typing import Final

import numpy as np
import pyqtgraph
from PySide6.QtCore import Slot
from PySide6.QtWidgets import QApplication, QMainWindow

from rm_zmq import ZmqRM
from ui_MainWindow import Ui_MainWindow

DEFAULT_PERIOD:    Final = 100

# MainWindow
class MainWindow(QMainWindow):

    def __init__(self, zmq: ZmqRM):
        super().__init__()

        ## Set up the GUI
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        ## Connect slots
        # Samples
        zmq.instance.samples_recv.connect(self.recv)

        # Quit menu item
        self.ui.actionQuit.triggered.connect(self.close)

        ## Set up the graphics
        self.ui.graphicsView.setXRange(0.0, DEFAULT_PERIOD)
        self.ui.graphicsView.setYRange(-1.0, 1.0)

        # Set the attributes to the plot data item
        self.plotData = pyqtgraph.PlotDataItem(pen=pyqtgraph.mkPen((200, 200, 200), width=1))

        self.ui.graphicsView.addItem(self.plotData)
        self.ui.graphicsView.addItem(pyqtgraph.GridItem())

    @Slot(np.uint8, np.uint32, np.ndarray)
    def recv(self, type: np.ubyte, rate: np.uint32, samples: np.ndarray):
        print(f"Recv type={type}, rate={rate}, samples={samples.size}")
        self.plotData.setData(samples)

def main():
    app = QApplication(sys.argv)

    screen = app.primaryScreen()
    print(f"Max screen WxH: {screen.availableSize().width()}, {screen.availableSize().height()}")

    zmq = ZmqRM("ipc:///tmp/radiomon_sock-zmq-test")

    mw = MainWindow(zmq)
    mw.show()

    app.aboutToQuit.connect(lambda: zmq.quit())

    sys.exit(app.exec())
