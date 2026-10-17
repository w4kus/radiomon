# Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
#
#  Licensed under the MIT License - see LICENSE file for details.

import argparse
import sys
import time  # noqa: F401
from typing import Final

import numpy as np
import pyqtgraph as pg
import scipy.fft
import scipy.signal
from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QApplication, QMainWindow

import rm_zmq as rmzmq
from ui_MainWindow import Ui_MainWindow

GRAPH_CURVE:            Final = 0
GRAPH_SCATTER:          Final = 1
GRAPH_STEM:             Final = 2

SAMPLE_REAL:            Final = 0
SAMPLE_IMAG:            Final = 1

DOMAIN_TIME:            Final = 0
DOMAIN_FREQ:            Final = 1

DFT_POINTS_512:         Final = 0
DFT_POINTS_1024:        Final = 1
DFT_POINTS_2048:        Final = 2
DFT_POINTS_4096:        Final = 3
DFT_POINTS_8192:        Final = 4

DFT_WINDOW_HANNING:     Final = 0
DFT_WINDOW_HAMMING:     Final = 1
DFT_WINDOW_BH:          Final = 2
DFT_WINDOW_KAISER:      Final = 3

DEFAULT_REAL_COLOR:     Final = (200, 200, 200)
DEFAULT_CMPLX_COLOR:    Final = (0, 153, 200)
DEFAULT_MENU_ENABLE:    Final = False
DEFAULT_IPC_SOCKET:     Final = "ipc:///tmp/radiomon_sock-zmq-test"
DEFAULT_GRAPH:          Final = GRAPH_CURVE
DEFAULT_POINTS:         Final = DFT_POINTS_1024
DEFAULT_WINDOW:         Final = DFT_WINDOW_HANNING

