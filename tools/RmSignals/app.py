# Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
#
#  Licensed under the MIT License - see LICENSE file for details.

import sys

# import time
from typing import Final

import numpy as np
import pyqtgraph as pg
from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QApplication, QMainWindow

import rm_zmq
from rm_zmq import ZmqRM
from ui_MainWindow import Ui_MainWindow

DEFAULT_PERIOD:         Final = 100
DEFAULT_COLOR:          Final = (200, 200, 200)
DEFAULT_DFT_BINS:       Final = 1024
DEFAULT_MENU_ENABLE:    Final = False
DEFAULT_IPC_SOCKET:     Final = "ipc:///tmp/radiomon_sock-zmq-test"

GRAPH_CURVE:            Final = 0
GRAPH_SCATTER:          Final = 1
GRAPH_STEM:             Final = 2

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
        self.zmq = zmq

        # Quit menu item
        self.ui.actionQuit.triggered.connect(self.close)

        # Grid check box
        self.ui.checkBoxGrid.checkStateChanged.connect(self.gridStateChange)

        ## Set up the graphics
        self.ui.graphicsView.setXRange(0.0, DEFAULT_PERIOD)
        # We'll default to normalized samples for now; the user can use pyqtgraph's ability to change the
        # y-axis range at will.
        self.ui.graphicsView.setYRange(-1.0, 1.0)

        if not DEFAULT_MENU_ENABLE:
            pi = self.ui.graphicsView.getPlotItem()
            if pi is not None:
                pi.setMenuEnabled(False)
                pi.setAcceptedMouseButtons(Qt.MouseButton.LeftButton)

        # Curve plot
        self.curve = pg.PlotDataItem(pen=pg.mkPen(DEFAULT_COLOR, width=1))

        # Scatter plot
        self.scatter = pg.PlotDataItem(pen=None, symbol='o', symbolSize=4)

        # Stem plot
        self.stem = pg.GraphItem(pen=DEFAULT_COLOR, symbolPen=DEFAULT_COLOR, symbol='o')

        # The nodes(N,2) array holds two nodes per sample - the (x,y) of the sample, and the (x,0) of the sample's projection into the x-axis
        # X is determined by the number of samples -- np.arange(len(samples))
        # nodes[0] --> (node0_x, node0_y)   -- sample
        # nodes[1] --> (node0_x, 0)         -- sample's x-axis projection
        # Etc..
        self.nodes = np.empty((2, 2), dtype=np.float32) # or np.complex64
        # Each entry of the conns(M,2) array contains the indices of the sample node and its x projection
        # So conns[0] --> (0, 1)
        #    conns[1] --> (2, 3)
        #    conns[2] --> (4, 5)
        #    Etc...
        self.conns = np.empty((1, 2), dtype=np.uint32)

        self.graph = [ self.curve, self.scatter, self.stem ]
        self.currentGraph = GRAPH_STEM

        # Grid
        self.grid = pg.GridItem()

        # Set the current graph
        self.ui.graphicsView.addItem(self.graph[self.currentGraph])

        # Enable the grid if desired
        if self.ui.checkBoxGrid.checkState == Qt.CheckState.Checked:
            self.ui.graphicsView.addItem(self.grid)

    def genNodeIndex(self, num: int):
        for cnt in range(num):
            yield cnt
            yield cnt

    def updateStemGraph(self, sampleNum: int, type: np.dtype):
        if sampleNum != self.conns.shape[0] or type.kind != self.nodes.dtype.kind:
            self.nodes = np.zeros((sampleNum * 2, 2), dtype=type)
            self.nodes[:, 0] = np.fromiter(self.genNodeIndex(sampleNum), dtype=type)
            self.conns = np.arange(sampleNum * 2, dtype=np.uint32).reshape(sampleNum, 2)

    @Slot(np.uint8, np.uint32, np.ndarray)
    def recv(self, type: np.uint8, rate: np.uint32, samples: np.ndarray):
        # tick = time.perf_counter_ns()

        if self.currentGraph == GRAPH_STEM:
            ndtype = np.dtype(np.float32) if type == rm_zmq.SAMPLES_FLOAT else np.dtype(np.complex64)
            self.updateStemGraph(sampleNum=samples.size, type=ndtype)
            self.nodes[0:samples.size * 2:2, 1] = samples
            self.stem.setData(pos=self.nodes, adj=self.conns)
        else:
            self.graph[self.currentGraph].setData(samples)

        # print(f"Recv type={type}, rate={rate}, samples={samples.size} [{(time.perf_counter_ns() - tick) / 1000000}]")

    @Slot(Qt.CheckState)
    def gridStateChange(self, state: Qt.CheckState):
        if state == Qt.CheckState.Checked:
            self.ui.graphicsView.addItem(self.grid)
        else:
            self.ui.graphicsView.removeItem(self.grid)

def main():
    app = QApplication(sys.argv)

    screen = app.primaryScreen()
    print(f"Max screen WxH: {screen.availableSize().width()}, {screen.availableSize().height()}")

    zmq = ZmqRM(DEFAULT_IPC_SOCKET, DEFAULT_PERIOD)

    mw = MainWindow(zmq)
    mw.show()

    app.aboutToQuit.connect(lambda: zmq.quit())

    sys.exit(app.exec())
