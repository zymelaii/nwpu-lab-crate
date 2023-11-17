module MUX2x1 (
  input  logic[1:0] dat_in,
  input  logic      sel,
  output logic      dat_out
);

assign dat_out = dat_in[sel];

endmodule : MUX2x1
