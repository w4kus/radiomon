# Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
#
#  Licensed under the MIT License - see LICENSE file for details.

import sys
import time
from typing import Final

import numpy as np
import pyqtgraph as pg
from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QApplication, QMainWindow

import rm_zmq as rmzmq
from ui_MainWindow import Ui_MainWindow

GRAPH_CURVE:            Final = 0
GRAPH_SCATTER:          Final = 1
GRAPH_STEM:             Final = 2

DEFAULT_REAL_COLOR:     Final = (200, 200, 200)
DEFAULT_CMPLX_COLOR:    Final = (0, 153, 200)
DEFAULT_DFT_BINS:       Final = 1024
DEFAULT_MENU_ENABLE:    Final = False
DEFAULT_IPC_SOCKET:     Final = "ipc:///tmp/radiomon_sock-zmq-test"
DEFAULT_GRAPH:          Final = GRAPH_STEM

SAMPLE_REAL:            Final = 0
SAMPLE_IMAG:            Final = 1

DOMAIN_TIME:            Final = 0
DOMAIN_FREQ:            Final = 1

# MainWindow
class MainWindow(QMainWindow):

    def __init__(self, zmq: rmzmq.ZmqRM):
        super().__init__()

        self.zmq = zmq

        # Default to the time domain
        self.domain = DOMAIN_TIME

        # Default to real samples
        self.currentSampleType = rmzmq.SAMPLE_TYPE_FLOAT

        ##################################
        # Set up the GUI
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        assert self.ui.plotWidget is not None
        assert self.ui.plotWidget.plotItem is not None

        # Save off the PlotItem for future use
        self.pi = self.ui.plotWidget.plotItem

        # Enable the pyqtgraph context menu if desired
        if not DEFAULT_MENU_ENABLE:
            self.pi.setMenuEnabled(False)
            self.pi.setAcceptedMouseButtons(Qt.MouseButton.LeftButton)

        # Use OpenGL acceleraion if available - falls back to CPU
        pg.setConfigOption(opt='useOpenGL', value=True)

        # x axis array
        self.xArray = np.zeros(1, dtype=np.float32)

        # Save off the view box for later use
        self.vb = self.pi.getViewBox()

        # Save off the axis items for later use
        self.leftAxis: pg.AxisItem = self.pi.getAxis("left")
        self.bottomAxis: pg.AxisItem = self.pi.getAxis("bottom")

        self.vb.setDefaultPadding(0.0)

        # Set up the graphics
        self.vb.setXRange(0.0, rmzmq.DEFAULT_SAMPLE_PERIOD)

        # We'll default to normalized samples for now; the user can use pyqtgraph's ability to change the
        # y-axis range at will.
        self.vb.setYRange(-1.0, 1.0)

        # Quit menu item
        self.ui.actionQuit.triggered.connect(self.close)

        # Grid check box
        self.ui.checkBoxGrid.checkStateChanged.connect(self.gridStateChange)

        # Graph combo box
        self.ui.comboBoxGraph.addItems((self.tr("Curve"), self.tr("Scatter"), self.tr("Stem")))
        self.ui.comboBoxGraph.currentIndexChanged.connect(self.graphChanged)

        # Domain combo box
        self.ui.comboBoxDomain.addItems((self.tr("Time"), self.tr("Frequency")))
        self.ui.comboBoxDomain.currentIndexChanged.connect(self.domainChanged)
        self.ui.comboBoxDomain.setEnabled(False)    # Disabled for now

        # Refresh period spin box
        self.ui.spinBoxPeriod.valueChanged.connect(self.periodChanged)

        # Play / pause button - we start out playing
        self.ui.pushButtonPlayPause.setText(self.tr("Pause"))

        ##################################
        # Graph setup
        # Curve plot
        self.curve = [ pg.PlotDataItem(pen=pg.mkPen(DEFAULT_REAL_COLOR, width=1)), \
                       pg.PlotDataItem(pen=pg.mkPen(DEFAULT_CMPLX_COLOR, width=1)) ]

        # Scatter plot
        self.scatter = [ pg.PlotDataItem(pen=pg.mkPen(DEFAULT_REAL_COLOR, width=1), symbolPen=pg.mkPen(DEFAULT_REAL_COLOR), symbol='o', symbolSize=8), \
                         pg.PlotDataItem(pen=pg.mkPen(DEFAULT_CMPLX_COLOR, width=1), symbolPen=pg.mkPen(DEFAULT_CMPLX_COLOR), symbol='o', symbolSize=8) ]
        self.scatter[SAMPLE_REAL].scatter.opts.update(hoverable=True)
        self.scatter[SAMPLE_IMAG].scatter.opts.update(hoverable=True)


        # Stem plot
        self.stem = [ pg.GraphItem(pen=DEFAULT_REAL_COLOR, symbolPen=DEFAULT_REAL_COLOR, symbol='o'), \
                      pg.GraphItem(pen=DEFAULT_CMPLX_COLOR, symbolPen=DEFAULT_CMPLX_COLOR, symbol='o') ]
        self.stem[SAMPLE_REAL].scatter.opts.update(hoverable=True)
        self.stem[SAMPLE_IMAG].scatter.opts.update(hoverable=True)

        # Stem plot related arrays
        # The node(N,2) arrays hold two nodes per sample - the (x,y) of the sample, and the (x,0) of the sample's projection onto the x-axis
        # nodes[0] --> (node0_x, node0_y)   -- sample
        # nodes[1] --> (node0_x, 0)         -- sample's x-axis projection
        # Etc..
        self.nodeRealArr = np.zeros((2, 2), dtype=np.float32)
        self.nodeImagArr = np.zeros((2, 2), dtype=np.float32)
        # Each entry of the conns(M,2) array contains the indices of the sample node with its x projection node
        # So conns[0] --> (0, 1)
        #    conns[1] --> (2, 3)
        #    conns[2] --> (4, 5)
        #    Etc...
        self.conns = np.zeros((1, 2), dtype=np.uint32)

        self.graph = [ self.curve, self.scatter, self.stem ]
        self.currentGraph = DEFAULT_GRAPH
        self.ui.comboBoxGraph.setCurrentIndex(DEFAULT_GRAPH)

        # Grid
        self.grid = pg.GridItem()

        # Set the current graph
        self.pi.addItem(self.graph[self.currentGraph][SAMPLE_REAL])
        self.pi.addItem(self.graph[self.currentGraph][SAMPLE_IMAG])

        self.bottomAxis.setLabel(self.tr("Time (ms)"))

        # Samples slot
        zmq.instance.samples_recv.connect(self.recv)

    ###################################
    # Methods
    # Iterator for updateStemGraph()
    def genNodeIndex(self, num: int):
        for cnt in range(num):
            yield self.xArray[cnt]
            yield self.xArray[cnt]

    # Update the stem graph on sample / rate change
    def updateStemGraph(self, sampleNum: int):
        self.nodeRealArr = np.zeros((sampleNum * 2, 2), dtype=np.float32)
        self.nodeRealArr[:, 0] = np.fromiter(self.genNodeIndex(sampleNum), dtype=np.float32)
        self.nodeImagArr = np.zeros((sampleNum * 2, 2), dtype=np.float32)
        self.nodeImagArr[:, 0] = np.fromiter(self.genNodeIndex(sampleNum), dtype=np.float32)

        self.conns = np.arange(sampleNum * 2, dtype=np.uint32).reshape(sampleNum, 2)

    @Slot(np.uint8, np.uint32, np.ndarray)
    def recv(self, type: np.uint8, rate: np.uint32, samples: np.ndarray):
        # tick = time.perf_counter_ns()

        # NOTE We do not expect the sample type to change once the stream starts.
        self.currentSampleType = type

        if self.xArray.size != self.zmq.instance.sampleSize:
            self.xArray = np.linspace(0, self.zmq.instance.period, num=samples.size, dtype=np.float32)
            self.updateStemGraph(samples.size)
            self.vb.setXRange(0.0, self.zmq.instance.period)

        if self.currentGraph == GRAPH_STEM:
            self.nodeRealArr[0:samples.size * 2:2, 1] = np.real(samples)
            self.nodeImagArr[0:samples.size * 2:2, 1] = np.imag(samples)
            self.stem[SAMPLE_REAL].setData(pos=self.nodeRealArr, adj=self.conns)

            if self.currentSampleType == rmzmq.SAMPLE_TYPE_CMPLX:
                self.stem[SAMPLE_IMAG].setData(pos=self.nodeImagArr, adj=self.conns)
        else:
            self.graph[self.currentGraph][SAMPLE_REAL].setData(self.xArray, np.real(samples))

            if self.currentSampleType == rmzmq.SAMPLE_TYPE_CMPLX:
                self.graph[self.currentGraph][SAMPLE_IMAG].setData(self.xArray, np.imag(samples))

        # print(f"Recv type={type}, rate={rate}, samples={samples.size} [{(time.perf_counter_ns() - tick) / 1000000}]")

    @Slot(Qt.CheckState)
    def gridStateChange(self, state: Qt.CheckState):
        if state == Qt.CheckState.Checked:
            self.pi.addItem(self.grid)
        else:
            self.pi.removeItem(self.grid)

    @Slot(int)
    def graphChanged(self, index: int):
        if index != self.currentGraph:
            self.pi.removeItem(self.graph[self.currentGraph][SAMPLE_REAL])
            self.pi.removeItem(self.graph[self.currentGraph][SAMPLE_IMAG])
            self.currentGraph = index
            self.pi.addItem(self.graph[self.currentGraph][SAMPLE_REAL])

            if self.currentSampleType == rmzmq.SAMPLE_TYPE_CMPLX:
                self.pi.addItem(self.graph[self.currentGraph][SAMPLE_IMAG])

    @Slot(int)
    def domainChanged(self, index: int):
        print(f"{index}")

    @Slot(int)
    def periodChanged(self, i: int):
       self.zmq.instance.period = i

    @Slot(bool)
    def playPauseClicked(self, checked: bool):
        if self.ui.pushButtonPlayPause.text() == "Pause":
            self.zmq.instance.samples_recv.disconnect(self.recv)
            self.ui.pushButtonPlayPause.setText(self.tr("Play"))
        else:
            self.zmq.instance.samples_recv.connect(self.recv)
            self.ui.pushButtonPlayPause.setText(self.tr("Pause"))

def main():
    app = QApplication(sys.argv)

    screen = app.primaryScreen()
    print(f"Max screen WxH: {screen.availableSize().width()}, {screen.availableSize().height()}")

    zmq = rmzmq.ZmqRM(DEFAULT_IPC_SOCKET, rmzmq.DEFAULT_SAMPLE_PERIOD)

    mw = MainWindow(zmq)
    mw.show()

    app.aboutToQuit.connect(lambda: zmq.quit())

    sys.exit(app.exec())
