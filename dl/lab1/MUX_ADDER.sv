// figure 2.72

module MUX_ADDER (
  input  logic      sel,
  input  logic[1:0] dat_in1,
  input  logic[1:0] dat_in2,
  output logic[1:0] dat_out
);

logic[1:0] ops;

assign ops = {dat_in2[sel], dat_in1[sel]};
assign dat_out = {ops[0] & ops[1], ops[0] ^ ops[1]};

endmodule : MUX_ADDER
