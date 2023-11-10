// figure 2.45

module ADDER (
  input  logic a,
  input  logic b,
  output logic s1,
  output logic s0
);

assign s1 = a & b;
assign s0 = a ^ b;

endmodule : ADDER
