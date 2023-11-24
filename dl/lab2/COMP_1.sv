module COMP_1 (
  input  logic lhs,
  input  logic rhs,
  output logic eq,
  output logic lt,
  output logic gt
);

assign eq = ~(lhs ^ rhs);
assign lt = ~lhs & rhs;
assign gt = lhs & ~rhs;

endmodule : COMP_1
