module COMP_1 (
  input  logic lhs,
  input  logic rhs,
  output logic eq,
  output logic lt,
  output logic gt
);

assign eq = ~lt & ~gt;

always_comb begin
  unique casez ({lhs, rhs})
    2'b10: {lt, gt} = 2'b01;
    2'b01: {lt, gt} = 2'b10;
    default: {lt, gt} = 2'b00;
  endcase
end

endmodule : COMP_1
