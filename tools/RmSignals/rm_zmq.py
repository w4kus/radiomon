# Copyright (c) 2026 John Mark White -- US Amateur Radio License: W4KUS
#
#  Licensed under the MIT License - see LICENSE file for details.

from typing import Final

import numpy as np
import zmq
from PySide6.QtCore import QObject, QThread, Signal, Slot

SAMPLES_FLOAT:          Final = 0x01
SAMPLES_CMPLX:          Final = 0x02
DEFAULT_SAMPLE_PERIOD:  Final = 100

# You should access the attributes of this object via ZmqRM.instance property
class ZmqHandler(QObject):

    # Signal to receive incoming samples
    samples_recv = Signal(np.uint8, np.uint32, np.ndarray, name="Received Samples Signal")

    # Signal for the typical QT thread design pattern 
    finished = Signal()

    def __init__(self, sock: zmq.SyncSocket):
        super().__init__()

        self.__sock = sock
        self.__running = True
        self.__sock.setsockopt(zmq.LINGER, 0)
        self.__period = DEFAULT_SAMPLE_PERIOD
        self.__newPeriod = DEFAULT_SAMPLE_PERIOD
        self.__samplesPerPeriod = 0
        self.__currentType = np.dtype(np.float32)
        self.__currentRate = 0
        self.__currentBuff = np.uint8(0)

        # Ping pong buffer with a shape of (x,). Default to 32 bit floats
        self.__sampleBuffer = self.__resetBuffer(type=np.dtype(np.float32))

    def stop(self):
        self.__running = False
        self.finished.emit()

    @property
    def period(self):
        return self.__period

    @period.setter
    def period(self, period: int):
        self.__newPeriod = period

    def __resetBuffer(self, type: np.dtype, dim = (2,)):
        self.__currentBuff = 0
        return [ np.empty(shape=dim, dtype=type), np.empty(shape=dim, dtype=type) ]

    @Slot()
    def run_task(self):

        print("Starting socket thread...")

        updateBuffer = False
        dt = np.dtype(np.float32)
        step = 4
        sampleCount = 0

        while self.__running:
            try:
                parts = self.__sock.recv_multipart(zmq.DONTWAIT, copy=False)

                type = np.uint8(parts[0].buffer[0])
                rate = np.frombuffer(parts[0].buffer, dtype=np.uint8, count=4, offset=1).view(dtype=np.uint32)[0]

                if type == SAMPLES_FLOAT:
                    dt = np.dtype(np.float32)
                    step = 4
                else:
                    dt = np.dtype(np.complex64)
                    step = 8

                # New type?
                if (dt.kind != self.__currentType.kind):
                    self.__currentType = dt
                    print(f"Update sampling buffer: [type={dt}]")
                    sampleCount = 0
                    self.__resetBuffer(dt, self.__sampleBuffer[0].shape[0])

                # New period?
                if (self.__newPeriod != self.__period):
                    self.__period = self.__newPeriod
                    updateBuffer = True

                # New rate?
                if self.__currentRate != rate:
                    self.__currentRate = rate
                    updateBuffer = True

                if updateBuffer:
                    updateBuffer = False
                    newSpp = int(self.__currentRate / 1000 * self.__period)

                    if self.__sampleBuffer[0].shape[0] < newSpp:
                        currentDim = self.__sampleBuffer[0].shape[0]
                        print(f"Update sampling buffer: ({currentDim}) [rate={rate} period={self.__period}]")
                        self.__sampleBuffer[0] = np.pad(self.__sampleBuffer[0], (0, newSpp - currentDim))
                        self.__sampleBuffer[1] = np.pad(self.__sampleBuffer[1], (0, newSpp - currentDim))

                    self.__samplesPerPeriod = newSpp
                    sampleCount = 0

                # Copy the samples from the ZMQ frame into the current sampling buffer and flush when the samples per
                # period is reached - this is the only copy we do.
                for os in range(0, len(parts[1].buffer), step):
                    self.__sampleBuffer[self.__currentBuff][sampleCount] = \
                        np.frombuffer(parts[1].buffer, dtype=np.uint8, count=step, offset=os).view(dtype=dt)[0]

                    sampleCount += 1
                    if sampleCount == self.__samplesPerPeriod:
                        # Send a reference of the current buffer and switch to the "other" buffer
                        self.samples_recv.emit(type, rate, self.__sampleBuffer[self.__currentBuff])
                        sampleCount = 0
                        self.__currentBuff = (self.__currentBuff + 1) & 1

            except zmq.error.Again:
                QThread.msleep(10)

            except zmq.ContextTerminated:
                print("Ending socket thread")
                self.__running = False


class ZmqRM(QObject):

    def __init__(self, ep_id: str, period: int = DEFAULT_SAMPLE_PERIOD):
        super().__init__()

        # Create a ZMQ socket
        self._sock = zmq.Context().instance().socket(zmq.SUB)

        # Subscribe to all messages
        self._sock.subscribe(b"")

        # Connect to the publisher
        self._sock.connect(ep_id)

        # Create the polling thread
        self.__thread = QThread()
        self.__thread.setObjectName("RMZMQ")
        self.__handler = ZmqHandler(self._sock)
        self.__handler.period = period;

        self.__handler.moveToThread(self.__thread)
        self.__thread.started.connect(self.__handler.run_task)

        self.__thread.finished.connect(self.__thread.deleteLater)
        self.__handler.finished.connect(self.__handler.deleteLater)

        # Start it up
        self.__thread.start()

    @property
    def instance(self):
        return self.__handler

    def quit(self):
        self._sock.close()
        self.__handler.stop()
        self.__thread.quit()
        self.__thread.wait()