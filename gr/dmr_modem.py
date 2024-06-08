#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: DMR Modem
# GNU Radio version: 3.10.1.1

from packaging.version import Version as StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from gnuradio import analog
import math
from gnuradio import blocks
from gnuradio import channels
from gnuradio.filter import firdes
from gnuradio import digital
from gnuradio import filter
from gnuradio import gr
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import zeromq



from gnuradio import qtgui

class dmr_modem(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "DMR Modem", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("DMR Modem")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "dmr_modem")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.test_burst_2 = test_burst_2 = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 223, 245, 125, 117, 223, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        self.test_burst = test_burst = (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 223, 245, 125, 117, 223, 93, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
        self.samp_rate = samp_rate = 24000
        self.rrc_tap_num = rrc_tap_num = 64
        self.rf_freq_interp = rf_freq_interp = 64
        self.radio_samp_rate = radio_samp_rate = 1536000
        self.fsk_deviation_hz = fsk_deviation_hz = 2749
        self.bs_sync_data_word = bs_sync_data_word = ( -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, -1.0 )
        self.baud_rate = baud_rate = 4800

        ##################################################
        # Blocks
        ##################################################
        self.zeromq_push_sink_0 = zeromq.push_sink(gr.sizeof_float, 1, 'tcp://127.0.0.1:55000', 100, False, -1)
        self.root_raised_cosine_filter_0_0 = filter.fir_filter_fff(
            1,
            firdes.root_raised_cosine(
                1.0,
                samp_rate,
                baud_rate,
                0.3,
                rrc_tap_num))
        self.root_raised_cosine_filter_0 = filter.interp_fir_filter_fff(
            5,
            firdes.root_raised_cosine(
                4.0,
                samp_rate,
                baud_rate,
                0.8,
                rrc_tap_num))
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=rf_freq_interp,
                decimation=1,
                taps=[],
                fractional_bw=0.4)
        self.low_pass_filter_0 = filter.fir_filter_ccf(
            rf_freq_interp,
            firdes.low_pass(
                1,
                samp_rate * rf_freq_interp,
                12000,
                2000,
                window.WIN_HAMMING,
                6.76))
        self.digital_chunks_to_symbols_xx_0 = digital.chunks_to_symbols_bf([ 0.4, 1.0, -0.4, -1.0 ], 1)
        self.channels_channel_model_0 = channels.channel_model(
            noise_voltage=0.0,
            frequency_offset=0.0,
            epsilon=1.0,
            taps=[1.0 + 0.0j],
            noise_seed=0,
            block_tags=False)
        self.blocks_vector_source_x_0 = blocks.vector_source_b(test_burst_2, True, 1, [])
        self.blocks_throttle_1 = blocks.throttle(gr.sizeof_float*1, baud_rate,True)
        self.blocks_packed_to_unpacked_xx_0 = blocks.packed_to_unpacked_bb(2, gr.GR_MSB_FIRST)
        self.analog_quadrature_demod_cf_0 = analog.quadrature_demod_cf(samp_rate/(2*math.pi*fsk_deviation_hz))
        self.analog_frequency_modulator_fc_0 = analog.frequency_modulator_fc(2*math.pi*fsk_deviation_hz/samp_rate)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_frequency_modulator_fc_0, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.root_raised_cosine_filter_0_0, 0))
        self.connect((self.blocks_packed_to_unpacked_xx_0, 0), (self.digital_chunks_to_symbols_xx_0, 0))
        self.connect((self.blocks_throttle_1, 0), (self.root_raised_cosine_filter_0, 0))
        self.connect((self.blocks_vector_source_x_0, 0), (self.blocks_packed_to_unpacked_xx_0, 0))
        self.connect((self.channels_channel_model_0, 0), (self.low_pass_filter_0, 0))
        self.connect((self.digital_chunks_to_symbols_xx_0, 0), (self.blocks_throttle_1, 0))
        self.connect((self.low_pass_filter_0, 0), (self.analog_quadrature_demod_cf_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.channels_channel_model_0, 0))
        self.connect((self.root_raised_cosine_filter_0, 0), (self.analog_frequency_modulator_fc_0, 0))
        self.connect((self.root_raised_cosine_filter_0_0, 0), (self.zeromq_push_sink_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "dmr_modem")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_test_burst_2(self):
        return self.test_burst_2

    def set_test_burst_2(self, test_burst_2):
        self.test_burst_2 = test_burst_2
        self.blocks_vector_source_x_0.set_data(self.test_burst_2, [])

    def get_test_burst(self):
        return self.test_burst

    def set_test_burst(self, test_burst):
        self.test_burst = test_burst

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.analog_frequency_modulator_fc_0.set_sensitivity(2*math.pi*self.fsk_deviation_hz/self.samp_rate)
        self.analog_quadrature_demod_cf_0.set_gain(self.samp_rate/(2*math.pi*self.fsk_deviation_hz))
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate * self.rf_freq_interp, 12000, 2000, window.WIN_HAMMING, 6.76))
        self.root_raised_cosine_filter_0.set_taps(firdes.root_raised_cosine(4.0, self.samp_rate, self.baud_rate, 0.8, self.rrc_tap_num))
        self.root_raised_cosine_filter_0_0.set_taps(firdes.root_raised_cosine(1.0, self.samp_rate, self.baud_rate, 0.3, self.rrc_tap_num))

    def get_rrc_tap_num(self):
        return self.rrc_tap_num

    def set_rrc_tap_num(self, rrc_tap_num):
        self.rrc_tap_num = rrc_tap_num
        self.root_raised_cosine_filter_0.set_taps(firdes.root_raised_cosine(4.0, self.samp_rate, self.baud_rate, 0.8, self.rrc_tap_num))
        self.root_raised_cosine_filter_0_0.set_taps(firdes.root_raised_cosine(1.0, self.samp_rate, self.baud_rate, 0.3, self.rrc_tap_num))

    def get_rf_freq_interp(self):
        return self.rf_freq_interp

    def set_rf_freq_interp(self, rf_freq_interp):
        self.rf_freq_interp = rf_freq_interp
        self.low_pass_filter_0.set_taps(firdes.low_pass(1, self.samp_rate * self.rf_freq_interp, 12000, 2000, window.WIN_HAMMING, 6.76))

    def get_radio_samp_rate(self):
        return self.radio_samp_rate

    def set_radio_samp_rate(self, radio_samp_rate):
        self.radio_samp_rate = radio_samp_rate

    def get_fsk_deviation_hz(self):
        return self.fsk_deviation_hz

    def set_fsk_deviation_hz(self, fsk_deviation_hz):
        self.fsk_deviation_hz = fsk_deviation_hz
        self.analog_frequency_modulator_fc_0.set_sensitivity(2*math.pi*self.fsk_deviation_hz/self.samp_rate)
        self.analog_quadrature_demod_cf_0.set_gain(self.samp_rate/(2*math.pi*self.fsk_deviation_hz))

    def get_bs_sync_data_word(self):
        return self.bs_sync_data_word

    def set_bs_sync_data_word(self, bs_sync_data_word):
        self.bs_sync_data_word = bs_sync_data_word

    def get_baud_rate(self):
        return self.baud_rate

    def set_baud_rate(self, baud_rate):
        self.baud_rate = baud_rate
        self.blocks_throttle_1.set_sample_rate(self.baud_rate)
        self.root_raised_cosine_filter_0.set_taps(firdes.root_raised_cosine(4.0, self.samp_rate, self.baud_rate, 0.8, self.rrc_tap_num))
        self.root_raised_cosine_filter_0_0.set_taps(firdes.root_raised_cosine(1.0, self.samp_rate, self.baud_rate, 0.3, self.rrc_tap_num))




def main(top_block_cls=dmr_modem, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
