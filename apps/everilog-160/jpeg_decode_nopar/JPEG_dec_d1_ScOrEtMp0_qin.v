// Verilog inter-page input queue module for JPEG_dec_d1_ScOrEtMp0
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "Q_wire.v"

module JPEG_dec_d1_ScOrEtMp0_qin (clock, reset, Huffin_qin_d, Huffin_qin_e, Huffin_qin_v, Huffin_qin_b, Huffin_qout_d, Huffin_qout_e, Huffin_qout_v, Huffin_qout_b);

  input  clock;
  input  reset;

  input  [7:0] Huffin_qin_d;
  input  Huffin_qin_e;
  input  Huffin_qin_v;
  output Huffin_qin_b;
  output [7:0] Huffin_qout_d;
  output Huffin_qout_e;
  output Huffin_qout_v;
  input  Huffin_qout_b;

  Q_wire #(0, 9) q_Huffin (clock, reset, {Huffin_qin_d, Huffin_qin_e}, Huffin_qin_v, Huffin_qin_b, {Huffin_qout_d, Huffin_qout_e}, Huffin_qout_v, Huffin_qout_b);

endmodule  // JPEG_dec_d1_ScOrEtMp0_qin
