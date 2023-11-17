// figure 2.42

module MUX2_COMB (
  input  logic sel,
  input  logic dat_in1,
  input  logic dat_in2,
  output logic dat_out
);

always_comb begin
  if (sel == 0) begin
    dat_out = dat_in1;
  end
  else begin
    dat_out = dat_in2;
  end
end

endmodule : MUX2_COMB
