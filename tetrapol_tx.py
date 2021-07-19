#!/usr/bin/env python2
##################################################
# GNU Radio Python Flow Graph
# Title: TETRAPOL TX
# Generated: Sat Nov 21 15:21:15 2015
##################################################

from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import osmosdr
import time


class tetrapol_tx(gr.top_block):

    def __init__(self, args="", freq=380000000, freq_offs=200000, in_file="", out_file="", oversample=10, ppm=0):
        gr.top_block.__init__(self, "TETRAPOL TX")

        ##################################################
        # Parameters
        ##################################################
        self.args = args
        self.freq = freq
        self.freq_offs = freq_offs
        self.in_file = in_file
        self.out_file = out_file
        self.oversample = oversample
        self.ppm = ppm

        ##################################################
        # Variables
        ##################################################
        self.resample = resample = 32
        self.samp_rate = samp_rate = 8000*oversample*resample

        ##################################################
        # Blocks
        ##################################################
        self.rational_resampler_xxx_0 = filter.rational_resampler_ccc(
                interpolation=resample,
                decimation=1,
                taps=None,
                fractional_bw=None,
        )
        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + args )
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq(freq+freq_offs, 0)
        self.osmosdr_sink_0.set_freq_corr(ppm, 0)
        self.osmosdr_sink_0.set_gain(10, 0)
        self.osmosdr_sink_0.set_if_gain(20, 0)
        self.osmosdr_sink_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0.set_antenna("", 0)
        self.osmosdr_sink_0.set_bandwidth(1e6, 0)
          
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(1, (1, ), freq_offs, samp_rate)
        self.digital_gmsk_mod_0 = digital.gmsk_mod(
        	samples_per_symbol=oversample,
        	bt=0.25,
        	verbose=False,
        	log=False,
        )
        self.blocks_pack_k_bits_bb_0 = blocks.pack_k_bits_bb(8)
        self.blocks_multiply_const_vxx_1 = blocks.multiply_const_vcc((0.4, ))
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_char*1, in_file, False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.blocks_pack_k_bits_bb_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_1, 0), (self.osmosdr_sink_0, 0))    
        self.connect((self.blocks_pack_k_bits_bb_0, 0), (self.digital_gmsk_mod_0, 0))    
        self.connect((self.digital_gmsk_mod_0, 0), (self.rational_resampler_xxx_0, 0))    
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.blocks_multiply_const_vxx_1, 0))    
        self.connect((self.rational_resampler_xxx_0, 0), (self.freq_xlating_fir_filter_xxx_0, 0))    


    def get_args(self):
        return self.args

    def set_args(self, args):
        self.args = args

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.osmosdr_sink_0.set_center_freq(self.freq+self.freq_offs, 0)

    def get_freq_offs(self):
        return self.freq_offs

    def set_freq_offs(self, freq_offs):
        self.freq_offs = freq_offs
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.freq_offs)
        self.osmosdr_sink_0.set_center_freq(self.freq+self.freq_offs, 0)

    def get_in_file(self):
        return self.in_file

    def set_in_file(self, in_file):
        self.in_file = in_file
        self.blocks_file_source_0.open(self.in_file, False)

    def get_out_file(self):
        return self.out_file

    def set_out_file(self, out_file):
        self.out_file = out_file

    def get_oversample(self):
        return self.oversample

    def set_oversample(self, oversample):
        self.oversample = oversample
        self.set_samp_rate(8000*self.oversample*self.resample)

    def get_ppm(self):
        return self.ppm

    def set_ppm(self, ppm):
        self.ppm = ppm
        self.osmosdr_sink_0.set_freq_corr(self.ppm, 0)

    def get_resample(self):
        return self.resample

    def set_resample(self, resample):
        self.resample = resample
        self.set_samp_rate(8000*self.oversample*self.resample)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate)


if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("", "--args", dest="args", type="string", default="",
        help="Set osmosdr args [default=%default]")
    parser.add_option("-f", "--freq", dest="freq", type="eng_float", default=eng_notation.num_to_str(380000000),
        help="Set Frequency [default=%default]")
    parser.add_option("", "--freq-offs", dest="freq_offs", type="intx", default=200000,
        help="Set Freq offset [default=%default]")
    parser.add_option("-i", "--in-file", dest="in_file", type="string", default="",
        help="Set Input file name [default=%default]")
    parser.add_option("-o", "--out-file", dest="out_file", type="string", default="",
        help="Set Output file name [default=%default]")
    parser.add_option("", "--oversample", dest="oversample", type="intx", default=10,
        help="Set oversample [default=%default]")
    parser.add_option("", "--ppm", dest="ppm", type="intx", default=0,
        help="Set PPM [default=%default]")
    (options, args) = parser.parse_args()
    tb = tetrapol_tx(args=options.args, freq=options.freq, freq_offs=options.freq_offs, in_file=options.in_file, out_file=options.out_file, oversample=options.oversample, ppm=options.ppm)
    tb.start()
    tb.wait()
