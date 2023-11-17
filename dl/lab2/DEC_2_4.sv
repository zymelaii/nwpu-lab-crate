module DEC_2_4 (
  input  logic[1:0] dat_in,
  output logic[3:0] dat_out
);

assign dat_out = 4'b1 << dat_in;

endmodule : DEC_2_4
