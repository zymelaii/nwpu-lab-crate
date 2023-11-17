module ADDER_1 (
  input  logic lhs,
  input  logic rhs,
  input  logic carry_in,
  output logic sum,
  output logic carry_out
);

assign sum = lhs ^ rhs ^ carry_in;
assign carry_out = (lhs & rhs) | (rhs & carry_in) | (lhs & carry_in);

endmodule : ADDER_1

module ADDER_2 (
  input  logic[1:0] lhs,
  input  logic[1:0] rhs,
  input  logic      carry_in,
  output logic[1:0] sum,
  output logic      carry_out
);

logic carry;

ADDER_1 u_adder_low
(
  .lhs       (lhs[0]  ),
  .rhs       (rhs[0]  ),
  .carry_in  (carry_in),
  .sum       (sum[0]  ),
  .carry_out (carry   )
);

ADDER_1 u_adder_high
(
  .lhs       (lhs[1]    ),
  .rhs       (rhs[1]    ),
  .carry_in  (carry     ),
  .sum       (sum[1]    ),
  .carry_out (carry_out )
);

endmodule : ADDER_2
