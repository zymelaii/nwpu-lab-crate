// figure 2.40

module MUX2_CA (
  input  logic sel,
  input  logic dat_in1,
  input  logic dat_in2,
  output logic dat_out
);

assign dat_out = (~sel & dat_in1) | (sel & dat_in2);

endmodule : MUX2_CA