# MainWindow
class MainWindow(QMainWindow):

    def __init__(self, zmq: rmzmq.ZmqRM):
        super().__init__()

        self.zmq = zmq

        # Default to the time domain
        self.domain = DOMAIN_TIME

        # Default to real samples
        self.currentSampleType = rmzmq.SAMPLE_TYPE_FLOAT

        # Default DFT points
        self.currentDFTPoints = DEFAULT_POINTS

        # Default DFT window
        self.currentDFTWindowIdx = DEFAULT_WINDOW

        # Whether or not we're recieving sample blocks
        self.receivingBlocks = True

        ##################################
        # Set up the GUI
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        assert self.ui.plotWidget is not None
        assert self.ui.plotWidget.plotItem is not None

        # Enable the pyqtgraph context menu if desired
        if not DEFAULT_MENU_ENABLE:
            self.ui.plotWidget.plotItem.setMenuEnabled(False)
            self.ui.plotWidget.plotItem.setAcceptedMouseButtons(Qt.MouseButton.LeftButton)

        # Use OpenGL acceleraion if available - falls back to CPU
        pg.setConfigOption(opt='useOpenGL', value=True)

        # x axis array
        self.xArray = np.zeros(1, dtype=np.float32)

        # Save off the view box for later use
        self.vb = self.ui.plotWidget.plotItem.getViewBox()

        # Save off the axis items for later use
        self.leftAxis: pg.AxisItem = self.ui.plotWidget.plotItem.getAxis("left")
        self.bottomAxis: pg.AxisItem = self.ui.plotWidget.plotItem.getAxis("bottom")

        self.vb.setDefaultPadding(0.0)

        # Startup X axis range (time domain)
        self.vb.setXRange(0.0, rmzmq.DEFAULT_SAMPLE_PERIOD)

        # We'll default to normalized samples for now; the user can use pyqtgraph's ability to change the
        # y-axis range at will.
        self.vb.setYRange(-1.0, 1.0)

        # Quit menu item
        self.ui.actionQuit.triggered.connect(self.close)

        # Grid check box
        self.ui.checkBoxGrid.checkStateChanged.connect(self.gridStateChange)

        # Graph combo box
        self.ui.comboBoxGraph.addItems((self.tr("Curve"), self.tr("Sample Curve"), self.tr("Stem")))
        self.ui.comboBoxGraph.currentIndexChanged.connect(self.graphChanged)

        # Domain combo box
        self.ui.comboBoxDomain.addItems((self.tr("Time"), self.tr("Frequency")))
        self.ui.comboBoxDomain.currentIndexChanged.connect(self.domainChanged)

        # Sample size in time
        self.ui.spinBoxPeriod.valueChanged.connect(self.periodChanged)

        # Play / pause button - we start out playing
        self.ui.pushButtonPlayPause.clicked.connect(self.playPauseClicked)
        self.ui.pushButtonPlayPause.setText(self.tr("Pause"))
        self.playPauseCount = np.uint8(0)
        self.ui.pushButtonPlayPause.setEnabled(True)

        # Sample size for frequency (DFT)
        self.ui.comboBoxNumPoints.addItems((self.tr("512"), self.tr("1024"), self.tr("2048"), self.tr("4096"), self.tr("8192")))
        self.ui.comboBoxNumPoints.setEnabled(False)
        self.ui.comboBoxNumPoints.setCurrentIndex(self.currentDFTPoints)
        self.ui.comboBoxNumPoints.currentIndexChanged.connect(self.numPointsChanged)

        self.dftPoints = [ 512, 1024, 2048, 4096, 8192 ]

        # Window (DFT)
        self.ui.comboBoxWindow.addItems((self.tr("Hanning"), self.tr("Hamming"), self.tr("Blackman-Harris"), self.tr("Kaiser")))
        self.ui.comboBoxWindow.setEnabled(False)
        self.ui.comboBoxWindow.setCurrentIndex(self.currentDFTWindowIdx)
        self.ui.comboBoxWindow.currentIndexChanged.connect(self.windowChanged)

        # Attenuation --> beta value for Kaiser windows
        self.ui.spinBoxAtten.setEnabled(False)

        self.windows = [ "hann", "hamming", "blackmanharris", "kaiser" ]

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

        # Initialize the graph list and the default graph
        self.graph = [ self.curve, self.scatter, self.stem ]
        self.currentGraph = DEFAULT_GRAPH
        self.ui.comboBoxGraph.setCurrentIndex(DEFAULT_GRAPH)

        # Grid - nothing special, just the default settings (which are good)
        self.grid = pg.GridItem()

        # Add the default graph to the view box
        self.vb.addItem(self.graph[self.currentGraph][SAMPLE_REAL])
        self.vb.addItem(self.graph[self.currentGraph][SAMPLE_IMAG])

        # Samples slot
        zmq.instance.samples_recv.connect(self.recv)

    ###################################
    # Methods
    # Iterator for updateStemGraph()
    def genNodeIndex(self, num: int):
        for cnt in range(num):
            yield self.xArray[cnt]
            yield self.xArray[cnt]

    # Update the x-axid on sample / rate change
    def updateXAxis(self, sampleSize: int):
        if self.domain == DOMAIN_TIME:
            self.xArray = np.linspace(0, self.zmq.instance.period, num=sampleSize, dtype=np.float32)
            self.vb.setXRange(0.0, self.zmq.instance.period)
            self.bottomAxis.setLabel(self.tr("Time (ms)"))
        else:
            if self.currentSampleType == rmzmq.SAMPLE_TYPE_CMPLX:
                self.xArray = np.linspace(-np.pi, np.pi, num=sampleSize, dtype=np.float32)
                self.vb.setXRange(-np.pi, np.pi)
            else:
                self.xArray = np.linspace(0, np.pi, num=sampleSize, dtype=np.float32)
                self.vb.setXRange(0, np.pi)

            self.bottomAxis.setLabel(self.tr("Frequency (radians/sample)"))

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

        # FIXME - handle receivingBlocks

        # NOTE We do not expect the sample type to change once the stream starts.
        self.currentSampleType = type

        # Are we in the frequency domain? If so transform the samples using the DFT
        if self.domain == DOMAIN_FREQ:
            # Ignore blocks in the signal queue which are not the correct size
            if samples.size == self.dftPoints[self.currentDFTPoints]:
                param = (self.windows[self.currentDFTWindowIdx], self.ui.spinBoxAtten.value()) \
                        if self.currentDFTWindowIdx == DFT_WINDOW_KAISER else self.windows[self.currentDFTWindowIdx]

                samples = samples * scipy.signal.get_window(param, self.dftPoints[self.currentDFTPoints])

                # Create a new 'samples' object overriding the passed in reference to 'samples'
                if type == rmzmq.SAMPLE_TYPE_FLOAT:
                    samples = np.abs(scipy.fft.rfft(samples, norm="forward")) # type: ignore
                else:
                    samples = np.abs(scipy.fft.fftshift(scipy.fft.fft(samples, norm="forward"))) # type: ignore
            else:
                return

        if self.xArray.size != samples.size:
            self.updateXAxis(samples.size)
            self.updateStemGraph(samples.size)

        if self.currentGraph == GRAPH_STEM:
            self.nodeRealArr[0:samples.size * 2:2, 1] = np.real(samples)
            self.nodeImagArr[0:samples.size * 2:2, 1] = np.imag(samples)
            self.stem[SAMPLE_REAL].setData(pos=self.nodeRealArr, adj=self.conns)

            if type == rmzmq.SAMPLE_TYPE_CMPLX:
                self.stem[SAMPLE_IMAG].setData(pos=self.nodeImagArr, adj=self.conns)
        else:
            self.graph[self.currentGraph][SAMPLE_REAL].setData(self.xArray, np.real(samples))

            if type == rmzmq.SAMPLE_TYPE_CMPLX:
                self.graph[self.currentGraph][SAMPLE_IMAG].setData(self.xArray, np.imag(samples))

        # print(f"Recv type={type}, rate={rate}, samples={samples.size} [{(time.perf_counter_ns() - tick) / 1000000}]")

    @Slot(Qt.CheckState)
    def gridStateChange(self, state: Qt.CheckState):
        if state == Qt.CheckState.Checked:
            self.vb.addItem(self.grid)
        else:
            self.vb.removeItem(self.grid)

    @Slot(int)
    def graphChanged(self, i: int):
        if i != self.currentGraph:
            self.vb.removeItem(self.graph[self.currentGraph][SAMPLE_REAL])

            if self.currentSampleType == rmzmq.SAMPLE_TYPE_CMPLX:
                self.vb.removeItem(self.graph[self.currentGraph][SAMPLE_IMAG])

            self.currentGraph = i
            self.vb.addItem(self.graph[self.currentGraph][SAMPLE_REAL])

            if self.currentSampleType == rmzmq.SAMPLE_TYPE_CMPLX:
                self.vb.addItem(self.graph[self.currentGraph][SAMPLE_IMAG])

    @Slot(int)
    def periodChanged(self, i:int):
        self.zmq.instance.period = i

    @Slot(int)
    def domainChanged(self, i: int):
        self.domain = i

        if i == DOMAIN_TIME:
            self.ui.comboBoxNumPoints.setEnabled(False)
            self.ui.comboBoxWindow.setEnabled(False)
            self.ui.spinBoxPeriod.setEnabled(True)

            self.zmq.instance.period = self.ui.spinBoxPeriod.value()
            self.vb.setYRange(-1.0, 1.0)
        else:
            self.ui.comboBoxNumPoints.setEnabled(True)
            self.ui.comboBoxWindow.setEnabled(True)
            self.ui.spinBoxPeriod.setEnabled(False)

            self.zmq.instance.setFixedSampleSize(self.dftPoints[self.ui.comboBoxNumPoints.currentIndex()])
            self.vb.setYRange(0, 0.5)

    @Slot(int)
    def windowChanged(self, i: int):
        if i == DFT_WINDOW_KAISER:
            self.ui.spinBoxAtten.setEnabled(True)
        else:
            self.ui.spinBoxAtten.setEnabled(False)

        self.currentDFTWindowIdx = i

    @Slot(bool)
    def playPauseClicked(self, checked: bool):
        if self.receivingBlocks:
            self.ui.pushButtonPlayPause.setEnabled(True)
            self.playPauseCount += 1

            if self.playPauseCount & 1:
                self.zmq.instance.samples_recv.disconnect(self.recv)
                self.ui.pushButtonPlayPause.setText(self.tr("Play"))
            else:
                self.zmq.instance.samples_recv.connect(self.recv)
                self.ui.pushButtonPlayPause.setText(self.tr("Pause"))

    @Slot(int)
    def numPointsChanged(self, i:int):
        self.currentDFTPoints = i
        self.zmq.instance.setFixedSampleSize(self.dftPoints[i])

def main():
    app = QApplication(sys.argv)

    screen = app.primaryScreen()
    print(f"Max screen WxH: {screen.availableSize().width()}, {screen.availableSize().height()}")

    zmq = rmzmq.ZmqRM(DEFAULT_IPC_SOCKET, rmzmq.DEFAULT_SAMPLE_PERIOD)

    mw = MainWindow(zmq)
    mw.show()

    app.aboutToQuit.connect(lambda: zmq.quit())

    sys.exit(app.exec())
