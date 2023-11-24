module DFF (
  input  logic clk,
  input  logic rst_n,
  input  logic dat_in,
  output logic dat_out
);

logic data;
logic rst;

assign rst = ~rst_n;
assign dat_out = data;

always_ff @(posedge clk, posedge rst) begin
  data <= rst ? 1'b0 : dat_in;
end

endmodule : DFF
