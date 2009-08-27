// Verilog inter-page module for JPEG_dec_d1_ScOrEtMp0
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

`include "JPEG_dec_d1_ScOrEtMp0_qin.v"
`include "JPEG_dec_d1_ScOrEtMp0_noin.v"

module JPEG_dec_d1_ScOrEtMp0 (clock, reset, outA_d, outA_e, outA_v, outA_b, outB_d, outB_e, outB_v, outB_b, outC_d, outC_e, outC_v, outC_b, outD_d, outD_e, outD_v, outD_b, outE_d, outE_e, outE_v, outE_b, outF_d, outF_e, outF_v, outF_b, outG_d, outG_e, outG_v, outG_b, outH_d, outH_e, outH_v, outH_b, Huffin_d, Huffin_e, Huffin_v, Huffin_b) /* synthesis syn_hier="firm" */ ;

  input  clock;
  input  reset;

  output [8:0] outA_d;
  output outA_e;
  output outA_v;
  input  outA_b;
  output [8:0] outB_d;
  output outB_e;
  output outB_v;
  input  outB_b;
  output [8:0] outC_d;
  output outC_e;
  output outC_v;
  input  outC_b;
  output [8:0] outD_d;
  output outD_e;
  output outD_v;
  input  outD_b;
  output [8:0] outE_d;
  output outE_e;
  output outE_v;
  input  outE_b;
  output [8:0] outF_d;
  output outF_e;
  output outF_v;
  input  outF_b;
  output [8:0] outG_d;
  output outG_e;
  output outG_v;
  input  outG_b;
  output [8:0] outH_d;
  output outH_e;
  output outH_v;
  input  outH_b;
  input  [7:0] Huffin_d;
  input  Huffin_e;
  input  Huffin_v;
  output Huffin_b;

  wire   [8:0] outA_qin_d, outA_qout_d;
  wire   outA_qin_e, outA_qout_e;
  wire   outA_qin_v, outA_qout_v;
  wire   outA_qin_b, outA_qout_b;
  wire   [8:0] outB_qin_d, outB_qout_d;
  wire   outB_qin_e, outB_qout_e;
  wire   outB_qin_v, outB_qout_v;
  wire   outB_qin_b, outB_qout_b;
  wire   [8:0] outC_qin_d, outC_qout_d;
  wire   outC_qin_e, outC_qout_e;
  wire   outC_qin_v, outC_qout_v;
  wire   outC_qin_b, outC_qout_b;
  wire   [8:0] outD_qin_d, outD_qout_d;
  wire   outD_qin_e, outD_qout_e;
  wire   outD_qin_v, outD_qout_v;
  wire   outD_qin_b, outD_qout_b;
  wire   [8:0] outE_qin_d, outE_qout_d;
  wire   outE_qin_e, outE_qout_e;
  wire   outE_qin_v, outE_qout_v;
  wire   outE_qin_b, outE_qout_b;
  wire   [8:0] outF_qin_d, outF_qout_d;
  wire   outF_qin_e, outF_qout_e;
  wire   outF_qin_v, outF_qout_v;
  wire   outF_qin_b, outF_qout_b;
  wire   [8:0] outG_qin_d, outG_qout_d;
  wire   outG_qin_e, outG_qout_e;
  wire   outG_qin_v, outG_qout_v;
  wire   outG_qin_b, outG_qout_b;
  wire   [8:0] outH_qin_d, outH_qout_d;
  wire   outH_qin_e, outH_qout_e;
  wire   outH_qin_v, outH_qout_v;
  wire   outH_qin_b, outH_qout_b;
  wire   [7:0] Huffin_qin_d, Huffin_qout_d;
  wire   Huffin_qin_e, Huffin_qout_e;
  wire   Huffin_qin_v, Huffin_qout_v;
  wire   Huffin_qin_b, Huffin_qout_b;

  assign outA_d = outA_qout_d;
  assign outA_e = outA_qout_e;
  assign outA_v = outA_qout_v;
  assign outA_qout_b = outA_b;
  assign outB_d = outB_qout_d;
  assign outB_e = outB_qout_e;
  assign outB_v = outB_qout_v;
  assign outB_qout_b = outB_b;
  assign outC_d = outC_qout_d;
  assign outC_e = outC_qout_e;
  assign outC_v = outC_qout_v;
  assign outC_qout_b = outC_b;
  assign outD_d = outD_qout_d;
  assign outD_e = outD_qout_e;
  assign outD_v = outD_qout_v;
  assign outD_qout_b = outD_b;
  assign outE_d = outE_qout_d;
  assign outE_e = outE_qout_e;
  assign outE_v = outE_qout_v;
  assign outE_qout_b = outE_b;
  assign outF_d = outF_qout_d;
  assign outF_e = outF_qout_e;
  assign outF_v = outF_qout_v;
  assign outF_qout_b = outF_b;
  assign outG_d = outG_qout_d;
  assign outG_e = outG_qout_e;
  assign outG_v = outG_qout_v;
  assign outG_qout_b = outG_b;
  assign outH_d = outH_qout_d;
  assign outH_e = outH_qout_e;
  assign outH_v = outH_qout_v;
  assign outH_qout_b = outH_b;
  assign Huffin_qin_d = Huffin_d;
  assign Huffin_qin_e = Huffin_e;
  assign Huffin_qin_v = Huffin_v;
  assign Huffin_b = Huffin_qin_b;

  JPEG_dec_d1_ScOrEtMp0_qin qin (clock, reset, Huffin_qin_d, Huffin_qin_e, Huffin_qin_v, Huffin_qin_b, Huffin_qout_d, Huffin_qout_e, Huffin_qout_v, Huffin_qout_b);

  JPEG_dec_d1_ScOrEtMp0_noin noin (clock, reset, outA_qout_d, outA_qout_e, outA_qout_v, outA_qout_b, outB_qout_d, outB_qout_e, outB_qout_v, outB_qout_b, outC_qout_d, outC_qout_e, outC_qout_v, outC_qout_b, outD_qout_d, outD_qout_e, outD_qout_v, outD_qout_b, outE_qout_d, outE_qout_e, outE_qout_v, outE_qout_b, outF_qout_d, outF_qout_e, outF_qout_v, outF_qout_b, outG_qout_d, outG_qout_e, outG_qout_v, outG_qout_b, outH_qout_d, outH_qout_e, outH_qout_v, outH_qout_b, Huffin_qout_d, Huffin_qout_e, Huffin_qout_v, Huffin_qout_b);

endmodule  // JPEG_dec_d1_ScOrEtMp0
